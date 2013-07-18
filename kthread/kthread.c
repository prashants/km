/*
 * kthread - kthread demo
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

int c1 = 1;
int c2 = 2;
int c3 = 3;

static int kthread_do_things(void *data)
{
	int val = *(int *)data;		/* read the value pointed to by data */

	set_current_state(TASK_UNINTERRUPTIBLE);
	schedule_timeout((4 - val) * HZ);	/* adding delay in seconds */

	printk(KERN_INFO "kthread: I am running in thread %d!\n", val);
	return 0;
}

static int __init kthread_init(void)
{
	struct task_struct *t1;
	struct task_struct *t2;
	struct task_struct *t3;
	printk(KERN_INFO "kthread: %s\n", __FUNCTION__);

	t1 = kthread_run(kthread_do_things, &c1, "mykthread1");

	t2 = kthread_run(kthread_do_things, &c2, "mykthread2");

	t3 = kthread_run(kthread_do_things, &c3, "mykthread3");

	return 0;
}

static void __exit kthread_exit(void)
{
	printk(KERN_INFO "kthread: %s\n", __FUNCTION__);
}

module_init(kthread_init);
module_exit(kthread_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

