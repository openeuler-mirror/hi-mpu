// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * mdio user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _MDIO_USR_API_H_
#define _MDIO_USR_API_H_

#define MDIO_DEV_NAME "/dev/mdio"

#define MDIO_IOC_MAGIC 'm'

#define MDIO_CMD_WRITE _IOW(MDIO_IOC_MAGIC, 0, unsigned int)
#define MDIO_CMD_READ _IOW(MDIO_IOC_MAGIC, 1, unsigned int)
#define MDIO_CMD_WRITE45 _IOWR(MDIO_IOC_MAGIC, 5, mdio_rw45_s)
#define MDIO_CMD_READ45 _IOWR(MDIO_IOC_MAGIC, 6, mdio_rw45_s)

/* Frequency configuration of the MDIO interface clock */
#define MDIO_FREQUENCE_0 0 /* 1MHz */
#define MDIO_FREQUENCE_1 1 /* 2.5MHz */
#define MDIO_FREQUENCE_2 2 /* 12.5MHz */
#define MDIO_FREQUENCE_3 3 /* 25MHz */

/* MDIO write data. The corresponding command word is MDIO_CMD_READ. */
typedef struct {
    u32 mdio_id;
    u32 phy_addr;
    u32 reg_addr;
    u32 value;
} mdio_ioctl_write_s;

/* MDIO read data. The corresponding command word is MDIO_CMD_WRITE. */
typedef struct {
    u32 mdio_id;
    u32 phy_addr;
    u32 reg_addr;
    u32 *value;
} mdio_ioctl_read_s;

/* The mdio cluse45 reads and writes data. The corresponding command word is MDIO_CMD_READ. */
typedef struct {
    u32 mdio_id;
    u32 prt_addr;
    u32 dev_addr;
    u32 reg_addr;
    u32 value;
} mdio_rw45_s;

#endif