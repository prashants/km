/*
 * jprobe - jprobe demo
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

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

