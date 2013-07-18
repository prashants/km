/*
 * c_client - Linux C client to access the kernel mmap area
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
	unsigned long size = 1024 * 1024 * 500;
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
		printf("ERROR : Failed to allocate memory");
		return 1;
	}

	/* Format : mmap(addr, length, prot, flags, fd, offset) */
	data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
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

