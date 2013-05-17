/*
 * dynamicbio - Dynamically allocate bio to read data on disk
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

char *data;
struct block_device *bd;
struct file *bd_file;

static void rq_complete(struct bio *b, int err)
{
        complete((struct completion *)b->bi_private);
        return;
}

static void readbio(sector_t sect, char *data)
{
        struct bio *bio;
        struct page *page;
	struct completion complete;
	int size;

	size = 4096;
	init_completion(&complete);

        /* Allocate read bio */
        bio = bio_alloc(GFP_NOIO, 1);
        page = alloc_page(GFP_KERNEL);
        // TO DO : Check alloc

        /* Prepare the read bio */
        bio->bi_bdev = bd->bd_contains;
        bio->bi_io_vec->bv_page = page;
        bio->bi_io_vec->bv_len = size;
        bio->bi_io_vec->bv_offset = 0;
        bio->bi_vcnt = 1;
        bio->bi_idx = 0;
        bio->bi_size = size;
        bio->bi_sector = sect;
        bio->bi_rw = READ;
        bio->bi_end_io = rq_complete;
        bio->bi_private = &complete;

        /* Submit the read bio */
        submit_bio(READ, bio);

        wait_for_completion(&complete);

	if (test_bit(BIO_UPTODATE, &bio->bi_flags)) {
		printk(KERN_INFO "BIO OK\n");
	} else {
		printk(KERN_INFO "BIO ERROR\n");
	}

	__free_page(page);
	bio_put(bio);
}

static int __init dynamicbio_init(void)
{
	sector_t c;
	printk(KERN_INFO "dynamicbio: %s\n", __FUNCTION__);

	bd_file = filp_open("/dev/sda1", O_RDONLY, 0600);
	if (IS_ERR(bd_file)) {
		printk(KERN_INFO "error opening bd file\n");
		return -EIO;
	}
	bd = bd_file->f_dentry->d_inode->i_bdev;
	filp_close(bd_file, 0);

	for (c = 0; c < 5000; c++) {
		readbio(c, data);
	}

	return 0;
}

static void __exit dynamicbio_exit(void)
{
	printk(KERN_INFO "dynamicbio: %s\n", __FUNCTION__);
}

module_init(dynamicbio_init);
module_exit(dynamicbio_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

