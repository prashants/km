/*
 * client - Linux client to access the kernel mmap area
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

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define DEVICE_FILE "./mmapdev"		/* Update this to the demo_mmap char device file path */

int main(void)
{
	char* data;
	int fd = open(DEVICE_FILE, O_RDWR);
	if (fd < 0) {
		perror("Failed to open demo_mmap char device file");
	}
	data = mmap(0, 0x00300000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0xd0000000);
	close(fd);
	munmap(data, 0x00300000);
	return 0;
}
