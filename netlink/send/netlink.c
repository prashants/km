/*
 * netlink - Netlink client send demo
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
#include <linux/netlink.h>
#include <linux/genetlink.h>
#include <net/netlink.h>
#include <net/genetlink.h>
#include <linux/proc_fs.h>

#define MSG_TYPE	100	

static struct sock *my_sock;

DEFINE_MUTEX(lock);

static int process_msg(struct sk_buff *skb, struct nlmsghdr *nlh)
{
	int type;
	char *data;
	printk(KERN_INFO "netlink: %s\n", __FUNCTION__);

	type = nlh->nlmsg_type;
	if (type != MSG_TYPE) {
		printk("netlink: expected %#x got %#x\n", MSG_TYPE, type);
		return -EINVAL;
	}

	data = NLMSG_DATA(nlh);
	printk("netlink: %s\n", data);
	return 0;
}

static void recv_msg(struct sk_buff *skb)
{
	printk(KERN_INFO "netlink: %s\n", __FUNCTION__);
	mutex_lock(&lock);
	netlink_rcv_skb(skb, &process_msg);
	mutex_unlock(&lock);
}

static int __init netlink_init(void)
{
	printk(KERN_INFO "netlink: %s\n", __FUNCTION__);

	my_sock = netlink_kernel_create(&init_net, NETLINK_USERSOCK, 0,
			recv_msg, NULL, THIS_MODULE);

	if (!my_sock) {
		printk(KERN_ERR "netlink: failed to create netlink socket\n");
		return -ENOMEM;
	}

	return 0;
}

static void __exit netlink_exit(void)
{
	printk(KERN_INFO "netlink: %s\n", __FUNCTION__);

	if (my_sock) {
		netlink_kernel_release(my_sock);
	}
}

module_init(netlink_init);
module_exit(netlink_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

