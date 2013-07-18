/*
 * slab allocator - Slab allocator demo 
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

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

