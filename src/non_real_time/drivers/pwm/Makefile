include $(SDK_ROOT)/Makefile.cfg

obj-m := pwm_drv.o
pwm_drv-objs := pwm_ker.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='pwm' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
