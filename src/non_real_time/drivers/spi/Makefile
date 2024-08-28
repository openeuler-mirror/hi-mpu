include $(SDK_ROOT)/Makefile.cfg

obj-m := spi_hi309x_drv.o
spi_hi309x_drv-objs := spi_hi309x.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='spi' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
