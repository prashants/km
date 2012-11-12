/*
 * completion - completion demo
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

int unused = 0;

DECLARE_COMPLETION(done);

static int kthread_do_things(void *data)
{
	printk(KERN_INFO "kthread: I am running in thread!\n");

	set_current_state(TASK_UNINTERRUPTIBLE);
	schedule_timeout(5 * HZ);	/* adding delay in seconds */

	complete(&done);

	printk(KERN_INFO "kthread: I am no longer running!\n");
	return 0;
}

static int __init mycompletion_init(void)
{
	struct task_struct *kt;
	printk(KERN_INFO "kthread: %s\n", __FUNCTION__);

	kt = kthread_run(kthread_do_things, &unused, "mykthread");

	wait_for_completion(&done);

	printk(KERN_INFO "kthread: END %s\n", __FUNCTION__);
	return 0;
}

static void __exit mycompletion_exit(void)
{
	printk(KERN_INFO "kthread: %s\n", __FUNCTION__);
}

module_init(mycompletion_init);
module_exit(mycompletion_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

