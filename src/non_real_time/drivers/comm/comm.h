// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * public module comm external interface header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _COMM_H_
#define _COMM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DEV_COMM_NAME "comm_dev"

/* using standard ioctl commands. */
#define COMM_IOC_MAGIC        'c'
#define COMM_CMD_GET_VERSION  _IOWR(COMM_IOC_MAGIC, 0, int)
#define COMM_CMD_MOD_RESET    _IOWR(COMM_IOC_MAGIC, 1, int)
#define COMM_CMD_IOCFG_CHECK  _IOWR(COMM_IOC_MAGIC, 2, int)
#define COMM_CMD_IOCFG_CONFIG _IOWR(COMM_IOC_MAGIC, 3, int)

#define LPC_DELAY_TIME 10

typedef struct m_reset_stru {
    unsigned int reg_offset; /* resetting register offset address. */
    unsigned int mask;       /* reset mask: 1, only 1 valid,
                              * indicates that the corresponding one or more modules should be reset. */
} m_reset_s;

#ifdef __cplusplus
}
#endif

#endif
