/*
 * client - Userspace client to verify the SHA1
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
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <openssl/sha.h>
#include <openssl/md5.h>

/**
 * Include the openssl library -lcrypto
 * gcc client.c -o client -lcrypto
 */

int main(void)
{
	unsigned long c;
	char buf[10];
	struct timeval start, end;
	unsigned char sha1_hash[SHA_DIGEST_LENGTH];

	memset(buf, 'A', 10);
	SHA1(buf, 10, sha1_hash);

	for (c = 0; c < 20; c++)
		printf("%d-%lu\n", sha1_hash[c], c);

	return 0;
}

