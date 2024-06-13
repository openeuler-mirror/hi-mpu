include $(SDK_ROOT)/Makefile.cfg

obj-m := hw_lock_drv.o
hw_lock_drv-objs := hw_lock.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='hw_lock' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
