/*
* Copyright (C) 2012 Prashant Shah, pshah.mumbai@gmail.com
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/blkdev.h>
#include <linux/ioctl.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/timer.h>
#include <linux/types.h>	/* size_t */
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/hdreg.h>	/* HDIO_GETGEO */
#include <linux/kdev_t.h>
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>	/* invalidate_bdev */
#include <linux/bio.h>

#define func()	printk(KERN_INFO "ndas: %s\n", __func__)

#define NDAS_MINORS	16

#define HARDSECT_SIZE		512
#define KERNEL_SECTOR_SIZE	512
#define NSECTOR			1024

struct ndas_dev {
	spinlock_t lock;
	struct request_queue *queue;
	struct gendisk *gd;
};

int major_number = 0;

struct ndas_dev *Device;

static void ndas_request(struct request_queue *q)
{
	struct request *req;

	func();

	while ((req = blk_fetch_request(q)) != NULL) {
		printk(KERN_INFO "request received\n");
		__blk_end_request_all(req, 0);
	}
}

static int ndas_open(struct block_device *blkdev, fmode_t fmode)
{
	func();

	return 0;
}

static int ndas_release(struct gendisk *blkdev, fmode_t fmode)
{
	func();

	return 0;
}

static int ndas_ioctl(struct block_device *blkdev, fmode_t fmode,
		unsigned int cmd, unsigned long arg)
{
	func();

	return 0;
}

static struct block_device_operations blk_ops = {
	.owner  = THIS_MODULE,
	.open = ndas_open,
	.release = ndas_release,
	.ioctl = ndas_ioctl,
};

static int __init ndas_init(void)
{
	int retval = 0;
	struct ndas_dev *dev;

	func();

	retval = register_blkdev(0, "myndas");
	if (retval <= 0) {
		printk(KERN_ERR "ndas: failed to register device\n");
		return retval;
	} else {
		major_number = retval;
		printk(KERN_INFO "ndas: register device major number %d\n", major_number);
	}

	/* init block device */
	dev = kmalloc(sizeof(struct ndas_dev), GFP_KERNEL);
	if (dev == NULL) {
		printk(KERN_ERR "ndas: failed to allocate memory for device\n");
		goto err1;
	}
	memset(dev, sizeof(struct ndas_dev), 0);
	spin_lock_init(&dev->lock);
	Device = dev;

	/* init queue */
	dev->queue = blk_init_queue(ndas_request, &dev->lock);
	if (dev->queue == NULL) {
		printk(KERN_ERR "ndas: failed to allocate memory for queue\n");
		goto err2;
	}
	blk_queue_logical_block_size(dev->queue, HARDSECT_SIZE);
	dev->queue->queuedata = dev;

	/* gendisk structure */
	dev->gd = alloc_disk(NDAS_MINORS);
	if (dev->gd == NULL) {
		printk(KERN_ERR "ndas: failed to allocate memory for gendisk\n");
		goto err3;
	}
	dev->gd->major = major_number;
	dev->gd->first_minor = 0;
	dev->gd->fops = &blk_ops;
	dev->gd->queue = dev->queue;
	dev->gd->private_data = dev;
	set_capacity(dev->gd, NSECTOR * (HARDSECT_SIZE / KERNEL_SECTOR_SIZE));
	snprintf(dev->gd->disk_name, 6, "myndas");
	add_disk(dev->gd);
	return 0;
err3:
	blk_cleanup_queue(dev->queue);
err2:
	kfree(dev);
err1:
	Device = NULL;
	unregister_blkdev(major_number, "ndas");
	return -ENOMEM;
}

static void __exit ndas_exit(void)
{
	struct ndas_dev *dev;

	func();

	dev = Device;
	if (dev) {
		if (dev->gd) {
			del_gendisk(dev->gd);
			put_disk(dev->gd);
		}
		if (dev->queue) {
			blk_cleanup_queue(dev->queue);
		}
		kfree(dev);
	}
	if (major_number > 0)
		unregister_blkdev(major_number, "myndas");
}

module_init(ndas_init);
module_exit(ndas_exit);

MODULE_AUTHOR("Prashant Shah <pshah.mumbai@gmail.com>");
MODULE_DESCRIPTION("NDAS Driver");
MODULE_LICENSE("GPL v2");

