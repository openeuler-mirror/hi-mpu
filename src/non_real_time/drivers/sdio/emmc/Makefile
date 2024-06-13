obj-m += emmc_drv.o
emmc_drv-objs := ../himci.o

EXTRA_CFLAGS += -I.. -I$(SDK_ROOT)/include/ -I$(SDK_ROOT)/include/register -I$(SDK_ROOT)/include/usr_api -I$(SDK_ROOT)/comm -I$(SRC_DIR)/include
EXTRA_CFLAGS += -DEMMC_COMPILE

MODULE_MAKE := $(shell pwd)

all:
	make -C${KERNEL_PATH} M=$(MODULE_MAKE) modules
clean:
	make -C${KERNEL_PATH} M=$(MODULE_MAKE) clean