/*
 * client - Waits for signal from kernel
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
#include <fcntl.h>
#include <string.h>
#include <signal.h>

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
	long flags;

	/* Get the current pocess pid */
	pid = getpid();
	printf("PID = %lu\n", (unsigned long)pid);

	fd = open("/dev/userspace", O_RDONLY);
	if (fd <= 0) {
		perror("error opening device file");
	}

	/* Mark process as owner of file */
	fcntl(fd, F_SETOWN, pid);

	/* Save current flags */
	flags = fcntl(fd, F_GETFL);

	/* Set FASYNC flag */
	fcntl(fd, F_SETFL, flags | FASYNC);

	/* Setup the signal handler */
	memset(&sigact, '\0', sizeof(sigact));
	sigact.sa_handler = &sighandler;
	if (sigaction(SIGIO, &sigact, NULL) < 0) {
		perror("sigaction");
	}

	while (1) {
	}

	close(fd);
}

