/*
 * completion - completion demo
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
	printk(KERN_INFO "mycompletion: %s\n", __FUNCTION__);

	kt = kthread_run(kthread_do_things, &unused, "mycompletion");

	wait_for_completion(&done);

	printk(KERN_INFO "mycompletion: END %s\n", __FUNCTION__);
	return 0;
}

static void __exit mycompletion_exit(void)
{
	printk(KERN_INFO "completion: %s\n", __FUNCTION__);
}

module_init(mycompletion_init);
module_exit(mycompletion_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

