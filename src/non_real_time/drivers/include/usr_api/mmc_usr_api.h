// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * mmc user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _MMC_USR_API__H_
#define _MMC_USR_API__H_

#define DEV_SDIO_NAME             "sdio"
#define SDIO_IOC_MAGIC         's'
#define SDIO_GET_REG                        _IOWR(SDIO_IOC_MAGIC, 0, mmc_reg_info_s)
#define SDIO_INIT_CONTROLOR                 _IOWR(SDIO_IOC_MAGIC, 1, MMC_INIT_CFG_S)
#define SDIO_STOP_TRANS                     _IOW(SDIO_IOC_MAGIC, 2, unsigned int)
#define SDIO_OP_FLASH                       _IOWR(SDIO_IOC_MAGIC, 3, MMC_FLASH_OP_INFO_S)
#define SDIO_RESET                          _IO(SDIO_IOC_MAGIC, 4)
#define SDIO_SET_FIFO                       _IOW(SDIO_IOC_MAGIC, 5, MMC_FIFO_INFO_S)

#define SDIO_CMD_MAXNR 5

#define DEV_MMC_NAME             "mmc"

#define MMC_IOC_MAGIC             'm'
#define MMC_GET_REG                        _IOWR(MMC_IOC_MAGIC, 0, mmc_reg_info_s)
#define MMC_GET_HYNIX_HEALTH_REPORT        _IOR(MMC_IOC_MAGIC, 1, int)
#define MMC_GET_TBW                        _IOWR(MMC_IOC_MAGIC, 2, mmc_write_allinfo_s)
#define MMC_ERASE_DATA                     _IOWR(MMC_IOC_MAGIC, 3, struct mmc_do_erase_info)
#define MMC_UPDATE_PROTECT                 _IOWR(MMC_IOC_MAGIC, 10, mmc_area_protect_s)
#define MMC_GPP_DIV                        _IOWR(MMC_IOC_MAGIC, 11, int)

#define MMC_CMD_MAXNR 10

typedef enum MMC_REG_TYPE {
    MMC_EXT_CSD,
    MMC_CID,
    MMC_CSD
}mmc_reg_type_s;

typedef struct mmc_reg_info {
    mmc_reg_type_s mmc_reg_type_v;
    unsigned int mmc_reg_offset;
    unsigned int length;
    unsigned char *buf;
}mmc_reg_info_s;

typedef struct mmc_write_allinfo {
    unsigned int mmc_clear_flag;
    unsigned long write_time;
    unsigned long long write_byte;
}mmc_write_allinfo_s;

typedef struct mmc_prot_op_info {
    unsigned int type;
    unsigned int addr;
    int result;
}mmc_prot_op_info_s;

typedef struct mmc_area_protect_info {
    unsigned char area_index;
    unsigned char type;
}mmc_area_protect_s;

struct mmc_do_erase_info {
    unsigned int start_addr_arg;
    unsigned int blk_num;
    unsigned int erase_arg;
};

#endif
