include $(SDK_ROOT)/Makefile.cfg

obj-m := i2c_hi309x_drv.o
i2c_hi309x_drv-objs := i2c_hi309x.o

MODULE_MAKE := $(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='i2c_hi309x' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
