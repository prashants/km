/*************************************************************************
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

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

static unsigned int shahp_poll(struct file *file, struct socket *sock, struct poll_table_struct *wait)
{
	pdebug();
	return 0;
}

static int shahp_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	pdebug();
	return 0;
}

static int shahp_listen(struct socket *sock, int len)
{
	pdebug();
	return 0;
}

static int shahp_shutdown(struct socket *sock, int flags)
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

/*****************************************************************************/
/******************************* PROTO ***************************************/
/*****************************************************************************/

static void shahp_close(struct sock *sk, long timeout)
{
	pdebug();
}

static int shahp_bind1(struct sock *sk, struct sockaddr *addr, int len)
{
	pdebug();
	return 0;
}

static int shahp_connect1(struct sock *sk, struct sockaddr *addr, int addr_len)
{
	pdebug();
	return 0;
}

static int shahp_disconnect(struct sock *sk, int flags)
{
	pdebug();
	return 0;
}

static int shahp_ioctl1(struct sock *sk, int cmd, unsigned long arg)
{
	pdebug();
	return 0;
}

static int shahp_init(struct sock *sk)
{
	pdebug();
	return 0;
}

static void shahp_destroy(struct sock *sk)
{
	pdebug();
}

static int shahp_setsockopt(struct sock *sk, int level, int optname, char __user *optval, unsigned int optlen)
{
	pdebug();
	return 0;
}

static int shahp_getsockopt(struct sock *sk, int level, int optname, char __user *optval, int __user *option)
{
	pdebug();
	return 0;
}

static int shahp_sendmsg1(struct kiocb *iocb, struct sock *sk, struct msghdr *msg, size_t len)
{
	pdebug();
	return 0;
}

static int shahp_recvmsg1(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
		size_t len, int noblock, int flags, int *addr_len)
{
	pdebug();
	return 0;
}

static void shahp_hash(struct sock *sk)
{
	pdebug();
}

static void shahp_unhash(struct sock *sk)
{
	pdebug();
}

static int shahp_get_port(struct sock *sk, unsigned short snum)
{
	pdebug();
	return 0;
}


/*****************************************************************************/

static int shahp_create_socket(struct net *net, struct socket *sock, int protocol, int kern)
{
	struct sock *sk;

	pdebug();

	return 0;

	sk = sk_alloc(net, AF_INET_SHAHP, GFP_KERNEL, &shahp_proto);
	if (!sk) {
		pmsg("failed to allocate socket\n");
		return -ENOMEM;
	}
	sock_init_data(sock, sk);
	sk->sk_protocol = 0x0;
	sock->ops = &shahp_ops;
	sock->state = SS_UNCONNECTED;
	return 0;
}


/* protocol specific socket structure */
struct shahp_sock {
};

struct proto shahp_proto = {
	.name = "SHAHP",
	.owner = THIS_MODULE,
	.close = shahp_close,
	.bind = shahp_bind1,
	.connect = shahp_connect1,
	.disconnect = shahp_disconnect,
	//.accept = shahp_accept,
	//.ioctl = shahp_ioctl1,
	//.init = shahp_init,
	//.destroy = shahp_destroy,
	//.shutdown = shahp_shutdown,
	.setsockopt = shahp_setsockopt,
	.getsockopt = shahp_getsockopt,
	.sendmsg = shahp_sendmsg1,
	.recvmsg = shahp_recvmsg1,
	.hash = shahp_hash,
	.unhash = shahp_unhash,
	//.get_port = shahp_get_port,
	//.enter_memory_pressure = shahp_enter_memory_pressure,
	//.sockets_allocated = &sockets_allocated,
	//.memory_allocated = &memory_allocated,
	//.memory_pressure = &memory_pressure,
	//.orphan_count = &orphan_count,
	//.sysctl_mem = sysctl_tcp_mem,
	//.sysctl_wmem = sysctl_tcp_wmem,
	//.sysctl_rmem = sysctl_tcp_rmem,
	//.max_header = 0,
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
	.socketpair = sock_no_socketpair,
	.accept = shahp_accept,
	.getname = shahp_getname,
	.poll = shahp_poll,
	.ioctl = shahp_ioctl,
	.listen = shahp_listen,
	.shutdown = shahp_shutdown,
	.setsockopt = sock_common_setsockopt,
	.getsockopt = sock_common_getsockopt,
	.sendmsg = shahp_sendmsg,
	.recvmsg = sock_common_recvmsg,
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
MODULE_AUTHOR("Me");
MODULE_LICENSE("GPL");

