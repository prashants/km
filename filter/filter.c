/*
 * filter - File system filter driver
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
#include <linux/blkdev.h>

#define PARTITION_NAME "/dev/sda1"

struct block_device *bd;

make_request_fn *orig_mrqfn;

static void filter_mrqfn(struct request_queue *q, struct bio *bio)
{
	printk(KERN_DEBUG "filter: %s\n", __FUNCTION__);

	/*
	 * this is the custom request function where you can do
	 * lot of stuff with the data sent to file system. Here
	 * we will call the original make_request_fn() so that
	 * the request can proceed normally. All data is accessible
	 * from the bio structure. Use clone_bio() to create a new
	 * request
	 */

	printk(KERN_DEBUG "filter: inode %lu\n",
		(unsigned long)bio->bi_io_vec->bv_page->mapping->host->i_ino);

	orig_mrqfn(q, bio); /* calling the original make_request_fn() */
}

/*
 * set_mrqfn() - Change the original make_request_fn() to our
 * modules request function
 */
static void set_mrqfn(void)
{
	struct super_block *sb =  bd->bd_super;
	printk(KERN_DEBUG "filter: %s\n", __FUNCTION__);

	/* lock filesystem to prevent any further changes */
	freeze_super(sb);

	if (bd->bd_disk->queue->make_request_fn == filter_mrqfn) {
		printk(KERN_INFO "filter: modules request function is already active\n");
	} else {
		/* save the pointer to the original make_request_fn() */
		orig_mrqfn = bd->bd_disk->queue->make_request_fn;
		/* replace the original with our modules request function */
		bd->bd_disk->queue->make_request_fn = filter_mrqfn;
	}

	/* unlock filesystem */
	thaw_super(sb);
}

/*
 * restore_mrqfn() - Restore the original make_request_fn()
 */
static void restore_mrqfn(void)
{
	struct super_block *sb =  bd->bd_super;
	printk(KERN_DEBUG "filter: %s\n", __FUNCTION__);

	if (orig_mrqfn) {
		/* lock filesystem to prevent any further changes */
		freeze_super(sb);

		/* restore the original request function */
		bd->bd_disk->queue->make_request_fn = orig_mrqfn;

		/* unlock filesystem */
		thaw_super(sb);
	}
	orig_mrqfn = NULL;
}

static int __init filter_init(void)
{
	printk(KERN_DEBUG "filter: %s\n", __FUNCTION__);

	orig_mrqfn = NULL;

	// Read block device from path
	bd = blkdev_get_by_path(PARTITION_NAME, FMODE_READ, NULL);

	if (IS_ERR(bd)) {
		printk(KERN_ERR "filter: failed to get block device\n");
		return 0;
	}

	printk(KERN_INFO "filter: found block device with major number %d\n",
		bd->bd_disk->major);

	printk(KERN_INFO "filter: file system block size %d\n",
			bd->bd_block_size);
	printk(KERN_INFO "filter: start sector %lu\n",
			(unsigned long)bd->bd_part->start_sect);
	printk(KERN_INFO "filter: number of sectors %lu\n",
			(unsigned long)bd->bd_part->nr_sects);
	printk(KERN_INFO "filter: logical block size %d\n",
			bdev_logical_block_size(bd));
	printk(KERN_INFO "filter: physical block size %d\n",
			bdev_physical_block_size(bd));

	set_mrqfn();

	return 0;
}

static void __exit filter_exit(void)
{
	printk(KERN_DEBUG "filter: %s\n", __FUNCTION__);

	restore_mrqfn();

	if (!IS_ERR(bd)) {
		blkdev_put(bd, FMODE_READ);
	}
}

module_init(filter_init);
module_exit(filter_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

