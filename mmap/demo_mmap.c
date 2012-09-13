/*
 * demo_mmap - Demo of linux kernel - userspace communication using mmap call
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
#include <linux/semaphore.h>
#include <linux/mm.h>
#include <linux/slab.h>

struct demo_mmap_device {
	int valid;
	struct semaphore sem;
	struct cdev cdev;
} demo_mmap_dev;

int demo_mmap_major  = 0;
int demo_mmap_minor  = 1;
int demo_mmap_nr_dev = 1;
int demo_mmap_block_size = 0;
int demo_mmap_part_size = 0;

static void *kmalloc_ptr = NULL;
static unsigned int *kmalloc_addr = NULL; /* page aligned kmalloc_ptr */

unsigned char counter = 0;

#define N_PAGES	1000

/* Forward declarations */
static struct vm_operations_struct demo_mmap_vm_ops;
static void demo_mmap_vma_open(struct vm_area_struct *vma);

static ssize_t demo_mmap_read(struct file *filp, char __user *buff,
		size_t count, loff_t *offp)
{
	printk(KERN_DEBUG "demo_mmap: %s\n", __FUNCTION__);
	return count;
}

static ssize_t demo_mmap_write(struct file *filp, const char __user *buff,
		size_t count, loff_t *offp)
{
	printk(KERN_DEBUG "demo_mmap: %s\n", __FUNCTION__);
	return count;
}

static int demo_mmap_open(struct inode *inode, struct file *filp)
{
	struct demo_mmap_device *dev;
	printk(KERN_DEBUG "demo_mmap: %s\n", __FUNCTION__);

	dev = container_of(inode->i_cdev, struct demo_mmap_device, cdev);
	filp->private_data = dev;
	return 0;
}

static int demo_mmap_release(struct inode *inode, struct file *filp)
{
	printk(KERN_DEBUG "demo_mmap: %s\n", __FUNCTION__);
	return 0;
}

static int demo_mmap_mmap(struct file *filp, struct vm_area_struct *vma)
{
        long length = vma->vm_end - vma->vm_start;
	printk(KERN_DEBUG "demo_mmap: %s\n", __FUNCTION__);

	/* check the size of shared memory requested */
	if (length > N_PAGES * PAGE_SIZE) {
		printk(KERN_NOTICE "demo_mmap: requested memory size exceeded max limits\n");
		return -EIO;
	}

	printk(KERN_NOTICE "demo_mmap: vma: start=%lx, pgoff=%lx end=%lx\n",
			vma->vm_start, vma->vm_pgoff, vma->vm_end);

	vma->vm_flags |= VM_LOCKED;

	/* build page tables to map range of physical memory */
	if (remap_pfn_range(vma, vma->vm_start,
			virt_to_phys((void *)kmalloc_addr) >> PAGE_SHIFT,
			length,
			vma->vm_page_prot)) {
		printk(KERN_NOTICE "demo_mmap: failed to map memory\n");
		return -EAGAIN;
	}

	printk(KERN_NOTICE "demo_mmap: vma virt %lx : virt_to_phys %lu\n",
		vma->vm_start, (unsigned long)(virt_to_phys((void *)kmalloc_addr) >> PAGE_SHIFT));

	vma->vm_ops = &demo_mmap_vm_ops;
	demo_mmap_vma_open(vma);
	return 0;
}

static void demo_mmap_vma_open(struct vm_area_struct *vma)
{
	int c;
	printk(KERN_DEBUG "demo_mmap: %s\n", __FUNCTION__);

	counter = *(unsigned char *)kmalloc_ptr;

	for (c = 0; c < (PAGE_SIZE * N_PAGES); c++) {
		*((unsigned char *)kmalloc_ptr + c) = counter;
	}
}

static void demo_mmap_vma_close(struct vm_area_struct *vma)
{
	printk(KERN_DEBUG "demo_mmap: %s\n", __FUNCTION__);
}

static struct vm_operations_struct demo_mmap_vm_ops = {
	.open	= demo_mmap_vma_open,
	.close	= demo_mmap_vma_close,
};

static struct file_operations demo_mmap_fops = {
	.owner		= THIS_MODULE,
	.read		= demo_mmap_read,
	.write		= demo_mmap_write,
	.open		= demo_mmap_open,
	.release	= demo_mmap_release,
	.mmap		= demo_mmap_mmap,
};

static void demo_mmap_setup_cdev(struct demo_mmap_device *dev, int index)
{
	int err, dev_num;
	printk(KERN_DEBUG "demo_mmap: %s\n", __FUNCTION__);

	dev_num = MKDEV(demo_mmap_major, demo_mmap_minor + index);

	cdev_init(&dev->cdev, &demo_mmap_fops);

	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &demo_mmap_fops;
	dev->valid = 1;
	err = cdev_add(&dev->cdev, dev_num, 1);
	if (err) {
		dev->valid = 0;
		printk(KERN_ERR "demo_mmap: error %d registering char device %d\n", err, index);
	}
}

static int __init demo_mmap_init(void)
{
	int ret;
	dev_t dev_num = 0;
	struct demo_mmap_device *dev = &demo_mmap_dev;	/* local 'dev' pointer to global demo_mmap_dev structure */
	printk(KERN_DEBUG "demo_mmap: %s\n", __FUNCTION__);

	/*
	 * allocate kernel shared memory for communication with userspace using mmap
	 * add 2 extra pages for header and footer, useful if the memory is not
	 * page aligned
	 */
	kmalloc_ptr = kmalloc(PAGE_SIZE * (N_PAGES + 2), GFP_KERNEL);
	if (kmalloc_ptr) {
		printk(KERN_INFO "demo_mmap: kmalloc_ptr %p\n", kmalloc_ptr);
	} else {
		printk(KERN_INFO "demo_mmap: failed to allocate shared memory\n");
		return -EIO;
	}

	/* kmalloc_addr is always page aligned value of kmalloc_ptr */
	if (((unsigned long)kmalloc_ptr % PAGE_SIZE) == 0) {
		kmalloc_addr = (int *)kmalloc_ptr;
	} else {
		kmalloc_addr = (int *)((unsigned long)kmalloc_ptr +
			(PAGE_SIZE - ((unsigned long)kmalloc_ptr % PAGE_SIZE)));
	}

	/* allocating major number for character device */
	if (demo_mmap_major > 0) {
		dev_num = MKDEV(demo_mmap_major, demo_mmap_minor);
		ret = register_chrdev_region(dev_num, demo_mmap_nr_dev, "demo_mmap");
	} else {
		ret = alloc_chrdev_region(&dev_num, demo_mmap_minor, demo_mmap_nr_dev, "demo_mmap");
		demo_mmap_major = MAJOR(dev_num);
	}

	if (ret < 0) {
		demo_mmap_major = 0;
		printk(KERN_ERR "demo_mmap: failed to allocate major number %d\n", demo_mmap_major);
		if (kmalloc_ptr) {
			kfree(kmalloc_ptr);
		}
		return ret;
	}
	printk(KERN_INFO "demo_mmap: allocated major number %d\n", demo_mmap_major);

	/* registering character device */
	sema_init(&dev->sem, 1);
	demo_mmap_setup_cdev(dev, 0);

	printk(KERN_INFO "demo_mmap: kmalloc_ptr %p # kmalloc_addr %p\n", kmalloc_ptr, kmalloc_addr);
	return 0;
}

static void __exit demo_mmap_exit(void)
{
	dev_t dev_num;
	struct demo_mmap_device *dev = &demo_mmap_dev;
	printk(KERN_DEBUG "demo_mmap: %s\n", __FUNCTION__);

	if (demo_mmap_major > 0) {
		if (dev->valid == 1) {
			cdev_del(&dev->cdev);
		}
		dev_num = MKDEV(demo_mmap_major, demo_mmap_minor);
		unregister_chrdev_region(dev_num, demo_mmap_nr_dev);
	}

	/* free shared memory */
	if (kmalloc_ptr) {
		kfree(kmalloc_ptr);
	}

}

module_init(demo_mmap_init);
module_exit(demo_mmap_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

