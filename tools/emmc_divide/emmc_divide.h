/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */

#ifndef _EMMC_DIVIDE_H_
#define _EMMC_DIVIDE_H_

/*****************************************************************************
内部宏定义
*****************************************************************************/
#define CMD_ERR (-1)

/* 命令字 */
#define MMC_IOC_MAGIC 'm'
#define MMC_GPP_DIV _IOWR(MMC_IOC_MAGIC, 11, int)

#define SET_GPP0_SIZE (384 * 1024)
#define SET_GPP3_SIZE (128 * 1024)

#define BIT10 (1 << 10)

#define NUM2 (2)
#endif
