TARGET_NAME := $(shell basename $(PWD))

ifeq ("$(BUILD_SAMPLE_HI309X)", "bm")
include ../../baremetal/common/Makefile.config
else ifeq ("$(BUILD_SAMPLE_HI309X)", "oe_bm")
include ../../baremetal/common/Makefile.config
else ifeq ("$(BUILD_SAMPLE_HI309X)", "oe_libck")
include ../../libck/common/Makefile.config
else
$(error "can not find matched makefile")
endif
