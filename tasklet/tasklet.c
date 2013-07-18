/*
 * tasklet - tasklet demo
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
#include <linux/interrupt.h>
#include <linux/slab.h>

static void do_things(unsigned long data);

DECLARE_TASKLET(static_task, do_things, 1);

struct tasklet_struct *dynamic_task;

static void do_things(unsigned long data)
{
	printk(KERN_INFO "tasklet: I am tasket with data %lu!\n", data);

	printk(KERN_INFO "tasklet: in_atomic=%d\n", in_atomic());
	printk(KERN_INFO "tasklet: in_interrupt=%lu\n", in_interrupt());
	printk(KERN_INFO "tasklet: in_irq=%lu\n", in_irq());
	printk(KERN_INFO "tasklet: in_softirq=%lu\n", in_softirq());

	return;
}

static int __init mytasklet_init(void)
{
	printk(KERN_INFO "tasklet: %s\n", __FUNCTION__);

	/* static tasklet */
	tasklet_schedule(&static_task);

	/* dynamic tasklet */
	dynamic_task = (struct tasklet_struct *)kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);
	if (!dynamic_task) {
		printk(KERN_INFO "tasklet: failed to create tasklet\n");
		return 0;
	}
	tasklet_init(dynamic_task, do_things, 2);
	tasklet_schedule(dynamic_task);

	return 0;
}

static void __exit mytasklet_exit(void)
{
	printk(KERN_INFO "tasklet: %s\n", __FUNCTION__);

	if (dynamic_task) {
		kfree(dynamic_task);
	}
}

module_init(mytasklet_init);
module_exit(mytasklet_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

