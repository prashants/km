/*
 * memzone - Print vm area statistics
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
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mmzone.h>
#include <linux/vmstat.h>
#include <linux/version.h>

static int __init memzone_init(void)
{
	int c;
	struct zone *curzone;
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

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

