include $(SDK_ROOT)/Makefile.cfg

obj-m := msg_scm3_drv.o
msg_scm3_drv-objs := msg_scm3.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='msg_scm3' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
