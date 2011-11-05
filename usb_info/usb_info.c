/**************************************************************************
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
 *************************************************************************/

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
MODULE_AUTHOR("Me");
MODULE_LICENSE("GPL");

