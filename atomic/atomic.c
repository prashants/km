/*
 * atomic - atomic integer demo
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
#include <linux/atomic.h>
#include <linux/interrupt.h>

#define DEBUG(m) printk("myatomic: " m " current value = %d\n", atomic_read(&atom));

static int __init myatomic_init(void)
{
	atomic_t atom;
	printk(KERN_INFO "myatomic: %s\n", __FUNCTION__);

	atomic_set(&atom, 0);	/* initialize */
	DEBUG("atomic_set");
	atomic_inc(&atom);
	DEBUG("atomic_inc");
	atomic_dec(&atom);
	DEBUG("atomic_dec");
	atomic_add(100, &atom);
	DEBUG("atomic_add");
	atomic_sub(50, &atom);
	DEBUG("atomic_sub");
	if (atomic_sub_and_test(50, &atom)) {
		DEBUG("atomic_sub_and_test");
	}
	if (atomic_inc_and_test(&atom)) {
		DEBUG("atomic_inc_and_test");
	}
	if (atomic_dec_and_test(&atom)) {
		DEBUG("atomic_dec_and_test");
	}

	return 0;
}

static void __exit myatomic_exit(void)
{
	printk(KERN_INFO "myatomic: %s\n", __FUNCTION__);
}

module_init(myatomic_init);
module_exit(myatomic_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

