include $(SDK_ROOT)/Makefile.cfg

obj-m += sfc0/
obj-m += sfc1/

MODULE_MAKE=$(shell pwd)

all:
	make -C${KERNEL_PATH} M='sfccom' modules
clean:
	make -C${KERNEL_PATH} M=${MODULE_MAKE} clean
