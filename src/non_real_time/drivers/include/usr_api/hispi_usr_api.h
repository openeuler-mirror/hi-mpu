// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * hispi user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _HISPI_USR_API_H_
#define _HISPI_USR_API_H_

#define HISPI_DEVNAME "/dev/spi"

#define SPI_IOC_MAGIC 's'
#define SPI_CMD_INIT  _IOWR(SPI_IOC_MAGIC, 0, int)
#define SPI_CMD_WRITE _IOWR(SPI_IOC_MAGIC, 1, int)
#define SPI_CMD_READ  _IOWR(SPI_IOC_MAGIC, 2, int)
#define SPI_CMD_STD_INIT  _IOWR(SPI_IOC_MAGIC, 3, int)
#define SPI_CMD_STD_WRITE _IOWR(SPI_IOC_MAGIC, 4, int)
#define SPI_CMD_STD_READ  _IOWR(SPI_IOC_MAGIC, 5, int)

#define SPI_IOC_MAXNR (5)

#define SPI_CS_NUM  2
#define SPI_CTL_NUM 2

/* SPI protocol type. */
#define HISPI_TYPE_SSPI  0
#define HISPI_TYPE_ADSPI 1

/* Polarity definition when SPI is idle */
#define HISPI_IDLE_CLOCK_POLARITYLOW  0
#define HISPI_IDLE_CLOCK_POLARITYHIGH 1
#define HISPI_IDLE_CLOCK_POLARITYERR  2

/* The serial clock phase determines the relationship between data and clock. */
#define HISPI_CLOCK_EDGEUP   0
#define HISPI_CLOCK_EDGEDOWN 1
#define HISPI_CLOCK_EDGEERR  2

#define HISPI_FRAME_LENGTH_8_BIT  7
#define HISPI_FRAME_LENGTH_16_BIT 15

typedef struct {
    u32 cs_id;
    u32 pro_type;
    u32 baud;
    u32 frame_len;
    u32 clk_edge;
    u32 clk_pol;
} spi_ioctl_init;

typedef struct {
    u32 cs_id;
    u8 *cmd;
    u32 cmd_len;
} spi_ioctl_write;

typedef struct {
    u32 cs_id;
    u8 *cmd;
    u32 cmd_len;
    u8 *pdata;
    u32 data_len;
} spi_ioctl_read;

typedef struct {
    u32 ctrl_id;
    u32 cs_id;
} dev_spi_id;

typedef struct {
    u32 ctrl_id;
    u32 cs_id;
    u32 pro_type;
    u32 baud;
    u32 frame_len;
    u32 clk_edge;
    u32 clk_pol;
} dev_spi_ioctl_std_init;

typedef struct {
    u32 ctrl_id;
    u32 cs_id;
    u8 *cmd;
    u32 cmd_len;
} dev_spi_ioctl_std_write;

typedef struct {
    u32 ctrl_id;
    u32 cs_id;
    u8 *cmd;
    u32 cmd_len;
    u8 *p_data;
    u32 data_len;
} dev_spi_ioctl_std_read;

#endif
