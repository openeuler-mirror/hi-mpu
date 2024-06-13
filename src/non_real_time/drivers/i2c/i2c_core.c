// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Realize data transmission of i2c
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "i2c.h"
#include "i2c_api.h"
#include "i2c_core.h"

static int i2c_check_for_msg_quirks(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    const struct i2c_adapter_quirks *quirks = adap->quirks;

    if (msg->wlen > quirks->max_write_len) {
        return -EMSGSIZE;
    }

    if (!(msg->flags & I2C_KMSG_RECV_LEN) && (msg->rlen > quirks->max_read_len)) {
        return -EMSGSIZE;
    }

    return 0;
}

static int i2c_check_for_msg_length(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    if (!(msg->flags & I2C_KMSG_RD)) {
        if (msg->flags & I2C_KMSG_RECV_LEN) {
            return -I2C_EMSGATTR;
        }

        if (msg->rbuf || msg->rlen) {
            return -I2C_EMSGWORBUF;
        }
    } else {
        if (!msg->rbuf || !msg->rlen) {
            return -I2C_EMSGNORBUF;
        }
    }

    return 0;
}

static int i2c_check_for_msg(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    int ret;

    ret = i2c_check_for_msg_length(adap, msg);
    if (ret) {
        return ret;
    }

    ret = i2c_check_for_msg_quirks(adap, msg);
    if (ret) {
        return ret;
    }

    if (msg->flags & I2C_KMSG_DMA) {
        if (msg->flags & (I2C_KMSG_RECV_LEN | I2C_KMSG_SMBUS | I2C_KMSG_PEC)) {
            return -I2C_EMSGATTR;
        }
    }

    if ((msg->flags & I2C_KMSG_PEC) && !(msg->flags & I2C_KMSG_SMBUS)) {
        return -I2C_EMSGATTR;
    }

    return 0;
}

static void i2c_xfer_result_dump(struct i2c_adapter *adap, struct i2c_kmsg *msg, int xfer_ret, int ret)
{
    unsigned char level;

    level = i2c_adapter_acquire_loglevel(adap, msg->addr);
    if (level != LOG_ERROR) {
        return;
    }

    i2c_print("[I2C-%02u-RT<%d|%d>] "
              "M<0x%x|0x%x|0x%x|%u|%lu> "
              "L<%u|%u|%u> "
              "C<%u|%u|%u> "
              "E<0x%x|0x%x|0x%x|0x%x> "
              "I<0x%x|0x%x|0x%x>\n",
              adap->drv_id, xfer_ret, ret,
              msg->addr, msg->flags, msg->protocol, msg->retries, msg->timeout,
              msg->wlen, msg->rlen, msg->cmd_len,
              msg->wcnt, msg->rcnt, msg->outstanding,
              adap->cmd_error, adap->msg_error, adap->status, adap->abort_source,
              adap->irq_status, adap->irq_raw_status, adap->irq_mask);
}

static int i2c_xfer_result(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    if (!msg->rcnt || msg->rcnt > msg->rlen) {
        return -I2C_EMSGRCVLEN;
    }

    if (msg->flags & I2C_KMSG_PEC) {
        msg->recvd_crc = msg->rbuf[msg->rcnt - 1];
        msg->rlen = msg->rcnt - 1;
    } else {
        msg->rlen = msg->rcnt;
    }

    if (msg->rlen > 0) {
        return 0;
    } else {
        return -I2C_EMSGRCVLEN;
    }
}

static void i2c_xfer_failed(struct i2c_adapter *adap, struct i2c_kmsg *msg, int xfer_ret, int ret)
{
    i2c_xfer_result_dump(adap, msg, xfer_ret, ret);

    i2c_adapter_platform_reset(adap);

    if (xfer_ret == -EBUSY || xfer_ret == -ETIME || adap->may_hangup) {
        i2c_adapter_platform_recovery(adap);
    }
}

static unsigned int i2c_xfer_rlen(struct i2c_kmsg *msg)
{
    unsigned int cmd_rlen;

    cmd_rlen = (msg->flags & I2C_KMSG_RECV_LEN) ? 1 : msg->rlen;
    cmd_rlen = (msg->flags & I2C_KMSG_RD) ? cmd_rlen : 0;
    return cmd_rlen;
}

static void i2c_xfer_init(struct i2c_kmsg *msg)
{
    msg->recvd_crc = 0;
    /* By default, retransmission is 3 times. */
    msg->retries = msg->retries ? : 3;
    msg->retries = min_t(unsigned int, msg->retries, 10);
    msg->timeout = msg->timeout ? : (3 * HZ); // HZ mul 3
}

static int i2c_xfer_verify(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    if (!adap->algo->i2c_master_xfer) {
        return -EOPNOTSUPP;
    }

    return i2c_check_for_msg(adap, msg);
}

int i2c_xfer(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    unsigned int tries = 0;
    unsigned int cmd_rlen;
    unsigned int delay;
    int xfer_ret = -I2C_EXFER;
    int ret;

    ret = i2c_xfer_verify(adap, msg);
    if (ret) {
        return ret;
    }

    i2c_xfer_init(msg);

    cmd_rlen = i2c_xfer_rlen(msg);

    while (tries++ < msg->retries) {
        msg->cmd_len = msg->wlen + cmd_rlen;

        ret = adap->algo->i2c_master_xfer(adap, msg);
        xfer_ret = ret;

        if (ret == -EAGAIN) {
            delay = i2c_random() % 90000 + 10000; /* 90000/10000 use to generate random numbers */
            i2c_udelay(delay);
            continue;
        } else if (ret || !(msg->flags & I2C_KMSG_RD)) {
            break;
        } else {
            ret = i2c_xfer_result(adap, msg);
            break;
        }
    }

    if (ret) {
        i2c_xfer_failed(adap, msg, xfer_ret, ret);
    }

    return ret;
}

__weak int i2c_smbus_xfer(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    return -EOPNOTSUPP;
}

