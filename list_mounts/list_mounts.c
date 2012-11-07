/*
 * list_mounts - Print the list of mounted partitions
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

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

