/*
 * jprobe - jprobe demo
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
#include <linux/kprobes.h>
#include <linux/blkdev.h>

static struct jprobe jp;
static unsigned int counter = 0;

static int Callback_handler(struct request_queue *q, struct request *rq) /* since hooking to sd_prep_fn function */
{
	printk(KERN_INFO "myjprobe: %s %d\n", __FUNCTION__, rq->cpu);
	jprobe_return();
	return 0;
}

static int Pre_handler(struct kprobe *p, struct pt_regs *regs)
{
	printk(KERN_INFO "myjprobe: %s = %d\n", __FUNCTION__, counter++);
	return 0;
}

static void Post_handler(struct kprobe *p, struct pt_regs *regs, unsigned long flags)
{
	printk(KERN_INFO "myjprobe: %s = %d\n", __FUNCTION__, counter++);
}

static int __init myjprobe_init(void)
{
	printk(KERN_INFO "myjprobe: %s\n", __FUNCTION__);

	jp.kp.pre_handler = Pre_handler;
	jp.kp.post_handler = Post_handler;
	jp.kp.addr = (kprobe_opcode_t *)0xffffffff81445ce0;	/* from /proc/kallsyms */
	jp.entry = (kprobe_opcode_t *)Callback_handler;
	register_jprobe(&jp);

	return 0;
}

static void __exit myjprobe_exit(void)
{
	printk(KERN_INFO "myjprobe: %s\n", __FUNCTION__);
	unregister_jprobe(&jp);
}

module_init(myjprobe_init);
module_exit(myjprobe_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

