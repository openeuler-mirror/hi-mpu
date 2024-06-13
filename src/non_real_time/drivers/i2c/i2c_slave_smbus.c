// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * I2C slave mode SMBus simulator.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "i2c_dw.h"
#include "lp_subsys_csr_addr_define.h"
#include "lp_subsys.h"

static inline void i2c_slave_smbus_slave_reset(struct i2c_adapter *adap)
{
    unsigned int offset, mask;

    offset = LP_SUBSYS_CSR_LPSC_SRST_REQ0_OFFSET_ADDR;
    mask = 0x1U << (adap->drv_id + IC_SRST_REG_BASE_OFFSET);
    lp_subsys_srst(offset, mask);
    i2c_print("[I2C-%02u-RT] reset i2c adapter.", adap->drv_id);
}

static void i2c_slave_smbus_abort(struct i2c_adapter *adap)
{
    struct i2c_smbus_control *smbus = adap->slave_priv;
    unsigned long flags = 0;

    i2c_dw_common_disable_no_wait(adap);
    adap->slave_status = IC_S_STATUS_IDLE;
    smbus->req_data_index = 0;

    spin_lock_irqsave(&smbus->lock, flags);
    smbus->res_data_index = 0;
    smbus->ret_response = false;
    spin_unlock_irqrestore(&smbus->lock, flags);
    i2c_dw_common_enable(adap);
}

static void i2c_slave_smbus_write(struct i2c_adapter *adap)
{
    struct i2c_smbus_control *smbus = adap->slave_priv;
    struct i2c_queue_node *node = NULL;
    unsigned int rxflr, i;

    rxflr = i2c_readl(adap, ICR_RXFLR);
    for (i = 0; i < rxflr; i++) {
        smbus->buf_request[smbus->req_data_index++] = (unsigned char)i2c_readl(adap, ICR_DATA_CMD);
        if (smbus->req_data_index >= smbus->buf_len) {
            smbus->req_data_index = smbus->req_data_index % smbus->buf_len;
            i2c_print_limit("[I2C-%02u-RT] write data exceeds buffer size.", smbus->drv_id);
        }
    }

    if (adap->slave_status & IC_S_STATUS_WRITE_REQUESTED) {
        return;
    }

    if (i2c_queue_empty(&smbus->req_queue_unused)) {
        node = i2c_deque(&smbus->req_queue_inused);
        i2c_print_limit("[I2C-%02u-RT] cache queue overflow!", smbus->drv_id);
    } else {
        node = i2c_deque(&smbus->req_queue_unused);
    }

    if (!node) {
        i2c_print("[I2C-%02u-RT] i2c dequeue error", smbus->drv_id);
        return;
    }

    memcpy_s(node->buf, smbus->req_data_index, smbus->buf_request, smbus->req_data_index);
    node->buf_len = smbus->req_data_index;
    smbus->req_data_index = 0;

    i2c_enque(&smbus->req_queue_inused, node);
    wake_up_interruptible(&smbus->wait);
}

static void i2c_slave_smbus_read(struct i2c_adapter *adap)
{
    struct i2c_smbus_control *smbus = adap->slave_priv;
    unsigned long flags = 0;
    unsigned char val = 0;
    unsigned int cnt = 0;
    unsigned int i = 0;

    cnt = min_t(unsigned int, IC_S_TX_FIFO_DEPTH_SMBUS - i2c_readl(adap, ICR_TXFLR),
                smbus->res_data_len - smbus->res_data_index);
    if (cnt == 0) {
        spin_lock_irqsave(&smbus->lock, flags);
        smbus->ret_response = false;
        mod_timer(&smbus->clk_strech_timer, jiffies + msecs_to_jiffies(smbus->clk_strech_timeout));
        spin_unlock_irqrestore(&smbus->lock, flags);
        return;
    }

    for (i = 0; i < cnt; i++) {
        val = smbus->buf_response[smbus->res_data_index++];
        i2c_writel(adap, val, ICR_DATA_CMD);
    }
}

static void i2c_slave_smbus_read_request(struct i2c_adapter *adap)
{
    struct i2c_smbus_control *smbus = adap->slave_priv;
    unsigned long flags = 0;

    spin_lock_irqsave(&smbus->lock, flags);
    if (!smbus->ret_response) {
        mod_timer(&smbus->clk_strech_timer, jiffies + msecs_to_jiffies(smbus->clk_strech_timeout));
        spin_unlock_irqrestore(&smbus->lock, flags);
    } else {
        spin_unlock_irqrestore(&smbus->lock, flags);
        i2c_slave_smbus_read(adap);
    }
}

static void i2c_slave_smbus_stop(struct i2c_adapter *adap)
{
    struct i2c_smbus_control *smbus = adap->slave_priv;
    unsigned long flags = 0;

    if (adap->slave_status & IC_S_STATUS_READ_REQUESTED) {
        spin_lock_irqsave(&smbus->lock, flags);
        smbus->res_data_index = 0;
        smbus->ret_response = false;
        spin_unlock_irqrestore(&smbus->lock, flags);
    } else if (i2c_readl(adap, ICR_RXFLR) || (adap->slave_status & IC_S_STATUS_WRITE_REQUESTED)) {
        adap->slave_status &= ~IC_S_STATUS_WRITE_REQUESTED;
        i2c_slave_smbus_write(adap);
    }
}

static void i2c_slave_smbus_stop_detect_timer_handler(struct timer_list *tl)
{
    struct i2c_smbus_control *smbus = from_timer(smbus, tl, stop_detect_timer);
    struct i2c_adapter *adap = NULL;
    struct i2c_queue_node *node = NULL;

    adap = i2c_adapter_lookup(smbus->drv_id);
    if (!adap) {
        i2c_print("[I2C-%02u-RT] bus id error.", smbus->drv_id);
        return;
    }

    while (!i2c_queue_empty(&smbus->req_queue_inused)) {
        node = i2c_deque(&smbus->req_queue_inused);
        i2c_enque(&smbus->req_queue_unused, node);
    }
    i2c_slave_smbus_abort(adap);
    i2c_slave_smbus_slave_reset(adap);
    i2c_print("[I2C-%02u-RT] stop event lost, clear data", smbus->drv_id);
}

static void i2c_slave_smbus_clk_strech_timer_handler(struct timer_list *tl)
{
    unsigned int i;
    unsigned long flags = 0;
    struct i2c_adapter *adap = NULL;
    struct i2c_smbus_control *smbus = from_timer(smbus, tl, clk_strech_timer);

    adap = i2c_adapter_lookup(smbus->drv_id);
    if (!adap) {
        i2c_print("[I2C-%02u-RT] bus id error.", smbus->drv_id);
        return;
    }

    del_timer(&smbus->stop_detect_timer);

    spin_lock_irqsave(&smbus->lock, flags);
    if (smbus->ret_errcode) {
        /* After the clock extension times out, the slave device directly resets and releases the SCL line
         * can result in the master device suspending.
         * The slave module returns an error code before being reset to prevent the master module from being suspended.
         */
        for (i = 0; i < smbus->err_code_len; i++) {
            i2c_writel(adap, smbus->err_code[i], ICR_DATA_CMD);
        }
    }
    spin_unlock_irqrestore(&smbus->lock, flags);
    /* Waiting for the master to read the error code */
    udelay(IC_MSG_TRAN_TIME_EACH_BYTE * smbus->err_code_len);

    i2c_slave_smbus_slave_reset(adap);
    adap->slave_status = IC_S_STATUS_IDLE;
}

static int i2c_slave_smbus_handle(struct i2c_adapter *adap, enum i2c_slave_event event)
{
    switch (event) {
        case I2C_SLAVE_READ_REQUESTED:
            if (i2c_readl(adap, ICR_RXFLR) || (adap->slave_status & IC_S_STATUS_WRITE_REQUESTED)) {
                adap->slave_status &= ~IC_S_STATUS_WRITE_REQUESTED;
                i2c_slave_smbus_write(adap);
            }
            i2c_slave_smbus_read_request(adap);
            break;
        case I2C_SLAVE_READ_PROCESSED:
            i2c_slave_smbus_read(adap);
            break;
        case I2C_SLAVE_WRITE_REQUESTED:
            break;
        case I2C_SLAVE_WRITE_RECEIVED:
            i2c_slave_smbus_write(adap);
            break;
        case I2C_SLAVE_STOP:
            i2c_slave_smbus_stop(adap);
            break;
        case I2C_SLAVE_ABORT:
            i2c_slave_smbus_abort(adap);
            break;
        default:
            return -I2C_EDEFAULT;
    }

    return 0;
}

static void i2c_slave_smbus_control_init(struct i2c_adapter *adap)
{
    struct i2c_smbus_control *smbus = adap->slave_priv;
    struct i2c_queue_node *node = NULL;
    unsigned int i;

    /* Sets the TX_FIFO and RX_FIFO thresholds. */
    i2c_writel(adap, IC_S_TX_FIFO_TL_COUNT_SMBUS - 1, ICR_TX_TL);
    i2c_writel(adap, IC_S_RX_FIFO_TL_COUNT_SMBUS - 1, ICR_RX_TL);

    smbus->ret_response = false;
    smbus->ret_errcode = false;
    smbus->err_code_len = 0;
    smbus->req_data_index = 0;
    smbus->res_data_index = 0;

    smbus->drv_id = adap->drv_id;
    smbus->clk_strech_timeout = adap->clk_strech_timeout;
    smbus->buf_len = adap->slave_dlen;
    smbus->buf_request = adap->slave_data;
    smbus->buf_response = adap->slave_data + smbus->buf_len;

    i2c_queue_init(&smbus->req_queue_inused);
    i2c_queue_init(&smbus->req_queue_unused);
    for (i = 0; i < IC_MSG_MAX_NUM; i++) {
        smbus->i2c_msg_buf[i].buf = smbus->buf_response + smbus->buf_len * (i + 1);
        node = &smbus->i2c_msg_buf[i];
        i2c_enque(&smbus->req_queue_unused, node);
    }

    spin_lock_init(&smbus->lock);
    init_waitqueue_head(&smbus->wait);
    timer_setup(&smbus->clk_strech_timer, i2c_slave_smbus_clk_strech_timer_handler, 0);
    timer_setup(&smbus->stop_detect_timer, i2c_slave_smbus_stop_detect_timer_handler, 0);
}

static int i2c_slave_smbus_slave_cb(struct i2c_adapter *adap, enum i2c_slave_event event)
{
    if (!adap->slave_priv || !adap->slave_data || !adap->slave_dlen) {
        return -I2C_EDEFAULT;
    }

    return i2c_slave_smbus_handle(adap, event);
}

struct i2c_slave g_slave_smbus = {
    .name = "smbus",
    .control_memlen = sizeof(struct i2c_smbus_control),
    .slave_cb = i2c_slave_smbus_slave_cb,
    .slave_init = i2c_slave_smbus_control_init,
};