include $(SDK_ROOT)/Makefile.cfg

obj-m := uart_drv.o
uart_drv-objs := uart_ker.o
obj-m += uart_core.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='uart' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
