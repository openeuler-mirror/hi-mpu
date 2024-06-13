include $(SDK_ROOT)/Makefile.cfg

obj-m := log_drv.o
log_drv-objs := log.o kbox.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='log' modules
clean:
	make   -C${KERNEL_PATH} M=${MODULE_MAKE} clean
