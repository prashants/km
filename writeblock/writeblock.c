/*
 * writeblock - Write a block of data on disk
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
#include <linux/slab.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>

#define BLOCK_NUM		137038

char *data;

static int writeblock(char *data)
{
	struct buffer_head *bh;
	struct file *bd_file;
	struct block_device *bd;

	bd_file = filp_open("/dev/sda1", O_RDONLY, 0600);
	if (IS_ERR(bd_file)) {
		printk(KERN_INFO "error opening bd file\n");
		return -EIO;
	}

	bd = bd_file->f_dentry->d_inode->i_bdev;

	bh = __bread(bd, BLOCK_NUM, 4096);

	if (bh) {
		if (bh->b_data) {
			printk(KERN_INFO "data: %s\n", bh->b_data);
			memcpy(bh->b_data, data, 4096);
			lock_buffer(bh);
			get_bh(bh);
			submit_bh(WRITE, bh);
			wait_on_buffer(bh);
			clear_buffer_uptodate(bh);
			put_bh(bh);
			unlock_buffer(bh);
			printk(KERN_INFO "bh_OK\n");
		} else {
			printk(KERN_ERR "bh_data_ERROR\n");
		}
	} else {
		printk(KERN_ERR "bh_ERROR");
	}

	filp_close(bd_file, 0);

	return 0;
}

static int __init writeblock_init(void)
{
	int c;
	printk(KERN_INFO "writeblock: %s\n", __FUNCTION__);

	data = kmalloc(4096, GFP_KERNEL);
	if (!data) {
		printk(KERN_ERR "writeblock: failed to allocate memory\n");
		return -EIO;
	}

	for (c = 0; c < 4096; c++) {
		data[c] = 'A';
	}

	writeblock(data);

	kfree(data);

	return 0;
}

static void __exit writeblock_exit(void)
{
	printk(KERN_INFO "list: %s\n", __FUNCTION__);
}

module_init(writeblock_init);
module_exit(writeblock_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

