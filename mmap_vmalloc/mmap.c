/*
 * mmap - Demo of linux kernel - userspace communication using mmap call
 * of a character device
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
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/blkdev.h>

int mmap_major  = 0;
int mmap_minor  = 0;
int mmap_count = 1;

struct mmap_device {
	int status;
	struct semaphore sem;
	struct cdev cdev;
} mmap_dev;

static void *vmalloc_ptr = NULL;
unsigned long vmalloc_size = 1024 * 1024 * 500;	/* size in MB */

/* Forward declarations */
static struct vm_operations_struct mmap_vm_ops;
static void mmap_vma_open(struct vm_area_struct *vma);

/*
 * On reading the device file it will print all the
 * page addresses for the vmalloc memory area in the
 * kernel log
 */
static ssize_t mmap_read(struct file *filp, char __user *buff,
		size_t count, loff_t *offp)
{
	unsigned long length;
	unsigned char *vmalloc_tmp;
	struct page *p;
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

	/* print all page frame number in kernel log */
	length = vmalloc_size;
	vmalloc_tmp = (unsigned char *)vmalloc_ptr;
	while (length > 0) {
		p = vmalloc_to_page(vmalloc_tmp);
		vmalloc_tmp += PAGE_SIZE;
		length -= PAGE_SIZE;
		printk(KERN_INFO "vmalloc: p=%llu page=%p ptr=%p length=%lu\n",
				page_to_phys(p), p, vmalloc_tmp, length);
	}

	return 0;
}

static ssize_t mmap_write(struct file *filp, const char __user *buff,
		size_t count, loff_t *offp)
{
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);
	return count;
}

static int mmap_open(struct inode *inode, struct file *filp)
{
	struct mmap_device *d;
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

	d = container_of(inode->i_cdev, struct mmap_device, cdev);
	filp->private_data = d;
	return 0;
}

static int mmap_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);
	return 0;
}

static int mmap_mmap(struct file *filp, struct vm_area_struct *vma)
{
        unsigned long start, length;
	unsigned char *vmalloc_tmp;
	struct page *p;
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

        length = vma->vm_end - vma->vm_start;

	/* check the size of shared memory requested */
	if (length > vmalloc_size) {
		printk(KERN_NOTICE "mmap: requested memory size exceeded max limits\n");
		return -EIO;
	}

	printk(KERN_NOTICE "mmap: vma: start=%lx, pgoff=%lx end=%lx flags=%lx\n",
			vma->vm_start, vma->vm_pgoff, vma->vm_end, vma->vm_flags);

	/* check for correct protection flags */
	if (!(vma->vm_flags & VM_SHARED)) {
		printk(KERN_ERR "mmap: shared memory must be shared to avoid COW\n");
		return -EINVAL;
	}

	/* dont swap out shared memory */
	vma->vm_flags |= VM_LOCKED;

	/* build page tables to map range of pages */
	vmalloc_tmp = (unsigned char *)vmalloc_ptr;
	start = vma->vm_start;
	while (length > 0) {
		p = vmalloc_to_page(vmalloc_tmp);
		/* remap each kernel pages to the vma address */
		if (remap_pfn_range(vma,
				start,
				page_to_pfn(p),
				PAGE_SIZE,
				vma->vm_page_prot)) {
			printk(KERN_NOTICE "mmap: failed to map memory at %lu\n", length);
			return -EAGAIN;
		}
		vmalloc_tmp += PAGE_SIZE;
		start += PAGE_SIZE;
		if (length > PAGE_SIZE)
			length -= PAGE_SIZE;
		else
			length = 0;
	}

	vma->vm_ops = &mmap_vm_ops;
	mmap_vma_open(vma);

	return 0;
}

static void mmap_vma_open(struct vm_area_struct *vma)
{
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

	printk(KERN_INFO "mmap: start=%c end=%c\n",
		*((unsigned char *)vmalloc_ptr),
		*((unsigned char *)vmalloc_ptr + vmalloc_size - 1));
}

static void mmap_vma_close(struct vm_area_struct *vma)
{
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

	printk(KERN_INFO "mmap: start=%c end=%c\n",
		*((unsigned char *)vmalloc_ptr),
		*((unsigned char *)vmalloc_ptr + vmalloc_size - 1));
}

static struct vm_operations_struct mmap_vm_ops = {
	.open	= mmap_vma_open,
	.close	= mmap_vma_close,
};

static struct file_operations mmap_fops = {
	.owner		= THIS_MODULE,
	.read		= mmap_read,
	.write		= mmap_write,
	.open		= mmap_open,
	.release	= mmap_release,
	.mmap		= mmap_mmap,
};

static int mmap_add_cdev(void)
{
	dev_t dev;
	int ret;
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

	mmap_dev.status = 0;

	/* allocating major number for character device */
	if (mmap_major > 0) {
		dev = MKDEV(mmap_major, mmap_minor);
		ret = register_chrdev_region(dev, mmap_count, "mmap");
	} else {
		ret = alloc_chrdev_region(&dev, mmap_minor, mmap_count, "mmap");
		mmap_major = MAJOR(dev);
	}

	if (ret < 0) {
		mmap_major = 0;
		printk(KERN_ERR "mmap: failed to allocate major number %d\n", mmap_major);
		return ret;
	}

	printk(KERN_INFO "mmap: allocated major number %d\n", mmap_major);

	/* registering character device */
	sema_init(&mmap_dev.sem, 1);
	cdev_init(&mmap_dev.cdev, &mmap_fops);
	mmap_dev.cdev.owner = THIS_MODULE;

	ret = cdev_add(&mmap_dev.cdev, dev, mmap_count);
	if (ret) {
		printk(KERN_ERR "mmap: error %d registering char device\n", ret);
		goto err;
	}

	mmap_dev.status = 1;

	return 0;

err:
	mmap_major = 0;
	unregister_chrdev_region(dev, mmap_count);
	return ret;
}

static void mmap_del_cdev(void)
{
	dev_t dev;
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

	if (mmap_dev.status == 1) {
		cdev_del(&mmap_dev.cdev);
	}

	if (mmap_major > 0) {
		dev = MKDEV(mmap_major, mmap_minor);
		unregister_chrdev_region(dev, mmap_count);
	}
}

static int __init mmap_init(void)
{
	int ret;
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

	/* allocate memory using vmalloc() */
	vmalloc_ptr = vmalloc(vmalloc_size);
	if (vmalloc_ptr) {
		printk(KERN_INFO "mmap: vmalloc_ptr=%p allocated %lu bytes\n",
				vmalloc_ptr, vmalloc_size);
	} else {
		printk(KERN_INFO "mmap: failed to vmalloc memory of %lu bytes\n",
				vmalloc_size);
		return -EIO;
	}

	/* initializing memory with 'A' */
	memset(vmalloc_ptr, 'A', vmalloc_size);

	/* add character device */
	ret = mmap_add_cdev();

	return ret;
}

static void __exit mmap_exit(void)
{
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

	/* delete character device */
	mmap_del_cdev();

	/* free shared memory */
	if (vmalloc_ptr) {
		vfree(vmalloc_ptr);
	}
}

module_init(mmap_init);
module_exit(mmap_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

