TOPDIR := $(shell pwd)
ROOT_DIR := $(TOPDIR)/../../../../..
SECUREC_PATH := ${ROOT_DIR}/platform/securec
REAL_TIME_DIR := $(ROOT_DIR)/src/real_time
UNIPROTON_DIR := $(REAL_TIME_DIR)/uniproton
UNIPROTON_KERNEL_DIR := $(UNIPROTON_DIR)/UniProton-v1.0.1
UNIPROTON_DEMOS_DIR := $(UNIPROTON_KERNEL_DIR)/demos/hi3093
UNIPROTON_SAMPLE_DIR := $(ROOT_DIR)/src/samples/real_time/uniproton
DRIVERS_DIR := $(REAL_TIME_DIR)/drivers
OSAL_DIR := $(ROOT_DIR)/platform/osal
UNIPROTON_CROSS_COMPILE := ${UNIPROTON_DIR}/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/bin/aarch64-none-elf-

AS		= $(UNIPROTON_CROSS_COMPILE)as
LD		= $(UNIPROTON_CROSS_COMPILE)ld
CC		= $(UNIPROTON_CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(UNIPROTON_CROSS_COMPILE)ar
NM		= $(UNIPROTON_CROSS_COMPILE)nm

STRIP		= $(UNIPROTON_CROSS_COMPILE)strip
OBJCOPY		= $(UNIPROTON_CROSS_COMPILE)objcopy
OBJDUMP		= $(UNIPROTON_CROSS_COMPILE)objdump
RANLIB		= $(UNIPROTON_CROSS_COMPILE)ranlib

export AS LD CC CPP AR NM STRIP OBJCOPY OBJDUMP

OUTPUT_DIR := $(ROOT_DIR)/build/build_uniproton/output

CFLAGS   := -fno-builtin -ffreestanding \
	-I$(UNIPROTON_SAMPLE_DIR)/driver/include \
	-I$(DRIVERS_DIR)/include/ \
	-I$(DRIVERS_DIR)/common/ \
	-I$(SECUREC_PATH)/include \
	-I$(UNIPROTON_DEMOS_DIR)/config \
	-I${UNIPROTON_KERNEL_DIR}/src/arch/include \
	-I${UNIPROTON_KERNEL_DIR}/src/core/ipc/include \
	-I${UNIPROTON_KERNEL_DIR}/src/core/kernel/include \
	-I${UNIPROTON_KERNEL_DIR}/src/include/uapi \
	-I${UNIPROTON_KERNEL_DIR}/src/core/kernel/irq \
	-I${UNIPROTON_KERNEL_DIR}/src/include/posix \
	-I${UNIPROTON_KERNEL_DIR}/src/mem/include \
	-I${UNIPROTON_KERNEL_DIR}/src/om/include \
	-I${UNIPROTON_KERNEL_DIR}/src/osal/posix/include \
	-I${UNIPROTON_KERNEL_DIR}/src/utility/lib/include \
	-I${UNIPROTON_KERNEL_DIR}/platform/libboundscheck/include \
	-I$(OSAL_DIR)/include \
	-DTEXT_BASE=$(TEXTBASE) -O2 \
	-fno-pic -ffunction-sections \
	-fdata-sections -fno-common \
	-fno-common -pipe -march=armv8-a \
	-Wall -Wstrict-prototypes -fstack-protector-strong \
	$(MKFLAGS) -fno-strict-aliasing

CFLAGS += -Wdate-time -Wmissing-declarations -Wmissing-prototypes\
	-Wfloat-equal -Wformat=2  -Wswitch-default -Wtrampolines -Winit-self \
	-Wundef -Wimplicit-fallthrough=2 -Wshadow

POSIX_OPTION := -D_POSIX_THREADS -D_POSIX_THREAD_PRIORITY_SCHEDULING -D_POSIX_PRIORITY_SCHEDULING \
 				-D_POSIX_TIMERS -D_POSIX_CPUTIME -D_POSIX_THREAD_CPUTIME -D_POSIX_MONOTONIC_CLOCK -D_POSIX_TIMEOUTS \
  				-D_POSIX_CLOCK_SELECTION -D_POSIX_THREAD_PRIO_PROTECT -D_UNIX98_THREAD_MUTEX_ATTRIBUTES \
				-D_POSIX_READER_WRITER_LOCKS

CFLAGS += $(POSIX_OPTION)
CFLAGS += -D__UNIPROTON__
ASFLAGS:= $(CFLAGS)

export CFLAGS LDFLAGS

TARGET := libdriver.a

obj-y += $(OSAL_DIR)/uniproton/osal_interrupt.o
obj-y += $(OSAL_DIR)/uniproton/osal_semaphore.o
obj-y += $(OSAL_DIR)/uniproton/osal_debug.o
obj-y += $(DRIVERS_DIR)/

$(TARGET):
	make -C ./ -f Makefile.build MAKE_BUILD_DIR=$(UNIPROTON_SAMPLE_DIR)/driver
	$(AR) cr $(TARGET) built-in.o
	$(RANLIB) $(TARGET)

clean:
	rm -f $(shell find $(UNIPROTON_SAMPLE_DIR)/driver -name "*.o")
	rm -f $(shell find $(UNIPROTON_SAMPLE_DIR)/driver -name "*.a")
	rm -f $(shell find $(DRIVERS_DIR) -name "*.o")
	rm -f $(shell find $(OSAL_DIR)/nonos -name "*.o")