// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * DesignWare i2c driver (slave only)
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "i2c.h"
#include "i2c_api.h"
#include "i2c_dw.h"

static void i2c_dw_slave_configure(struct i2c_adapter *adap, struct i2c_kdepl *deploy)
{
    unsigned int val;

    i2c_writel(adap, adap->control, ICR_CONTROL);
    i2c_writel(adap, adap->slave_address, ICR_SAR);
    i2c_writel(adap, IC_INTR_SLAVE_MASK, ICR_INTR_MASK);

    val = deploy->scl_low_timeout ? : IC_SCL_LOW_TIMEOUT_DEFAULT;
    i2c_writel(adap, val, ICR_SCL_LOW_TIMEOUT);
    val = deploy->sda_low_timeout ? : IC_SDA_LOW_TIMEOUT_DEFAULT;
    i2c_writel(adap, val, ICR_SDA_LOW_TIMEOUT);
}

static void i2c_dw_slave_irq_handler(struct i2c_adapter *adap, unsigned int irq_status)
{
    struct i2c_smbus_control *smbus = NULL;
    enum i2c_slave_event event;
    unsigned int requested;
    unsigned int status;

    if (irq_status & (IC_INTR_TX_ABRT | IC_INTR_RX_UNDER | IC_INTR_RX_OVER |
        IC_INTR_TX_OVER | IC_INTR_SCL_STUCK_LOW)) {
        status = adap->slave_status;
        i2c_adapter_slave_event(adap, I2C_SLAVE_ABORT);
        i2c_log("exception detected. slave status 0x%x, irq status 0x%x, abort source 0x%x",
                status, irq_status, adap->abort_source);
        return;
    }

    /* Prevents the packet interval from being too short. */
    if (irq_status & (IC_INTR_RX_DONE | IC_INTR_STOP_DET)) {
        if ((irq_status & IC_INTR_STOP_DET) && (adap->deploy_flags & I2C_KDEPLOY_SMBUS)) {
            smbus = adap->slave_priv;
            del_timer(&smbus->stop_detect_timer);
        }

        i2c_adapter_slave_event(adap, I2C_SLAVE_STOP);
        adap->slave_status = IC_S_STATUS_IDLE;
    }

    if (irq_status & IC_INTR_START_DET) {
        if (adap->deploy_flags & I2C_KDEPLOY_SMBUS) {
            smbus = adap->slave_priv;
            mod_timer(&smbus->stop_detect_timer, jiffies +
                      msecs_to_jiffies(smbus->clk_strech_timeout + I2C_SLAVE_WAIT_STOP_TIME_MAX));
        }
    }

    if (irq_status & IC_INTR_RX_FULL) {
        requested = adap->slave_status & IC_S_STATUS_WRITE_REQUESTED;
        if (!requested) {
            adap->slave_status |= IC_S_STATUS_WRITE_REQUESTED;
            i2c_adapter_slave_event(adap, I2C_SLAVE_WRITE_REQUESTED);
        }

        i2c_adapter_slave_event(adap, I2C_SLAVE_WRITE_RECEIVED);
    }

    if (irq_status & IC_INTR_RD_REQ) {
        requested = adap->slave_status & IC_S_STATUS_READ_REQUESTED;
        if (!requested) {
            event = I2C_SLAVE_READ_REQUESTED;
            adap->slave_status |= IC_S_STATUS_READ_REQUESTED;
        } else {
            event = I2C_SLAVE_READ_PROCESSED;
        }

        i2c_adapter_slave_event(adap, event);
    }
}

static irqreturn_t i2c_dw_slave_isr(int irq, void *arg)
{
    struct i2c_adapter *adap = arg;
    unsigned int irq_status;

    irq_status = i2c_dw_common_rc_intrbits(adap);

    if (!adap->slave) {
        return IRQ_HANDLED;
    }

    if (!(irq_status & ~IC_INTR_ACTIVITY)) {
        return IRQ_HANDLED;
    }

    if (!(i2c_readl(adap, ICR_ENABLE) & IC_ENA_ENABLE)) {
        return IRQ_HANDLED;
    }

    i2c_dw_slave_irq_handler(adap, irq_status);
    return IRQ_HANDLED;
}

static int i2c_dw_slave_check_for_deploy(struct i2c_kdepl *deploy)
{
    /* Length (in bytes) of the memory used by the controller when the bus is initialized as the slave.
     * The value must be a multiple of 8. */
    if (!deploy->slave_dlen || deploy->slave_dlen % 8) {
        return -I2C_ESLAVESCOPE;
    }

    if ((deploy->flags & I2C_KDEPLOY_SM_MASK) != I2C_KDEPLOY_SLAVE) {
        return -EINVAL;
    }

    if (!(deploy->flags & I2C_KDEPLOY_SLV_TYPE_MASK)) {
        return -I2C_ENOSLAVETYPE;
    } else {
        return 0;
    }
}

static int i2c_dw_slave_init_controller(struct i2c_adapter *adap, struct i2c_kdepl *deploy)
{
    adap->control = IC_CON_RX_FIFO_FULL_HLD_CTRL |
                    IC_CON_RESTART_EN |
                    IC_CON_STOP_DET_IFADDRESSED;

    if (deploy->flags & I2C_KDEPLOY_SLV_ADDR_MODE_10BIT) {
        adap->control |= IC_CON_10BITADDR_SLAVE;
        adap->slave_address = deploy->slave_address & I2C_10BIT_ADDR_MASK;
    } else {
        adap->slave_address = deploy->slave_address & I2C_7BIT_ADDR_MASK;
    }

    switch (deploy->flags & I2C_KDEPLOY_SPEED_MODE_MASK) {
        case I2C_KDEPLOY_SPEED_MODE_HIGH:
            adap->control |= IC_CON_SPEED_HIGH;
            break;
        case I2C_KDEPLOY_SPEED_MODE_FAST:
            adap->control |= IC_CON_SPEED_FAST;
            break;
        case I2C_KDEPLOY_SPEED_MODE_STD:
            adap->control |= IC_CON_SPEED_STD;
            break;
        default:
            return -I2C_ENOTSUPSPEED;
    }

    return 0;
}

static struct i2c_slave *i2c_dw_slave_lookup(struct i2c_kdepl *deploy)
{
    char *slave_name = NULL;

    if (deploy->flags & I2C_KDEPLOY_SLV_TYPE_EEPROM) {
        slave_name = "eeprom";
    }

    if (deploy->flags & I2C_KDEPLOY_SLV_TYPE_SMBUS) {
        slave_name = "smbus";
    }

    return i2c_slave_lookup_byname(slave_name);
}

static void i2c_fill_adapt_with_slave(struct i2c_adapter *adap, struct i2c_kdepl *deploy,
                                      struct i2c_slave *slave, void *ptr)
{
    adap->slave = slave;
    adap->slave_priv = ptr;
    adap->slave_data = ptr + slave->control_memlen;
    adap->slave_dlen = deploy->slave_dlen;
    adap->clk_strech_timeout = deploy->clk_strech_timeout;
    adap->slave_status = IC_S_STATUS_IDLE;
}

static int i2c_dw_slave_initialize(struct i2c_adapter *adap, struct i2c_kdepl *deploy)
{
    struct i2c_slave *slave = NULL;
    unsigned int dlen_multi = 1;
    void *ptr = NULL;
    int ret;

    ret = i2c_dw_slave_check_for_deploy(deploy);
    if (ret) {
        return ret;
    }

    if (deploy->flags & I2C_KDEPLOY_SLV_TYPE_SMBUS) {
        dlen_multi = IC_S_DATA_LEN_MULTI;
    }

    slave = i2c_dw_slave_lookup(deploy);
    if (!slave) {
        return -I2C_ENOSLAVE;
    }

    if (i2c_dw_common_disable(adap)) {
        i2c_log("Controller busy.");
        return -EBUSY;
    }

    ret = i2c_adapter_platform_init(adap);
    if (ret) {
        return ret;
    }

    ptr = i2c_mcalloc(I2C_MTYPE_SLAVE_DATA,
                      deploy->slave_dlen * dlen_multi + slave->control_memlen);
    if (!ptr) {
        i2c_adapter_platform_destroy(adap);
        return -ENOMEM;
    }

    i2c_fill_adapt_with_slave(adap, deploy, slave, ptr);

    ret = i2c_dw_slave_init_controller(adap, deploy);
    if (ret) {
        i2c_mfree(I2C_MTYPE_SLAVE_DATA, ptr);
        i2c_adapter_platform_destroy(adap);
        return ret;
    }

    i2c_dw_common_configure_sda_hold(adap, deploy->sda_hold, 0, 0);
    i2c_dw_slave_configure(adap, deploy);

    if (slave->slave_init) {
        slave->slave_init(adap);
    }

    ret = i2c_adapter_isr_register(adap, i2c_dw_slave_isr);
    if (ret) {
        i2c_mfree(I2C_MTYPE_SLAVE_DATA, ptr);
        i2c_adapter_platform_destroy(adap);
        return ret;
    } else {
        i2c_dw_common_enable(adap);
        return 0;
    }
}

static int i2c_dw_slave_destroy(struct i2c_adapter *adap)
{
    struct i2c_smbus_control *smbus = NULL;

    i2c_dw_common_disable_int(adap);
    i2c_dw_common_disable_no_wait(adap);
    i2c_adapter_isr_unregister(adap);
    if (adap->deploy_flags & I2C_KDEPLOY_SMBUS) {
        smbus = adap->slave_priv;
        del_timer_sync(&smbus->stop_detect_timer);
        del_timer_sync(&smbus->clk_strech_timer);
    }
    i2c_mfree(I2C_MTYPE_SLAVE_DATA, adap->slave_priv);
    i2c_adapter_platform_destroy(adap);
    return 0;
}

struct i2c_algorithm g_i2c_dw_slave_algo = {
    .algo_name = "dw-slave",
    .algo_type = I2C_ALGO_TYPE_SLAVE,
    .i2c_master_xfer = NULL,
    .initialize = i2c_dw_slave_initialize,
    .destroy = i2c_dw_slave_destroy,
    .idle = i2c_dw_common_idle,
};

