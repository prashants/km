/*
 * list_mounts - Print the list of mounted partitions
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
#include <linux/random.h>
#include <linux/list.h>
#include <linux/namespace.h>
#include <linux/fs.h>

static int __init list_mounts_init(void)
{
	struct namespace *curns = current->namespace;
	struct vfsmount *mnt;
	struct list_head *p;
	printk(KERN_INFO "list_mounts: %s\n", __FUNCTION__);

	/* traversing list of mounted partitions in current namespace */
	list_for_each(p, &curns->list) {
		/* get the vfsmount stucture */
		mnt = list_entry(p, struct vfsmount, mnt_list);
		if (mnt) {
			printk(KERN_INFO "found partition: mount_count=%d\n",
				atomic_read(&mnt->mnt_count));
		}
	}

	return 0;
}

static void __exit list_mounts_exit(void)
{
	printk(KERN_INFO "list_mounts: %s\n", __FUNCTION__);
}

module_init(list_mounts_init);
module_exit(list_mounts_exit);

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

