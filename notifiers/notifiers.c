/*
 * notifiers - Shutdown notifier demo
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

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

