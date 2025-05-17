#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/fcntl.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/uaccess.h>
#include <linux/swap.h>
#include <linux/proc_fs.h>
#include <linux/kmod.h>
#include <linux/export.h>
#include <linux/wait.h>
#include <linux/uio.h>
#include <linux/falloc.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/rtc.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Daniel Bransky");
MODULE_DESCRIPTION("A module that creates and sets up a swap file");
MODULE_VERSION("0.1");

#define PROC_DIR "vma_swap"
#define PROC_CREATE "create"
#define VMA_SWAPFILE_SIZE ((loff_t)1<<31) /* 2GiB */
static struct proc_dir_entry *proc_dir, *proc_create_entry;

/**
 * create_swap_file - Creates and formats a swap file
 * @path: Path where the swap file should be created
 * @size: Size of the swap file in bytes
 *
 * Returns 0 on success, negative error code on failure
 */
int create_swap_file(char* path, size_t path_size)
{
    struct file *file;
    int ret;
	char* base_path= "/scratch/vma_swaps";
	//use the current time to create a unique file name
    struct timespec64 ts;
    struct rtc_time tm;
    
    // Get current time
    ktime_get_real_ts64(&ts);
    rtc_time64_to_tm(ts.tv_sec, &tm);
	sprintf(path, "%s/vma_swap_%d-%02d-%02d_%02d:%02d:%02d:%02d.swap", base_path, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,ts.tv_nsec);
    printk(KERN_INFO "Creating swap file at %s\n", path);
    file = filp_open(path, O_CREAT | O_WRONLY | O_LARGEFILE, 0600);
    if(IS_ERR(file)) {
        printk(KERN_ERR "Failed to open file: %ld\n", PTR_ERR(file));
        return PTR_ERR(file);
    }
	printk(KERN_INFO "File opened: %p\n", file);
	printk(KERN_INFO "allocating fize of size %ld\n", VMA_SWAPFILE_SIZE);
    ret = vfs_fallocate(file, FALLOC_FL_KEEP_SIZE, 0, VMA_SWAPFILE_SIZE);
	if(ret!=0){
		printk(KERN_INFO "fallocate failed with code %d\n",ret);
        filp_close(file, NULL);
        return ret;
    }
    printk(KERN_INFO "file allocated");
    filp_close(file, NULL);
	printk(KERN_INFO "File closed");
	//now call the mkswap utilty
	char *argv[] = {"/root/my_mkswap.sh", path, NULL};
	char *envp[] = {"PATH=/sbin:/usr/sbin", NULL};

	// Call user mode helper correctly
	ret = call_usermodehelper("/root/my_mkswap.sh", argv, envp, UMH_WAIT_PROC);
	if (ret < 0) {
		printk(KERN_ERR "Failed to execute mkswap: %d\n", ret);
		return -1;
	}

	printk(KERN_INFO "mkswap completed successfully\n");
    return 0;
}
EXPORT_SYMBOL_GPL(create_swap_file);

static ssize_t proc_create_write(struct file *file, const char __user *buffer,
                             size_t count, loff_t *pos)
{
    printk(KERN_INFO "Proc trigger: create_swap_file()\n");
    char path[256];
    create_swap_file(path, sizeof(path));
    return count;
}

static const struct proc_ops proc_create_fops = {
    .proc_write = proc_create_write,
};



/**
 * activate_swap_file - Activates a swap file
 * @path: Path to the swap file
 *
 * Returns 0 on success, negative error code on failure
 */
int activate_swap_file(const char *path)
{
    char *swapon_argv[] = {
        "/sbin/swapon",
        (char *)path,
        NULL
    };
    char *envp[] = {
        "HOME=/",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin",
        NULL
    };
    int ret;

    if (!path)
        return -EINVAL;

    printk(KERN_INFO "SwapfileAPI: Activating swap file at %s\n", path);
    ret = call_usermodehelper(swapon_argv[0], swapon_argv, envp, UMH_WAIT_PROC);
    if (ret) {
        printk(KERN_ERR "SwapfileAPI: Failed to run swapon: %d\n", ret);
        return ret;
    }

    printk(KERN_INFO "SwapfileAPI: Swap file activated successfully\n");
    return 0;
}
EXPORT_SYMBOL_GPL(activate_swap_file);

/**
 * deactivate_swap_file - Deactivates a swap file
 * @path: Path to the swap file
 *
 * Returns 0 on success, negative error code on failure
 */
int deactivate_swap_file(const char *path)
{
    char *swapoff_argv[] = {
        "/sbin/swapoff",
        (char *)path,
        NULL
    };
    char *envp[] = {
        "HOME=/",
        "PATH=/sbin:/bin:/usr/sbin:/usr/bin",
        NULL
    };
    int ret;

    if (!path)
        return -EINVAL;

    printk(KERN_INFO "SwapfileAPI: Deactivating swap file at %s\n", path);
    ret = call_usermodehelper(swapoff_argv[0], swapoff_argv, envp, UMH_WAIT_PROC);
    if (ret) {
        printk(KERN_ERR "SwapfileAPI: Failed to run swapoff: %d\n", ret);
        return ret;
    }

    printk(KERN_INFO "SwapfileAPI: Swap file deactivated successfully\n");
    return 0;
}
EXPORT_SYMBOL_GPL(deactivate_swap_file);

static int __init swapfile_api_init(void)
{
    printk(KERN_INFO "SwapfileAPI: Module loaded\n");
    proc_dir = proc_mkdir(PROC_DIR, NULL);
    if (!proc_dir) {
        printk(KERN_ERR "Failed to create /proc/%s\n", PROC_DIR);
        return -ENOMEM;
    }
    proc_create_entry = proc_create(PROC_CREATE, 0666, proc_dir, &proc_create_fops);
    if (!proc_create_entry) {
        printk(KERN_ERR "Failed to create /proc/%s/%s\n", PROC_DIR, PROC_CREATE);
        return -ENOMEM;
    }
    return 0;
}

static void __exit swapfile_api_exit(void)
{
    printk(KERN_INFO "SwapfileAPI: Module unloaded\n");
    remove_proc_entry(PROC_CREATE, proc_dir);
    remove_proc_entry(PROC_DIR, NULL);
}

module_init(swapfile_api_init);
module_exit(swapfile_api_exit);