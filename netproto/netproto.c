/*
 * netproto - Network protocol demo
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
#include <linux/proc_fs.h>

#include <net/sock.h>
#include <linux/net.h>

#define pdebug(msg) printk(KERN_INFO "netproto: %s\n", __FUNCTION__);
#define pmsg(msg) printk(KERN_INFO "netproto: %s "#msg"\n", __FUNCTION__);

/* to be defined in include/linux/socket.h */ 
#define AF_INET_SHAHP 27

static struct proto_ops shahp_ops;
struct shahp_sock;
struct proto shahp_proto;

/*****************************************************************************/
/******************************* PROTO OPS ***********************************/
/*****************************************************************************/

static int shahp_release(struct socket *sock)
{
	pdebug();
	return 0;
}

static int shahp_bind(struct socket *sock, struct sockaddr *myaddr, int sockaddr_len)
{
	pdebug();
	return 0;
}

static int shahp_connect(struct socket *sock, struct sockaddr *myaddr, int sockaddr_len, int flags)
{
	pdebug();
	return 0;
}

static int shahp_accept(struct socket *sock, struct socket *newsock, int flags)
{
	pdebug();
	return 0;
}

static int shahp_getname(struct socket *sock, struct sockaddr *addr, int *sockaddr_len, int peer)
{
	pdebug();
	return 0;
}

static int shahp_listen(struct socket *sock, int len)
{
	pdebug();
	return 0;
}

static int shahp_sendmsg(struct kiocb *iocb, struct socket *sock,
		struct msghdr *m, size_t total_len)
{
	pdebug();
	return 0;
}

static int shahp_recvmsg(struct kiocb *iocb, struct socket *sock,
		struct msghdr *m, size_t total_len, int flags)
{
	pdebug();
	return 0;
}

/*************************** PROTO OPS ENDS **********************************/

/*****************************************************************************/
/******************************* PROTO ***************************************/
/*****************************************************************************/

static void shahp_proto_close(struct sock *sk, long timeout)
{
	pdebug();
}

static int shahp_proto_bind(struct sock *sk, struct sockaddr *addr, int len)
{
	pdebug();
	return 0;
}

static int shahp_proto_connect(struct sock *sk, struct sockaddr *addr, int addr_len)
{
	pdebug();
	return 0;
}

static int shahp_proto_disconnect(struct sock *sk, int flags)
{
	pdebug();
	return 0;
}

static int shahp_proto_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg, size_t len)
{
	pdebug();
	return 0;
}

static int shahp_proto_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
		size_t len, int noblock, int flags, int *addr_len)
{
	pdebug();
	return 0;
}

/*************************** PROTO ENDS **************************************/

static int shahp_create_socket(struct net *net, struct socket *sock, int protocol, int kern)
{
	struct sock *sk;

	pdebug();

	sk = sk_alloc(net, AF_INET_SHAHP, GFP_KERNEL, &shahp_proto);
	if (!sk) {
		pmsg("failed to allocate socket\n");
		return -ENOMEM;
	}
	sock_init_data(sock, sk);
	sk->sk_protocol = 0x0;
	sock->ops = &shahp_ops;
	return 0;
}


/* protocol specific socket structure */
struct shahp_sock {
};

struct proto shahp_proto = {
	.name = "SHAHP",
	.owner = THIS_MODULE,
	.close = shahp_proto_close,
	.bind = shahp_proto_bind,
	.connect = shahp_proto_connect,
	.disconnect = shahp_proto_disconnect,
	.sendmsg = shahp_proto_sendmsg,
	.recvmsg = shahp_proto_recvmsg,
	.obj_size = sizeof(struct sock),
};

struct net_proto_family shahp_proto_family = {
	.family = AF_INET_SHAHP,
	.create = shahp_create_socket,
	.owner = THIS_MODULE,
};

static struct proto_ops shahp_ops = {
	.family = AF_INET,
	.owner = THIS_MODULE,
	.release = shahp_release,
	.bind = shahp_bind,
	.connect = shahp_connect,
	.accept = shahp_accept,
	.getname = shahp_getname,
	.listen = shahp_listen,
	.sendmsg = shahp_sendmsg,
	.recvmsg = shahp_recvmsg,
};

static int __init netproto_init(void)
{
	pdebug();

	if (proto_register(&shahp_proto, 1)) {
		pmsg("failed to register shahp protocol\n");
		return -1;
	}

	if (sock_register(&shahp_proto_family)) {
		pmsg("failed to register shahp socket\n");
		goto sock_err;
	}
	return 0;
sock_err:
	proto_unregister(&shahp_proto);
	return -1;
}

static void __exit netproto_exit(void)
{
	pdebug();

	sock_unregister(AF_INET_SHAHP);
	proto_unregister(&shahp_proto);
}

module_init(netproto_init);
module_exit(netproto_exit);

MODULE_DESCRIPTION("SHAHP Protocol");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

