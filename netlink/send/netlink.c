/*
 * netlink - Netlink client send demo
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

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

