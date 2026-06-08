/* SPDX-License-Identifier: GPL-2.0 */
#undef TRACE_SYSTEM
#define TRACE_SYSTEM named_swap

#if !defined(_TRACE_NAMED_SWAP_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_NAMED_SWAP_H

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/tracepoint.h>
#include <linux/types.h>

#ifndef _TRACE_NAMED_SWAP_DEFS
#define _TRACE_NAMED_SWAP_DEFS

#define NAMED_SWAP_INDEX_NONE	(~0ULL)

enum named_swap_fault_path {
	NAMED_SWAP_FAULT_MISSING_READ,
	NAMED_SWAP_FAULT_MISSING_WRITE,
	NAMED_SWAP_FAULT_REFAULT,
	NAMED_SWAP_FAULT_WP_ZERO,
	NAMED_SWAP_FAULT_WP_REUSE,
	NAMED_SWAP_FAULT_WP_COW,
	NAMED_SWAP_FAULT_ACCESS,
	NAMED_SWAP_FAULT_SWAP_FALLBACK,
	NAMED_SWAP_FAULT_SIGBUS,
};

#define NAMED_SWAP_FAULT_PATHS					\
	{ NAMED_SWAP_FAULT_MISSING_READ,	"missing_read" },	\
	{ NAMED_SWAP_FAULT_MISSING_WRITE,	"missing_write" },	\
	{ NAMED_SWAP_FAULT_REFAULT,		"refault" },		\
	{ NAMED_SWAP_FAULT_WP_ZERO,		"wp_zero" },		\
	{ NAMED_SWAP_FAULT_WP_REUSE,		"wp_reuse" },		\
	{ NAMED_SWAP_FAULT_WP_COW,		"wp_cow" },		\
	{ NAMED_SWAP_FAULT_ACCESS,		"access" },		\
	{ NAMED_SWAP_FAULT_SWAP_FALLBACK,	"swap_fallback" },	\
	{ NAMED_SWAP_FAULT_SIGBUS,		"sigbus" }

#endif /* _TRACE_NAMED_SWAP_DEFS */

TRACE_EVENT(named_swap_file_create,
	TP_PROTO(struct file *file, u64 index, unsigned long len, int ret),

	TP_ARGS(file, index, len, ret),

	TP_STRUCT__entry(
		__field(pid_t, pid)
		__array(char, comm, TASK_COMM_LEN)
		__field(struct file *, file)
		__field(u64, index)
		__field(unsigned long, len)
		__field(unsigned long, pages)
		__field(int, ret)
	),

	TP_fast_assign(
		__entry->pid = current->pid;
		memcpy(__entry->comm, current->comm, TASK_COMM_LEN);
		__entry->file = file;
		__entry->index = index;
		__entry->len = len;
		__entry->pages = len >> PAGE_SHIFT;
		__entry->ret = ret;
	),

	TP_printk("pid=%d comm=%s file=%p index=%llu len=%lu pages=%lu ret=%d",
		__entry->pid, __entry->comm, __entry->file,
		__entry->index, __entry->len, __entry->pages, __entry->ret)
);

TRACE_EVENT(named_swap_file_release,
	TP_PROTO(struct file *file, u64 index, unsigned long pages),

	TP_ARGS(file, index, pages),

	TP_STRUCT__entry(
		__field(struct file *, file)
		__field(u64, index)
		__field(unsigned long, pages)
	),

	TP_fast_assign(
		__entry->file = file;
		__entry->index = index;
		__entry->pages = pages;
	),

	TP_printk("file=%p index=%llu pages=%lu",
		__entry->file, __entry->index, __entry->pages)
);

TRACE_EVENT(named_swap_link,
	TP_PROTO(struct file *file, struct vm_area_struct *vma,
		 struct anon_vma *anon_vma, u64 index, bool refreshed),

	TP_ARGS(file, vma, anon_vma, index, refreshed),

	TP_STRUCT__entry(
		__field(struct file *, file)
		__field(struct vm_area_struct *, vma)
		__field(struct anon_vma *, anon_vma)
		__field(u64, index)
		__field(bool, refreshed)
	),

	TP_fast_assign(
		__entry->file = file;
		__entry->vma = vma;
		__entry->anon_vma = anon_vma;
		__entry->index = index;
		__entry->refreshed = refreshed;
	),

	TP_printk("file=%p index=%llu vma=%p anon_vma=%p refreshed=%d",
		__entry->file, __entry->index, __entry->vma,
		__entry->anon_vma, __entry->refreshed)
);

TRACE_EVENT(named_swap_fault,
	TP_PROTO(struct vm_area_struct *vma, unsigned long address,
		 unsigned int fault_flags, u64 index,
		 enum named_swap_fault_path path, unsigned int alias_count,
		 int mapcount, vm_fault_t ret),

	TP_ARGS(vma, address, fault_flags, index, path, alias_count, mapcount,
		ret),

	TP_STRUCT__entry(
		__field(pid_t, pid)
		__array(char, comm, TASK_COMM_LEN)
		__field(struct mm_struct *, mm)
		__field(struct vm_area_struct *, vma)
		__field(unsigned long, address)
		__field(pgoff_t, pgoff)
		__field(unsigned int, fault_flags)
		__field(u64, index)
		__field(enum named_swap_fault_path, path)
		__field(unsigned int, alias_count)
		__field(int, mapcount)
		__field(vm_fault_t, ret)
	),

	TP_fast_assign(
		__entry->pid = current->pid;
		memcpy(__entry->comm, current->comm, TASK_COMM_LEN);
		__entry->mm = vma->vm_mm;
		__entry->vma = vma;
		__entry->address = address;
		__entry->pgoff = linear_page_index(vma, address);
		__entry->fault_flags = fault_flags;
		__entry->index = index;
		__entry->path = path;
		__entry->alias_count = alias_count;
		__entry->mapcount = mapcount;
		__entry->ret = ret;
	),

	TP_printk("pid=%d comm=%s mm=%p vma=%p address=%lx pgoff=%lu index=%llu path=%s flags=%s alias_count=%u mapcount=%d ret=%s",
		__entry->pid, __entry->comm, __entry->mm, __entry->vma,
		__entry->address, __entry->pgoff, __entry->index,
		__print_symbolic(__entry->path, NAMED_SWAP_FAULT_PATHS),
		__print_flags(__entry->fault_flags, "|", FAULT_FLAG_TRACE),
		__entry->alias_count, __entry->mapcount,
		__print_flags(__entry->ret, "|", VM_FAULT_RESULT_TRACE))
);

TRACE_EVENT(named_swap_alias_count,
	TP_PROTO(struct vm_area_struct *vma, unsigned long address,
		 u64 index, unsigned int count),

	TP_ARGS(vma, address, index, count),

	TP_STRUCT__entry(
		__field(pid_t, pid)
		__array(char, comm, TASK_COMM_LEN)
		__field(struct mm_struct *, mm)
		__field(struct vm_area_struct *, vma)
		__field(unsigned long, address)
		__field(pgoff_t, pgoff)
		__field(u64, index)
		__field(unsigned int, count)
	),

	TP_fast_assign(
		__entry->pid = current->pid;
		memcpy(__entry->comm, current->comm, TASK_COMM_LEN);
		__entry->mm = vma->vm_mm;
		__entry->vma = vma;
		__entry->address = address;
		__entry->pgoff = linear_page_index(vma, address);
		__entry->index = index;
		__entry->count = count;
	),

	TP_printk("pid=%d comm=%s mm=%p vma=%p address=%lx pgoff=%lu index=%llu count=%u",
		__entry->pid, __entry->comm, __entry->mm, __entry->vma,
		__entry->address, __entry->pgoff, __entry->index,
		__entry->count)
);

TRACE_EVENT(named_swap_filemap_lookup,
	TP_PROTO(struct address_space *mapping, pgoff_t pgoff, u64 index,
		 bool hit, bool uptodate),

	TP_ARGS(mapping, pgoff, index, hit, uptodate),

	TP_STRUCT__entry(
		__field(dev_t, dev)
		__field(unsigned long, ino)
		__field(pgoff_t, pgoff)
		__field(u64, index)
		__field(bool, hit)
		__field(bool, uptodate)
	),

	TP_fast_assign(
		__entry->dev = mapping->host->i_sb ?
			mapping->host->i_sb->s_dev : mapping->host->i_rdev;
		__entry->ino = mapping->host->i_ino;
		__entry->pgoff = pgoff;
		__entry->index = index;
		__entry->hit = hit;
		__entry->uptodate = uptodate;
	),

	TP_printk("dev=%d:%d ino=%lx pgoff=%lu index=%llu hit=%d uptodate=%d",
		MAJOR(__entry->dev), MINOR(__entry->dev), __entry->ino,
		__entry->pgoff, __entry->index, __entry->hit,
		__entry->uptodate)
);

TRACE_EVENT(named_swap_writeback,
	TP_PROTO(struct address_space *mapping, pgoff_t pgoff,
		 unsigned long pfn, unsigned int order, int mapcount,
		 int result),

	TP_ARGS(mapping, pgoff, pfn, order, mapcount, result),

	TP_STRUCT__entry(
		__field(dev_t, dev)
		__field(unsigned long, ino)
		__field(pgoff_t, pgoff)
		__field(unsigned long, pfn)
		__field(unsigned int, order)
		__field(int, mapcount)
		__field(int, result)
	),

	TP_fast_assign(
		__entry->dev = mapping->host->i_sb ?
			mapping->host->i_sb->s_dev : mapping->host->i_rdev;
		__entry->ino = mapping->host->i_ino;
		__entry->pgoff = pgoff;
		__entry->pfn = pfn;
		__entry->order = order;
		__entry->mapcount = mapcount;
		__entry->result = result;
	),

	TP_printk("dev=%d:%d ino=%lx pgoff=%lu pfn=%lx order=%u mapcount=%d result=%d",
		MAJOR(__entry->dev), MINOR(__entry->dev), __entry->ino,
		__entry->pgoff, __entry->pfn, __entry->order,
		__entry->mapcount, __entry->result)
);

#endif /* _TRACE_NAMED_SWAP_H */

/* This part must be outside protection */
#include <trace/define_trace.h>
