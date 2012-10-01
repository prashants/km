/*
 * slab allocator - Slab allocator demo 
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
#include <linux/slab.h>

static struct kmem_cache *mycache;

#define OBJECT_COUNT	10
#define OBJECT_SIZE	PAGE_SIZE

void *objs[OBJECT_COUNT];

/* cache object constructor */
static void mycache_constructor(void *ptr)
{
	printk(KERN_DEBUG "slab: %s\n", __FUNCTION__);
	memset(ptr, 0, OBJECT_SIZE);
}

static int __init slab_init(void)
{
	int c = 0;
	printk(KERN_DEBUG "slab: %s\n", __FUNCTION__);

	/* create the slab cache */
	mycache = kmem_cache_create("mycache", OBJECT_SIZE, 0, SLAB_HWCACHE_ALIGN,
			mycache_constructor);
	if (mycache) {
		printk(KERN_INFO "slab: allocated slab cache\n");
	 } else {
		printk(KERN_ERR "slab: failed to allocate slab cache\n");
		return -ENOMEM;
	 }

	/* allocate objects within the slab cache */
	for (c = 0; c < OBJECT_COUNT; c++) {
		objs[c] = kmem_cache_alloc(mycache, GFP_KERNEL);
		if (objs[c]) {
			printk(KERN_INFO "slab: allocated cache object numbered %d\n", c);
		} else {
			printk(KERN_ERR "slab: failed to allocate cache object numbered %d\n", c);
			return 0;
		}
	}

	printk(KERN_INFO "slab: each cache is of size %d\n", kmem_cache_size(mycache));

	return 0;
}

static void __exit slab_exit(void)
{
	int c = 0;
	printk(KERN_DEBUG "slab: %s\n", __FUNCTION__);

	/* destroy objects within the slab cache */
	for (c = 0; c < OBJECT_COUNT; c++) {
		if (objs[c]) {
			kmem_cache_free(mycache, objs[c]);
			printk(KERN_INFO "slab: de-allocated cache object numbered %d\n", c);
		}
	}

	/* destroy the slab cache */
	if (mycache) {
		kmem_cache_destroy(mycache);
		printk(KERN_INFO "slab: de-allocated slab cache\n");
	}
}

module_init(slab_init);
module_exit(slab_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

