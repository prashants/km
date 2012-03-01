obj-m := ndasblock.o

KERNELDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order  Module.symvers

re:
	sudo rmmod si700x
	make clean
	$(MAKE) -C $(KERNELDIR) M=$(PWD)
	sudo insmod ./si700x.ko
	sudo ./test

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend

ifeq (.depend,$(wildcard .depend))
	include .depend
endif
