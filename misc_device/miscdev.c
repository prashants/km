/*
 * miscdev - miscellaneous device demo
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

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

