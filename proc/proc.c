/*
 * proc - Proc filesystem entry demo
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
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

static struct proc_dir_entry *myproc;

static char mybuf[256];

static int myproc_read(char *buf, char **start, off_t offset,
	int count, int *eof, void *data)
{
	int len;
	printk(KERN_INFO "proc: %s\n", __FUNCTION__);

	printk(KERN_INFO "proc: off=(%lu) count=(%d)\n",
		(unsigned long)offset, count);

	if (count > 255)
		count = 255;

	len = snprintf(buf, count, "%s", mybuf);
	return len;
}

static int myproc_write(struct file *f, const char *buf,
	unsigned long count, void *data)
{
	printk(KERN_INFO "proc: %s\n", __FUNCTION__);

	printk(KERN_INFO "proc: count=(%lu)\n", count);

	if (count > 255)
		count = 255;
	copy_from_user(mybuf, buf, count);
	mybuf[count] = '\0';
	return count;
}

static int __init proc_init(void)
{
	printk(KERN_INFO "proc: %s\n", __FUNCTION__);

	myproc = create_proc_entry("myproc", 0666, 0);
	if (!myproc) {
		printk(KERN_ERR "proc: error creating proc entry\n");
	}
	myproc->read_proc = myproc_read;
	myproc->write_proc = myproc_write;

	return 0;
}

static void __exit proc_exit(void)
{
	printk(KERN_INFO "proc: %s\n", __FUNCTION__);
	if (myproc) {
		remove_proc_entry("myproc", NULL);
	}
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

