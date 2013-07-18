/*
 * workqueue - workqueue demo
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

static void do_things(struct work_struct *work);

DECLARE_WORK(static_work, do_things);
DECLARE_DELAYED_WORK(static_delay_work, do_things);

struct work_struct *dynamic_work;

struct workqueue_struct *wq; /* custom workqueue */

static void do_things(struct work_struct *work)
{
	printk(KERN_INFO "workqueue: I am in a workqueue\n");

	printk(KERN_INFO "workqueue: in_atomic=%d\n", in_atomic());
	printk(KERN_INFO "workqueue: in_interrupt=%lu\n", in_interrupt());
	printk(KERN_INFO "workqueue: in_irq=%lu\n", in_irq());
	printk(KERN_INFO "workqueue: in_softirq=%lu\n", in_softirq());
	printk(KERN_INFO "\n");

	return;
}

static int __init myworkqueue_init(void)
{
	int ret;
	unsigned long delay;
	printk(KERN_INFO "workqueue: %s\n", __FUNCTION__);

	/************************ static work *********************/
	atomic_long_set(&static_work.data, 10);
	ret = schedule_work(&static_work); /* schedule on kernel global workqueue */
	if (ret == 0) {
		printk(KERN_INFO "workqueue: static work already on the kernel-global workqueue\n");
	}

	/*********************** dynamic work *********************/
	dynamic_work = (struct work_struct *)kmalloc(sizeof(struct work_struct), GFP_KERNEL);
	if (!dynamic_work) {
		printk(KERN_INFO "workqueue: failed to create work\n");
		return 0;
	}
	INIT_WORK(dynamic_work, do_things);
	atomic_long_set(&dynamic_work->data, 20);
	ret = schedule_work(dynamic_work); /* schedule on kernel global workqueue */
	if (ret == 0) {
		printk(KERN_INFO "workqueue: dynamic work already on the kernel-global workqueue\n");
	}

	/******************* static delayed work *****************/
	/* delay is in jiffies, delaying by 3 seconds since 1 HZ jiffies = 1 sec */
	delay = HZ * 10;
	ret = schedule_delayed_work(&static_delay_work, delay); /* schedule on kernel global workqueue */
	if (ret == 0) {
		printk(KERN_INFO "workqueue: static delayed work already on the kernel-global workqueue\n");
	}

	/************ static work on custom workqueue ************/
	wq = create_workqueue("myqueue");
	if (!wq) {
		printk(KERN_INFO "workqueue: failed to create workqueue\n");
		return 0;
	}
	atomic_long_set(&static_work.data, 40);
	ret = queue_work(wq, &static_work); /* schedule on custom workqueue */
	if (ret == 0) {
		printk(KERN_INFO "workqueue: static work already on the custom workqueue\n");
	}

	return 0;
}

static void __exit myworkqueue_exit(void)
{
	printk(KERN_INFO "workqueue: %s\n", __FUNCTION__);

	if (dynamic_work) {
		flush_work(dynamic_work);
		kfree(dynamic_work);
	}

	flush_delayed_work(&static_delay_work);

	if (wq) {
		flush_workqueue(wq);
		destroy_workqueue(wq);
	}
}

module_init(myworkqueue_init);
module_exit(myworkqueue_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

