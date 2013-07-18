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

