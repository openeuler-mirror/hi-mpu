# SPDX-License-Identifier: GPL-2.0+
#
# (C) Copyright 2000-2007
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.

obj-$(CONFIG_$(SPL_)BLK) += blk-uclass.o
obj-$(CONFIG_HI3093_MMC_BOOT) += hi309x_mmc_desc_sim.o
obj-$(CONFIG_HI3093_SPI_BOOT) += hi309x_sfc_desc_sim.o

ifndef CONFIG_$(SPL_)BLK
obj-$(CONFIG_HAVE_BLOCK_DEVICE) += blk_legacy.o
endif

ifndef CONFIG_SPL_BUILD
obj-$(CONFIG_IDE) += ide.o
endif
obj-$(CONFIG_SANDBOX) += sandbox.o
obj-$(CONFIG_$(SPL_TPL_)BLOCK_CACHE) += blkcache.o

obj-$(CONFIG_EFI_MEDIA) += efi-media-uclass.o
obj-$(CONFIG_EFI_MEDIA_SANDBOX) += sb_efi_media.o
obj-$(CONFIG_EFI_MEDIA_BLK) += efi_blk.o
