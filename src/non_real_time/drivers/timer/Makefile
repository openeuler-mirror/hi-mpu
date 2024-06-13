include $(SDK_ROOT)/Makefile.cfg

obj-m := hitimer_drv.o
hitimer_drv-objs := timer.o timer_core.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='timer' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
