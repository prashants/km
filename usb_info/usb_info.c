/*
 * usb_info - Display USB information demo
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

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#define VENDOR_ID	0x0951
#define PRODUCT_ID	0x1607

static struct usb_device *device;

static int usb_info_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	int i = 0;

	iface_desc = interface->cur_altsetting;
	printk(KERN_INFO "USB: Probing device interface %d: (%04X:%04X)\n",
			iface_desc->desc.bInterfaceNumber, id->idVendor, id->idProduct);
	printk(KERN_INFO "USB: ID->bNumEndpoints: %02X\n", iface_desc->desc.bNumEndpoints);
	printk(KERN_INFO "USB: ID->bInterfaceClass: %02X\n", iface_desc->desc.bInterfaceClass);
	for (i = 0; i < iface_desc->desc.bNumEndpoints; i++) {
		endpoint = &iface_desc->endpoint[i].desc;
		printk(KERN_INFO "USB: ED[%d]->bEndpointAddress 0x%02X\n", i, endpoint->bEndpointAddress);
		printk(KERN_INFO "USB: ED[%d]->bmAttributes 0x%02X\n", i, endpoint->bmAttributes);
		printk(KERN_INFO "USB: ED[%d]->wMaxPacketSize 0x%04X (%d)\n",
				i, endpoint->wMaxPacketSize, endpoint->wMaxPacketSize);
	}
	device = interface_to_usbdev(interface);
	return 0;
}

static void usb_info_disconnect(struct usb_interface *interface)
{
	printk(KERN_INFO "USB: Disconnecting device interface %d\n",
			interface->cur_altsetting->desc.bInterfaceNumber);
}

static struct usb_device_id usb_info_table[] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{} /* Terminating Entry */
};

MODULE_DEVICE_TABLE(usb, usb_info_table);

static struct usb_driver usb_info_driver = {
	.name = "usb_info",
	.probe = usb_info_probe,
	.disconnect = usb_info_disconnect,
	.id_table = usb_info_table,
};

static int __init usb_info_init(void)
{
	return usb_register(&usb_info_driver);
}

static void __exit usb_info_exit(void)
{
	usb_deregister(&usb_info_driver);
}

module_init(usb_info_init);
module_exit(usb_info_exit);

MODULE_DESCRIPTION("USB info driver");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("Me");

