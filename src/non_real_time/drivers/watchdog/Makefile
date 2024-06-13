include $(SDK_ROOT)/Makefile.cfg

obj-m := watchdog_drv.o
watchdog_drv-objs := watchdog.o

MODULE_MAKE := $(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='watchdog' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
