/*
 * atomic - atomic integer demo
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
#include <linux/atomic.h>
#include <linux/interrupt.h>

#define DEBUG(m) printk("myatomic: " m " current value = %d\n", atomic_read(&atom));

static int __init myatomic_init(void)
{
	atomic_t atom;
	printk(KERN_INFO "myatomic: %s\n", __FUNCTION__);

	atomic_set(&atom, 0);	/* initialize */
	DEBUG("atomic_set");
	atomic_inc(&atom);
	DEBUG("atomic_inc");
	atomic_dec(&atom);
	DEBUG("atomic_dec");
	atomic_add(100, &atom);
	DEBUG("atomic_add");
	atomic_sub(50, &atom);
	DEBUG("atomic_sub");
	if (atomic_sub_and_test(50, &atom)) {
		DEBUG("atomic_sub_and_test");
	}
	if (atomic_inc_and_test(&atom)) {
		DEBUG("atomic_inc_and_test");
	}
	if (atomic_dec_and_test(&atom)) {
		DEBUG("atomic_dec_and_test");
	}

	return 0;
}

static void __exit myatomic_exit(void)
{
	printk(KERN_INFO "myatomic: %s\n", __FUNCTION__);
}

module_init(myatomic_init);
module_exit(myatomic_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

