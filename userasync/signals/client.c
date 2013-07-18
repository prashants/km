/*
 * client - Waits for signal from kernel
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
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#include "ioctl.h"

int sig_count = 0;

/* Signal handler */
static void sighandler(int sig)
{
	sig_count++;
	printf("Signal received %d\n", sig_count);
}

int main(int argc, char *argv[])
{
	int fd;
	pid_t pid;
	int ret;
	char buf[5];
	struct sigaction sigact;

	/* Get the current pocess pid */
	pid = getpid();
	printf("PID = %lu\n", (unsigned long)pid);

	fd = open("/dev/userspace", O_RDONLY);
	if (fd <= 0) {
		perror("error opening device file");
	}

	/* Inform kernel of the client process ID to send the signal to */
	ret = ioctl(fd, PROCESS_ID, (unsigned long)pid);
	if (ret < 0) {
		perror("ioctli failed");
	}

	close(fd);

	/* Setup the signal handler */
	memset(&sigact, '\0', sizeof(sigact));
	sigact.sa_handler = &sighandler;
	if (sigaction(SIGIO, &sigact, NULL) < 0) {
		perror("sigaction");
	}

	while (1) {
	}
}

