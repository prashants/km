/*
 * miscdev - miscellaneous device demo
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

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/blkdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/buffer_head.h>
#include <linux/miscdevice.h>

static ssize_t miscdev_read(struct file *filp, char __user *buf,
		size_t count, loff_t *offp)
{
	return -EPERM;
}

static ssize_t miscdev_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *offp)
{
	return -EPERM;
}

static int miscdev_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int miscdev_rel(struct inode *inode, struct file *filp)
{
	return 0;
}

static long miscdev_ioctl(struct file *f,
		unsigned int cmd, unsigned long arg)
{
	return -ENOTTY;
}

static struct file_operations miscdev_fops = {
	.owner		= THIS_MODULE,
	.open		= miscdev_open,
	.release	= miscdev_rel,
	.read		= miscdev_read,
	.write		= miscdev_write,
	.unlocked_ioctl	= miscdev_ioctl,
};

static struct miscdevice miscdev_dev = {
	.name		= "prashants",
	.nodename	= "prashants/miscdev",
	.fops		= &miscdev_fops,
};

static int __init miscdev_init(void)
{
	int ret;
	printk(KERN_INFO "miscdev: %s\n", __FUNCTION__);

	ret = misc_register(&miscdev_dev);
	if (ret) {
		printk(KERN_ERR "miscdev: failed to register misc device\n");
		return ret;
	}
	printk(KERN_ERR "miscdev: registered misc device\n");

	return 0;
}

static void __exit miscdev_exit(void)
{
	printk(KERN_INFO "miscdev: %s\n", __FUNCTION__);

	misc_deregister(&miscdev_dev);
}

module_init(miscdev_init);
module_exit(miscdev_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

