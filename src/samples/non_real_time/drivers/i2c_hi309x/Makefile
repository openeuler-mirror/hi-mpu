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

SOURCE  := $(wildcard *.c)
OBJS    := $(patsubst %.c,%.o,$(SOURCE))

TARGET_NAME := sample_i2c_hi309x

$(TARGET_NAME):$(OBJS)
	$(CC) -o $@ $^

%.o : %.c
	$(CC) -c -o $@ $<

clean:
	rm -rf *.o