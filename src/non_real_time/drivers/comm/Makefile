include $(SDK_ROOT)/Makefile.cfg

obj-m := comm_drv.o
comm_drv-objs := a5x_subsys.o comm.o host_subsys.o hp_subsys.o lp_subsys.o m_reset.o sysctrl.o io_config.o bdinfo.o usr_data.o

MODULE_MAKE=$(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='comm' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
