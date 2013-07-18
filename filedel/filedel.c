/*
 * filedel - File delete from kernel module 
 *
 * Written in 2013 by Prashant P Shah <pshah.mumbai@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fcntl.h>
#include <linux/syscalls.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#define FILE_NAME "/test.txt"

static int __init filedel_init(void)
{
	int ret;
	struct dentry *d;
	struct file *f;
	mm_segment_t orig_fs;
	printk(KERN_DEBUG "filedel: %s\n", __FUNCTION__);

	/* Change address space mappings so it can accept kernel addresses */
	orig_fs = get_fs();
	set_fs(KERNEL_DS);

	f = filp_open(FILE_NAME, O_RDWR, 0);
	if (IS_ERR(f)) {
		printk(KERN_DEBUG "filedel: error opening file\n");
	} else {
		d = f->f_dentry;
		filp_close(f, 0);
		if (d->d_parent) {
			if (d->d_parent->d_inode) {
				ret = vfs_unlink(d->d_parent->d_inode, d);
				printk(KERN_DEBUG "filedel: return = %d\n", ret);
			}
		}
	}

	set_fs(orig_fs);

	return 0;
}

static void __exit filedel_exit(void)
{
	printk(KERN_DEBUG "filedel: %s\n", __FUNCTION__);
}

module_init(filedel_init);
module_exit(filedel_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

