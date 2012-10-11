/*
 * tasklet - tasklet demo
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

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

