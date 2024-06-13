// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * peci user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _PECI_USR_API_H_
#define _PECI_USR_API_H_

#define DEV_PECI_NAME           "/dev/peci"

#define PECI_IOC_MAGIC          'p'

#define PECI_CMD_TRANS          _IOWR(PECI_IOC_MAGIC, 0, int)
#define PECI_CMD_ABORT          _IOWR(PECI_IOC_MAGIC, 1, int)

#define PECI_CMD_MAX_NR         1

#define PECI_MAX_DATA_NUM       32


typedef struct PECI_CMD_STRU {
    u32 reg_wr_len;
    u32 reg_rd_len;
    u8 wr_data[PECI_MAX_DATA_NUM];
    u8 rd_data[PECI_MAX_DATA_NUM];
}PECI_CMD_S;

#endif

