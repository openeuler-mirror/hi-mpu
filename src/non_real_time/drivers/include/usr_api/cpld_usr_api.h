// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * cpld usr api interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _CPLD_USR_API_H_
#define _CPLD_USR_API_H_

#define DEV_CPLD_NAME  "/dev/cpld"
#define CPLD_IOC_MAGIC 'c'

/* CPLD loading */
#define CPLD_CMD_LOAD_CPLD _IOW(CPLD_IOC_MAGIC, 0, int)
/* After the loading is complete, reset is required. */
#define CPLD_CMD_RESET _IO(CPLD_IOC_MAGIC, 1)
/*
 * Configure the GPIO pins required for the CPLD online upgrade.
 * The configured GPIO pins are configured as input pins.
 */
#define CPLD_CMD_UPDATE_GPIO_CFG _IOW(CPLD_IOC_MAGIC, 2, struct cpld_update_gpio_cfg_t)
/* Configure the GPIO pin for resetting the CPLD. This pin is set to output high level during initialization. */
#define CPLD_CMD_RESET_GPIO_CFG _IOW(CPLD_IOC_MAGIC, 3, int)
#define CPLD_CMD_UPDATE_REG_CFG _IOW(CPLD_IOC_MAGIC, 4, struct cpld_update_reg_cfg_t)
#define JLC_CMD_GET_ID  _IOW(CPLD_IOC_MAGIC, 5, unsigned int)
#define JLC_CMD_SET_NUM _IOW(CPLD_IOC_MAGIC, 6, unsigned int)
#define JLC_CMD_SPI_UPDATE_CFG _IOW(CPLD_IOC_MAGIC, 7, unsigned int)
#define JLC_CMD_SET_BYPASS_REPLACE _IOW(CPLD_IOC_MAGIC, 8, unsigned char)
#define JLC_CMD_SET_MODE _IOW(CPLD_IOC_MAGIC, 9, unsigned int)
/* Set the JTAG mode to BYPASS to test whether the link is normal. */
#define JLC_CMD_BYPASS _IOW(CPLD_IOC_MAGIC, 10, struct bypass_data_info)

#define CPLD_CMD_MAX_NR 10

typedef struct {
    unsigned int channel_id;
    unsigned int mode;       /* 0:master; 1:slave */
} jlc_mode_info;

/* Maximum length of a CPLD file */
#define CPLD_FILE_LEN 0x300000

struct cpld_update_gpio_cfg_t {
    unsigned char tck;
    unsigned char tms;
    unsigned char tdo;
    unsigned char tdi;
};

struct cpld_update_reg_cfg_t {
    /* CPLD Register Offset for Upgrade */
    unsigned char reg_offset;
    unsigned char tck_bit : 3;
    unsigned char tms_bit : 3;
    unsigned char tdo_bit : 3;
    unsigned char tdi_bit : 3;
};

struct load_file_t {
    unsigned int flag;
    unsigned long file_length;
    unsigned long block_length;
    unsigned long product_id;
    unsigned long board_id;
    unsigned long pcb;
    unsigned long component_id;
    unsigned char *data;        /* CPLD file in VME format */
};

#define CPLD_FILE_START_LOAD_FLAG 0x1 /* Start CPLD loading flag */
#define CPLD_FILE_IN_LOAD_FLAG    0x2 /* Continue to load the CPLD ID. */
#define CPLD_FILE_END_LOAD_FLAG   0x4 /* Indicates that CPLD loading ends. */

#define LATTICE_PRODUCT_ID    0x10
#define ALTERA_PRODUCT_ID     0x20
#define ALTERA_PRODUCT_ID_SPI 0x21

#endif
