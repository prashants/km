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
#include <string.h>

#define DEVICE_FILE	"./mmapdev"	/* Update this to the demo_mmap char device file path */
#define PAGE_SIZE	4096
#define N_PAGES		1000

/*
 * Some random data that is copied to shared memory and replicated by kernel
 * througout the shared memory area
 */
#define COUNTER 	25

int main(void)
{
	unsigned char* data;
	int c;
	unsigned char blockdata[PAGE_SIZE * N_PAGES];
	int fd = open(DEVICE_FILE, O_RDWR);

	if (fd < 0) {
		perror("Failed to open demo_mmap char device file");
	}

	/*
	 * format : mmap(addr, length, prot, flags, fd, offset)
	 * BUG: Boot with 'nopat' kernel option if you get this error :
	 * map pfn expected mapping type uncached-minus for xx-xx, got write-back
	 */
	data = mmap(NULL, PAGE_SIZE * N_PAGES, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	printf("mmap address : %p\n", data);

	/*
	 * set the first byte of shared memory to COUNTER, on next mmap open this
	 * value is replicated in all the shared memory by kernel. This also
	 * checks if we can write to shared memory
	 */
	*data = COUNTER;

	// Read each byte of shared memory directly and it's correct if all
	// values are equal to COUNTER
	printf("Byte Copy : %d", *data);
	for (c = 0; c < PAGE_SIZE * N_PAGES; c++) {
		if (*(data + c) != COUNTER)
			printf("Invalid data at %d : %d\n", c, *(data + c));
	}
	printf("\n");

	// Copying data into application buffer from shared memory using memcpy
	printf("*************** Memcpy data ******************\n");

	// initializing memory to 0
	for (c = 0; c < PAGE_SIZE * N_PAGES; c++) {
		blockdata[c] = 0;
	}

	// copying to program memory from shared memory
	memcpy(blockdata, data, PAGE_SIZE * N_PAGES);

	printf("Mem Copy : %d", blockdata[0]);
	for (c = 0; c < PAGE_SIZE * N_PAGES; c++) {
		if (blockdata[c] != COUNTER)	// Program memory data is correct if all values are equal to COUNTER
			printf("Invalid data at %d : %d\n", c, blockdata[c]);
	}
	printf("\n");

	close(fd);
	munmap(data, PAGE_SIZE * N_PAGES);	// Unmapping the shared memory
	return 0;
}
