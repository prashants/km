/*
 * mmap - Demo of linux kernel - userspace communication using mmap call
 * of a character device
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

static unsigned char **kmalloc_ptr = NULL;
unsigned long kmalloc_size = PAGE_SIZE * 50;	/* size in MB */
unsigned long kmalloc_maxindex = 0;

/* Forward declarations */
static struct vm_operations_struct mmap_vm_ops;
static void mmap_vma_open(struct vm_area_struct *vma);

/*
 * On reading the device file it will print all the
 * page addresses for the kmalloc memory area in the
 * kernel log
 */
static ssize_t mmap_read(struct file *filp, char __user *buff,
		size_t count, loff_t *offp)
{
	int c;
	struct page *p;
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

	/* print all page frame number in kernel log */
	printk(KERN_INFO "kmalloc: kmalloc_ptr=%p &kmalloc_ptr=%p\n", kmalloc_ptr, &kmalloc_ptr);
	for (c = 0; c < kmalloc_maxindex; c++) {
		p = virt_to_page(kmalloc_ptr[c]);
		printk(KERN_INFO "kmalloc: phys=%llu page=%p ptr=%p &ptr=%p\n",
				page_to_phys(p), p, kmalloc_ptr[c], &kmalloc_ptr[c]);
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
	struct page *p;
	int c;
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

        length = vma->vm_end - vma->vm_start;

	/* check the size of shared memory requested */
	if (length > kmalloc_size) {
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
	c = 0;
	start = vma->vm_start;
	while (length > 0) {
		if (c > (kmalloc_maxindex - 1)) {
			printk(KERN_NOTICE "mmap: failed to map out of range memory at %lu\n", length);
			return -EINVAL;
		}
		p = virt_to_page(kmalloc_ptr[c]);
		/* remap each kernel pages to the vma address */
		if (remap_pfn_range(vma,
				start,
				page_to_pfn(p),
				PAGE_SIZE,
				vma->vm_page_prot)) {
			printk(KERN_NOTICE "mmap: failed to map memory at %lu\n", length);
			return -EAGAIN;
		}
		c++;
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
		*(kmalloc_ptr[0]),
		*(kmalloc_ptr[kmalloc_maxindex - 1]));
}

static void mmap_vma_close(struct vm_area_struct *vma)
{
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

	printk(KERN_INFO "mmap: start=%c end=%c\n",
		*(kmalloc_ptr[0]),
		*(kmalloc_ptr[kmalloc_maxindex - 1]));
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
	int c1, c2, ret;
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

	/* allocate memory for array of pointers */
	kmalloc_maxindex = kmalloc_size / PAGE_SIZE;
	kmalloc_ptr = (unsigned char **)kmalloc(kmalloc_maxindex * sizeof(unsigned char *), GFP_KERNEL);
	if (kmalloc_ptr) {
		printk(KERN_INFO "mmap: allocated memory for holding %lu pointers of %lu size\n",
				kmalloc_maxindex, PAGE_SIZE);
	} else {
		printk(KERN_ERR "mmap: failed to kmalloc memory of %lu bytes\n",
				kmalloc_size);
		ret = -ENOMEM;
		goto err1;
	}

	/* allocate memory blocks and storing their pointer in the above array */
	for (c1 = 0; c1 < kmalloc_maxindex; c1++) {
		kmalloc_ptr[c1] = (unsigned char *)kmalloc(PAGE_SIZE, GFP_KERNEL);
		if (!kmalloc_ptr[c1]) {
			printk(KERN_ERR "mmap: failed to kmalloc memory for %d block\n", c1);
			ret = -ENOMEM;
			goto err2;
		}
		/* initializing memory with 'A' */
		memset(kmalloc_ptr[c1], 'A', PAGE_SIZE);
	}

	/* add character device */
	ret = mmap_add_cdev();
	if (ret != 0) {
		goto err2;
	}

	return 0;
err2:
	for (c2 = 0; c2 < c1; c2++) {
		kfree(kmalloc_ptr[c2]);
	}
err1:
	kfree(kmalloc_ptr);
	return ret;
}

static void __exit mmap_exit(void)
{
	int c;
	printk(KERN_INFO "mmap: %s\n", __FUNCTION__);

	/* delete character device */
	mmap_del_cdev();

	/* free shared memory */
	for (c = 0; c < kmalloc_maxindex; c++) {
		if (kmalloc_ptr[c])
			kfree(kmalloc_ptr[c]);
	}

	if (kmalloc_ptr) {
		kfree(kmalloc_ptr);
	}
}

module_init(mmap_init);
module_exit(mmap_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

