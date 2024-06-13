include $(SDK_ROOT)/Makefile.cfg

obj-m := uartconnect_drv.o
uartconnect_drv-objs := uart_connect.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='uart_connect' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
