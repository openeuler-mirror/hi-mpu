include $(SDK_ROOT)/Makefile.cfg

obj-m := i2c_drv.o
i2c_drv-objs := i2c.o i2c_api.o i2c_core.o i2c_smbus_core.o i2c_dw.o i2c_dw_master.o i2c_dw_master_dma.o \
				i2c_dw_slave.o i2c_slave_eeprom.o i2c_slave_smbus.o i2c_platdrv_a55.o i2c_dev.o

MODULE_MAKE := $(PWD)

all:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M='i2c' modules
clean:
	make -j${COMPILE_PROCESS_NUM} -C${KERNEL_PATH} M=${MODULE_MAKE} clean
