/*
 * curtime - Print current time
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

static int __init curtime_init(void)
{
	struct timeval curtv;

	printk(KERN_INFO "curtime: %s\n", __FUNCTION__);

	do_gettimeofday(&curtv);
	printk(KERN_INFO "curtime: %ld %ld\n", curtv.tv_sec, curtv.tv_usec);

	return 0;
}

static void __exit curtime_exit(void)
{
	printk(KERN_INFO "curtime: %s\n", __FUNCTION__);
}

module_init(curtime_init);
module_exit(curtime_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

