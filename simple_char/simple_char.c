/*************************************************************************
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>

static char char_buf = 'A';	/* single byte character device buffer */
static int char_major = 0;	/* character device major number */

ssize_t char_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	printk(KERN_INFO "mychar: %s\n", __FUNCTION__);
	/* transfer single byte of data to user space */
	copy_to_user(buf, &char_buf, 1);
	/* changing read position and returning number of bytes read */
	if (*f_pos == 0) {
		*f_pos += 1;
		return 1;
	} else {
		return 0;
	}
	return 1;
}

ssize_t char_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	printk(KERN_INFO "mychar: %s\n", __FUNCTION__);
	/* transfer single byte of data from user space */
	copy_from_user(&char_buf, buf, 1);
	/* changing read number of bytes read */
	return count;
}

int char_release(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "mychar: %s\n", __FUNCTION__);
	return 0;
}

int char_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "mychar: %s\n", __FUNCTION__);
	return 0;
}

/* operations that can be performed on the device */
struct file_operations char_fops = {
	.owner = THIS_MODULE,
	.read = char_read,
	.write = char_write,
	.open = char_open,
	.release = char_release
};

/* This function is called when the module is loaded */
static int __init char_init(void)
{
	printk(KERN_INFO "mychar: %s\n", __FUNCTION__);
	char_major = register_chrdev(0, "mychar", &char_fops);
	if (char_major < 0)
		return char_major;
	printk(KERN_INFO "mychar: registered with major number: %d\n", char_major);
	return 0;
}

/* This function is called when the module is unloaded */
static void __exit char_exit(void)
{
	printk(KERN_INFO "mychar: %s\n", __FUNCTION__);
	if (char_major > 0)
		unregister_chrdev(char_major, "mychar");
}

module_init(char_init);
module_exit(char_exit);

MODULE_DESCRIPTION("My first character module");
MODULE_AUTHOR("Me");
MODULE_LICENSE("GPL");

