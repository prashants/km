/*
 * chardev - Send signal to user application using fasync
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
#include <linux/timer.h>
#include <linux/signal.h>
#include <asm-generic/siginfo.h>
#include <linux/buffer_head.h>

int chardev_major = 0;
char device_name[] = "userspace";
struct class *chardev_class;

struct timer_list read_timer;
unsigned long pid;

struct chardev {
	int minor;
	int status;
	struct cdev cdev;
	struct device *device;
	struct fasync_struct *fasync;
};
struct chardev *d;

static void timer_func(unsigned long data)
{
	printk(KERN_INFO "chardev: %s\n", __FUNCTION__);

	if (d->fasync) {
		kill_fasync(&d->fasync, SIGIO, POLL_IN);
	}

	/* Reset timer */
	read_timer.expires = jiffies + (HZ * 10);
	add_timer(&read_timer);
}

static int chardev_async(int fd, struct file *filp, int mode)
{
	printk(KERN_INFO "chardev: %s\n", __FUNCTION__);

	return fasync_helper(fd, filp, mode, &d->fasync);
}

static ssize_t chardev_read(struct file *filp, char __user *buf,
		size_t count, loff_t *offp)
{
	return -EPERM;
}

static ssize_t chardev_write(struct file *filp, const char __user *buf,
		size_t count, loff_t *offp)
{
	return -EPERM;
}

static int chardev_open(struct inode *inode, struct file *filp)
{
	struct chardev *d = NULL;

	/* Saving pointer to nucdp_dev in the file private_data pointer */ 
	d = container_of(inode->i_cdev, struct chardev, cdev);
	filp->private_data = d;

	return 0;
}

static int chardev_rel(struct inode *inode, struct file *filp)
{
	return 0;
}

static long chardev_ioctl(struct file *f,
		unsigned int cmd, unsigned long arg)
{
	return -ENOTTY;
}

static struct file_operations chardev_fops = {
	.owner		= THIS_MODULE,
	.open		= chardev_open,
	.release	= chardev_rel,
	.read		= chardev_read,
	.write		= chardev_write,
	.unlocked_ioctl	= chardev_ioctl,
	.fasync		= chardev_async,
};

static int __init chardev_init(void)
{
	int ret = 0;
	dev_t dev;
	printk(KERN_INFO "chardev: %s\n", __FUNCTION__);

	d = (struct chardev *)kmalloc(sizeof(*d), GFP_KERNEL);
	if (!d) {
		printk(KERN_ERR "chardev: failed to allocate memory\n");
		return -ENOMEM;
	}
	d->fasync = NULL;

	d->status = 0;
	d->minor = 0;

	/* Allocating major number for partition device */
	if (chardev_major > 0) {
		dev = MKDEV(chardev_major, d->minor);
		/* Dev, number of devices, name */
		ret = register_chrdev_region(dev, 1, device_name);
	} else {
		/* Dev, minor, number of devices, name */
		ret = alloc_chrdev_region(&dev, d->minor, 1, device_name);
		chardev_major = MAJOR(dev);
	}
	if (ret < 0) {
		printk(KERN_ERR "chardev: failed to allocate major number (%d)\n", ret);
		goto err1;
	}
	printk(KERN_INFO "chardev: allocated major number %d\n", chardev_major);

	/* Creating class device */
	chardev_class = class_create(THIS_MODULE, "chardev");
	if (IS_ERR(chardev_class)) {
		printk(KERN_ERR "chardev: failed to create nucdp class\n");
		chardev_class = NULL;
		ret = -ENOMEM;
		goto err2;
	}
	printk(KERN_INFO "created class device\n");

	/* Creating partition device in /dev */
	/* Class, parent, dev, data, fmt */
	d->device = device_create(chardev_class, NULL, dev, NULL, device_name);
	if (IS_ERR(d->device)) {
		printk(KERN_ERR "chardev: failed to create device %s %d %d\n",
				device_name, MAJOR(dev), MINOR(dev));
		ret = -ENOMEM;
		goto err3;
	}
	printk(KERN_INFO "created char device /dev/%s\n", device_name);

	/* Registering partition device */
	cdev_init(&d->cdev, &chardev_fops);
	d->cdev.owner = THIS_MODULE;

	/* Cdev, dev, count */
	ret = cdev_add(&d->cdev, dev, 1);
	if (ret < 0) {
		d->status = 0;
		printk(KERN_ERR "chardev: failed to register device (%d)\n", ret);
		goto err4;
	}
	printk(KERN_ERR "registered device with minor number %d\n", d->minor);

	d->status = 1;

	/* Setup timer to send data */
	init_timer(&read_timer);
	read_timer.data = (unsigned long)jiffies;
	read_timer.function = timer_func; 
	read_timer.expires = jiffies + (HZ * 10);
	add_timer(&read_timer);

	return 0;

err4:
	device_destroy(chardev_class, dev);
err3:
	class_destroy(chardev_class);
err2:
	/* Dev, number of devices */
	unregister_chrdev_region(dev, 1);
err1:
	if (d) {
		kfree(d);
	}
	return ret;
}

static void __exit chardev_exit(void)
{
	dev_t dev;
	printk(KERN_INFO "chardev: %s\n", __FUNCTION__);

	del_timer(&read_timer);

	/* Removing partition device */
	if (d->status == 1) {
		cdev_del(&d->cdev);
	}

	dev = MKDEV(chardev_major, d->minor);

	if (!IS_ERR(d->device) && d->device) {
		device_destroy(chardev_class, dev);
		d->device = NULL;
	}

	if (chardev_major > 0) {
		/* Dev, number of devices */
		unregister_chrdev_region(dev, 1);
	}

	if (!IS_ERR(chardev_class) && chardev_class) {
		class_destroy(chardev_class);
		chardev_class = NULL;
	}

	printk("removed char device\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

