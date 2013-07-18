/*
 * spinlock - spinlock demo
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
#include <linux/spinlock.h>
#include <linux/interrupt.h>

DEFINE_SPINLOCK(static_lock);

spinlock_t dynamic_lock;

static int __init myspinlock_init(void)
{
	unsigned long flags;
	printk(KERN_INFO "myspinlock: %s\n", __FUNCTION__);

	/* static spinlock - irq safe */
	spin_lock_irqsave(&static_lock, flags);		/* acquire lock */
	printk(KERN_INFO "myspinlock: I have acquired a static spinlock\n");
	printk(KERN_INFO "myspinlock: No one else can enter here unless I release this lock\n\n");
	spin_unlock_irqrestore(&static_lock, flags);	/* release lock */

	/* static spinlock - cannot use within interrupt handlers */
	spin_lock(&static_lock);		/* acquire lock */
	printk(KERN_INFO "myspinlock: I have acquired a static non-irq safe spinlock\n");
	printk(KERN_INFO "myspinlock: No one else can enter here unless I release this lock\n");
	printk(KERN_INFO "myspinlock: But please never use me within a interrupt handler!!\n\n");
	spin_unlock(&static_lock);		/* release lock */

	/* dynamic spinlock - irq safe */
	spin_lock_init(&dynamic_lock);
	spin_lock_irqsave(&dynamic_lock, flags);	/* acquire lock */
	printk(KERN_INFO "myspinlock: I have acquired a dynamic spinlock\n");
	printk(KERN_INFO "myspinlock: No one else can enter here unless I release this lock\n\n");
	spin_unlock_irqrestore(&dynamic_lock, flags);	/* release lock */

	return 0;
}

static void __exit myspinlock_exit(void)
{
	printk(KERN_INFO "myspinlock: %s\n", __FUNCTION__);
}

module_init(myspinlock_init);
module_exit(myspinlock_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

