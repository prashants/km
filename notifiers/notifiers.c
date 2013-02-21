/*
 * notifiers - Shutdown notifier demo
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
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/reboot.h>

static int notifier_func(struct notifier_block *self,
		unsigned long action, void *hcpu)
{
	printk(KERN_ERR "notifiers: %s\n", __FUNCTION__);

	return 0;
}

static struct notifier_block notify = {
	.notifier_call = notifier_func,
};

static int __init notifiers_init(void)
{
	printk(KERN_INFO "notifiers: %s\n", __FUNCTION__);

	register_reboot_notifier(&notify);

	return 0;
}

static void __exit notifiers_exit(void)
{
	printk(KERN_INFO "notifiers: %s\n", __FUNCTION__);

	unregister_reboot_notifier(&notify);
}

module_init(notifiers_init);
module_exit(notifiers_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

