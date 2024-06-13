include $(SDK_ROOT)/Makefile.cfg

obj-m := localbus_drv.o
localbus_drv-objs := local_bus_ker.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='local_bus' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
