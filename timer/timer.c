/*
 * timer - timer demo
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
#include <linux/timer.h>
#include <linux/sched.h>

struct timer_list mytimer;

static void timer_do_things(unsigned long data)
{
	printk(KERN_INFO "timer: I am running a timer function @ %lu\n", jiffies);

	/* Renew timer */
	mytimer.expires = jiffies + (HZ * 5);
	add_timer(&mytimer);
}

static int __init timer_init(void)
{
	printk(KERN_INFO "timer: %s\n", __FUNCTION__);
	init_timer(&mytimer);

	mytimer.data = 0;
	mytimer.function = timer_do_things;
	mytimer.expires = jiffies + (HZ * 5);

	add_timer(&mytimer);

	return 0;
}

static void __exit timer_exit(void)
{
	printk(KERN_INFO "timer: %s\n", __FUNCTION__);
	del_timer(&mytimer);
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

