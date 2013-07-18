/*
 * filerw - File read and write from kernel module 
 *
 * Written in 2012 by Prashant P Shah <pshah.mumbai@gmail.com>
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
		printk(KERN_DEBUG "filerw: inode %lu\n", f->f_path.dentry->d_inode->i_ino);
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

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

