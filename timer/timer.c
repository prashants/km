/*
 * timer - timer demo
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

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

