include $(SDK_ROOT)/Makefile.cfg

obj-m := devmem_drv.o
devmem_drv-objs := devmem.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='devmem' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
