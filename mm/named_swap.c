// SPDX-License-Identifier: GPL-2.0
/*
 * Named swap backing files.
 *
 * This is intentionally light for now: it creates a root directory and one
 * backing file per anon_vma under /.named_swap/<index>.
 */
#include <linux/anon_inodes.h>
#include <linux/atomic.h>
#include <linux/err.h>
#include <linux/dcache.h>
#include <linux/falloc.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <linux/rmap.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/spinlock.h>
#include <linux/xarray.h>
#include <linux/mman.h>
#include "internal.h"

/*
 * Facade struct file (named_swap_fops) over a real backing file on disk.
 * Page cache and I/O use the lower file; the wrapper carries named_swap hooks.
 */
struct named_swap_file {
	struct file *lower;
	spinlock_t bind_lock;
	u64 index;
	unsigned long nr_pages;
};

static DEFINE_XARRAY(named_swap_files);
static DEFINE_MUTEX(named_swap_xa_lock);
static atomic_long_t named_swap_pages = ATOMIC_LONG_INIT(0);

static struct file *named_swap_file_peek(u64 index);
static struct file *file_for_named_swap_entry(swp_entry_t entry);

unsigned long named_swap_total_pages(void)
{
	return atomic_long_read(&named_swap_pages);
}

/*
 * Named swap mapcount is packed into the upper bits of the normal page
 * cache shadow (xa_value).  The lower bits remain a valid workingset
 * shadow for workingset_refault().
 */
#define NAMED_SWAP_MC_SHIFT		(BITS_PER_LONG - EVICTION_SHIFT)
#define NAMED_SWAP_MC_MASK		((1UL << NAMED_SWAP_MC_SHIFT) - 1)
#define NAMED_SWAP_MC_IN_SHADOW(val)	((val) >> NAMED_SWAP_MC_SHIFT)

static int named_swap_shadow_mapcount(void *shadow)
{
	unsigned long val = xa_to_value(shadow);

	if (!(val >> NAMED_SWAP_MC_SHIFT))
		return 0;
	return NAMED_SWAP_MC_IN_SHADOW(val);
}

void *named_swap_eviction_shadow(struct folio *folio, void *shadow)
{
	VM_BUG_ON_FOLIO(folio_mapcount(folio) > NAMED_SWAP_MC_MASK, folio);
	unsigned long entry = xa_to_value(shadow);
	entry = ((unsigned long)folio_mapcount(folio) << NAMED_SWAP_MC_SHIFT) | entry ;
	//now set mapcount to 0
	atomic_set(&folio->_mapcount, -1);
	return xa_mk_value(entry);
}

void named_swap_refault_shadow(struct folio *folio, void *shadow)
{
	int mapcount = named_swap_shadow_mapcount(shadow);

	VM_BUG_ON_FOLIO(mapcount > NAMED_SWAP_MC_MASK, folio);
	atomic_set(&folio->_mapcount, mapcount);
}

int named_swap_mapcount(struct address_space *mapping, pgoff_t index)
{
	void *entry;
	int mapcount;

	if (!mapping_named_swap(mapping))
		return 0;

	entry = filemap_get_entry(mapping, index);
	if (!entry)
		return 0;

	if (!xa_is_value(entry)) {
		mapcount = folio_mapcount(entry);
		folio_put(entry);
		return mapcount;
	}

	return named_swap_shadow_mapcount(entry);
}
EXPORT_SYMBOL_GPL(named_swap_mapcount);

static void *named_swap_shadow_mapcount_sub(void *shadow)
{
	unsigned long val = xa_to_value(shadow);
	int mapcount;

	if (!(val >> NAMED_SWAP_MC_SHIFT))
		return shadow;

	mapcount = NAMED_SWAP_MC_IN_SHADOW(val);
	if (--mapcount <= 0)
		val >>= NAMED_SWAP_MC_SHIFT;
	else
		val = (val & ~NAMED_SWAP_MC_MASK) | mapcount;

	return val ? xa_mk_value(val) : NULL;
}

/*
 * Decrement named-swap mapcount stored in a page-cache shadow slot.
 * There is no filemap helper for in-place shadow updates; swap and
 * workingset paths also store shadows with xas_store under i_pages lock.
 */
static void named_swap_pagecache_shadow_sub(struct address_space *mapping,
					    pgoff_t index)
{
	XA_STATE(xas, &mapping->i_pages, index);
	void *slot;
	unsigned long flags;

	xa_lock_irqsave(&mapping->i_pages, flags);
	slot = xas_load(&xas);
	if (slot && !xa_is_value(slot)) {
		atomic_dec(&((struct folio *)slot)->_mapcount);
		goto out_unlock;
	}
	if (!slot || !xa_is_value(slot))
		goto out_unlock;

	slot = named_swap_shadow_mapcount_sub(slot);
	xas_store(&xas, slot);
out_unlock:
	xa_unlock_irqrestore(&mapping->i_pages, flags);
}

void named_swap_zap_nonpresent(struct vm_area_struct *vma,
			       unsigned long address, swp_entry_t entry)
{
	struct file *file;
	struct address_space *mapping;
	pgoff_t index;
	struct folio *folio;

	VM_BUG_ON_VMA(!vma_is_named_swap(vma), vma);

	file = file_for_named_swap_entry(entry);
	if (!file)
		return;

	mapping = file->f_mapping;
	index = linear_page_index(vma, address);

	folio = filemap_get_entry(mapping, index);
	if (!folio)
		return;

	if (!xa_is_value(folio)) {
		atomic_dec(&folio->_mapcount);
		folio_put(folio);
		return;
	}

	named_swap_pagecache_shadow_sub(mapping, index);
}

static struct file *named_swap_lower(struct file *file)
{
	struct named_swap_file *ns = file->private_data;

	if (ns && ns->lower)
		return ns->lower;
	return file;
}

char *named_swap_file_path(struct file *file, char *buf, int buflen)
{
	struct named_swap_file *ns;

	if (!file)
		return ERR_PTR(-EINVAL);

	ns = file->private_data;
	if (!ns || !ns->lower)
		return file_path(file, buf, buflen);

	return file_path(ns->lower, buf, buflen);
}
EXPORT_SYMBOL_GPL(named_swap_file_path);

#define NAMED_SWAP_ROOT		"/.named_swap"
#define NAMED_SWAP_PATH_LEN	128
/* Indices are stored in the swap PTE offset field (see swp_offset()). */
#define NAMED_SWAP_INDEX_MAX	SWP_OFFSET_MASK

static bool named_swap_enabled;
static DEFINE_MUTEX(named_swap_lock);
static struct inode *named_swap_root_inode;
static atomic64_t named_swap_index_counter = ATOMIC64_INIT(0);

int named_swap_min_vma_size = 256; /* 256 pages = 1MB */
EXPORT_SYMBOL_GPL(named_swap_min_vma_size);

static int named_swap_cache_root_inode(void);

/*
 * Allocate the next named swap file index.  Indices must fit in
 * SWP_OFFSET_MASK so they can be encoded in swap PTE offsets.
 */
static int get_named_swap_file_index(u64 *index)
{
	u64 idx = atomic64_fetch_add(1, &named_swap_index_counter);

	if (unlikely(idx > NAMED_SWAP_INDEX_MAX)) {
		atomic64_dec(&named_swap_index_counter);
		return -EOVERFLOW;
	}

	*index = idx;
	return 0;
}

struct named_swap_readdir {
	struct dir_context ctx;
	char name[NAME_MAX + 1];
	unsigned int type;
	bool found;
};

static struct file *named_swap_file_peek(u64 index)
{
	XA_STATE(xas, &named_swap_files, index);
	struct file *file;

	xa_lock(&named_swap_files);
	file = xas_load(&xas);
	xa_unlock(&named_swap_files);
	return file;
}

static bool named_swap_file_matches(struct file *file, u64 index)
{
	struct named_swap_file *ns;

	if (!file || !mapping_named_swap(file->f_mapping))
		return false;

	ns = file->private_data;
	return ns && ns->index == index;
}

static int named_swap_xa_insert(struct file *file, u64 index)
{
	struct file *stored;
	int err;

	stored = get_file(file);
	mutex_lock(&named_swap_xa_lock);
	err = xa_err(xa_store(&named_swap_files, index, stored, GFP_KERNEL));
	mutex_unlock(&named_swap_xa_lock);
	if (err)
		fput(stored);
	return err;
}

static void named_swap_xa_remove(u64 index)
{
	struct file *file;

	mutex_lock(&named_swap_xa_lock);
	file = xa_erase(&named_swap_files, index);
	mutex_unlock(&named_swap_xa_lock);
	if (file)
		fput(file);
}

int named_swap_file_size(struct vm_area_struct *vma){
	
	if (!vma)
		return -EINVAL;

	if (!vma_is_named_swap(vma))
		return -EINVAL;

	return vma->vm_end - vma->vm_start;
}
EXPORT_SYMBOL(named_swap_file_size);

int named_swap_enlarge(struct vm_area_struct *vma, unsigned long delta)
{
	struct anon_vma *anon_vma;
    struct file *file;
    loff_t old_size;

    if (!vma)
        return -EINVAL;

    anon_vma = vma->anon_vma;
    if (!anon_vma)
        return -EINVAL;

    file = anon_vma->named_swap_file;
    if (!file)
        return -EINVAL;

    old_size = named_swap_file_size(vma); 
	
	VM_BUG_ON_VMA(old_size + delta < old_size, vma); //(condition,vma).

    return vfs_fallocate(file, 0, old_size, delta);
}


static void named_swap_xa_destroy(void)
{
	struct file *file;
	unsigned long index;

	mutex_lock(&named_swap_xa_lock);
	xa_for_each(&named_swap_files, index, file) {
		xa_erase(&named_swap_files, index);
		fput(file);
	}
	mutex_unlock(&named_swap_xa_lock);
}

static struct file *file_for_named_swap_entry(swp_entry_t entry)
{
	return named_swap_file_peek(named_swap_entry_index(entry));
}

static pte_t named_swap_pte_for_file(struct file *file)
{
	struct named_swap_file *ns = file->private_data;

	VM_BUG_ON(!ns);
	return make_named_swap_pte(ns->index);
}

void named_swap_store_pte(struct mm_struct *mm, struct vm_area_struct *vma,
			  unsigned long address, pte_t *pte)
{
	VM_BUG_ON_VMA(!vma_is_named_swap(vma), vma);
	set_pte_at(mm, address, pte, named_swap_pte_for_file(vma->vm_file));
}

void setup_named_swap_vmf(struct vm_fault *vmf)
{
	swp_entry_t entry;
	struct file *file;
	u64 index;

	if (!is_named_swap_pte(vmf->orig_pte))
		return;

	entry = pte_to_swp_entry(vmf->orig_pte);
	index = named_swap_entry_index(entry);
	file = vmf->vma->vm_file;
	if (!named_swap_file_matches(file, index))
		file = named_swap_file_peek(index);
	if (!file)
		return;

	vmf->vm_file = file;
	pte_unmap(vmf->pte);
	vmf->pte = NULL;
	vmf->named_swap_alloc = true;
}


static bool named_swap_filldir(struct dir_context *ctx, const char *name,
			       int namelen, loff_t offset, u64 ino,
			       unsigned int type)
{
	struct named_swap_readdir *iter =
		container_of(ctx, struct named_swap_readdir, ctx);

	if (namelen == 1 && name[0] == '.')
		return true;
	if (namelen == 2 && name[0] == '.' && name[1] == '.')
		return true;
	if (namelen > NAME_MAX)
		return true;

	memcpy(iter->name, name, namelen);
	iter->name[namelen] = '\0';
	iter->type = type;
	iter->found = true;
	return false;
}

static int named_swap_first_child(const char *path,
				  struct named_swap_readdir *iter)
{
	struct file *file;
	int ret;

	memset(iter, 0, sizeof(*iter));
	iter->ctx.actor = named_swap_filldir;

	file = filp_open(path, O_RDONLY | O_DIRECTORY | O_LARGEFILE, 0);
	if (IS_ERR(file))
		return PTR_ERR(file);

	ret = iterate_dir(file, &iter->ctx);
	fput(file);
	if (ret)
		return ret;

	return iter->found ? 1 : 0;
}

static int named_swap_remove_path(const char *path, bool dir)
{
	struct filename *filename;
	struct dentry *dentry;
	struct path parent;
	struct qstr last;
	unsigned int lookup_flags = 0;
	int type;
	int ret;

retry:
	filename = getname_kernel(path);
	if (IS_ERR(filename))
		return PTR_ERR(filename);

	ret = vfs_path_parent_lookup(filename, lookup_flags, &parent, &last,
				     &type, NULL);
	if (ret)
		goto out_name;
	if (type != LAST_NORM) {
		ret = -EINVAL;
		goto out_path;
	}

	ret = mnt_want_write(parent.mnt);
	if (ret)
		goto out_path;

	inode_lock_nested(d_inode(parent.dentry), I_MUTEX_PARENT);
	dentry = lookup_one_qstr_excl(&last, parent.dentry, lookup_flags);
	if (IS_ERR(dentry)) {
		ret = PTR_ERR(dentry);
		goto out_unlock;
	}

	if (d_is_negative(dentry)) {
		ret = -ENOENT;
		goto out_dput;
	}

	if (dir)
		ret = vfs_rmdir(mnt_idmap(parent.mnt), d_inode(parent.dentry),
				dentry);
	else
		ret = vfs_unlink(mnt_idmap(parent.mnt), d_inode(parent.dentry),
				 dentry, NULL);

out_dput:
	dput(dentry);
out_unlock:
	inode_unlock(d_inode(parent.dentry));
	mnt_drop_write(parent.mnt);
out_path:
	path_put(&parent);
out_name:
	putname(filename);
	if (retry_estale(ret, lookup_flags)) {
		lookup_flags |= LOOKUP_REVAL;
		goto retry;
	}
	return ret == -ENOENT ? 0 : ret;
}

static int named_swap_unlink_file(struct file *file)
{
	struct path *path = &file->f_path;
	struct dentry *dentry;
	struct dentry *parent;
	struct inode *dir;
	int ret;

	dentry = dget(path->dentry);
	if (IS_ROOT(dentry)) {
		ret = -EBUSY;
		goto out_dput;
	}

	ret = mnt_want_write(path->mnt);
	if (ret)
		goto out_dput;

	parent = dget_parent(dentry);
	dir = d_inode(parent);
	inode_lock_nested(dir, I_MUTEX_PARENT);
	if (dentry->d_parent != parent || d_is_negative(dentry))
		ret = -ENOENT;
	else
		ret = vfs_unlink(mnt_idmap(path->mnt), dir, dentry, NULL);
	inode_unlock(dir);
	dput(parent);
	mnt_drop_write(path->mnt);

out_dput:
	dput(dentry);
	return ret == -ENOENT ? 0 : ret;
}

static loff_t named_swap_llseek(struct file *file, loff_t offset, int whence)
{
	return vfs_llseek(named_swap_lower(file), offset, whence);
}

static ssize_t named_swap_read_iter(struct kiocb *iocb, struct iov_iter *iter)
{
	struct file *file = iocb->ki_filp;
	struct file *lower = named_swap_lower(file);
	const struct file_operations *fops = lower->f_op;
	ssize_t ret;

	if (!fops->read_iter)
		return -EINVAL;

	iocb->ki_filp = lower;
	ret = fops->read_iter(iocb, iter);
	iocb->ki_filp = file;
	return ret;
}

static ssize_t named_swap_write_iter(struct kiocb *iocb, struct iov_iter *iter)
{
	struct file *file = iocb->ki_filp;
	struct file *lower = named_swap_lower(file);
	const struct file_operations *fops = lower->f_op;
	ssize_t ret;

	if (!fops->write_iter)
		return -EINVAL;

	iocb->ki_filp = lower;
	ret = fops->write_iter(iocb, iter);
	iocb->ki_filp = file;
	return ret;
}

static int named_swap_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct file *lower = named_swap_lower(file);

	if (!lower->f_op->mmap)
		return -ENODEV;

	return lower->f_op->mmap(lower, vma);
}

static int named_swap_fsync(struct file *file, loff_t start, loff_t end,
			    int datasync)
{
	struct file *lower = named_swap_lower(file);

	if (!lower->f_op->fsync)
		return -EINVAL;

	return lower->f_op->fsync(lower, start, end, datasync);
}

static int named_swap_release(struct inode *inode, struct file *file)
{
	struct named_swap_file *ns = file->private_data;
	struct file *lower;

	if (!ns)
		return 0;

	lower = ns->lower;
	if (lower) {
		/*
		 * ns->lower should be the sole reference (from filp_open).
		 * Extra refs mean a refcount bug in named_swap teardown.
		 */
		WARN_ON(file_count(lower) > 1);
		atomic_long_sub(ns->nr_pages, &named_swap_pages);
		named_swap_xa_remove(ns->index);
		spin_lock(&ns->bind_lock);
		lower->f_mapping->anon_vma = NULL;
		spin_unlock(&ns->bind_lock);
		named_swap_unlink_file(lower);
		fput(lower);
		ns->lower = NULL;
	}

	kfree(ns);
	file->private_data = NULL;
	return 0;
}

static long named_swap_fallocate(struct file *file, int mode, loff_t offset, loff_t len)
{
	long ret = vfs_fallocate(named_swap_lower(file), mode, offset, len);
	if (ret) {
		printk(KERN_ERR "named_swap_fallocate: vfs_fallocate failed: file=%px mode=%d offset=%llu len=%llu ret=%ld\n", file, mode, offset, len, ret);
	}
	printk(KERN_INFO "named_swap_fallocate: file=%px mode=%d offset=%llu len=%llu ret=%ld\n", file, mode, offset, len, ret);
	file->f_inode->i_size = named_swap_lower(file)->f_inode->i_size; // update size of wrapper file to match lower file
	return ret;
}

static const struct file_operations named_swap_fops = {
	.llseek		= named_swap_llseek,
	.read_iter	= named_swap_read_iter,
	.write_iter	= named_swap_write_iter,
	.mmap		= named_swap_mmap,
	.fallocate	= named_swap_fallocate,
	.fsync		= named_swap_fsync,
	.release	= named_swap_release,
};

static int named_swap_wipe_dir(const char *path)
{
	struct named_swap_readdir iter;
	char child[NAMED_SWAP_PATH_LEN];
	struct path child_path;
	bool is_dir;
	int ret;

	while ((ret = named_swap_first_child(path, &iter)) > 0) {
		ret = scnprintf(child, sizeof(child), "%s/%s", path, iter.name);
		if (ret >= sizeof(child))
			return -ENAMETOOLONG;

		ret = kern_path(child, 0, &child_path);
		if (ret)
			return ret == -ENOENT ? 0 : ret;
		is_dir = d_is_dir(child_path.dentry);
		path_put(&child_path);

		if (is_dir) {
			ret = named_swap_wipe_dir(child);
			if (ret)
				return ret;
		}

		ret = named_swap_remove_path(child, is_dir);
		if (ret)
			return ret;
	}

	return ret;
}

static int named_swap_mkdir(const char *path, umode_t mode)
{
	struct dentry *dentry;
	struct path parent;
	int ret;

	ret = kern_path(path, LOOKUP_DIRECTORY, &parent);
	if (!ret) {
		path_put(&parent);
		return 0;
	}
	if (ret != -ENOENT)
		return ret;

	dentry = kern_path_create(AT_FDCWD, path, &parent, LOOKUP_DIRECTORY);
	if (IS_ERR(dentry))
		return PTR_ERR(dentry);

	ret = vfs_mkdir(mnt_idmap(parent.mnt), d_inode(parent.dentry),
			dentry, mode);
	done_path_create(&parent, dentry);
	return ret == -EEXIST ? 0 : ret;
}

static int named_swap_cache_root_inode(void)
{
	struct path path;
	struct inode *inode;
	int ret;

	ret = kern_path(NAMED_SWAP_ROOT, LOOKUP_DIRECTORY, &path);
	if (ret)
		return ret;

	inode = d_inode(path.dentry);
	ihold(inode);
	path_put(&path);

	if (named_swap_root_inode)
		iput(named_swap_root_inode);
	named_swap_root_inode = inode;
	return 0;
}

static int named_swap_enable_locked(void)
{
	int ret;

	WRITE_ONCE(named_swap_enabled, false);
	if (named_swap_root_inode) {
		iput(named_swap_root_inode);
		named_swap_root_inode = NULL;
	}
	atomic64_set(&named_swap_index_counter, 0);
	named_swap_xa_destroy();

	ret = named_swap_wipe_dir(NAMED_SWAP_ROOT);
	if (ret == -ENOENT)
		ret = 0;
	if (ret)
		return ret;

	ret = named_swap_mkdir(NAMED_SWAP_ROOT, 0700);
	if (ret)
		return ret;

	ret = named_swap_cache_root_inode();
	if (ret)
		return ret;

	WRITE_ONCE(named_swap_enabled, true);
	return 0;
}

static int named_swap_ensure_enabled(void)
{
	int ret;

	if (likely(READ_ONCE(named_swap_enabled)))
		return 0;

	mutex_lock(&named_swap_lock);
	ret = READ_ONCE(named_swap_enabled) ? 0 : named_swap_enable_locked();
	mutex_unlock(&named_swap_lock);
	return ret;
}

static int named_swap_build_path(u64 index, char *path, size_t len)
{
	int ret;

	ret = scnprintf(path, len, NAMED_SWAP_ROOT "/%llu", index);
	if (ret >= len)
		return -ENAMETOOLONG;

	return 0;
}

static struct file *named_swap_create_file(unsigned long len)
{
	char *path;
	struct file *lower;
	struct file *file;
	struct inode *inode;
	struct named_swap_file *ns;
	u64 index;
	int ret;

	ret = named_swap_ensure_enabled();
	if (ret) {
		printk(KERN_ERR "named_swap_create_file: failed to ensure enabled");
		return ERR_PTR(ret);
	}

	ret = get_named_swap_file_index(&index);
	if (ret) {
		printk(KERN_ERR "named_swap_create_file: index overflow (> %lu)\n",
		       NAMED_SWAP_INDEX_MAX);
		return ERR_PTR(ret);
	}

	path = kmalloc(NAMED_SWAP_PATH_LEN, GFP_KERNEL);
	if (!path) {
		printk(KERN_ERR "named_swap_create_file: failed to allocate path");
		return ERR_PTR(-ENOMEM);
	}

	ret = named_swap_build_path(index, path, NAMED_SWAP_PATH_LEN);
	if (ret) {
		printk(KERN_ERR "named_swap_create_file: failed to build path");
		file = ERR_PTR(ret);
		goto out;
	}

	lower = filp_open(path, O_CREAT | O_EXCL | O_RDWR | O_LARGEFILE, 0600);
	if (IS_ERR(lower)) {
		long err = PTR_ERR(lower);

		printk(KERN_ERR
		       "named_swap_create_file: filp_open failed: path=%s err=%ld flags=0%o pid=%d comm=%s len=%lu file_index=%llu\n",
		       path, err, O_CREAT | O_EXCL | O_RDWR | O_LARGEFILE,
		       current->pid, current->comm,
		       len, index);
		file = lower;
		goto out;
	}

	ns = kzalloc(sizeof(*ns), GFP_KERNEL);
	if (!ns) {
		fput(lower);
		file = ERR_PTR(-ENOMEM);
		goto out;
	}
	spin_lock_init(&ns->bind_lock);
	ns->lower = lower;
	ns->index = index;

	file = anon_inode_create_getfile("[named_swap]", &named_swap_fops, ns,
				  O_RDWR | O_LARGEFILE, NULL);
	if (IS_ERR(file)) {
		fput(lower);
		kfree(ns);
		goto out;
	}
	inode = file_inode(file);
	inode->i_size = 0;
	inode->i_mode |= S_IFREG;


	/*
	 * Share the backing file's page cache; named_swap flags live on that
	 * mapping so vma_is_named_swap() and folio paths stay consistent.
	 */
	file->f_mapping = lower->f_mapping;
	mapping_set_named_swap(lower->f_mapping);

	ret = vfs_fallocate(file, FALLOC_FL_ZERO_RANGE, 0, len);
	if (ret) {
		printk(KERN_ERR "named_swap_create_file: vfs_fallocate failed: file=%px len=%lu ret=%d\n", file, len, ret);
		fput(file);
		file = ERR_PTR(ret);
		goto out;
	}
	ns->nr_pages = len >> PAGE_SHIFT;
	atomic_long_add(ns->nr_pages, &named_swap_pages);
	printk(KERN_INFO "named_swap_create_file: vfs_fallocate: file=%px len=%lu file_index=%llu\n", file, len, index);
	lower->f_mapping->anon_vma = NULL;

	ret = named_swap_xa_insert(file, index);
	if (ret) {
		printk(KERN_ERR "named_swap_create_file: xa_insert failed: index=%llu ret=%d\n",
		       index, ret);
		fput(file);
		file = ERR_PTR(ret);
		goto out;
	}
out:
	kfree(path);
	return file;
}

struct file *named_swap_prepare_mmap(unsigned long len, unsigned long *flag) {

	struct file *file = named_swap_create_file(len);
	
	if (IS_ERR(file)){
		printk(KERN_ERR "named_swap_prepare_mmap: failed to create file (falling back to anon memory): file=%px len=%lu\n", file, len);
		return NULL;
	}
	printk(KERN_INFO "named_swap_prepare_mmap: created file: file=%px len=%lu pid=%d comm=%s\n", file, len, current->pid, current->comm);
	if (flag)
		*flag |= MAP_NAMED_SWAP;
	return file;

}

/*
 * Link vma's anon_vma to its named swap backing file.  Per-vma lock lives
 * in named_swap_file (wrapper private_data).  Safe in page faults.
 * Call after anon_vma_prepare(); may take get_file() on first link.
 */
void named_swap_link(struct vm_area_struct *vma)
{
	struct file *file = vma->vm_file;
	struct named_swap_file *ns = file->private_data;
	struct anon_vma *anon_vma = vma->anon_vma;
	struct address_space *mapping = file->f_mapping;

	VM_BUG_ON_VMA(!anon_vma, vma);
	VM_BUG_ON(!ns || !mapping_named_swap(mapping));

	spin_lock(&ns->bind_lock);
	if (mapping->anon_vma){
		if(mapping->anon_vma != anon_vma)
			VM_BUG_ON_VMA(mapping->anon_vma != anon_vma, vma);}
	else
		mapping->anon_vma = anon_vma;
	if (!anon_vma->named_swap_file) {
		get_file(file);
		anon_vma->named_swap_file = file;
	}
	spin_unlock(&ns->bind_lock);
	printk(KERN_INFO "named_swap_link: file=%px vma=%px anon_vma=%px mapping=%px\n", file, vma, anon_vma, mapping);
}

/*
 * Break the link when anon_vma is torn down.  fput() is outside the lock.
 */
void named_swap_unlink(struct anon_vma *anon_vma)
{
	struct file *file;
	struct named_swap_file *ns;
	struct address_space *mapping;

	file = anon_vma->named_swap_file;
	if (!file)
		return;

	ns = file->private_data;
	if (WARN_ON(!ns))
		return;

	spin_lock(&ns->bind_lock);
	file = anon_vma->named_swap_file;
	if (file) {
		u64 index = ns->index;

		mapping = file->f_mapping;
		if (mapping->anon_vma == anon_vma)
			mapping->anon_vma = NULL;
		anon_vma->named_swap_file = NULL;
		spin_unlock(&ns->bind_lock);
		fput(file);
		/*
		 * xa_insert() stored an extra get_file() ref.  Drop it so
		 * named_swap_release() runs and unlinks the backing file.
		 */
		named_swap_xa_remove(index);
	} else {
		spin_unlock(&ns->bind_lock);
	}
}