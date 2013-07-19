/*
 * notifier - Notifier chain demo
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
#include <linux/notifier.h>
#include <linux/usb.h>
#include <linux/netdevice.h>

static int netlink_notifier_func(struct notifier_block *self, unsigned long action,
		void *dev)
{
	printk(KERN_INFO "notifier: %s\n", __FUNCTION__);

	switch (action) {
		case NETDEV_UP:
			printk(KERN_INFO "NETDEV_UP\n");
			break;
		case NETDEV_DOWN:
			printk(KERN_INFO "NETDEV_DOWN\n");
			break;
		case NETDEV_REBOOT:
			printk(KERN_INFO "NETDEV_REBOOT\n");
			break;
		case NETDEV_CHANGE:
			printk(KERN_INFO "NETDEV_CHANGE\n");
			break;
		case NETDEV_REGISTER:
			printk(KERN_INFO "NETDEV_REGISTER\n");
			break;
		case NETDEV_UNREGISTER:
			printk(KERN_INFO "NETDEV_UNREGISTER\n");
			break;
		case NETDEV_CHANGEMTU:
			printk(KERN_INFO "NETDEV_CHANGEMTU\n");
			break;
		case NETDEV_CHANGEADDR:
			printk(KERN_INFO "NETDEV_CHANGEADDR\n");
			break;
		case NETDEV_GOING_DOWN:
			printk(KERN_INFO "NETDEV_GOING_DOWN\n");
			break;
		case NETDEV_CHANGENAME:
			printk(KERN_INFO "NETDEV_CHANGENAME\n");
			break;
		case NETDEV_FEAT_CHANGE:
			printk(KERN_INFO "NETDEV_FEAT_CHANGE\n");
			break;
		case NETDEV_BONDING_FAILOVER:
			printk(KERN_INFO "NETDEV_BONDING_FAILOVER\n");
			break;
		case NETDEV_PRE_UP:
			printk(KERN_INFO "NETDEV_PRE_UP\n");
			break;
		case NETDEV_RELEASE:
			printk(KERN_INFO "NETDEV_RELEASE\n");
			break;
		case NETDEV_JOIN:
			printk(KERN_INFO "NETDEV_JOIN\n");
			break;
	}
	return NOTIFY_OK;
}

static int usb_notifier_func(struct notifier_block *self, unsigned long action,
		void *dev)
{
	printk(KERN_INFO "notifier: %s\n", __FUNCTION__);

	// check include/linux/usb.h
	switch (action) {
		case USB_DEVICE_ADD:
			printk(KERN_INFO "USB device added\n");
			break;
		case USB_DEVICE_REMOVE:
			printk(KERN_INFO "USB device removed\n");
			break;
		case USB_BUS_ADD:
			printk(KERN_INFO "USB bus added\n");
			break;
		case USB_BUS_REMOVE:
			printk(KERN_INFO "USB bus removed\n");
			break;
	}

	return NOTIFY_OK;
}

struct notifier_block netlink_notify = {
	.notifier_call = netlink_notifier_func,
};

struct notifier_block usb_notify = {
	.notifier_call = usb_notifier_func,
};

static int __init notifier_init(void)
{
	printk(KERN_INFO "notifier: %s\n", __FUNCTION__);

	usb_register_notify(&usb_notify);
	register_netdevice_notifier(&netlink_notify);

	return 0;
}

static void __exit notifier_exit(void)
{
	printk(KERN_INFO "notifier: %s\n", __FUNCTION__);

	unregister_netdevice_notifier(&netlink_notify);
	usb_unregister_notify(&usb_notify);
}

module_init(notifier_init);
module_exit(notifier_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

