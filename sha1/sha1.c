/*
 * sha1 - SHA1 of a string
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
#include <linux/scatterlist.h>

#define SHA1_LENGTH		20

static int __init sha1_init(void)
{
	struct scatterlist sg;
	struct crypto_hash *tfm;
	struct hash_desc desc;
	unsigned char output[SHA1_LENGTH];
	unsigned char buf[10];
	int i;

	printk(KERN_INFO "sha1: %s\n", __FUNCTION__);

	memset(buf, 'A', 10);
	memset(output, 0x00, SHA1_LENGTH);

	tfm = crypto_alloc_hash("sha1", 0, CRYPTO_ALG_ASYNC);

	desc.tfm = tfm;
	desc.flags = 0;

	sg_init_one(&sg, buf, 10);
	crypto_hash_init(&desc);

	crypto_hash_update(&desc, &sg, 10);
	crypto_hash_final(&desc, output);

	for (i = 0; i < 20; i++) {
		printk(KERN_ERR "%d-%d\n", output[i], i);
	}

	crypto_free_hash(tfm);

	return 0;
}

static void __exit sha1_exit(void)
{
	printk(KERN_INFO "sha1: %s\n", __FUNCTION__);
}

module_init(sha1_init);
module_exit(sha1_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

