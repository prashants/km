/*
 * filerw - File read and write from kernel module 
 *
 * Written in 2012 by Prashant P Shah <pshah.mumbai@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated
 * all copyright and related and neighboring rights to this software
 * to the public domain worldwide. This software is distributed
 * without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication
 * along with this software.
 * If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fcntl.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define FILE_NAME "/etc/passwd"

static int __init filerw_init(void)
{
	struct file *f;
	char data[50];
	loff_t off = 0;
	int ret;
	mm_segment_t orig_fs;
	printk(KERN_DEBUG "filerw: %s\n", __FUNCTION__);

	/* Change address space mappings so it can accept kernel addresses */
	orig_fs = get_fs();
	set_fs(KERNEL_DS);

	f = filp_open(FILE_NAME, O_RDONLY, 0);
	if (IS_ERR(f)) {
		printk(KERN_DEBUG "filerw: error opening file\n");
	} else {
		printk(KERN_DEBUG "filerw: opening file successful\n");
		ret = vfs_read(f, data, sizeof(data), &off);
		data[49] = '\0';
		printk(KERN_DEBUG "filerw: read %d bytes :\n%s\n", ret, data);
		filp_close(f, 0);
	}

	set_fs(orig_fs);
	return 0;
}

static void __exit filerw_exit(void)
{
	printk(KERN_DEBUG "filerw: %s\n", __FUNCTION__);
}

module_init(filerw_init);
module_exit(filerw_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

