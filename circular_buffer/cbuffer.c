/*
 * cbuffer - Circular buffers demo
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
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/circ_buf.h>

struct circ_buf circ;

#define MAX_BUF_SIZE 4096	/* should be always in power of 2 */

/* Adding 1 byte items to circular buffer */
static void producer(void)
{
	if (CIRC_SPACE(circ.head, circ.tail, MAX_BUF_SIZE) >= 1) {
		circ.buf[circ.head] = 1;
		smp_wmb();
		circ.head = (circ.head + 1) & (MAX_BUF_SIZE - 1);
		printk(KERN_INFO "cbuffer: producer = %d\n", circ.buf[circ.tail]);
	}
}

/* Removing 1 byte item from circular buffer */
static void consumer(void)
{
	if (CIRC_CNT(circ.head, circ.tail, MAX_BUF_SIZE) >= 1) {
		smp_read_barrier_depends();
		printk(KERN_INFO "cbuffer: consumer = %d\n", circ.buf[circ.tail]);
		smp_mb();
		circ.tail = (circ.tail + 1) & (MAX_BUF_SIZE - 1);
	}
}

static int __init cbuffer_init(void)
{
	printk(KERN_INFO "cbuffer: %s\n", __FUNCTION__);

	/* allocating buffer space */
	circ.buf = kzalloc(MAX_BUF_SIZE, GFP_KERNEL);
	if (!circ.buf) {
		printk(KERN_INFO "cbuffer: failed to allocate memory for circular buffers\n");
	} else {
		printk(KERN_INFO "cbuffer: allocated circular buffer of size=%d bytes\n",
			CIRC_SPACE(circ.head, circ.tail, MAX_BUF_SIZE));
	}

	return 0;
}

static void __exit cbuffer_exit(void)
{
	printk(KERN_INFO "cbuffer: %s\n", __FUNCTION__);

	if (circ.buf) {
		kfree(circ.buf);
	}
}

module_init(cbuffer_init);
module_exit(cbuffer_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

