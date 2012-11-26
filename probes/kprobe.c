/*
 * kprobe - kprobe demo
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

static struct kprobe kp;
static unsigned int counter = 0;

static int Pre_handler(struct kprobe *p, struct pt_regs *regs)
{
	printk(KERN_INFO "mykprobe: %s = %d\n", __FUNCTION__, counter++);
	return 0;
}

static void Post_handler(struct kprobe *p, struct pt_regs *regs, unsigned long flags)
{
	printk(KERN_INFO "mykprobe: %s = %d\n", __FUNCTION__, counter++);
}

static int __init mykprobe_init(void)
{
	printk(KERN_INFO "mykprobe: %s\n", __FUNCTION__);

	kp.pre_handler = Pre_handler;
	kp.post_handler = Post_handler;
	kp.addr = (kprobe_opcode_t *)0xffffffff81445ce0;	/* from /proc/kallsyms */
	register_kprobe(&kp);

	return 0;
}

static void __exit mykprobe_exit(void)
{
	printk(KERN_INFO "mykprobe: %s\n", __FUNCTION__);
	unregister_kprobe(&kp);
}

module_init(mykprobe_init);
module_exit(mykprobe_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

