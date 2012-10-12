/*
 * spinlock - spinlock demo
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

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

