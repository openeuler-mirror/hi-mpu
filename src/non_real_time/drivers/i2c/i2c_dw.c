// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * DesignWare i2c driver (common)
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "i2c.h"
#include "i2c_dw.h"

void i2c_dw_common_disable_int(struct i2c_adapter *adap)
{
    i2c_writel(adap, 0x0, ICR_INTR_MASK);
}

int i2c_dw_common_idle(struct i2c_adapter *adap)
{
    unsigned int *status = i2c_register(adap, ICR_STATUS);
    unsigned long orig_jiffies = jiffies;
    int timeout = 0;

    i2c_dw_common_disable_int(adap);
    adap->cmd_error = 0;
    i2c_readl(adap, ICR_CLR_INTR);
    i2c_writel(adap, IC_INTR_TX_ABRT | IC_INTR_SCL_STUCK_LOW, ICR_INTR_MASK);

    while ((*status & IC_STATUS_ACTIVITY) && !adap->cmd_error) {
        if (time_after(jiffies, orig_jiffies + HZ)) {
            timeout = 1;
            break;
        }
    }

    i2c_dw_common_disable_int(adap);

    return (adap->cmd_error || timeout) ? 0 : 1;
}

void i2c_dw_common_disable_no_wait(struct i2c_adapter *adap)
{
    i2c_writel(adap, 0x0, ICR_ENABLE);
}

int i2c_dw_common_disable(struct i2c_adapter *adap)
{
    if (!i2c_dw_common_idle(adap)) {
        adap->cmd_error |= I2C_CMD_ERR_CONTROLLER_BUSY;
        return -EBUSY;
    } else {
        i2c_dw_common_disable_no_wait(adap);
        return 0;
    }
}

void i2c_dw_common_enable(struct i2c_adapter *adap)
{
    i2c_writel(adap, IC_ENA_ENABLE, ICR_ENABLE);
}

void i2c_dw_common_abort(struct i2c_adapter *adap)
{
    unsigned int val;

    val = i2c_readl(adap, ICR_ENABLE);
    i2c_writel(adap, IC_ENA_ABORT | val, ICR_ENABLE);
}

void i2c_dw_common_configure_sda_hold(struct i2c_adapter *adap, unsigned int sda_hold,
                                      unsigned int sda_hold_ratio, unsigned int is_master)
{
    unsigned int ratio = sda_hold_ratio;
    unsigned int hold = 0;
    unsigned int val;

    val = i2c_readl(adap, ICR_COMP_VERSION);
    if (val < IC_SDA_HOLD_MIN_VERS) {
        return;
    }

    if (is_master) {
        ratio = (ratio > 50) ? 50 : ratio;               /* 50 sda hold ratio */
        hold = sda_hold ? : ((adap->lcnt * ratio) / 100); /* Divided by 100 indicates the number of SDA hold periods. */
    }

    hold = hold ? : IC_SDA_HOLD_DEFAULT;

    if (!(hold & IC_SDA_HOLD_TX_MASK)) {
        hold |= (unsigned int)(0x1U << IC_SDA_HOLD_TX_SHIFT);
    }

    if (!(hold & IC_SDA_HOLD_RX_MASK)) {
        hold |= (unsigned int)(0x1U << IC_SDA_HOLD_RX_SHIFT);
    }

    i2c_writel(adap, hold, ICR_SDA_HOLD);
}

unsigned int i2c_dw_common_rc_intrbits(struct i2c_adapter *adap)
{
    unsigned int irq_status = i2c_readl(adap, ICR_INTR_STAT);
    unsigned int i;
    static const struct {
        unsigned int msk;
        unsigned int clr;
    } rcinfo[] = {
        { .msk = IC_INTR_RX_UNDER, .clr = ICR_CLR_RX_UNDER },
        { .msk = IC_INTR_RX_OVER, .clr = ICR_CLR_RX_OVER },
        { .msk = IC_INTR_TX_OVER, .clr = ICR_CLR_TX_OVER },
        { .msk = IC_INTR_RX_DONE, .clr = ICR_CLR_RX_DONE },
        { .msk = IC_INTR_ACTIVITY, .clr = ICR_CLR_ACTIVITY },
        { .msk = IC_INTR_STOP_DET, .clr = ICR_CLR_STOP_DET },
        { .msk = IC_INTR_START_DET, .clr = ICR_CLR_START_DET },
        { .msk = IC_INTR_GEN_CALL, .clr = ICR_CLR_GEN_CALL },
        { .msk = IC_INTR_SCL_STUCK_LOW, .clr = ICR_CLR_SCL_STUCK_DET },
    };

    for (i = 0; i < ARRAY_SIZE(rcinfo); i++) {
        if (irq_status & rcinfo[i].msk) {
            i2c_readl(adap, rcinfo[i].clr);
        }
    }

    if (irq_status & IC_INTR_RD_REQ) {
        if (adap->algo) {
            i2c_readl(adap, ICR_CLR_RD_REQ);
        }
    }

    if (irq_status & IC_INTR_TX_ABRT) {
        adap->abort_source = i2c_readl(adap, ICR_TX_ABRT_SOURCE);
        i2c_readl(adap, ICR_CLR_TX_ABRT);
    }

    return irq_status;
}

unsigned int i2c_dw_common_scl_hcnt(unsigned long long clk_khz, unsigned long long t_high,
                                    unsigned long long tf, int cond, long long offset)
{
    if (cond) {
        /* 500000/1000000/8 use to calculate hcnt, the calculation formula refers to the kernel */
        return (unsigned int)((((clk_khz * t_high + 500000) / 1000000) - 8) + offset);
    } else {
        /* 500000/1000000/3 use to calculate hcnt, the calculation formula refers to the kernel */
        return (unsigned int)((((clk_khz * (t_high + tf) + 500000) / 1000000) - 3) + offset);
    }
}

unsigned int i2c_dw_common_scl_lcnt(unsigned long long clk_khz, unsigned long long t_low,
                                    unsigned long long tf, long long offset)
{
    /* 500000/1000000 use to calculate lcnt, the calculation formula refers to the kernel */
    return (unsigned int)((((clk_khz * (t_low + tf) + 500000) / 1000000) - 1) + offset);
}