/*
 * proc - Proc filesystem entry demo
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
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

static struct proc_dir_entry *myproc;

static char mybuf[256];

static int myproc_read(char *buf, char **start, off_t offset,
	int count, int *eof, void *data)
{
	int len;
	printk(KERN_INFO "proc: %s\n", __FUNCTION__);

	printk(KERN_INFO "proc: off=(%lu) count=(%d)\n",
		(unsigned long)offset, count);

	if (count > 255)
		count = 255;

	len = snprintf(buf, count, "%s", mybuf);
	return len;
}

static int myproc_write(struct file *f, const char *buf,
	unsigned long count, void *data)
{
	printk(KERN_INFO "proc: %s\n", __FUNCTION__);

	printk(KERN_INFO "proc: count=(%lu)\n", count);

	if (count > 255)
		count = 255;
	copy_from_user(mybuf, buf, count);
	mybuf[count] = '\0';
	return count;
}

static int __init proc_init(void)
{
	printk(KERN_INFO "proc: %s\n", __FUNCTION__);

	myproc = create_proc_entry("myproc", 0666, 0);
	if (!myproc) {
		printk(KERN_ERR "proc: error creating proc entry\n");
	}
	myproc->read_proc = myproc_read;
	myproc->write_proc = myproc_write;

	return 0;
}

static void __exit proc_exit(void)
{
	printk(KERN_INFO "proc: %s\n", __FUNCTION__);
	if (myproc) {
		remove_proc_entry("myproc", NULL);
	}
}

module_init(proc_init);
module_exit(proc_exit);

/* Note: Actual license is PUBLIC DOMAIN but since its compatible
 * with GPL so I have substituted "GPL" string here. Currently linux kernel
 * doesnt support public domain license
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");

