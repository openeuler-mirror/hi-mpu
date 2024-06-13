include $(SDK_ROOT)/Makefile.cfg

obj-m := adc_drv.o
adc_drv-objs := adc_ker.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='adc' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
