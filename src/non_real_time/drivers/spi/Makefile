include $(SDK_ROOT)/Makefile.cfg

obj-m := spi_drv.o
spi_drv-objs := hi_spi.o spi_hi1710_process.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='spi' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
