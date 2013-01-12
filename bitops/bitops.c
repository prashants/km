/*
 * bitops - Bit operations demo
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
#include <linux/random.h>
#include <linux/list.h>

static int __init bitops_init(void)
{
	unsigned int c;
	unsigned int d;
	printk(KERN_INFO "bitops: %s\n", __FUNCTION__);

	d = 240;
	printk(KERN_INFO "bitops: value = %d\n", d);
	set_bit(0, (long unsigned int *)&d);
	printk(KERN_INFO "bitops: value = %d\n", d);
	for (c = 0; c < 8; c++) {
		printk(KERN_INFO "bitops: %d is %d\n",
			c, test_bit(c, (long unsigned int *)&d));
	}
	printk(KERN_INFO "bitops: test bit 1 = %d\n",
			test_and_set_bit(1, (long unsigned int *)&d));
	printk(KERN_INFO "bitops: value = %d\n", d);

	return 0;
}

static void __exit bitops_exit(void)
{
	printk(KERN_INFO "bitops: %s\n", __FUNCTION__);
}

module_init(bitops_init);
module_exit(bitops_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

