/*
 * simple_char -Character device demo
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
MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

