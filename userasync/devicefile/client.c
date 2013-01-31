/*
 * client - Client to read from char device in blocking mode
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

int main(int argc, char *argv[])
{
	int fd;
	int c;
	int ret;
	char buf[5];

	// fd = open("/dev/userspace", O_RDONLY | O_NONBLOCK);
	fd = open("/dev/userspace", O_RDONLY);
	if (fd <= 0) {
		perror("error opening device file");
	}

	for (c = 0; c < 1000; c++) {
		memset(buf, 0x00, 4);
		ret = read(fd, buf, 4);
		buf[4] = '\0';
		if (ret > 0)
			printf("%s %d\n", buf, ret);
		else
			printf("No data\n");
		sleep(1);
	}

	close(fd);
}

