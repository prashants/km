#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>

#define IRQN 1	/* replace this with the interrupt id from /proc/interrups */

int my_dev_id;
int counter;

static irqreturn_t my_interrupt(int i, void *data)
{
	printk(KERN_INFO "basic_interrupts: %s\n", __FUNCTION__);
	printk(KERN_INFO "basic_interrupts: Interrupt Handler called %d\n", counter);
	counter++;
	return IRQ_NONE;
}

static int __init basic_interrupts_init(void)
{
	printk(KERN_INFO "basic_interrupts: %s\n", __FUNCTION__);
	my_dev_id = 1;
	counter = 1;
	if (request_irq(IRQN, my_interrupt, IRQF_SHARED, "basic_interrupts", &my_dev_id)) {
		printk(KERN_ERR "basic_interrupts: cannot register IRQ %d\n", IRQN);
		return -EIO;
	}
	printk(KERN_INFO "basic_interrupts: registered IRQ %d\n", IRQN);
	return 0;
}

static void __exit basic_interrupts_exit(void)
{
	printk(KERN_INFO "basic_interrupts: %s\n", __FUNCTION__);
	free_irq(IRQN, &my_dev_id);
	return;
}

module_init(basic_interrupts_init);
module_exit(basic_interrupts_exit);

MODULE_DESCRIPTION("Basic interrupt module");
MODULE_AUTHOR("Me");
MODULE_LICENSE("GPL");
