/*
 * client - Monitor inotify events
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
#include <sys/inotify.h>

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN	1024

int main(int argc, char *argv[])
{
	int fd, wd;
	pid_t pid;
	int len, i;
	char buf[BUF_LEN];

	fd = inotify_init();
	if (fd <= 0) {
		perror("inotify init");
	}

	wd = inotify_add_watch(fd, "/dev/userspace", IN_ALL_EVENTS); //IN_MODIFY | IN_CREATE | IN_DELETE);
	if (wd < 0) {
		perror("inotify add watch");
	}

	while (1) {
		len = read(fd, buf, BUF_LEN);

		i = 0;
		while (i < len) {
			struct inotify_event *event = (struct inotify_event *)&buf[i];
			printf("inotify: %d\n", event->mask);
			i += EVENT_SIZE + event->len;
		}
	}

	inotify_rm_watch(fd, wd);
	close(fd);
}

