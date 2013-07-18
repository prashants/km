/*
 * writeblock - Write a block of data on disk
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
#include <linux/slab.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>

#define BLOCK_NUM		10240 	// 5100	// 137038
#define SECTOR_NUM		1061885 // 81920 // 40801 // 1096304

char *data;
struct block_device *bd;
struct file *bd_file;

static int writebh(char *data)
{
	struct buffer_head *bh;

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
		printk(KERN_ERR "bh_ERROR\n");
	}

	brelse(bh);


	return 0;
}

static int rq_complete(struct bio *b, unsigned int bytes_done, int err)
{
        complete((struct completion *)b->bi_private);
        return 0;
}

static void writebio(sector_t sect, char *data)
{
        struct bio bio;
        struct bio_vec bio_vec;
        struct completion complete;
        size_t size;

	struct page *p = NULL;
	char *d = NULL;

        size = 4096;

	p = alloc_page(GFP_KERNEL);
	d = page_address(p);
	memcpy(d, data, 4096);

        bio_init(&bio);
        bio.bi_io_vec = &bio_vec;
        bio_vec.bv_page = p;
        bio_vec.bv_len = size;
        bio_vec.bv_offset = 0;
        bio.bi_vcnt = 1;
        bio.bi_idx = 0;
        bio.bi_size = size;
        bio.bi_bdev = bd->bd_contains;
        bio.bi_sector = sect;
        init_completion(&complete);
        bio.bi_private = &complete;
        bio.bi_end_io = rq_complete;

        submit_bio(WRITE_SYNC, &bio);
        wait_for_completion(&complete);

	if (test_bit(BIO_UPTODATE, &bio.bi_flags)) {
		printk(KERN_INFO "BIO OK\n");
	} else {
		printk(KERN_INFO "BIO ERROR\n");
	}
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
		data[c] = 'J';
	}

	bd_file = filp_open("/dev/sde2", O_RDONLY, 0600);
	if (IS_ERR(bd_file)) {
		printk(KERN_INFO "error opening bd file\n");
		return -EIO;
	}

	bd = bd_file->f_dentry->d_inode->i_bdev;

	//writebh(data);
	writebio(SECTOR_NUM, data);

	kfree(data);

	return 0;
}

static void __exit writeblock_exit(void)
{
	printk(KERN_INFO "list: %s\n", __FUNCTION__);

	filp_close(bd_file, 0);
}

module_init(writeblock_init);
module_exit(writeblock_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

