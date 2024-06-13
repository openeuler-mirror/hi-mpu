include $(SDK_ROOT)/Makefile.cfg

obj-m := sfc0_drv.o
sfc0_drv-objs := ../sfc350.o \
                 ../sfc351_godbox.o \
                 ../spi351_ids.o \
                 ../sfc0_core.o \
                 ../sfc351_spi_ids.o \
                 ../spi_flash_devices_ops.o \
                 ../sfc_api.o \
                 ../sfc_osadp.o \
                 ../sfc_sys_func.o

EXTRA_CFLAGS += -DSFC0_COMPILE
EXTRA_CFLAGS += -I..
MODULE_MAKE=$(shell pwd)

all:
	make -C${KERNEL_PATH} M=$(MODULE_MAKE) modules
clean:
	make -C${KERNEL_PATH} M=${MODULE_MAKE} clean
