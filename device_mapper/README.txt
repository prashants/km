How to use this device mapper :

* Compile and install the module

* Edit the table.txt file with correct values
  Format : <start_sector> <number_of_sectors> test_target <block_device> <offset>

  Note : Block device size can be calculated with
  $sudo blockdev --getsize <block_device>

* Add the device mapper target
  $sudo dmsetup create testdm table.txt
  $sudo dmsetup remove testdm

* Check if a new block device is available as /dev/dm-0 or something similar

