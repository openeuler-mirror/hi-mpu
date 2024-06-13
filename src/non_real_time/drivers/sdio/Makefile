ccflags-y += -I$(SDK_ROOT)/include/ -I$(SDK_ROOT)/include/register -I$(SDK_ROOT)/include/usr_api -I$(SDK_ROOT)/comm -I$(SRC_DIR)/include
ccflags-y += -DMMC_BLOCK_MINORS=16
ccflags-y += -fstack-protector-strong -Wl,-z,relro -z,now -Wl,-z,noexecstack -Wformat=2 -Wfloat-equal

KERNEL_VERSION_4_4=4.4
KERNEL_VERSION_5_10=5.10
MMC_OPEN_SOURCE_DIR=$(SRC_DIR)/../open_source/$(LINUX_PATH)/drivers/mmc
MMC_CORE_PATH=$(MMC_OPEN_SOURCE_DIR)/core

ifeq ($(KERNEL_VERSION_MAIN), $(KERNEL_VERSION_4_4))
card_DIR=card
MMC_CARD_PATH=$(MMC_OPEN_SOURCE_DIR)/card
endif

core_DIR=core

obj-m := mmc_core.o
obj-m += mmc_block.o
obj-m += emmc/
obj-m += sdio/

mmc_core-y := $(core_DIR)/core.o $(core_DIR)/bus.o $(core_DIR)/host.o $(core_DIR)/mmc.o
mmc_core-y += $(core_DIR)/mmc_ops.o $(core_DIR)/sd.o $(core_DIR)/sd_ops.o $(core_DIR)/sdio.o
mmc_core-y += $(core_DIR)/sdio_ops.o $(core_DIR)/sdio_bus.o $(core_DIR)/sdio_cis.o $(core_DIR)/sdio_io.o

ifeq ($(KERNEL_VERSION_MAIN), $(KERNEL_VERSION_4_4))
mmc_core-y += $(core_DIR)/sdio_irq.o $(core_DIR)/quirks.o $(core_DIR)/slot-gpio.o $(core_DIR)/pwrseq.o
mmc_core-y += $(core_DIR)/pwrseq_simple.o $(core_DIR)/pwrseq_emmc.o $(core_DIR)/debugfs.o
mmc_block-y := $(card_DIR)/block.o $(card_DIR)/queue.o
else ifeq ($(KERNEL_VERSION_MAIN), $(KERNEL_VERSION_5_10))
mmc_core-y += $(core_DIR)/sdio_irq.o $(core_DIR)/regulator.o $(core_DIR)/slot-gpio.o $(core_DIR)/pwrseq.o
mmc_core-y += $(core_DIR)/debugfs.o
mmc_block-y := $(core_DIR)/block.o $(core_DIR)/queue.o
endif

MODULE_MAKE=$(shell pwd)

all:
	cp -r $(MMC_CORE_PATH) .
ifeq (${KERNEL_VERSION_MAIN},${KERNEL_VERSION_4_4})
	cp -r $(MMC_CARD_PATH) .
	sed -i 's/CONFIG_MMC_BLOCK_MINORS/MMC_BLOCK_MINORS/g' ${card_DIR}/block.c
endif

ifeq (${KERNEL_VERSION_MAIN},${KERNEL_VERSION_5_10})
	sed -i 's/CONFIG_MMC_BLOCK_MINORS/MMC_BLOCK_MINORS/g' ${core_DIR}/block.c
endif

	make -C${KERNEL_PATH} M='sdio' modules
clean:
	rm -rf $(card_DIR) $(core_DIR)
	make -C${KERNEL_PATH} M=${MODULE_MAKE} clean
