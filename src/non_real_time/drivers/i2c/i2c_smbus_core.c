// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Implemention of smbus based on i2c.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "i2c.h"
#include "i2c_core.h"
#include "i2c_api.h"

#define I2C_SMBUS_LEN_BYTE         1
#define I2C_SMBUS_LEN_WORD         2
#define I2C_SMBUS_LEN_CMD          1
#define I2C_SMBUS_LEN_CMD_BYTE     2
#define I2C_SMBUS_LEN_CMD_WORD     3
#define I2C_SMBUS_LEN_CMD_BLOCK    3
#define I2C_SMBUS_LEN_MAX          32
#define I2C_SMBUS_CRC_START        BIT(0)
#define I2C_SMBUS_CRC_RESTART      BIT(1)
#define I2C_SMBUS_CRC_COMMAND      BIT(4)
#define I2C_SMBUS_CRC_START_READ   BIT(5)
#define I2C_SMBUS_CRC_RESTART_READ BIT(6)

struct smbus_crc_parm {
    unsigned char address;
    unsigned char command;
    unsigned char flags;
    unsigned char old_crc;
    unsigned char *data;
    unsigned int dlen;
};

typedef int (*i2c_smbus_prepare_t)(struct i2c_kmsg *);
typedef int (*i2c_smbus_verify_t)(const struct i2c_kmsg *);

unsigned char i2c_smbus_crc(unsigned char previous, unsigned char data)
{
    unsigned char temp_data = data;
    static const unsigned char crc_table[16] = {
        0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
        0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D
    };
    unsigned char crc = 0;

    temp_data = previous ^ temp_data;

    crc = (unsigned char)((crc << 4) ^ crc_table[((crc ^ temp_data) >> 4) & 0x0F]); // bit field, field width 4
    temp_data <<= 4; // bit field, field width 4
    crc = (unsigned char)((crc << 4) ^ crc_table[((crc ^ temp_data) >> 4) & 0x0F]); // bit field, field width 4

    return crc;
}

unsigned char i2c_smbus_pec(struct smbus_crc_parm crc_parm)
{
    unsigned int i;
    unsigned char crc = crc_parm.old_crc;

    if (crc_parm.flags & I2C_SMBUS_CRC_START) {
        if (crc_parm.flags & I2C_SMBUS_CRC_START_READ) {
            crc = i2c_smbus_crc(crc, ((unsigned char)(crc_parm.address << 1) | 0x1));
        } else {
            crc = i2c_smbus_crc(crc, ((unsigned char)(crc_parm.address << 1) | 0x0));
        }
    }

    if (crc_parm.flags & I2C_SMBUS_CRC_COMMAND) {
        crc = i2c_smbus_crc(crc, crc_parm.command);
    }

    if (crc_parm.flags & I2C_SMBUS_CRC_RESTART) {
        if (crc_parm.flags & I2C_SMBUS_CRC_RESTART_READ) {
            crc = i2c_smbus_crc(crc, ((unsigned char)(crc_parm.address << 1) | 0x1));
        } else {
            crc = i2c_smbus_crc(crc, ((unsigned char)(crc_parm.address << 1) | 0x0));
        }
    }

    for (i = 0; (i < crc_parm.dlen) && crc_parm.data; i++) {
        crc = i2c_smbus_crc(crc, crc_parm.data[i]);
    }

    return crc;
}

static int i2c_smbus_pec_prepare_read(struct i2c_kmsg *msg)
{
    if (!(msg->flags & I2C_KMSG_RECV_LEN)) {
        msg->rlen++;
    }

    return 0;
}

static int i2c_smbus_pec_prepare_sendbyte(struct i2c_kmsg *msg)
{
    unsigned char address = msg->addr & 0xFF;
    unsigned char flags;
    unsigned char val;
    struct smbus_crc_parm crc_parm;

    flags = I2C_SMBUS_CRC_START;
    crc_parm = (struct smbus_crc_parm){address, 0x0, flags, 0x0, msg->wbuf, msg->wlen};
    val = i2c_smbus_pec(crc_parm);
    msg->wbuf[msg->wlen++] = val;
    return 0;
}

static int i2c_smbus_pec_prepare_write(struct i2c_kmsg *msg)
{
    unsigned char address = msg->addr & 0xFF;
    unsigned char flags;
    unsigned char val;
    struct smbus_crc_parm crc_parm;

    flags = I2C_SMBUS_CRC_START | I2C_SMBUS_CRC_COMMAND;
    crc_parm = (struct smbus_crc_parm){address, msg->wbuf[0], flags, 0x0, &msg->wbuf[1], msg->wlen - 1};
    val = i2c_smbus_pec(crc_parm);
    msg->wbuf[msg->wlen++] = val;
    return 0;
}

static int i2c_smbus_pec_prepare(struct i2c_kmsg *msg)
{
    static const i2c_smbus_prepare_t routine[I2C_SMBUS_KM_MAX] = {
        NULL,                           /* quick write */
        NULL,                           /* quick read */
        i2c_smbus_pec_prepare_sendbyte, /* send byte */
        i2c_smbus_pec_prepare_read,     /* recieve byte */
        i2c_smbus_pec_prepare_write,    /* write byte */
        i2c_smbus_pec_prepare_read,     /* read byte */
        i2c_smbus_pec_prepare_write,    /* write word */
        i2c_smbus_pec_prepare_read,     /* read word */
        i2c_smbus_pec_prepare_write,    /* write block */
        i2c_smbus_pec_prepare_read,     /* read block */
        i2c_smbus_pec_prepare_read,     /* process call */
        i2c_smbus_pec_prepare_read,     /* block process call */
    };

    if (!(msg->flags & I2C_KMSG_PEC)) {
        return 0;
    }

    if (msg->protocol >= I2C_SMBUS_KM_MAX || !routine[msg->protocol]) {
        return -I2C_EUNSUPSMBUSPROTO;
    } else {
        return routine[msg->protocol](msg);
    }
}

static int i2c_smbus_pec_verify_recievebyte(const struct i2c_kmsg *msg)
{
    unsigned char flags;
    unsigned char val;
    struct smbus_crc_parm crc_parm1;
    struct smbus_crc_parm crc_parm2;
    unsigned char address = msg->addr & 0xFF;

    flags = I2C_SMBUS_CRC_START | I2C_SMBUS_CRC_START_READ;
    crc_parm1 = (struct smbus_crc_parm){address, 0x0, flags, 0x0, NULL, 0};
    val = i2c_smbus_pec(crc_parm1);
    crc_parm2 = (struct smbus_crc_parm){address, 0x0, 0x0, val, msg->rbuf, msg->rlen};
    val = i2c_smbus_pec(crc_parm2);
    return (val == msg->recvd_crc) ? 0 : -I2C_ECRC;
}

static int i2c_smbus_pec_verify_read(const struct i2c_kmsg *msg)
{
    unsigned char flags;
    unsigned char val;
    struct smbus_crc_parm crc_parm1;
    struct smbus_crc_parm crc_parm2;
    unsigned char address = msg->addr & 0xFF;

    flags = I2C_SMBUS_CRC_START | I2C_SMBUS_CRC_COMMAND |
            I2C_SMBUS_CRC_RESTART |
            I2C_SMBUS_CRC_RESTART_READ;
    crc_parm1 = (struct smbus_crc_parm){address, msg->wbuf[0], flags, 0x0, NULL, 0};
    val = i2c_smbus_pec(crc_parm1);
    crc_parm2 = (struct smbus_crc_parm){address, 0x0, 0x0, val, msg->rbuf, msg->rlen};
    val = i2c_smbus_pec(crc_parm2);
    return (val == msg->recvd_crc) ? 0 : -I2C_ECRC;
}

static int i2c_smbus_pec_verify_processcall(const struct i2c_kmsg *msg)
{
    unsigned char flags;
    unsigned char val;
    struct smbus_crc_parm crc_parm1;
    struct smbus_crc_parm crc_parm2;
    struct smbus_crc_parm crc_parm3;
    unsigned char address = msg->addr & 0xFF;

    flags = I2C_SMBUS_CRC_START | I2C_SMBUS_CRC_COMMAND;
    crc_parm1 = (struct smbus_crc_parm){address, msg->wbuf[0], flags, 0x0,
                                       &msg->wbuf[1], msg->wlen - 1};
    val = i2c_smbus_pec(crc_parm1);
    flags = I2C_SMBUS_CRC_RESTART | I2C_SMBUS_CRC_RESTART_READ;
    crc_parm2 = (struct smbus_crc_parm){address, 0x0, flags, val, NULL, 0};
    val = i2c_smbus_pec(crc_parm2);
    crc_parm3 = (struct smbus_crc_parm){address, 0x0, 0x0, val, msg->rbuf, msg->rlen};
    val = i2c_smbus_pec(crc_parm3);
    return (val == msg->recvd_crc) ? 0 : -I2C_ECRC;
}

static int i2c_smbus_pec_verify(struct i2c_kmsg *msg)
{
    static const i2c_smbus_verify_t routine[I2C_SMBUS_KM_MAX] = {
        NULL,                             /* quick write */
        NULL,                             /* quick read */
        NULL,                             /* send byte */
        i2c_smbus_pec_verify_recievebyte, /* recieve byte */
        NULL,                             /* write byte */
        i2c_smbus_pec_verify_read,        /* read byte */
        NULL,                             /* write word */
        i2c_smbus_pec_verify_read,        /* read word */
        NULL,                             /* write block */
        i2c_smbus_pec_verify_read,        /* read block */
        i2c_smbus_pec_verify_processcall, /* process call */
        i2c_smbus_pec_verify_processcall, /* block process call */
    };

    if (!(msg->flags & I2C_KMSG_PEC) ||
        !(msg->flags & I2C_KMSG_RD)) {
        return 0;
    }

    if (msg->protocol >= I2C_SMBUS_KM_MAX || !routine[msg->protocol]) {
        return -I2C_EUNSUPSMBUSPROTO;
    } else {
        return routine[msg->protocol](msg);
    }
}

static int i2c_smbus_prepare_quick(struct i2c_kmsg *msg)
{
    if (msg->flags & (I2C_KMSG_RD | I2C_KMSG_RECV_LEN | I2C_KMSG_PEC)) {
        return -I2C_EMSGATTR;
    }

    if (msg->wbuf || msg->wlen || msg->rbuf || msg->rlen) {
        return -I2C_EMSGSMBUS;
    } else {
        return i2c_smbus_pec_prepare(msg);
    }
}

static int i2c_smbus_prepare_sendbyte(struct i2c_kmsg *msg)
{
    if (msg->flags & (I2C_KMSG_RD | I2C_KMSG_RECV_LEN)) {
        return -I2C_EMSGATTR;
    }

    if (!msg->wbuf || msg->wlen != I2C_SMBUS_LEN_BYTE ||
        msg->rbuf || msg->rlen) {
        return -I2C_EMSGSMBUS;
    } else {
        return i2c_smbus_pec_prepare(msg);
    }
}

static int i2c_smbus_prepare_recvbyte(struct i2c_kmsg *msg)
{
    if (!(msg->flags & I2C_KMSG_RD)) {
        return -I2C_EMSGATTR;
    }

    if (msg->flags & I2C_KMSG_RECV_LEN) {
        return -I2C_EMSGATTR;
    }

    if (msg->wbuf || msg->wlen || !msg->rbuf ||
        msg->rlen != I2C_SMBUS_LEN_BYTE) {
        return -I2C_EMSGSMBUS;
    } else {
        return i2c_smbus_pec_prepare(msg);
    }
}

static int i2c_smbus_prepare_writebyte(struct i2c_kmsg *msg)
{
    if (msg->flags & (I2C_KMSG_RD | I2C_KMSG_RECV_LEN)) {
        return -I2C_EMSGATTR;
    }

    if (!msg->wbuf || msg->wlen != I2C_SMBUS_LEN_CMD_BYTE ||
        msg->rbuf || msg->rlen) {
        return -I2C_EMSGSMBUS;
    } else {
        return i2c_smbus_pec_prepare(msg);
    }
}

static int i2c_smbus_prepare_readbyte(struct i2c_kmsg *msg)
{
    if (!(msg->flags & I2C_KMSG_RD)) {
        return -I2C_EMSGATTR;
    }

    if (msg->flags & I2C_KMSG_RECV_LEN) {
        return -I2C_EMSGATTR;
    }

    if (!msg->wbuf || msg->wlen != I2C_SMBUS_LEN_CMD ||
        !msg->rbuf || msg->rlen != I2C_SMBUS_LEN_BYTE) {
        return -I2C_EMSGSMBUS;
    } else {
        return i2c_smbus_pec_prepare(msg);
    }
}

static int i2c_smbus_prepare_writeword(struct i2c_kmsg *msg)
{
    if (msg->flags & (I2C_KMSG_RD | I2C_KMSG_RECV_LEN)) {
        return -I2C_EMSGATTR;
    }

    if (!msg->wbuf || msg->wlen != I2C_SMBUS_LEN_CMD_WORD || msg->rbuf || msg->rlen) {
        return -I2C_EMSGSMBUS;
    } else {
        return i2c_smbus_pec_prepare(msg);
    }
}

static int i2c_smbus_prepare_readword(struct i2c_kmsg *msg)
{
    if (!(msg->flags & I2C_KMSG_RD)) {
        return -I2C_EMSGATTR;
    }

    if (msg->flags & I2C_KMSG_RECV_LEN) {
        return -I2C_EMSGATTR;
    }

    if (!msg->wbuf || msg->wlen != I2C_SMBUS_LEN_CMD ||
        !msg->rbuf || msg->rlen != I2C_SMBUS_LEN_WORD) {
        return -I2C_EMSGSMBUS;
    } else {
        return i2c_smbus_pec_prepare(msg);
    }
}

static int i2c_smbus_prepare_writeblock(struct i2c_kmsg *msg)
{
    if (msg->flags & (I2C_KMSG_RD | I2C_KMSG_RECV_LEN)) {
        return -I2C_EMSGATTR;
    }

    if (!msg->wbuf || msg->wlen < I2C_SMBUS_LEN_CMD_BLOCK ||
        msg->rbuf || msg->rlen) {
        return -I2C_EMSGSMBUS;
    }

    if (msg->wlen - 2 != msg->wbuf[1] || msg->wbuf[1] > I2C_SMBUS_LEN_MAX) { // sub 2
        return -I2C_EMSGSMBUS;
    } else {
        return i2c_smbus_pec_prepare(msg);
    }
}

static int i2c_smbus_prepare_readblock(struct i2c_kmsg *msg)
{
    if (!(msg->flags & I2C_KMSG_RD)) {
        return -I2C_EMSGATTR;
    }

    if (!(msg->flags & I2C_KMSG_RECV_LEN)) {
        return -I2C_EMSGATTR;
    }

    if (!msg->wbuf || msg->wlen != I2C_SMBUS_LEN_CMD ||
        !msg->rbuf || !msg->rlen) {
        return -I2C_EMSGSMBUS;
    } else {
        return i2c_smbus_pec_prepare(msg);
    }
}

static int i2c_smbus_prepare_processcall(struct i2c_kmsg *msg)
{
    if (!(msg->flags & I2C_KMSG_RD)) {
        return -I2C_EMSGATTR;
    }

    if (msg->flags & I2C_KMSG_RECV_LEN) {
        return -I2C_EMSGATTR;
    }

    if (!msg->wbuf || msg->wlen != I2C_SMBUS_LEN_CMD_WORD ||
        !msg->rbuf || msg->rlen != I2C_SMBUS_LEN_WORD) {
        return -I2C_EMSGSMBUS;
    } else {
        return i2c_smbus_pec_prepare(msg);
    }
}

static int i2c_smbus_prepare_block_processcall(struct i2c_kmsg *msg)
{
    if (!(msg->flags & I2C_KMSG_RD)) {
        return -I2C_EMSGATTR;
    }

    if (!(msg->flags & I2C_KMSG_RECV_LEN)) {
        return -I2C_EMSGATTR;
    }

    if (!msg->wbuf || msg->wlen < I2C_SMBUS_LEN_CMD_BLOCK ||
        !msg->rbuf || !msg->rlen) {
        return -I2C_EMSGSMBUS;
    }

    if (msg->wlen - 2 != msg->wbuf[1] || msg->wbuf[1] > I2C_SMBUS_LEN_MAX) { // sub 2
        return -I2C_EMSGSMBUS;
    } else {
        return i2c_smbus_pec_prepare(msg);
    }
}

static int i2c_smbus_prepare_msg(struct i2c_kmsg *msg)
{
    static const i2c_smbus_prepare_t routine[I2C_SMBUS_KM_MAX] = {
        i2c_smbus_prepare_quick,             /* quick write */
        i2c_smbus_prepare_quick,             /* quick read */
        i2c_smbus_prepare_sendbyte,          /* send byte */
        i2c_smbus_prepare_recvbyte,          /* recieve byte */
        i2c_smbus_prepare_writebyte,         /* write byte */
        i2c_smbus_prepare_readbyte,          /* read byte */
        i2c_smbus_prepare_writeword,         /* write word */
        i2c_smbus_prepare_readword,          /* read word */
        i2c_smbus_prepare_writeblock,        /* write block */
        i2c_smbus_prepare_readblock,         /* read block */
        i2c_smbus_prepare_processcall,       /* process call */
        i2c_smbus_prepare_block_processcall, /* block process call */
    };

    if (msg->flags & (I2C_KMSG_TEN | I2C_KMSG_DMA)) {
        return -I2C_EMSGATTR;
    }

    if (!(msg->flags & I2C_KMSG_SMBUS)) {
        return -I2C_EMSGATTR;
    }

    if (msg->protocol >= I2C_SMBUS_KM_MAX || !routine[msg->protocol]) {
        return -I2C_EUNSUPSMBUSPROTO;
    } else {
        return (*routine[msg->protocol])(msg);
    }
}

static int i2c_smbus_xfer_verify(struct i2c_kmsg *msg)
{
    unsigned char wlen = 0;
    unsigned char rlen = 0;

    switch (msg->protocol) {
        case I2C_SMBUS_KM_BLK_PROC_CALL:
            wlen = msg->wbuf[1];
            rlen = msg->rbuf[0];
            break;
        case I2C_SMBUS_KM_READ_BLOCK:
            rlen = msg->rbuf[0];
            break;
        default:
            break;
    }

    if (wlen + rlen > I2C_SMBUS_LEN_MAX) {
        return -I2C_EMSGSMBUS;
    } else {
        return i2c_smbus_pec_verify(msg);
    }
}

int i2c_smbus_xfer_emulated(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    int ret;

    ret = i2c_smbus_prepare_msg(msg);
    if (ret) {
        return ret;
    }

    ret = i2c_xfer(adap, msg);
    if (ret) {
        return ret;
    }

    return i2c_smbus_xfer_verify(msg);
}

int i2c_smbus_xfer(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    if (!(adap->deploy_flags & I2C_KDEPLOY_SPEED_MODE_STD)) {
        return -EOPNOTSUPP;
    }

    if (!(adap->deploy_flags & I2C_KDEPLOY_SMBUS)) {
        return -EOPNOTSUPP;
    } else {
        return i2c_smbus_xfer_emulated(adap, msg);
    }
}
