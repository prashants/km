/*
 * memzone - Print vm area statistics
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
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/vmstat.h>
#include <linux/version.h>

static int __init memzone_init(void)
{
	printk(KERN_INFO "memzone: %s\n", __FUNCTION__);

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 19)
	for (c = 0; c < 2; c++) {
		curzone = zone_table[c];
		printk(KERN_INFO "%s zone : free=%lu min=%lu low=%lu high=%lu\n", curzone->name,
			curzone->free_pages, curzone->pages_min,
			curzone->pages_low, curzone->pages_high);
	}
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 21)
	printk(KERN_INFO "free_pages=%lu\n", atomic_long_read(&vm_stat[0]));
#endif

	return 0;
}

static void __exit memzone_exit(void)
{
	printk(KERN_INFO "memzone: %s\n", __FUNCTION__);
}

module_init(memzone_init);
module_exit(memzone_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

