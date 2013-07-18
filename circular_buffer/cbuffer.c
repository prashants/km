/*
 * cbuffer - Circular buffers demo
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

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

