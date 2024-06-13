// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * DesignWare i2c driver (master only).
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "i2c.h"
#include "i2c_api.h"
#include "i2c_dw.h"

struct dma_map_addr_info {
    void *addr;
    size_t buf_size;
    size_t transfer_size;
};

static int i2c_dw_master_dma_prepare_write_byte(struct i2c_adapter *adap, struct i2c_kmsg *msg,
                                                unsigned short *wdata)
{
    unsigned short val;
    unsigned short cmd = 0;

    if (msg->wcnt >= msg->wlen) {
        return -1;
    }

    if (msg->wcnt + 1 == msg->wlen) {
        if (!(msg->flags & I2C_KMSG_RD)) {
            cmd |= IC_DATA_CMD_RECV;
        }
    }

    val = msg->wbuf[msg->wcnt++];
    *wdata = (cmd | val);
    return 0;
}

static void i2c_dw_master_dma_prepare_read_byte(struct i2c_adapter *adap, struct i2c_kmsg *msg,
                                                unsigned short *wdata)
{
    unsigned short cmd = 0;

    if (msg->cmd_len == 1) {
        cmd |= IC_DATA_CMD_RECV;
    }

    if (msg->wcnt && !adap->restart_sent) {
        if (adap->control & IC_CON_RESTART_EN) {
            cmd |= IC_DATA_CMD_RESATRT;
        }
        adap->restart_sent = 1;
    }

    msg->rcnt++;
    *wdata = (cmd | IC_DATA_CMD_WRITE);
}

static int i2c_dw_master_dma_prepare_byte(struct i2c_adapter *adap, struct i2c_kmsg *msg, unsigned short *wdata)
{
    int ret;

    ret = i2c_dw_master_dma_prepare_write_byte(adap, msg, wdata);
    if (!ret) {
        return I2C_DW_MASTER_CONTINUE;
    }

    if (!(msg->flags & I2C_KMSG_RD)) {
        return I2C_DW_MASTER_BREAK;
    } else {
        i2c_dw_master_dma_prepare_read_byte(adap, msg, wdata);
        return I2C_DW_MASTER_NEXT;
    }
}

static void i2c_dw_master_dma_prepare(struct i2c_adapter *adap, struct i2c_kmsg *msg, unsigned short *wdata)
{
    unsigned int i;
    int ret;

    for (i = 0; msg->cmd_len > 0; msg->cmd_len--, i++) {
        ret = i2c_dw_master_dma_prepare_byte(adap, msg, &wdata[i]);
        if (ret == I2C_DW_MASTER_BREAK) {
            break;
        }
    }
}

static int i2c_dw_master_dma_map(struct i2c_adapter *adap, struct dma_map_addr_info map_addr_info,
                                 enum i2c_dma_type type, dma_addr_t *dma_buf, int *channel)
{
    dma_addr_t dbuf;
    int chnl;

    dbuf = i2c_adapter_platform_dma_map(adap, map_addr_info.addr, map_addr_info.buf_size, type);
    if (!dbuf) {
        return -I2C_EDMAMAP;
    }

    chnl = i2c_adapter_platform_dma_config(adap, dbuf, map_addr_info.transfer_size, type);
    if (chnl < 0) {
        i2c_adapter_platform_dma_unmap(adap, dbuf, map_addr_info.buf_size, type);
        return -I2C_EDMACONFIG;
    }

    *dma_buf = dbuf;
    *channel = chnl;

    return 0;
}

static void i2c_dw_master_dma_xfer_init(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    i2c_writel(adap, IC_DMA_RDLR_CNT, ICR_DMA_RDLR);
    i2c_writel(adap, IC_DMA_TDLR_CNT, ICR_DMA_TDLR);
    i2c_dw_common_enable(adap);
    i2c_readl(adap, ICR_ENABLE_STATUS);
}

int i2c_dw_master_dma_xfer(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    unsigned int interrupt_mask;
    dma_addr_t wdma = 0;
    dma_addr_t rdma = 0;
    unsigned short *wdata = NULL;
    int wchnl = -1;
    int rchnl = -1;
    size_t wsize;
    size_t wcnt;
    int tmp;
    int ret;
    unsigned int val = 0;
    struct dma_map_addr_info map_addr_info;
    struct dma_map_addr_info map_addr_info_r;

    adap->dma_cmd_address = (dma_addr_t)(adap->di.phy_base + ICR_DATA_CMD);
    wcnt = msg->cmd_len;
    wsize = wcnt * sizeof(unsigned short);
    wdata = i2c_mcalloc(I2C_MTYPE_DMA_WRITE, wsize);
    if (!wdata) {
        return -ENOMEM;
    }

    i2c_dw_master_dma_prepare(adap, msg, wdata);
    i2c_dw_master_dma_xfer_init(adap, msg);

    map_addr_info = (struct dma_map_addr_info){wdata, wsize, wcnt};
    ret = i2c_dw_master_dma_map(adap, map_addr_info, I2C_DMA_TO_DEVICE, &wdma, &wchnl);
    if (ret) {
        goto end_process;
    }

    map_addr_info_r = (struct dma_map_addr_info){msg->rbuf, msg->rlen, msg->rlen};
    if (msg->flags & I2C_KMSG_RD) {
        ret = i2c_dw_master_dma_map(adap, map_addr_info_r, I2C_DMA_FROM_DEVICE, &rdma, &rchnl);
        if (ret) {
            goto end_process;
        }
    }

    val |= IC_DMA_CR_RDMAE;
    val |= rdma ? IC_DMA_CR_TDMAE : 0x0;
    i2c_writel(adap, val, ICR_DMA_CR);

    ret = i2c_adapter_platform_dma_wait_complete(adap, wchnl);
    tmp = i2c_adapter_platform_dma_wait_complete(adap, rchnl);
    ret = tmp ? : ret;
    interrupt_mask = ret ? 0 : IC_INTR_MASTER_DMA_MASK;

    i2c_writel(adap, 0x0, ICR_DMA_CR);
    i2c_writel(adap, interrupt_mask, ICR_INTR_MASK);
end_process:
    i2c_adapter_platform_dma_unmap(adap, wdma, wsize, I2C_DMA_TO_DEVICE);
    i2c_adapter_platform_dma_unmap(adap, rdma, msg->rlen, I2C_DMA_FROM_DEVICE);
    i2c_mfree(I2C_MTYPE_DMA_WRITE, wdata);

    return ret;
}

int i2c_dw_master_dma_xfer_start(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    if (msg->flags & (I2C_KMSG_RECV_LEN | I2C_KMSG_SMBUS | I2C_KMSG_PEC)) {
        return -I2C_EMSGATTR;
    }

    if (!msg->cmd_len) {
        return -I2C_EDMAZEROWRITE;
    }

    return i2c_dw_master_dma_xfer(adap, msg);
}

