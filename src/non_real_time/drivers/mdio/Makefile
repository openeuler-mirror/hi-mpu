include $(SDK_ROOT)/Makefile.cfg

obj-m := mdio_drv.o
mdio_drv-objs := mdio_hi1711.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='mdio' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean