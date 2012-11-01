/*
 * c_client - Linux C client to access the kernel mmap area
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEVICE_FILE	"./mmapdev"	/* Update this to the mmap char device file path */

int main(void)
{
	unsigned char *data, *blockdata;
	unsigned long size = 4096 * 50;
	unsigned long c;

	/* Open the char device */
	int fd = open(DEVICE_FILE, O_RDWR);
	if (fd < 0) {
		printf("ERROR : Failed to open mmap char device file.\n");
		printf("Create a device with $mknod ./mmapdev c <major_number> 0\n");
		printf("Major number allocated to the device can be located from the kernel log using $dmesg\n");
		return 1;
	}

	/* Allocate memory for local buffer */
	blockdata = (unsigned char *)malloc(size);
	if (!blockdata) {
		close(fd);
		printf("ERROR : Failed to allocate memory\n");
		return 1;
	}

	/* Format : mmap(addr, length, prot, flags, fd, offset) */
	data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		free(blockdata);
		close(fd);
		printf("ERROR : Failed to mmap memory\n");
		return 1;
	}
	printf("mmap address : %p (%lu)\n", data, size);

	/*
	 * Copy the device mmap memory to local buffer and check if its
	 * correct if all values are equal to 'A'
	 */
	memcpy(blockdata, data, size);
	for (c = 0; c < size; c++) {
		if (*(blockdata + c) != 'A')
			printf("Invalid data at %lu : %d\n", c, *(blockdata + c));
	}
	printf("Verification completed!\n");

	/* Update local buffer to 'B' */
	for (c = 0; c < size; c++) {
		*(blockdata + c) = 'B';
	}

	/* Copy local buffer to device mmap memory */
	memcpy(data, blockdata, size);
	printf("Copy completed!\n");

	/* Unmapping the shared memory */
	munmap(data, size);
	free(blockdata);
	close(fd);
	return 0;
}

