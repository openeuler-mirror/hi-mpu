# This Makefile is a sample file, do not use in product.
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm

STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

TOPDIR := $(shell pwd)

ROOT_DIR ?= $(TOPDIR)/../../../../..

MCS_PATH=$(ROOT_DIR)/open_source/mcs
IPC_COMMON_PATH=$(ROOT_DIR)/src/samples/non_real_time/ipc/common
CFLAGS   := -fno-builtin -ffreestanding -I${MCS_PATH}/library/include/mcs -I${IPC_COMMON_PATH} -O2

EXTEND_INCLUDE := libsecurec.a libopen_amp.a
SYS_LIBS := -lmetal -lopen_amp -lsysfs

SOURCE  := $(wildcard *.c ${MCS_PATH}/library/*.c ${IPC_COMMON_PATH}/*.c)
OBJS    := $(patsubst %.c,%.o,$(SOURCE))

TARGET_NAME := rpmsg_uniproton_ipc

$(TARGET_NAME):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -fPIE -pie $(EXTEND_INCLUDE) -Wl,-Bdynamic $(SYS_LIBS)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf ${MCS_PATH}/library/*.o
	rm -rf *.o
	rm -rf ${IPC_COMMON_PATH}/*.o
	rm -f $(TARGET_NAME)
	rm -f ${EXTEND_INCLUDE}