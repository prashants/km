$make
$sudo insmod simple_char.ko

$cat /proc/devices (this will show device registered and the major number)
$dmesg (this will also show device major number)

$sudo mknod <file_name> c <major_number) 0 (create the device file)
$sudo chmod o+rw <file_name> (change permissions to allow normal users to read/write the file)

$echo "B" > <file_name> (write something to the device)
$cat <file_name> (read it back from the device)

$sudo rmmod simple_char
$cat /proc/devices (device gone)
$make clean

