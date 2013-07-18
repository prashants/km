/*
 * memblock - Memory block device
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
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/bio.h>
#include <linux/blkdev.h>
#include <linux/types.h>
#include <linux/hdreg.h>
#include <linux/major.h>
#include <linux/file.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/spinlock.h>

#define KERNEL_SECTOR_SIZE 512
int hardsect_size = 512;

#define DEBUG(fmt, args...) \
	printk(KERN_ERR "memblock: %s %d " fmt "\n", __FUNCTION__ , __LINE__ , ##args);

int major = 0;
int minors = 16;
struct gendisk *gd;

struct memblock_dev {
	int size;
	int nsectors;
	u8 *data;
	spinlock_t lock;
	int users;
	struct request_queue *queue;
	struct gendisk *gd;
};

struct memblock_dev *d;

static void memblock_req(request_queue_t *q)
{
	struct request *req;
	DEBUG();

	while ((req = elv_next_request(q)) != NULL) {
		//struct memblock_dev *dev = req->rq_disk->private_data;
		if (!blk_fs_request(req)) {
			printk(KERN_NOTICE "Skip non-fs request\n");
			end_request(req, 0);
			continue;
		}
		printk(KERN_NOTICE "Processed request\n");
		end_request(req, 1);
	}
}

static int memblock_open(struct inode *inode, struct file *filp)
{
	struct memblock_dev *dev = inode->i_bdev->bd_disk->private_data;
	DEBUG();

	filp->private_data = dev;
	spin_lock(&dev->lock);
	dev->users++;
	spin_unlock(&dev->lock);
	return 0;
}

static int memblock_release(struct inode *inode, struct file *filp)
{
	struct memblock_dev *dev = inode->i_bdev->bd_disk->private_data;
	DEBUG();

	spin_lock(&dev->lock);
	dev->users--;
	spin_unlock(&dev->lock);
	return 0;
}

static int memblock_revalidate(struct gendisk *gd)
{
	struct memblock_dev *dev = gd->private_data;
	DEBUG();

	memset(dev->data, 0, dev->size);
	return 0;
}

static int memblock_ioctl(struct inode *inode, struct file *filp,
		unsigned int cmd, unsigned long arg)
{
	long size;
	struct hd_geometry geo;
	struct memblock_dev *dev = filp->private_data;
	DEBUG();

	switch (cmd) {
		case HDIO_GETGEO:
			size = dev->size * (hardsect_size/KERNEL_SECTOR_SIZE);
			geo.cylinders = (size & ~0x3F) >> 6;
			geo.heads = 4;
			geo.sectors = 16;
			geo.start = 4;
			if (copy_to_user((void __user *)arg, &geo, sizeof(geo)))
				return -EFAULT;
			return 0;
	}

	return -ENOTTY;
}

struct block_device_operations memblock_fops = {
	.owner			= THIS_MODULE,
	.open			= memblock_open,
	.release		= memblock_release,
	.ioctl			= memblock_ioctl,
	.revalidate_disk	= memblock_revalidate,
};

static int __init memblock_init(void)
{
	int ret;
	DEBUG();

	d = (struct memblock_dev *)kmalloc(sizeof(struct memblock_dev), GFP_KERNEL);

	ret = register_blkdev(major, "memblock");
	if (ret < 0) {
		printk(KERN_ERR "error registering device\n");
		goto err1;
	}
	major = ret;

	d->users = 0;
	d->nsectors = 1024 * 1024;
	d->size = d->nsectors * hardsect_size;
	d->data = vmalloc(d->size);
	if (!d->data) {
		printk(KERN_ERR "failed vmalloc\n");
		goto err2;
	}

	spin_lock_init(&d->lock);
	d->queue = blk_init_queue(memblock_req, &d->lock);

	d->gd = alloc_disk(minors);
	if (!d->gd) {
		printk(KERN_ERR "alloc_disk failed\n");
		goto err3;
	}

	d->gd->major = major;
	d->gd->first_minor = minors;
	d->gd->fops = &memblock_fops;
	d->gd->queue = d->queue;
	d->gd->private_data = d;
	snprintf(d->gd->disk_name, 32, "memblock");
	set_capacity(d->gd, d->nsectors * (hardsect_size/KERNEL_SECTOR_SIZE));

	add_disk(d->gd);

	return 0;

err3:
	unregister_blkdev(major, "memblock");
err2:
	vfree(d->data);
err1:
	kfree(d);
	return -ENOMEM;
}

static void __exit memblock_exit(void)
{
	DEBUG();

	if (d->gd) {
		del_gendisk(d->gd);
		put_disk(d->gd);
	}

	if (d->queue) {
		blk_cleanup_queue(d->queue);
	}

	if (d->data) {
		vfree(d->data);
	}

	if (major > 0) {
		unregister_blkdev(major, "memblock");
	}

	if (d) {
		kfree(d);
	}
}

module_init(memblock_init);
module_exit(memblock_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

