/*
 * bitops - Bit operations demo
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

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

