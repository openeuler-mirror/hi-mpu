include $(SDK_ROOT)/Makefile.cfg

obj-m := trng_drv.o
trng_drv-objs := trng_ker.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='trng' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
