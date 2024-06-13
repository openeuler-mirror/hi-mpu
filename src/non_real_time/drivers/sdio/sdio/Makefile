obj-m += sdio_drv.o
sdio_drv-objs := ../himci.o

EXTRA_CFLAGS += -I.. -I$(SDK_ROOT)/include/ -I$(SDK_ROOT)/include/register -I$(SDK_ROOT)/include/usr_api -I$(SDK_ROOT)/comm -I$(SRC_DIR)/include
MODULE_MAKE := $(shell pwd)

all:
	make -C${KERNEL_PATH} M=$(MODULE_MAKE) modules
clean:
	make -C${KERNEL_PATH} M=$(MODULE_MAKE) clean
