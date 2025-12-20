/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Public API for minimal mkswap kernel module
 * Include this for applications that want to use mkswap via ioctl
 */

#ifndef __LINUX_MKSWAP_MINIMAL_H
#define __LINUX_MKSWAP_MINIMAL_H

#include <linux/types.h>

/* Magic number for ioctl */
#define MKSWAP_IOCTL_MAGIC 0xAB

/* Parameter structure for mkswap ioctl */
struct mkswap_params {
	__u32 version;          /* Swap version (should be 1) */
	__u32 pagesize;         /* Page size in bytes */
	__u64 npages;           /* Number of pages */
	__u8 uuid[16];          /* UUID (optional) */
	char volume_name[16];   /* Volume name/label (optional) */
	int fd;                 /* File descriptor of swap file */
};

/* Ioctl command to create swap header
 *
 * Usage:
 *   int fd = open(swap_file, O_RDWR);
 *   struct mkswap_params params = {
 *     .version = 1,
 *     .pagesize = 4096,
 *     .npages = 262144,  // 1GB with 4K pages
 *     .fd = fd
 *   };
 *   
 *   int dev_fd = open("/dev/mkswap", O_RDWR);
 *   ioctl(dev_fd, MKSWAP_IOCTL_CREATE_HEADER, &params);
 *   close(dev_fd);
 *   close(fd);
 */
#define MKSWAP_IOCTL_CREATE_HEADER _IOW(MKSWAP_IOCTL_MAGIC, 1, struct mkswap_params)

#endif /* __LINUX_MKSWAP_MINIMAL_H */
