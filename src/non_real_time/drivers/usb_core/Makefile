USB_OPEN_SOURCE_DIR=$(SRC_DIR)/../open_source/$(LINUX_PATH)/drivers/usb

KERNEL_VERSION_4_4=4.4
KERNEL_VERSION_5_10=5.10

USB_UDC_DIR_PATH=$(USB_OPEN_SOURCE_DIR)/gadget/udc

UDC_DIR=udc

ccflags-y += -I$(SDK_ROOT)/include/ -I$(SDK_ROOT)/include/usr_api -I$(SDK_ROOT)/comm -I$(SRC_DIR)/include
ccflags-y += -I$(SDK_ROOT)/usb
ccflags-y += -DSDK_VER=\"$(SDK_VERSION)\"
ccflags-y += -fstack-protector-strong -Wl,-z,relro -z,now -Wl,-z,noexecstack -Wformat=2 -Wfloat-equal
ccflags-y += -DCONFIG_USB_GADGET

obj-m += udc-core.o

SRC := $(shell pwd)
ifeq ($(KERNEL_VERSION_MAIN), $(KERNEL_VERSION_4_4))
udc-core-y := $(UDC_DIR)/udc-core.o
KBUILD_EXTRA_SYMBOLS += ${SRC}/Module.symvers
export KBUILD_EXTRA_SYMBOLS
else
udc-core-y := $(UDC_DIR)/core.o $(UDC_DIR)/trace.o
endif

all:
	cp -rf $(USB_UDC_DIR_PATH) .
ifeq (${KERNEL_VERSION_MAIN},${KERNEL_VERSION_5_10})
	cp $(UDC_DIR)/trace.h ${KERNEL_PATH}/include/trace
endif
	make -j${COMPILE_PROCESS_NUM} -C $(KERNEL_PATH)  M="usb_core" modules

clean: $(dirs)
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
	rm -rf $(UDC_DIR)
