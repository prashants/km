/*
 * sha1 - SHA1 of a string
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

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

