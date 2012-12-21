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

#include <stdio.h>
#include <stdlib.h>
#include <netlink/netlink.h>

#define MSG_TYPE	100

int main(int argc, char *argv[])
{
	struct nl_sock *nls;
	char *msg = "Hello World";
	int ret;

	nls = nl_socket_alloc();
	if (!nls) {
		printf("err: nl_socket_alloc\n");
		return EXIT_FAILURE;
	}

	ret = nl_connect(nls, NETLINK_USERSOCK);
	if (ret < 0) {
		printf("err: nl_connect\n");
		nl_socket_free(nls);
		return EXIT_FAILURE;
	}

	ret = nl_send_simple(nls, MSG_TYPE, 0, msg, 11);
	if (ret < 0) {
		printf("err: nl_send_simple\n");
		nl_close(nls);
		nl_socket_free(nls);
		return EXIT_FAILURE;
	}

	printf("sent %d bytes\n", ret);

	nl_close(nls);
	nl_socket_free(nls);
	return 0;
}

