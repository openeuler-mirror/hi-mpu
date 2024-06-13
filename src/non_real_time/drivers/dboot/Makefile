include $(SDK_ROOT)/Makefile.cfg

obj-m := dboot_drv.o
dboot_drv-objs := dboot_ker.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='dboot' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
