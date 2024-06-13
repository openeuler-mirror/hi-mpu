// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * i2c user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _I2C_USR_API_H_
#define _I2C_USR_API_H_

#include <linux/fs.h>

#define I2C_DEVNAME            "/dev/i2c"
#define I2C_CMD_MAGIC          'i'
#define I2C_CMD_INIT           _IOW(I2C_CMD_MAGIC, 0, int)
#define I2C_CMD_WRITE          _IOW(I2C_CMD_MAGIC, 1, int)
#define I2C_CMD_READ           _IOWR(I2C_CMD_MAGIC, 2, int)
#define I2C_CMD_RESET          _IOW(I2C_CMD_MAGIC, 6, int)
#define I2C_CMD_SLAVE_CACHE_WR _IOW(I2C_CMD_MAGIC, 8, i2c_slave_cache_rdwr)
#define I2C_CMD_SLAVE_CACHE_RD _IOWR(I2C_CMD_MAGIC, 9, i2c_slave_cache_rdwr)
#define I2C_CMD_MODE_CHECK     _IOWR(I2C_CMD_MAGIC, 12, int)
#define I2C_CMD_REVOCATION     _IOW(I2C_CMD_MAGIC, 14, int)
#define I2C_CMD_DEPLOY         _IOW(I2C_CMD_MAGIC, 15, dev_i2c_deploy)
#define I2C_CMD_MSG_XFER       _IOWR(I2C_CMD_MAGIC, 16, dev_i2c_msg)
#define I2C_CMD_SET_SLAVE_ERR_CODE   _IOWR(I2C_CMD_MAGIC, 17, struct i2c_slave_err_code)
#define I2C_CMD_MAXNR          18

#define I2C_CLK_100K        100
#define I2C_CLK_400K        400
#define I2C_CLK_3400K       3400
#define I2C_RE_READ_DEFAULT 0xFF

#define I2C_MAX_CMD_LEN       256
#define I2C_MSG_RETRIES       3
#define I2C_DEFAULT_SLAVE_LEN 256

typedef struct st_dev_i2c_ioctl_init {
    u32 drv_id;
    u32 speed;
    u32 is_slave;
    u32 address;
    /* Ratio of the hold time of the SDA signal after the falling edge of the SCL signal to the SCL low level. */
    u32 sda_hold_ratio;
} dev_i2c_ioctl_init_s;

typedef struct st_dev_i2c_ioctl_write {
    u32 drv_id;
    u8 *p_tx_buffer;
    u32 length;
    u32 time_out_cnt;
} dev_i2c_ioctl_write_s;

typedef struct st_dev_i2c_ioctl_read {
    u32 drv_id;
    u8 *tx_buffer;
    u32 tx_len;
    u8 *p_read_buf;
    u32 length;
    u32 time_out_cnt;
    u32 re_read_cnt;
} dev_i2c_ioctl_read_s;

typedef struct st_dev_i2c_ioctl_mode_check {
    u32 drv_id;
    int mode;
} i2c_ioctl_mode_check_s;

#define I2C_DEPLOY_SLAVE               BIT(0)
#define I2C_DEPLOY_MASTER              BIT(1)
#define I2C_DEPLOY_SPEED_MODE_STD      BIT(2) /* 100k */
#define I2C_DEPLOY_SPEED_MODE_FAST     BIT(3) /* 400k */
#define I2C_DEPLOY_SPEED_MODE_HIGH     BIT(4) /* 3400k */
#define I2C_DEPLOY_SLV_ADDR_MODE_10BIT BIT(6)
#define I2C_DEPLOY_SLV_TYPE_EEPROM     BIT(7)
#define I2C_DEPLOY_SMBUS               BIT(8)
#define I2C_DEPLOY_FORCE               BIT(9)
#define I2C_DEPLOY_SLV_TYPE_SMBUS      (I2C_DEPLOY_SLAVE | I2C_DEPLOY_SMBUS)

struct i2c_depl {
    u32 drv_id;
    u32 flags;
    u32 sda_hold;
    u32 scl_high;
    u32 scl_low;
    u32 scl_low_timeout;
    u32 sda_low_timeout;
    u16 slave_address;
    u16 clk_strech_timeout;
    u64 slave_dlen;
    u64 extension;
} __attribute__((aligned(sizeof(u32))));

typedef struct i2c_depl dev_i2c_deploy;

enum i2c_smbus_protocol {
    I2C_SMBUS_WRITE_QUICK = 0,
    I2C_SMBUS_READ_QUICK,
    I2C_SMBUS_SEND_BYTE,
    I2C_SMBUS_RECEIVE_BYTE,
    I2C_SMBUS_WRITE_BYTE,
    I2C_SMBUS_READ_BYTE,
    I2C_SMBUS_WRITE_WORD,
    I2C_SMBUS_READ_WORD,
    I2C_SMBUS_WRITE_BLOCK,
    I2C_SMBUS_READ_BLOCK,
    I2C_SMBUS_PROC_CALL,
    I2C_SMBUS_BLK_PROC_CALL,
};

#define I2C_MSG_RD       0x0001
#define I2C_MSG_SMBUS    0x0002
#define I2C_MSG_TEN      0x0010
#define I2C_MSG_RECV_LEN 0x0400
#define I2C_MSG_PEC      0x0800
#define I2C_MSG_DMA      0x8000

struct i2c_msg {
    u32 drv_id;
    u16 addr;
    u16 flags;
    u8 smbus_protocol;
    u8 rsvd[3];
    u32 wlen;
    u8 *wbuf;
    u32 rlen;
    u8 *rbuf;
    u32 timeout;
    u32 retries;
    u64 extension;
} __attribute__((aligned(sizeof(u32))));

typedef struct i2c_msg dev_i2c_msg;

struct i2c_slave_memory_rdwr {
    u32 drv_id;
    u32 offset;
    u32 size;
    u32 timeout;
    u8 *buff;
} __attribute__((aligned(sizeof(u32))));

typedef struct i2c_slave_memory_rdwr i2c_slave_cache_rdwr;

struct i2c_slave_err_code {
    u32 drv_id;
    u32 size;
    u32 enable;
    u8 *buff;
} __attribute__((aligned(sizeof(u32))));

#endif
