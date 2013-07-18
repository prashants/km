/*
 * list - Link list demo
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

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

