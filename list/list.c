/*
 * list - Link list demo
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

struct player_entry {
	int player_id;
	unsigned short score;
	struct list_head list;
};

struct player_entry player_head;

static int __init mylist_init(void)
{
	struct player_entry *tmp;
	struct list_head *cur;
	int c;
	printk(KERN_INFO "list: %s\n", __FUNCTION__);

	INIT_LIST_HEAD(&player_head.list);

	/* adding elements to list */
	for (c = 1; c <= 20; c++) {
		tmp = (struct player_entry *)kmalloc(sizeof(struct player_entry), GFP_KERNEL);
		if (tmp) {
			get_random_bytes(&tmp->score, sizeof(unsigned short));
			tmp->player_id = c;
			printk(KERN_INFO "list: adding player %d with score %d\n",
					tmp->player_id, tmp->score);
			list_add_tail(&tmp->list, &player_head.list);
		}
	}

	/* traversing list */
	printk(KERN_INFO "list: Printing list...\n");
	list_for_each(cur, &player_head.list) {
		tmp = list_entry(cur, struct player_entry, list);
		printk(KERN_INFO "list: player %02d : score %d\n",
				tmp->player_id, tmp->score);
	}

	return 0;
}

static void __exit mylist_exit(void)
{
	struct player_entry *tmp;
	struct list_head *cur, *q;
	printk(KERN_INFO "list: %s\n", __FUNCTION__);

	/* freeing list */
	list_for_each_safe(cur, q, &player_head.list) {
		tmp = list_entry(cur, struct player_entry, list);
		printk(KERN_INFO "list: deleting player %d\n", tmp->player_id);
		list_del(cur);
		kfree(tmp);
	}
}

module_init(mylist_init);
module_exit(mylist_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

