USB_OPEN_SOURCE_DIR=$(SRC_DIR)/../open_source/$(LINUX_PATH)/drivers/usb
USB_KERNEL_SOURCE_DIR=$(SDK_ROOT)/../kernel/drivers/usb

KERNEL_VERSION_4_4=4.4
KERNEL_VERSION_5_10=5.10

USB_COMMON_DIR =$(USB_OPEN_SOURCE_DIR)/common
USB_DWC3_DIR_PATH=$(USB_OPEN_SOURCE_DIR)/dwc3
USB_gadget_DIR_PATH=$(USB_OPEN_SOURCE_DIR)/gadget
CONFIGFS_OPEN_SOURCE_DIR=$(SRC_DIR)/../open_source/$(LINUX_PATH)/fs/configfs
USB_HOST_DIR =$(USB_OPEN_SOURCE_DIR)/host

DWC3_DIR=dwc3
GADGET_DIR=gadget
CONFIGFS_DIR=configfs
COMMON_DIR = common
HOST_DIR = host

ccflags-y += -I$(SDK_ROOT)/include/ -I$(SDK_ROOT)/include/usr_api -I$(SDK_ROOT)/comm -I$(SRC_DIR)/include
ccflags-y += -I$(KERNEL_PATH)/usb -I$(KERNEL_PATH)/usb/dwc3
ccflags-y += -DSDK_VER=\"$(SDK_VERSION)\"
ccflags-y += -fstack-protector-strong -Wl,-z,relro -z,now -Wl,-z,noexecstack -Wformat=2 -Wfloat-equal
ccflags-y += -DCONFIG_USB_DWC3_DUAL_ROLE=1
ccflags-y += -DCONFIG_USB_GADGET_VBUS_DRAW=2

ifeq ($(KERNEL_VERSION_MAIN), $(KERNEL_VERSION_5_10))
ccflags-y += -DCONFIG_USB_GADGET
endif

obj-m += usb_drv.o
obj-m += libcomposite.o
obj-m += configfs.o
obj-m += dwc3.o
obj-m += usb-common.o

usb-common-y      += $(COMMON_DIR)/common.o
usb-common-y      += $(COMMON_DIR)/debug.o

libcomposite-y := $(GADGET_DIR)/usbstring.o
libcomposite-y += $(GADGET_DIR)/config.o
libcomposite-y += $(GADGET_DIR)/epautoconf.o
libcomposite-y += $(GADGET_DIR)/composite.o
libcomposite-y += $(GADGET_DIR)/functions.o
libcomposite-y += $(GADGET_DIR)/configfs.o
libcomposite-y += $(GADGET_DIR)/u_f.o

configfs-y := $(CONFIGFS_DIR)/inode.o
configfs-y += $(CONFIGFS_DIR)/file.o
configfs-y += $(CONFIGFS_DIR)/dir.o
configfs-y += $(CONFIGFS_DIR)/symlink.o
configfs-y += $(CONFIGFS_DIR)/mount.o
configfs-y += $(CONFIGFS_DIR)/item.o

dwc3-y := $(DWC3_DIR)/core.o
ifeq ($(KERNEL_VERSION_MAIN), $(KERNEL_VERSION_4_4))
dwc3-y += $(DWC3_DIR)/debug.o
endif
dwc3-y += $(DWC3_DIR)/trace.o
dwc3-y += $(DWC3_DIR)/host.o
dwc3-y += $(DWC3_DIR)/gadget.o
dwc3-y += $(DWC3_DIR)/ep0.o
dwc3-y += $(DWC3_DIR)/debugfs.o
dwc3-y += $(DWC3_DIR)/drd.o
ifeq ($(KERNEL_VERSION_MAIN), $(KERNEL_VERSION_5_10))
dwc3-y += $(DWC3_DIR)/drd_cdev.o
endif

SRC := $(shell pwd)

ifeq ($(KERNEL_VERSION_MAIN), $(KERNEL_VERSION_4_4))
KBUILD_EXTRA_SYMBOLS+=${SRC}/Module.symvers
export KBUILD_EXTRA_SYMBOLS
endif

all:
	cp -rf $(USB_DWC3_DIR_PATH) .
	cp -rf $(USB_gadget_DIR_PATH) .
	cp -rf $(CONFIGFS_OPEN_SOURCE_DIR) .
	cp -rf $(USB_COMMON_DIR) .
	cp -rf $(USB_HOST_DIR) .

ifeq (${KERNEL_VERSION_MAIN},${KERNEL_VERSION_5_10})
	cp -rf $(USB_KERNEL_SOURCE_DIR)/drd_cdev.* $(DWC3_DIR)/
else
	cp -rf $(USB_KERNEL_SOURCE_DIR)/drd.* $(DWC3_DIR)/
endif

ifeq (${KERNEL_VERSION_MAIN},${KERNEL_VERSION_5_10})
	cp $(DWC3_DIR)/trace.h ${KERNEL_PATH}/include/trace
endif

	make -j${COMPILE_PROCESS_NUM} -C$(KERNEL_PATH)  M="usb"

clean: $(dirs)
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	if [ '${KERNEL_VERSION_MAIN}' == '${KERNEL_VERSION_4_4}' ];   \
	 then                                                          \
		rm -f Module.markers Module.symvers modules.order;       \
		rm -rf .tmp_versions Modules.symvers;       \
	 fi
	rm -rf $(DWC3_DIR) $(GADGET_DIR) $(CONFIGFS_DIR) $(COMMON_DIR) $(HOST_DIR)
