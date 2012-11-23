/*
 * device_mapper - Custom device mapper target demo
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
#include <linux/bio.h>
#include <linux/device-mapper.h>

struct test_target {
	struct dm_dev *dev;
	sector_t start;
};

static int test_target_ctr(struct dm_target *ti, unsigned int argc, char **argv)
{
	struct test_target *tdt;
	unsigned long long start;
	printk(KERN_INFO "device_mapper: %s\n", __FUNCTION__);

	if (argc != 2) {
		printk(KERN_ERR "device_mapper: invalid number of arguments\n");
		ti->error = "Invalid number of arguments";
		return -EINVAL;
	}

	tdt = kmalloc(sizeof(struct test_target), GFP_KERNEL);
	if (!tdt) {
		printk(KERN_ERR "device_mapper: failed to allocate memory for target\n");
		ti->error = "Failed to allocated memory";
		return -ENOMEM;
	}

	if (sscanf(argv[1], "%llu", &start) != 1) {
		ti->error = "Invalid device selector";
		goto err;
	}

	tdt->start = (sector_t)start;

	if (dm_get_device(ti, argv[0], dm_table_get_mode(ti->table), &tdt->dev)) {
		ti->error = "Device lookup failed";
		goto err;
	}

	ti->private = tdt;

	return 0;

err:
	kfree(tdt);
	return -EINVAL;
}

static void test_target_dtr(struct dm_target *ti)
{
	struct test_target *tdt = (struct test_target *)ti->private;
	printk(KERN_INFO "device_mapper: %s\n", __FUNCTION__);

	dm_put_device(ti, tdt->dev);
	if (tdt)
		kfree(tdt);
}

static int test_target_map(struct dm_target *ti, struct bio *b,
		union map_info *map_context)
{
	struct test_target *tdt = (struct test_target *)ti->private;
	printk(KERN_INFO "device_mapper: %s\n", __FUNCTION__);

	b->bi_bdev = tdt->dev->bdev;

	if (bio_data_dir(b) == 1) {
		printk(KERN_INFO "mapper: write\n");
	} else {
		printk(KERN_INFO "mapper: read\n");
	}

	submit_bio(b->bi_rw, b);

	return DM_MAPIO_SUBMITTED;
}

static int test_target_status(struct dm_target *ti, status_type_t type,
		char *result, unsigned int maxlen)
{
	printk(KERN_INFO "device_mapper: %s\n", __FUNCTION__);
	return 0;
}

static int test_target_ioctl(struct dm_target *ti, struct inode *inode,
		struct file *filp, unsigned int cmd,
		unsigned long arg)
{
	printk(KERN_INFO "device_mapper: %s\n", __FUNCTION__);
	return 0;
}

static struct target_type test_target = {
	.name		= "test_target",
	.version	= {1, 0, 0},
	.module		= THIS_MODULE,
	.ctr		= test_target_ctr,
	.dtr		= test_target_dtr,
	.map		= test_target_map,
	.status		= test_target_status,
	.ioctl		= test_target_ioctl,
};

static int __init device_mapper_init(void)
{
	int ret;
	printk(KERN_INFO "device_mapper: %s\n", __FUNCTION__);

	ret = dm_register_target(&test_target);
	if (ret < 0) {
		printk(KERN_ERR "device_mapper: error registering target\n");
	}

	return ret;
}

static void __exit device_mapper_exit(void)
{
	printk(KERN_INFO "device_mapper: %s\n", __FUNCTION__);

	dm_unregister_target(&test_target);
}

module_init(device_mapper_init);
module_exit(device_mapper_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

