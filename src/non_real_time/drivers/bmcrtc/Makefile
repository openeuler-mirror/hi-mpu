include $(SDK_ROOT)/Makefile.cfg

obj-m := bmcrtc_drv.o
bmcrtc_drv-objs := bmcrtc.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='bmcrtc' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
