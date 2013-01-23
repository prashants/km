/*
 * chardev - character device demo
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

int chardev_major = 0;
char device_name[] = "prashants";
struct class *chardev_class;

struct chardev {
	int minor;
	int status;
	struct cdev cdev;
	struct device *device;
};
struct chardev *d;

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
};

static int __init chardev_init(void)
{
	int ret = 0;
	dev_t dev;

	d = (struct chardev *)kmalloc(sizeof(*d), GFP_KERNEL);
	if (!d) {
		printk(KERN_ERR "chardev: failed to allocate memory\n");
		return -ENOMEM;
	}

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
	printk(KERN_INFO "created partition device %s\n", device_name);

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

	printk("removed partition device\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

