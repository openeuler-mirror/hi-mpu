// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * DesignWare i2c driver (master only).
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "i2c.h"
#include "i2c_api.h"
#include "i2c_dw.h"

static void i2c_dw_master_configure(struct i2c_adapter *adap, struct i2c_kdepl *deploy)
{
    unsigned int val;

    i2c_writel(adap, adap->tx_fifo_depth / 2, ICR_TX_TL); // div 2
    i2c_writel(adap, 0, ICR_RX_TL);
    i2c_writel(adap, adap->control, ICR_CONTROL);

    val = deploy->scl_low_timeout ? : IC_SCL_LOW_TIMEOUT_DEFAULT;
    i2c_writel(adap, val, ICR_SCL_LOW_TIMEOUT);
    val = deploy->sda_low_timeout ? : IC_SDA_LOW_TIMEOUT_DEFAULT;
    i2c_writel(adap, val, ICR_SDA_LOW_TIMEOUT);
}

static void i2c_dw_master_set_fifo_depth(struct i2c_adapter *adap)
{
    unsigned int val, tx_depth, rx_depth;

    val = i2c_readl(adap, ICR_COMP_PARAM_1);
    tx_depth = ((val >> 16) & 0xff) + 1; /* Bit16~bit23 is tx_fifo_depth */
    rx_depth = ((val >> 8) & 0xff) + 1;  /* Bit8~bit15 is rx_fifo_depth */

    adap->tx_fifo_depth = adap->tx_fifo_depth ? : tx_depth;
    adap->rx_fifo_depth = adap->rx_fifo_depth ? : rx_depth;
    adap->tx_fifo_depth = min_t(unsigned int, adap->tx_fifo_depth, tx_depth);
    adap->rx_fifo_depth = min_t(unsigned int, adap->rx_fifo_depth, rx_depth);
}

static void i2c_dw_master_recv_length(struct i2c_adapter *adap, struct i2c_kmsg *msg, unsigned int len)
{
    unsigned int val;
    unsigned int temp_len = len;

    val = min_t(unsigned int, msg->rlen, adap->quirks->max_read_len);
    temp_len += (msg->flags & I2C_KMSG_PEC) ? 1 : 0;
    adap->msg_error |= (temp_len > val) ? I2C_MSG_ERR_RX_EXCEED : 0x0;
    msg->cmd_len = temp_len - min_t(unsigned int, temp_len, msg->outstanding);
    msg->flags &= ~I2C_KMSG_RECV_LEN;
}

static void i2c_dw_master_irq_read(struct i2c_adapter *adap)
{
    struct i2c_kmsg *msg = adap->msg;
    unsigned int rxflr = i2c_readl(adap, ICR_RXFLR);
    unsigned char val;
    unsigned int cnt;

    if (msg == NULL) {
        return;
    }

    for (cnt = 0; cnt < rxflr; cnt++) {
        val = (unsigned char)i2c_readl(adap, ICR_DATA_CMD);
        msg->outstanding--;

        if (msg->rcnt >= msg->rlen) {
            break;
        }

        msg->rbuf[msg->rcnt++] = val;

        if (!(msg->flags & I2C_KMSG_RECV_LEN)) {
            continue;
        }

        i2c_dw_master_recv_length(adap, msg, val);
    }

    if (msg->cmd_len > 0) {
        adap->status |= IC_M_STATUS_READ_IN_PROGRESS;
    } else {
        adap->status &= ~IC_M_STATUS_READ_IN_PROGRESS;
    }
}

static int i2c_dw_master_irq_xfer_write_byte(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    unsigned int cmd = 0;
    unsigned char val;

    if (msg->wcnt >= msg->wlen || !adap->msg->wbuf) {
        return -1;
    }

    if (msg->wcnt + 1 == msg->wlen) {
        if (!(msg->flags & I2C_KMSG_RD)) {
            cmd |= IC_DATA_CMD_RECV;
        }
    }

    val = msg->wbuf[msg->wcnt++];
    i2c_writel(adap, cmd | val, ICR_DATA_CMD);
    return 0;
}

static void i2c_dw_master_irq_xfer_read_byte(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    unsigned int cmd = 0;

    if (msg->cmd_len == 1) {
        if (!(msg->flags & I2C_KMSG_RECV_LEN)) {
            cmd |= IC_DATA_CMD_RECV;
        }
    }

    if (msg->wcnt && !adap->restart_sent) {
        if (adap->control & IC_CON_RESTART_EN) {
            cmd |= IC_DATA_CMD_RESATRT;
        }
        adap->restart_sent = 1;
    }

    i2c_writel(adap, cmd | IC_DATA_CMD_WRITE, ICR_DATA_CMD);
    msg->outstanding++;
}

static int i2c_dw_master_irq_xfer_msg_byte(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    int ret;

    ret = i2c_dw_master_irq_xfer_write_byte(adap, msg);
    if (!ret) {
        return I2C_DW_MASTER_CONTINUE;
    }

    if (!(msg->flags & I2C_KMSG_RD)) {
        return I2C_DW_MASTER_BREAK;
    }

    if (msg->outstanding >= adap->rx_fifo_depth) {
        return I2C_DW_MASTER_BREAK;
    }

    i2c_dw_master_irq_xfer_read_byte(adap, msg);

    return I2C_DW_MASTER_NEXT;
}

static void i2c_dw_master_irq_xfer_msg(struct i2c_adapter *adap)
{
    struct i2c_kmsg *msg = adap->msg;
    unsigned int interrupt_mask = IC_INTR_MASTER_MASK;
    int wlimit, rlimit;
    int ret;

    wlimit = adap->tx_fifo_depth - i2c_readl(adap, ICR_TXFLR);
    rlimit = adap->rx_fifo_depth - i2c_readl(adap, ICR_RXFLR);

    for (; msg->cmd_len > 0 && wlimit > 0 && rlimit > 0; wlimit--, msg->cmd_len--) {
        ret = i2c_dw_master_irq_xfer_msg_byte(adap, msg);
        if (ret == I2C_DW_MASTER_BREAK) {
            break;
        } else if (ret == I2C_DW_MASTER_NEXT) {
            rlimit--;
        }
    }

    if (msg->cmd_len > 0 || (msg->flags & I2C_KMSG_RECV_LEN)) {
        interrupt_mask |= IC_INTR_TX_EMPTY;
        adap->status |= IC_M_STATUS_WRITE_IN_PROGRESS;
    } else {
        interrupt_mask &= ~IC_INTR_TX_EMPTY;
        adap->status &= ~IC_M_STATUS_WRITE_IN_PROGRESS;
    }

    i2c_writel(adap, interrupt_mask, ICR_INTR_MASK);
}

static void i2c_dw_master_irq_xfer_complete(struct i2c_adapter *adap, unsigned int irq_status)
{
    unsigned int interrupt_mask = IC_INTR_TX_ABRT | IC_INTR_SCL_STUCK_LOW;

    if ((irq_status & IC_INTR_STOP_DET) || adap->cmd_error || adap->msg_error) {
        if (i2c_readl(adap, ICR_TXFLR)) {
            adap->cmd_error |= I2C_CMD_ERR_TX_UNFINISHED;
        }

        if (i2c_readl(adap, ICR_RXFLR)) {
            adap->cmd_error |= I2C_CMD_ERR_RX_UNFINISHED1;
        }

        if (adap->msg && adap->msg->outstanding) {
            adap->cmd_error |= I2C_CMD_ERR_RX_UNFINISHED2;
        }

        if (adap->cmd_error || adap->msg_error) {
            interrupt_mask = 0;
        }

        adap->status &= ~IC_M_STATUS_WAIT4_COMPLETION;
        i2c_writel(adap, interrupt_mask, ICR_INTR_MASK);
        complete(&adap->cmd_complete);
    }
}

static unsigned int i2c_dw_master_irq_errors(struct i2c_adapter *adap, unsigned int irq_status)
{
    unsigned int cmd_errors = 0;

    if (!(i2c_readl(adap, ICR_ENABLE) & IC_ENA_ENABLE)) {
        cmd_errors |= I2C_CMD_ERR_NOT_ENABLE;
    } else if (!adap->msg) {
        cmd_errors |= I2C_CMD_ERR_NO_MSG_PTR;
    } else if (!(irq_status & ~IC_INTR_ACTIVITY)) {
        cmd_errors |= I2C_CMD_ERR_NO_ACTIVITY_INT;
    } else if (irq_status & IC_INTR_TX_ABRT) {
        cmd_errors |= I2C_CMD_ERR_TX_ABRT;
    } else if (irq_status & IC_INTR_RX_UNDER) {
        cmd_errors |= I2C_CMD_ERR_RX_UNDER;
    } else if (irq_status & IC_INTR_RX_OVER) {
        cmd_errors |= I2C_CMD_ERR_RX_OVER;
    } else if (irq_status & IC_INTR_TX_OVER) {
        cmd_errors |= I2C_CMD_ERR_TX_OVER;
    } else if (irq_status & IC_INTR_SCL_STUCK_LOW) {
        cmd_errors |= I2C_CMD_ERR_SCL_STUCK_LOW;
    } else {
        /* do nothing */
    }

    return cmd_errors;
}

static void i2c_dw_master_irq_handler(struct i2c_adapter *adap, unsigned int irq_status)
{
    unsigned int cmd_errors;

    cmd_errors = i2c_dw_master_irq_errors(adap, irq_status);
    if (cmd_errors) {
        goto I2C_END;
    }

    if (irq_status & IC_INTR_RX_FULL) {
        if (!adap->msg->rbuf) {
            cmd_errors |= I2C_CMD_ERR_NO_MSG_RBUF;
            goto I2C_END;
        }
        i2c_dw_master_irq_read(adap);
    }

    if (!adap->msg_error && (irq_status & IC_INTR_TX_EMPTY)) {
        i2c_dw_master_irq_xfer_msg(adap);
    }

I2C_END:
    adap->cmd_error |= cmd_errors;
    i2c_dw_master_irq_xfer_complete(adap, irq_status);
}

irqreturn_t i2c_dw_master_isr(int irq, void *arg)
{
    struct i2c_adapter *adap = arg;
    unsigned int irq_status;

    if (!i2c_readl(adap, ICR_ENABLE)) {
        return IRQ_NONE;
    }

    if (!(i2c_readl(adap, ICR_RAW_INTR_STAT) & ~IC_INTR_ACTIVITY)) {
        return IRQ_NONE;
    }

    irq_status = i2c_dw_common_rc_intrbits(adap);

    i2c_dw_master_irq_handler(adap, irq_status);

    if (adap->abort_source & IC_TX_ABRT_SDA_STUCK_LOW) {
        adap->may_hangup = 1;
    }

    if (adap->cmd_error & I2C_CMD_ERR_SCL_STUCK_LOW) {
        adap->may_hangup = 1;
    }

    return IRQ_HANDLED;
}

static void i2c_dw_master_reinit(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    reinit_completion(&adap->cmd_complete);
    adap->cmd_error = 0;
    adap->msg_error = 0;
    adap->restart_sent = 0;
    adap->status = IC_M_STATUS_INIT;
    adap->abort_source = 0;
    adap->may_hangup = 0;
    adap->irq_mask = 0;
    adap->irq_status = 0;
    adap->irq_raw_status = 0;

    msg->quick_write = 0;
    msg->quick_cmd = 0;
    msg->rcnt = 0;
    msg->wcnt = 0;
    msg->outstanding = 0;
    msg->flags = adap->msg_flags;
}

__weak int i2c_dw_master_dma_xfer_start(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    adap->cmd_error |= I2C_CMD_ERR_DMA_UNSUPPORTED;
    return -EOPNOTSUPP;
}

static void i2c_dw_master_smbus_xfer_init(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    unsigned int ic_tar = 0;

    switch (msg->protocol) {
        case I2C_SMBUS_KM_WRITE_QUICK:
            msg->quick_write = 1;
            msg->quick_cmd = 1;
            ic_tar = IC_TAR_SPECIAL | IC_TAR_SMBUS_QUICK_CMD;
            break;
        case I2C_SMBUS_KM_READ_QUICK:
            msg->quick_cmd = 1;
            ic_tar = IC_TAR_SPECIAL | IC_TAR_SMBUS_QUICK_CMD;
            break;
        default:
            break;
    }

    i2c_dw_common_disable_int(adap);
    i2c_readl(adap, ICR_CLR_INTR);
    i2c_writel(adap, adap->control, ICR_CONTROL);
    i2c_writel(adap, msg->addr | ic_tar, ICR_TAR);
}

static void i2c_dw_master_xfer_init(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    unsigned int ic_con = 0;
    unsigned int ic_tar = 0;

    if (msg->flags & I2C_KMSG_TEN) {
        ic_con |= IC_CON_10BITADDR_MASTER;
        ic_tar = IC_TAR_10BITADDR_MASTER;
    }

    i2c_dw_common_disable_int(adap);
    i2c_readl(adap, ICR_CLR_INTR);
    i2c_writel(adap, adap->control | ic_con, ICR_CONTROL);
    i2c_writel(adap, msg->addr | ic_tar, ICR_TAR);
}

static void i2c_dw_master_xfer_start(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    unsigned int interrupt_mask = IC_INTR_MASTER_MASK;
    unsigned int cmd;

    if (!msg->cmd_len) {
        interrupt_mask &= ~IC_INTR_TX_EMPTY;
    }

    i2c_dw_common_enable(adap);
    i2c_readl(adap, ICR_ENABLE_STATUS);

    if (msg->quick_cmd) {
        cmd = IC_DATA_CMD_RECV;
        cmd |= msg->quick_write ? 0 : IC_DATA_CMD_WRITE;
        i2c_writel(adap, cmd, ICR_DATA_CMD);
    }

    i2c_writel(adap, interrupt_mask, ICR_INTR_MASK);
}

static int i2c_dw_master_xfer_result(struct i2c_adapter *adap, long wait_ret)
{
    if (adap->msg_error) {
        return -EIO;
    } else if (adap->cmd_error) {
        if (adap->cmd_error & I2C_CMD_ERR_TIMEOUT) {
            return -ETIME;
        }
        if (adap->cmd_error & I2C_CMD_ERR_TX_ABRT) {
            if (adap->abort_source & IC_TX_ABRT_ARB_LOST) {
                return -EAGAIN;
            }
        }
        return -EIO;
    } else if (adap->status) {
        return -EIO;
    } else {
        return (wait_ret < 0) ? wait_ret : 0;
    }
}

static int i2c_dw_master_xfer_handle(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    unsigned long timeout = HZ;
    unsigned int wait = 1;
    long wait_ret = 1;
    int ret;

    if (msg->flags & I2C_KMSG_DMA) {
        ret = i2c_dw_master_dma_xfer_start(adap, msg);
        if (ret) {
            adap->cmd_error |= I2C_CMD_ERR_DMA;
            wait = 0;
        }
    } else {
        timeout = msg->timeout;
        i2c_dw_master_xfer_start(adap, msg);
    }

    if (wait) {
        wait_ret = wait_for_completion_interruptible_timeout(&adap->cmd_complete, timeout);

        if (adap->status & IC_M_STATUS_WAIT4_COMPLETION) {
            adap->cmd_error |= I2C_CMD_ERR_TIMEOUT;

            if (i2c_readl(adap, ICR_INTR_MASK) & IC_INTR_TX_ABRT) {
                i2c_dw_common_abort(adap);
                i2c_udelay(1000);
            }
        }
    }

    adap->irq_status = i2c_readl(adap, ICR_INTR_STAT);
    adap->irq_raw_status = i2c_readl(adap, ICR_RAW_INTR_STAT);
    adap->irq_mask = i2c_readl(adap, ICR_INTR_MASK);

    i2c_dw_common_disable_int(adap);
    i2c_dw_common_disable_no_wait(adap);

    return i2c_dw_master_xfer_result(adap, wait_ret);
}

static int i2c_dw_master_xfer(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    i2c_dw_master_reinit(adap, msg);

    if (i2c_dw_common_disable(adap)) {
        return -EBUSY;
    }

    if (msg->flags & I2C_KMSG_SMBUS) {
        i2c_dw_master_smbus_xfer_init(adap, msg);
    } else {
        i2c_dw_master_xfer_init(adap, msg);
    }

    return i2c_dw_master_xfer_handle(adap, msg);
}

static int i2c_dw_master_init_speed(struct i2c_adapter *adap, struct i2c_kdepl *deploy)
{
    unsigned int param1 = i2c_readl(adap, ICR_COMP_PARAM_1);

    switch (deploy->flags & I2C_KDEPLOY_SPEED_MODE_MASK) {
        case I2C_KDEPLOY_SPEED_MODE_HIGH:
            if ((param1 & IC_COMP_PARAM_1_SPEED_MODE_MASK) != IC_COMP_PARAM_1_SPEED_MODE_HIGH) {
                i2c_adapter_platform_destroy(adap);
                return -EOPNOTSUPP;
            }

            adap->control |= IC_CON_SPEED_HIGH;
            adap->hcnt = deploy->scl_high ? : 0x6; /* The default register value 0x6 is used if no input. */
            adap->lcnt = deploy->scl_low ? : 0x10; /* The default register value 0x10 is used if no input. */
            i2c_writel(adap, adap->hcnt, ICR_HS_SCL_HCNT);
            i2c_writel(adap, adap->lcnt, ICR_HS_SCL_LCNT);
            break;
        case I2C_KDEPLOY_SPEED_MODE_FAST:
            adap->control |= IC_CON_SPEED_FAST;

            /* 600:hcnt minimum width, 300:the diff between the minimum width of hcnt and the recommended width */
            adap->hcnt = deploy->scl_high ? : i2c_dw_common_scl_hcnt(i2c_clk_hz() / I2C_HZ_TO_KHZ, 600, 300, 0, 0x0);
            /* 1300:lcnt minimum width, 300:the diff between the minimum width of lcnt and the recommended width */
            adap->lcnt = deploy->scl_low ? : i2c_dw_common_scl_lcnt(i2c_clk_hz() / I2C_HZ_TO_KHZ, 1300, 300, 0x0);
            i2c_writel(adap, adap->hcnt, ICR_FS_SCL_HCNT);
            i2c_writel(adap, adap->lcnt, ICR_FS_SCL_LCNT);
            break;
        case I2C_KDEPLOY_SPEED_MODE_STD:
            adap->control |= IC_CON_SPEED_STD;
            /* 4000:hcnt minimum width, 300:the diff between the minimum width of hcnt and the recommended width */
            adap->hcnt = deploy->scl_high ? : i2c_dw_common_scl_hcnt(i2c_clk_hz() / I2C_HZ_TO_KHZ, 4000, 300, 0, 0x24);
            /* 4700:lcnt minimum width, 300:the diff between the minimum width of lcnt and the recommended width */
            adap->lcnt = deploy->scl_low ? : i2c_dw_common_scl_lcnt(i2c_clk_hz() / I2C_HZ_TO_KHZ, 4700, 300, 0x28);
            i2c_writel(adap, adap->hcnt, ICR_SS_SCL_HCNT);
            i2c_writel(adap, adap->lcnt, ICR_SS_SCL_LCNT);
            break;
        default:
            return -I2C_ENOTSUPSPEED;
    }

    return 0;
}

static int i2c_dw_master_initialize(struct i2c_adapter *adap, struct i2c_kdepl *deploy)
{
    int ret;

    if ((deploy->flags & I2C_KDEPLOY_SM_MASK) != I2C_KDEPLOY_MASTER) {
        return -EINVAL;
    }

    if (i2c_dw_common_disable(adap)) {
        i2c_log("Controller busy.");
        return -EBUSY;
    }

    ret = i2c_adapter_platform_init(adap);
    if (ret) {
        return ret;
    }

    adap->tx_fifo_depth = 0;
    adap->rx_fifo_depth = 0;
    adap->control = IC_CON_MASTER | IC_CON_SLAVE_DISABLE | IC_CON_RESTART_EN;

    ret = i2c_dw_master_init_speed(adap, deploy);
    if (ret) {
        i2c_adapter_platform_destroy(adap);
        return ret;
    }

    i2c_dw_master_set_fifo_depth(adap);
    i2c_dw_common_configure_sda_hold(adap, deploy->sda_hold, deploy->sda_hold_ratio, 1);
    i2c_dw_master_configure(adap, deploy);

    ret = i2c_adapter_isr_register(adap, i2c_dw_master_isr);
    if (ret) {
        i2c_adapter_platform_destroy(adap);
    }

    return ret;
}

static int i2c_dw_master_destroy(struct i2c_adapter *adap)
{
    i2c_dw_common_disable_int(adap);
    i2c_dw_common_disable_no_wait(adap);
    i2c_adapter_isr_unregister(adap);
    i2c_adapter_platform_destroy(adap);

    return 0;
}

struct i2c_algorithm g_i2c_dw_master_algo = {
    .algo_name = "dw-master",
    .algo_type = I2C_ALGO_TYPE_MASTER,
    .i2c_master_xfer = i2c_dw_master_xfer,
    .initialize = i2c_dw_master_initialize,
    .destroy = i2c_dw_master_destroy,
    .idle = i2c_dw_common_idle,
};

