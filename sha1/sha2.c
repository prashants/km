/*
 * sha2 - SHA1 of a large buffer
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

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/crypto.h>
#include <linux/err.h>
#include <linux/version.h>
#include <linux/scatterlist.h>
#include <linux/buffer_head.h>
#include <linux/blkdev.h>

#define SHA1_LENGTH		20
#define MAX_LEN			10485760

int plaintext_to_sha1(unsigned char *hash,
		const char *plaintext, unsigned int len)
{
	struct page *page;
	char *data;
	int c;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 19)

	struct crypto_hash *tfm;
	struct scatterlist sg;
	struct hash_desc desc;
	printk(KERN_ERR "sha1: %s\n", __FUNCTION__);

	page = alloc_page(GFP_KERNEL);
	if (!page) {
		return -ENOMEM;
	}
	data = (char *)page_address(page);

	tfm = crypto_alloc_hash("sha1", 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(tfm)) {
		__free_page(page);
		return -EINVAL;
	}

	desc.tfm = tfm;
	desc.flags = 0;

	crypto_hash_init(&desc);

	for (c = 0; c < MAX_LEN; c += PAGE_SIZE) {
		memset(data, 0x00, PAGE_SIZE);
		if ((MAX_LEN - c) <= PAGE_SIZE) {
			memcpy(data, plaintext + c, (MAX_LEN - c));
			sg_init_one(&sg, (void *)data, (MAX_LEN - c));
			crypto_hash_update(&desc, &sg, (MAX_LEN - c));
		} else {
			memcpy(data, plaintext + c, PAGE_SIZE);
			sg_init_one(&sg, (void *)data, PAGE_SIZE);
			crypto_hash_update(&desc, &sg, PAGE_SIZE);
		}
	}

	crypto_hash_final(&desc, hash);

	crypto_free_hash(tfm);

#else

	struct crypto_tfm *tfm;
	struct scatterlist sg;
	printk(KERN_ERR "sha1: %s\n", __FUNCTION__);

	page = alloc_page(GFP_KERNEL);
	if (!page) {
		return -ENOMEM;
	}
	data = (char *)page_address(page);

	tfm = crypto_alloc_tfm("sha1", CRYPTO_TFM_REQ_MAY_SLEEP);
	if (tfm == NULL) {
		__free_page(page);
		return -EINVAL;
	}

	crypto_digest_init(tfm);

	for (c = 0; c < MAX_LEN; c += PAGE_SIZE) {
		memset(data, 0x00, PAGE_SIZE);
		if ((MAX_LEN - c) <= PAGE_SIZE) {
			memcpy(data, plaintext + c, (MAX_LEN - c));
			sg_init_one(&sg, (u8 *)data, (MAX_LEN - c));
			crypto_digest_update(tfm, &sg, 1);
		} else {
			memcpy(data, plaintext + c, PAGE_SIZE);
			sg_init_one(&sg, (u8 *)data, PAGE_SIZE);
			crypto_digest_update(tfm, &sg, 1);
		}
	}
	crypto_digest_final(tfm, hash);
	crypto_free_tfm(tfm);

#endif

	__free_page(page);
	return 0;
}

static int __init sha1_init(void)
{
	unsigned char output[SHA1_LENGTH];
	unsigned char *buf;
	int i, ret;
	printk(KERN_ERR "sha1: %s\n", __FUNCTION__);

	buf = vmalloc(MAX_LEN);
	if (!buf) {
		return -ENOMEM;
	}

	memset(buf, 0x0A, MAX_LEN);
	memset(output, 0x00, SHA1_LENGTH);

	ret = plaintext_to_sha1(output, buf, MAX_LEN);

	for (i = 0; i < 20; i++) {
		printk(KERN_ERR "%02d - %d\n", i, output[i]);
	}

	if (buf)
		vfree(buf);

	return 0;
}

static void __exit sha1_exit(void)
{
	printk(KERN_ERR "sha1: %s\n", __FUNCTION__);
}

module_init(sha1_init);
module_exit(sha1_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

