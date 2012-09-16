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
#include <linux/blkdev.h>

struct demo_mmap_device {
	int valid;
	struct semaphore sem;
	struct cdev cdev;
} demo_mmap_dev;

struct block_device *bd;

int demo_mmap_major  = 0;
int demo_mmap_minor  = 1;
int demo_mmap_nr_dev = 1;

unsigned int demo_mmap_page_size;
unsigned int demo_mmap_block_size = 0;
u64 demo_mmap_start_sector = 0;
u64 demo_mmap_nr_sector = 0;
unsigned short demo_mmap_logical_block_size = 0;
unsigned short demo_mmap_physical_block_size = 0;
u64 demo_mmap_disk_size = 0;
u64 demo_mmap_nr_block = 0;
u64 demo_mmap_bitmap_size;

static void *vmalloc_ptr = NULL;
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
	if (length > (demo_mmap_bitmap_size - demo_mmap_block_size)) { // removing the extra block size allocation that we did when allocating memory
		printk(KERN_NOTICE "demo_mmap: requested memory size exceeded max limits\n");
		return -EIO;
	}

	printk(KERN_NOTICE "demo_mmap: vma: start=%lx, pgoff=%lx end=%lx flags=%lx\n",
			vma->vm_start, vma->vm_pgoff, vma->vm_end, vma->vm_flags);

	/* check for correct protection flags */
	if (!(vma->vm_flags & VM_SHARED)) {
		printk(KERN_ERR "demo_mmap: shared memory must be shared to avoid COW\n");
		return -EINVAL;
	}
	if (vma->vm_flags & VM_WRITE) {
		printk(KERN_ERR "demo_mmap: shared memory cannot be writable\n");
		return -EINVAL;
	}
	vma->vm_flags |= VM_LOCKED; /* dont swap out shared memory */

	printk(KERN_NOTICE "demo_mmap: vma: start=%lx, pgoff=%lx end=%lx\n",
			vma->vm_start, vma->vm_pgoff, vma->vm_end);

	/* build page tables to map range of physical memory */
	/* USE KMALLOC */
	if (remap_pfn_range(vma, vma->vm_start,
			virt_to_phys((void *)kmalloc_addr) >> PAGE_SHIFT,
			length,
			vma->vm_page_prot)) {
		printk(KERN_NOTICE "demo_mmap: failed to map memory\n");
		return -EAGAIN;
	}

	/* USE VMALLOC */


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

	/* setting all the shared memory to the counter value */
	for (c = 0; c < demo_mmap_bitmap_size; c++) {
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
	unsigned int rem;
	u64 temp;
	//unsigned long virt_addr;
	printk(KERN_DEBUG "demo_mmap: %s\n", __FUNCTION__);

	// Read block device and calculate various parameter
	bd = blkdev_get_by_path("/dev/sda5", FMODE_READ, NULL); /* mode in which to open the block device */
	if (!IS_ERR(bd)) {
		printk(KERN_INFO "nucdp: found block device with major number %d\n",
			bd->bd_disk->major);
	}

	demo_mmap_page_size = PAGE_SIZE;
	demo_mmap_block_size = bd->bd_block_size;
	demo_mmap_start_sector = bd->bd_part->start_sect;
	demo_mmap_nr_sector = bd->bd_part->nr_sects;
	demo_mmap_logical_block_size = bdev_logical_block_size(bd);
	demo_mmap_physical_block_size = bdev_physical_block_size(bd);
	// TODO : What to do if demo_mmap_disk_size overflows u64 size
	demo_mmap_disk_size = (demo_mmap_nr_sector * demo_mmap_logical_block_size);
	temp = demo_mmap_disk_size; // For do_div() since it will store the result in the first parameter itself
	do_div(temp, demo_mmap_block_size);
	demo_mmap_nr_block = temp;

	printk(KERN_INFO "demo_mmap: page size %d\n", demo_mmap_page_size);
	printk(KERN_INFO "demo_mmap: fs block size %d\n", demo_mmap_block_size);
	printk(KERN_INFO "demo_mmap: start sector %llu\n", demo_mmap_start_sector);
	printk(KERN_INFO "demo_mmap: number of sectors %llu\n", demo_mmap_nr_sector);
	printk(KERN_INFO "demo_mmap: logical block size %d\n", demo_mmap_logical_block_size);
	printk(KERN_INFO "demo_mmap: physical block size %d\n", demo_mmap_physical_block_size);
	printk(KERN_INFO "demo_mmap: disk size %llu\n", demo_mmap_disk_size);
	printk(KERN_INFO "demo_mmap: number of fs blocks %llu\n", demo_mmap_nr_block);

	temp = demo_mmap_nr_block;
	rem = do_div(temp, 8);
	// check if it all blocks fit in 8 BIT boundary
	if (rem == 0) {
		temp = demo_mmap_nr_block;
		do_div(temp, 8);
	} else {
		temp = demo_mmap_nr_block;
		do_div(temp, 8);
		temp++;		// Extra byte for last few blocks that are outside of 8 BIT boundary
	}
	demo_mmap_bitmap_size = temp;

	/*
	 * Add one page extra memory so that if the allocation is not page
	 * aligned we can start our bitmap futher in memory so that it is page
	 * aligned. To do that we need to allocate extra memory
	 */
	demo_mmap_bitmap_size += demo_mmap_block_size;
	printk(KERN_INFO "demo_mmap: bitmap size %llu\n", demo_mmap_bitmap_size);

	/************************ KMALLOC ***********************/
	/* allocate kernel shared memory for communication with userspace using mmap */
	// Memory allocation fails at anything above 4194304 bytes (4096 * 1024)
	kmalloc_ptr = kmalloc(demo_mmap_bitmap_size, GFP_KERNEL);
	if (kmalloc_ptr) {
		printk(KERN_INFO "demo_mmap: kmalloc_ptr %p allocated %llu bytes ok\n", kmalloc_ptr, demo_mmap_bitmap_size);
	} else {
		printk(KERN_INFO "demo_mmap: failed to kmallocate shared memory of %llu bytes\n", demo_mmap_bitmap_size);
		return -EIO;
	}

	/* kmalloc_addr is always page aligned value of kmalloc_ptr */
	if (((unsigned long)kmalloc_ptr % PAGE_SIZE) == 0) {
		kmalloc_addr = (int *)kmalloc_ptr;
	} else {
		kmalloc_addr = (int *)((unsigned long)kmalloc_ptr +
			(PAGE_SIZE - ((unsigned long)kmalloc_ptr % PAGE_SIZE)));
	}

	/************************ VMALLOC ***********************/
	/* allocate kernel shared memory for communication with userspace using mmap */
	// Memory allocation fails at anything above 80MB
	vmalloc_ptr = vmalloc(demo_mmap_bitmap_size * 30);
	if (vmalloc_ptr) {
		printk(KERN_INFO "demo_mmap: vmalloc_ptr %p allocated %llu bytes ok\n", vmalloc_ptr, demo_mmap_bitmap_size * 30);
	} else {
		printk(KERN_INFO "demo_mmap: failed to vmallocate shared memory of %llu bytes\n", demo_mmap_bitmap_size * 30);
		return -EIO;
	}

	/* reserve all pages to make them remapable */
	//for (virt_addr = (unsigned long)kmalloc_addr;
	//		virt_addr < (unsigned long)kmalloc_addr + (N_PAGES * PAGE_SIZE);
	//		virt_addr += PAGE_SIZE) {
	//	mem_map_reserve(virt_to_page(virt_addr));
	//}

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
	//unsigned long virt_addr;
	printk(KERN_DEBUG "demo_mmap: %s\n", __FUNCTION__);

	if (demo_mmap_major > 0) {
		if (dev->valid == 1) {
			cdev_del(&dev->cdev);
		}
		dev_num = MKDEV(demo_mmap_major, demo_mmap_minor);
		unregister_chrdev_region(dev_num, demo_mmap_nr_dev);
	}

	/* unreserve all pages */
	//for (virt_addr = (unsigned long)kmalloc_addr;
	//		virt_addr < (unsigned long)kmalloc_addr + (N_PAGES * PAGE_SIZE);
	//		virt_addr += PAGE_SIZE) {
	//	mem_map_unreserve(virt_to_page(virt_addr));
	//}

	if (!IS_ERR(bd)) {
		blkdev_put(bd, FMODE_READ);
	}

	/* free shared memory */
	if (kmalloc_ptr) {
		kfree(kmalloc_ptr);
	}
	if (vmalloc_ptr) {
		vfree(vmalloc_ptr);
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

