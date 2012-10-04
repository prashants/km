/*
 * kthread - kthread demo
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
#include <linux/kthread.h>
#include <linux/sched.h>

int c1 = 1;
int c2 = 2;
int c3 = 3;

static int kthread_do_things(void *data)
{
	int val = *(int *)data;		/* read the value pointed to by data */

	set_current_state(TASK_UNINTERRUPTIBLE);
	schedule_timeout((4-val) * HZ);	/* adding delay in seconds */

	printk(KERN_INFO "kthread: I am running in thread %d!\n", val);
	return 0;
}

static int __init kthread_init(void)
{
	struct task_struct *t1;
	struct task_struct *t2;
	struct task_struct *t3;
	printk(KERN_DEBUG "kthread: %s\n", __FUNCTION__);

	t1 = kthread_run(kthread_do_things, &c1, "mykthread1");

	t2 = kthread_run(kthread_do_things, &c2, "mykthread2");

	t3 = kthread_run(kthread_do_things, &c3, "mykthread3");

	return 0;
}

static void __exit kthread_exit(void)
{
	printk(KERN_DEBUG "kthread: %s\n", __FUNCTION__);
}

module_init(kthread_init);
module_exit(kthread_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

