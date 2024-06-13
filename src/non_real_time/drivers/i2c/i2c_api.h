// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Definitions for i2c API
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __I2C_API_H__
#define __I2C_API_H__

#include "i2c_usr_api.h"
#include "i2c_platform.h"

#define I2C_MAX_DEV_COUNT 1024

#define I2C_HZ_TO_KHZ 1000

#define I2C_7BIT_MAX_ADDR 0x7F
#define I2C_7BIT_ADDR_MASK I2C_7BIT_MAX_ADDR
#define I2C_10BIT_MAX_ADDR 0x3FF
#define I2C_10BIT_ADDR_MASK I2C_10BIT_MAX_ADDR

/*
 * Extended reserved length of the read/write buffer (4 x u8).
 * Currently, one u8 length is used for the CRC check of the SMBUS.
 */
#define I2C_RSERVED_EXTERN_LEN 4

#define I2C_SLAVE_RD_WAITTIME_MIN      3000  /* ms */
#define I2C_SLAVE_CLK_STRECH_TIME_MIN  10    /* ms */
#define I2C_SLAVE_CLK_STRECH_TIME_MAX  3000  /* ms */
#define I2C_SLAVE_WAIT_STOP_TIME_MAX   3000  /* ms */

enum i2c_error_type {
    I2C_EDEFAULT = 1000,
    I2C_EDIINVAL,
    I2C_ENOALGO,
    I2C_EMEMSET,
    I2C_EMEMCPY,
    I2C_EREVOCATION,
    I2C_EFORCE,
    I2C_ENOSLAVEMEM,
    I2C_ESLAVESCOPE,
    I2C_ENOADAP,
    I2C_EMSG,
    I2C_EMSGATTR,
    I2C_EMSGWORBUF,
    I2C_EMSGNORBUF,
    I2C_EMSGRCVLEN,
    I2C_ECPYTOUSR,
    I2C_ECPYFROMUSR,
    I2C_EBADCMDSIZE,
    I2C_ENOTSUPSPEED,
    I2C_EDMAMAP,
    I2C_EDMACONFIG,
    I2C_EDMAWAIT,
    I2C_EDMAZEROWRITE,
    I2C_ENOSLAVETYPE,
    I2C_ENOSLAVE,
    I2C_EIRQ,
    I2C_EIOCONFIG,
    I2C_ECRC,
    I2C_EUNSUPSMBUSPROTO,
    I2C_EMSGSMBUS,
    I2C_EXFER,
    I2C_EADDRESS,
    I2C_ENONEWMSG,
    I2C_EMSGEXISTED,
    I2C_ESLAVENAME,
};

enum {
    I2C_SMBUS_KM_WRITE_QUICK = 0,
    I2C_SMBUS_KM_READ_QUICK,
    I2C_SMBUS_KM_SEND_BYTE,
    I2C_SMBUS_KM_RECEIVE_BYTE,
    I2C_SMBUS_KM_WRITE_BYTE,
    I2C_SMBUS_KM_READ_BYTE,
    I2C_SMBUS_KM_WRITE_WORD,
    I2C_SMBUS_KM_READ_WORD,
    I2C_SMBUS_KM_WRITE_BLOCK,
    I2C_SMBUS_KM_READ_BLOCK,
    I2C_SMBUS_KM_PROC_CALL,
    I2C_SMBUS_KM_BLK_PROC_CALL,
    I2C_SMBUS_KM_MAX,
};

#define I2C_KMSG_RD       0x0001 /* Messages need to be received from the bus */
#define I2C_KMSG_SMBUS    0x0002 /* The message sent is an SMBus message. */
#define I2C_KMSG_TEN      0x0010 /* The destination address of the message is a 10-bit address. */
#define I2C_KMSG_RECV_LEN 0x0400 /* The length of the received message needs to be returned by the slave component. */
#define I2C_KMSG_PEC      0x0800 /* The driver needs to perform PEC error detection. */
#define I2C_KMSG_DMA      0x8000 /* Data transfer in DMA mode */

struct i2c_kmsg {
    unsigned short addr;    /* Message destination address */
    unsigned short flags;   /* Message Properties */
    unsigned char protocol; /* SMBus message protocol ID. This parameter is valid only for SMBus messages. */
    unsigned char recvd_crc;
    unsigned char quick_cmd;
    unsigned char quick_write;
    unsigned int retries;  /* Number of retransmission times after communication failure */
    unsigned long timeout; /* Timeout interval for sending and receiving messages, in jiffies. */
    unsigned int cmd_len;
    unsigned int outstanding;
    unsigned int rcnt;
    unsigned int wcnt;
    unsigned int wlen;
    unsigned char *wbuf;
    unsigned int rlen;
    unsigned char *rbuf;
};

#define I2C_KDEPLOY_SLAVE           BIT(0)
#define I2C_KDEPLOY_MASTER          BIT(1)
#define I2C_KDEPLOY_SM_MASK         (I2C_KDEPLOY_SLAVE | I2C_KDEPLOY_MASTER)
#define I2C_KDEPLOY_SPEED_MODE_STD  BIT(2) /* 100k */
#define I2C_KDEPLOY_SPEED_MODE_FAST BIT(3) /* 400k */
#define I2C_KDEPLOY_SPEED_MODE_HIGH BIT(4) /* 3400k */
#define I2C_KDEPLOY_SPEED_MODE_MASK (I2C_KDEPLOY_SPEED_MODE_STD |  \
                                     I2C_KDEPLOY_SPEED_MODE_FAST | I2C_KDEPLOY_SPEED_MODE_HIGH)
#define I2C_KDEPLOY_SLV_ADDR_MODE_10BIT BIT(6)
#define I2C_KDEPLOY_SLV_TYPE_EEPROM     BIT(7)
#define I2C_KDEPLOY_SMBUS           BIT(8)
#define I2C_KDEPLOY_FORCE           BIT(9)
#define I2C_KDEPLOY_SLV_TYPE_SMBUS  I2C_KDEPLOY_SMBUS
#define I2C_KDEPLOY_SLV_TYPE_MASK   (I2C_KDEPLOY_SLV_TYPE_EEPROM | I2C_KDEPLOY_SLV_TYPE_SMBUS)

struct i2c_kdepl {
    unsigned long flags;
    unsigned int sda_hold;        /* Number of hold delay cycles of the SDA signal */
    unsigned int sda_hold_ratio;  /* Percentage of hold delay cycles to low-level cycles of the SDA signal */
    unsigned int scl_high;        /* SCL number of high-level cycles */
    unsigned int scl_low;         /* Number of SCL low-level cycles */
    unsigned int scl_low_timeout; /* Number of SCL low-level cycles */
    unsigned int sda_low_timeout; /* Number of SDL Low Timeout Periods */
    unsigned long slave_dlen;
    unsigned short clk_strech_timeout;
    unsigned short slave_address;
};

extern void i2c_init(void *arg);
extern void i2c_destroy(void *arg);
extern int i2c_reset(unsigned int drv_id);
extern int i2c_clk_operate(unsigned int drv_id, unsigned int code);
extern int i2c_revocation(unsigned int drv_id);
extern void i2c_revocation_all(void);
extern int i2c_deploy(unsigned int drv_id, const char *algo_name, struct i2c_kdepl *deploy);
extern int i2c_transaction(unsigned int drv_id, struct i2c_kmsg *msg);
extern int i2c_acquire_mode(unsigned int drv_id, unsigned int *mode);
extern int i2c_slave_write_cache(i2c_slave_cache_rdwr *rdwr);
extern int i2c_slave_read_cache(i2c_slave_cache_rdwr *rdwr);
extern int i2c_slave_set_err_code(struct i2c_slave_err_code *err_code);
extern unsigned int i2c_random(void);

#endif /* __I2C_API_H__ */

