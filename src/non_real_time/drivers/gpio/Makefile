include $(SDK_ROOT)/Makefile.cfg

obj-m := gpio_drv.o
gpio_drv-objs := gpio_hi309x.o gpio_pinmux_tbl.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='gpio' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
