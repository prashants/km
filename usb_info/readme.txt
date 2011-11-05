1. Edit "/etc/modprobe.d/blacklist" and add following lines to
stop auto-loading the default usb modules and restart machine :
---------------------
blacklist ehci_hcd
blacklist usb-storage
blacklist scsi9
---------------------

2. Edit the "usb_info.c" file and change the vendor id and product id
as per your USB device.
#define VENDOR_ID	0x0951
#define PRODUCT_ID	0x1607

You can locate the vendor and product id with the following command
$sudo lsusb
Bus 002 Device 002: ID 0951:1607 Kingston Technology DataTraveler 100

3. Run the following commands to compile and load the module
$make
$sudo insmod usb_info.ko

$dmesg (this will show USB device information)

$sudo rmmod usb_info
$make clean

