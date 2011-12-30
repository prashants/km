/*************************************************************************
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/blkdev.h>
#include <linux/major.h>
#include <linux/fs.h>
#include <linux/genhd.h>

#define DEVICE_NAME "simpleblock"
#define DEVICE_REQUEST myrequest
#define DEVICE_NR(device) (MINOR(device) >> 6)
#define DEVICE_ON(device)
#define DEVICE_OFF(device)

int major_number;

struct gendisk *simpleblock_gd;

static void myrequest(void)
{
initreq:
	INIT_REQUEST;
	if (!valid_device(CURRENT->dev)) {
		end_request(0);
		goto initreq;
	}
	if (CURRENT->cmd == WRITE) {
		if(simple_write(CURRENT->sector, CURRENT->buffer, CURRENT->nr_sectors)) {
			end_request(1);
		} else {
			end_request(0);
		}
		goto initreq;
	}
	if (CURRENT->cmd == READ) {
		if(simple_read(CURRENT->sector, CURRENT->buffer, CURRENT->nr_sectors)) {
			end_request(1);
		} else {
			end_request(0);
		}
		goto initreq;
	}
}

struct block_device_operations blk_ops = {
	.owner = THIS_MODULE,
	.open = simpleblock_open,
	.release = simpleblock_release,
	.ioctl = simpleblock_ioctl,
	.media_changed = simpleblock_media_changed,
	.revalidate_disk = simpleblock_revalidate_disk,
};

/* This function is called when the module is loaded */
static int __init block_init(void)
{
	printk(KERN_INFO "myblock: %s\n", __FUNCTION__);
	major_number = register_blkdev(0, "simpleblock");
	if (!major_number) {
		printk(KERN_ERR "simpleblock: unable to get the major number\n");
		return -EINVAL;
	}
	simpleblock_gd = alloc_disk(16);
	if (!simpleblock_gd) {
		printk(KERN_ERR "simpleblock: cannot allocate gendisk\n");
		return -EINVAL;
	}
	printk(KERN_INFO "simpleblock: registered with major number: %d\n", major_number);
	return 0;
}

/* This function is called when the module is unloaded */
static void __exit block_exit(void)
{
	printk(KERN_INFO "myblock: %s\n", __FUNCTION__);
	if (major_number)
		unregister_blkdev(major_number, "simpleblock");
	if (simpleblock_gd)
		del_gendisk(simpleblock_gd);
}

module_init(block_init);
module_exit(block_exit);

MODULE_DESCRIPTION("My first block device module");
MODULE_AUTHOR("Me");
MODULE_LICENSE("GPL");

