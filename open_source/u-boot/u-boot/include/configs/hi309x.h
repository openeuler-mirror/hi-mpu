/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Hi309x uboot header file for platform definations
 * Create: 2018-05-10
 */
#ifndef __HI309x_H
#define __HI309x_H

#if defined(CONFIG_TARGET_HI309x)

#include <linux/sizes.h>
#include <config_distro_bootcmd.h>
#include "hi309x_memmap.h"

#define USE_SPI1_SFC_PIN 1
#define CONFIG_ENV_OVERWRITE
#define UBOOT_COPY_ADDR (0x0)
#define UBOOT_COPY_SIZE (0x80000)

/* Use simple spin table for multi-core booting */
#define CONFIG_ARMV8_MULTIENTRY 1
#define CONFIG_SYS_INIT_SP_ADDR (CONFIG_SYS_SRAM_BASE + 0x1FF00)
/* Generic Interrupt Controller Definitions */
#define CONFIG_GICV3
#define GICD_BASE 0x24000000
#define GICR_BASE 0x24040000

/* Command line configuration */
#define CONFIG_MTD_PARTITIONS

/* Initial environment variables */
#define BOOT_TARGET_DEVICES(func) func(DHCP, dhcp, na)

#define CONFIG_EXTRA_ENV_SETTINGS                                                                                 \
    "boot_kernel_media=0\0"                                                                                       \
    "part2_user_offset=0\0"                                                                                       \
    "packet_load_addr=0x87200000\0"                                                                               \
    "packet_size=0x7c00000\0"                                                                                     \
    "uboot_flash_addr=0x0\0"                                                                                      \
    "uboot_size=0x100000\0"                                                                                       \
    "ext4_img_flash_addr=0x400000\0"                                                                              \
    "ext4_img_size=0x1b00000\0"                                                                                   \
    "initrd_size=0x2400000\0"                                                                                      \
    "uboot_load_addr=0x87200000\0"                                                                                \
    "dtb_load_addr=0x87400000\0"                                                                                  \
    "kernel_load_addr=0x87480000\0"                                                                               \
    "ext4_img_load_addr=0x88680000\0"                                                                             \
    "initrd_load_addr=0xa1480000\0"                                                                               \
    "fitimage_load_addr=0x88680000\0"                                                                             \
    "load_ext4_kernel=ext4load private_device 0:0 ${kernel_load_addr} /boot/uImage\0"                            \
    "load_ext4_dtb=ext4load private_device 0:0 ${dtb_load_addr} /boot/hi1711_asic.dtb\0"                         \
    "load_ext4_initrd=ext4load private_device 0:0 ${initrd_load_addr} /boot/initrd_boot.cpio.gz\0"               \
    "run_linux=bootm ${kernel_load_addr} ${initrd_load_addr}:${initrd_size} ${dtb_load_addr}\0"                   \
    "hi309x_baremetal=run_all_cores\0"                                                                            \
    "hi309x_linux=run load_ext4_kernel;run load_ext4_dtb;run load_ext4_initrd;run run_linux\0"                    \
    "hi309x_sfc_fitimage_boot=bootm ${fitimage_load_addr}\0"                                                      \
    "tftp_packet=tftpboot ${packet_load_addr} Hi3093_flash_ext4.bin\0"                                            \
    "tftp_uboot=tftpboot ${uboot_load_addr} u-boot.bin\0"                                                         \
    "tftp_ext4_img=tftpboot ${ext4_img_load_addr} Hi3093_ext4fs.img\0"                                            \
    "tftpwrite_packet=if test ${serverip} = \"\";then echo Need to config IP;else run tftp_packet; \
    run write_packet; fi\0" \
    "tftpwrite_ext4_img=if test ${serverip} = \"\";then echo Need to config IP;else run tftp_ext4_img; \
    run write_ext4_img; fi\0" \
    "tftpwrite_uboot=if test ${serverip} = \"\";then echo Need to config IP;else run tftp_uboot; \
    run write_uboot; fi\0" \
    "fdt_high=0xffffffffffffffff\0" \
    "initrd_high=0xffffffffffffffff\0" \
    "tftp_network_boot=tftp ${dtb_load_addr} hi3093.dtb; \
    tftp ${kernel_load_addr} uImage; \
    tftp ${initrd_load_addr} initrd_boot.cpio.gz; \
    bootm ${kernel_load_addr} ${initrd_load_addr} : ${initrd_size} ${dtb_load_addr}\0" \
    BOOTENV

#define EMMC_PART_TABLE_OFFSET 0x2000  // user区分区表大小，0开始偏移
/* No enviroment args saving */
#define CONFIG_ENV_SECT_SIZE 0x200
#define CONFIG_ENV_OFFSET EMMC_PART_TABLE_OFFSET

#define TOTAL_OFFSET_EMMC_USER (CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE)

#define CMD_ENV

#define EMMC_BIN_UPDATE_ADDR (0x200000 / 512)
// 1 Block = 512 Byte
#define USER_BIN_BLOCK_NUM 20480
/* MON Command Prompt */
#define CONFIG_SYS_CBSIZE 512 /* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_BARGSIZE CONFIG_SYS_CBSIZE
#define CONFIG_SYS_MAXARGS 64 /* max command args */

#endif // #if defined(CONFIG_TARGET_HI309x)

#endif /* __HI309x_H */
