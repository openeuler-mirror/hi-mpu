/*
 * SPDX-License-Identifier: GPL-2.0
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2021-2031. All rights reserved.
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <i2c.h>
#include <log.h>
#include <asm/io.h>
#include <clk.h>
#include <reset.h>
#include <asm/arch-hi309x/i2c.h>
#include <linux/delay.h>
#include <linux/err.h>

/*              i2c xfer                    */
static int i2c_wait_idle(ulong regbase)
{
    uint udelay_count = I2C_INIT_TIMEOUT;

    while ((i2c_readl(regbase, ICR_STATUS) & IC_STATUS_ACTIVITY) && (udelay_count > 0)) {
        udelay(1);
        udelay_count--;
        dsb();
    }

    if (!udelay_count) {
        return -ETIMEDOUT;
    }

    i2c_common_disable(regbase);
    return 0;
}

static int i2c_check_xfer_finish(struct hi309x_i2c_adapter *priv, struct i2c_kmsg *msg, int flag)
{
    uint i2c_status;
    uint time_out = msg->timeout;

    while (!(i2c_readl(priv->regbase, ICR_STATUS) & flag) && time_out > 0) {
        i2c_status =i2c_readl(priv->regbase, ICR_STATUS);

        if (flag == IC_STATUS_TFE && ((i2c_status >> 2) & 0x1)) { /* Shift right by 2 bits */
            break;
        } else if (flag == IC_STATUS_RFNE && !((i2c_status >> 3) & 0x1)) { /* Shift right by 3 bits */
            break;
        }

        time_out--;
        udelay(1);
    }

    if (!time_out) {
        printf("i2c xfer timeout\n");
        return -ETIMEDOUT;
    }

    return 0;
}

static int i2c_dw_master_write(struct hi309x_i2c_adapter *priv, struct i2c_kmsg *msg)
{
    int ret;
    uchar val;
    int i2c_txflr;
    int i2c_status;

    while (msg->wcnt < msg->wlen) {
        i2c_txflr = i2c_readl(priv->regbase, ICR_TXFLR);
        i2c_status = i2c_readl(priv->regbase, ICR_STATUS);

        if (i2c_txflr == 0 && ((i2c_status >> 2) & 0X1) == 1) {
            val = msg->wbuf[msg->wcnt++];
            i2c_writel(priv->regbase, val, ICR_DATA_CMD);
            dsb();
        }

        if (msg->timeout <= 0) {
            break;
        }

        msg->timeout--;
        udelay(1);
    }

    ret = i2c_check_xfer_finish(priv, msg, IC_STATUS_TFE);

    return ret;
}

static int i2c_dw_master_read(struct hi309x_i2c_adapter *priv, struct i2c_kmsg *msg)
{
    int ret;
    uchar val;
    uint cnt = 0;
    int rlimit = priv->rx_fifo_depth - i2c_readl(priv->regbase, ICR_RXFLR);

    while ((cnt < msg->rlen) && (rlimit > 0)) {
        i2c_writel(priv->regbase, IC_DATA_CMD_WRITE, ICR_DATA_CMD);
        cnt++;
        rlimit--;
    }

    while(msg->rcnt < msg->rlen) {
        if (i2c_readl(priv->regbase, ICR_RXFLR)) {
            val = (uchar)i2c_readl(priv->regbase, ICR_DATA_CMD);
            msg->rbuf[msg->rcnt++] = val;
            dsb();

            if (cnt < msg->rlen) {
                i2c_writel(priv->regbase, IC_DATA_CMD_WRITE, ICR_DATA_CMD);
                cnt++;
            }
        }

        if (msg->timeout <= 0) {
            break;
        }

        msg->timeout--;
        udelay(1);
    }

    ret = i2c_check_xfer_finish(priv, msg, IC_STATUS_RFNE);
    return ret;
}

static void i2c_master_xfer_init(struct hi309x_i2c_adapter *priv, struct i2c_kmsg *msg)
{
    msg->rcnt = 0;
    msg->wcnt = 0;
    priv->cmd_len = 0;

    i2c_readl(priv->regbase, ICR_CLR_INTR);
    i2c_writel(priv->regbase, priv->control, ICR_CONTROL);
    i2c_writel(priv->regbase, msg->addr, ICR_TAR);

    i2c_common_enable(priv->regbase);
}

static int i2c_send_package(struct hi309x_i2c_adapter *priv, struct i2c_kmsg *msg)
{
    priv->msg_flags = msg->flags;
    int ret;

    if (i2c_wait_idle(priv->regbase)) {
        printf("Controller busy.");
        return -ETIMEDOUT;
    }

    i2c_master_xfer_init(priv, msg);

    if (msg->flags & I2C_M_RD) {
        ret = i2c_dw_master_read(priv, msg);
    } else {
        ret = i2c_dw_master_write(priv, msg);
    }

    return ret;
}

static void set_send_package(struct i2c_kmsg *msg, uchar chip, uchar *buf, uint len)
{
    msg->addr = chip;

    if (msg->flags & I2C_M_RD) {
        msg->wbuf = NULL;
        msg->rbuf = buf;
        msg->rlen = len;
    } else {
        msg->wbuf = buf;
        msg->wlen = len;
        msg->rbuf = NULL;
    }

    msg->timeout = 3 * I2C_USEC_PER_SEC;    /* The default timeout period is 3 seconds */
}

static int i2c_read_data(struct hi309x_i2c_adapter *priv, uchar chip, uchar *buf, uint len)
{
    struct i2c_kmsg msg = {0, 0, 0, 0, 0, 0, NULL, 0, NULL};
    int ret;

    msg.flags = I2C_M_RD;
    debug("inside i2c_read_data():\n");
    set_send_package(&msg, chip, buf, len);
    ret = i2c_send_package(priv, &msg);

    if (!ret) {
        debug("i2c_read_data: ");
	    for (int rc = 0; rc < len; ++rc)
		    debug(" 0x%02x", buf[rc]);
	    debug("\n");
    }

    return ret;
}

static int i2c_write_data(struct hi309x_i2c_adapter *priv, uchar chip, uchar *buf, uint len)
{
    struct i2c_kmsg msg = {0, 0, 0, 0, 0, 0, NULL, 0, NULL};
    int ret;

    debug("i2c_write_data: chip=0x%x, len=0x%x\n", chip, len);
	debug("write_data: ");
	/* use rc for counter */
	for (int rc = 0; rc < len; ++rc)
		debug(" 0x%02x", buf[rc]);
	debug("\n");

    msg.flags = I2C_M_WE;
    set_send_package(&msg, chip, buf, len);
    ret = i2c_send_package(priv, &msg);

    return ret;
}

static int i2c_check_xfer_info(struct i2c_msg *msg)
{
    if (!msg) {
        return -EINVAL;
    }

    if (!msg->addr || !msg->buf || !msg->len || msg->addr > I2C_7BIT_MAX_ADDR) {
        printf("invaild msg\n");
        return -EINVAL;
    }

    return 0;
}

/* i2c init */
static void i2c_master_set_fifo_depth(struct hi309x_i2c_adapter *priv)
{
    uint val;
    uint tx_depth;
    uint rx_depth;

    val = i2c_readl(priv->regbase, ICR_COMP_PARAM_1);
    tx_depth = ((val >> 16) & 0xff) + 1; /* Bit16~bit23  tx_fifo_depth */
    rx_depth = ((val >> 8) & 0xff) + 1;  /* bit8~bit15   rx_fifo_depth */

    priv->tx_fifo_depth = priv->tx_fifo_depth ? : tx_depth;
    priv->rx_fifo_depth = priv->tx_fifo_depth ? : rx_depth;
    priv->tx_fifo_depth = min(priv->tx_fifo_depth, tx_depth);
    priv->rx_fifo_depth = min(priv->rx_fifo_depth, rx_depth);
}

static void i2c_master_configure(struct hi309x_i2c_adapter *priv, struct i2c_kdepl *deploy)
{
    uint val;

    i2c_writel(priv->regbase, priv->tx_fifo_depth / 2, ICR_TX_TL);
    i2c_writel(priv->regbase, priv->control, ICR_CONTROL);

    val = deploy->scl_low_timeout ? : IC_SCL_LOW_TIMEOUT_DEFAULT;
    i2c_writel(priv->regbase, val, ICR_SCL_LOW_TIMEOUT);
    val = deploy->sda_low_timeout ? : IC_SDA_LOW_TIMEOUT_DEFAULT;
    i2c_writel(priv->regbase, val, ICR_SDA_LOW_TIMEOUT);
}

static int i2c_master_init_speed(struct hi309x_i2c_adapter *priv, struct i2c_kdepl *deploy)
{
    switch (deploy->flags & I2C_KDEPLOY_SPEED_MODE_MASK) {
        case I2C_KDEPLOY_SPEED_MODE_FAST:
            priv->control |= IC_CON_SPEED_FAST;
            priv->hcnt = I2C_FS_SCL_HCNT_CYCLE;
            priv->lcnt = I2C_FS_SCL_LCNT_CYCLE;

            i2c_writel(priv->regbase, priv->hcnt, ICR_FS_SCL_HCNT);
            i2c_writel(priv->regbase, priv->lcnt, ICR_FS_SCL_LCNT);
            break;

        case I2C_KDEPLOY_SPEED_MODE_STD:
            priv->control |= IC_CON_SPEED_STD;
            priv->hcnt = I2C_SS_SCL_HCNT_CYCLE;
            priv->lcnt = I2C_SS_SCL_LCNT_CYCLE;

            i2c_writel(priv->regbase, priv->hcnt, ICR_SS_SCL_HCNT);
            i2c_writel(priv->regbase, priv->hcnt, ICR_SS_SCL_LCNT);
            break;

        default:
            return -EINVAL;
    }

    return 0;
}

static void i2c_set_lpsc_smbus_i2c_mux(i2c_ids i2c_id)
{
    uint reg_offset;
    uint val;
    uint reg_mask;

    reg_offset = LP_SUBSYS_CSR_LPSC_SMBUS_I2C_MUX_OFFSET_ADDR;
    val = 0x0U;
    reg_mask = (0x1U << i2c_id);

    lp_subsys_reg_read_write(reg_offset, reg_mask, val);
}

static int i2c_master_initialize(struct hi309x_i2c_adapter *priv, struct i2c_kdepl *deploy)
{
    int ret = 0;

    i2c_set_lpsc_smbus_i2c_mux(priv->i2c_id);

    if (i2c_wait_idle(priv->regbase)) {
        printf("Controller busy.");
        return -ETIMEDOUT;
    }

    priv->control = IC_CON_MASTER | IC_CON_SLAVE_DISABLE | IC_CON_RESTART_EN;
    ret = i2c_master_init_speed(priv, deploy);
    if (ret) {
        return ret;
    }

    i2c_master_set_fifo_depth(priv);
    i2c_master_configure(priv, deploy);

    i2c_mask_irq_signal(priv->regbase);

    return 0;
}

static void i2c_adapter_set_default(struct hi309x_i2c_adapter *priv)
{
    priv->control = 0;
    priv->tx_fifo_depth = 0;
    priv->rx_fifo_depth = 0;
}

static int i2c_master_init(struct hi309x_i2c_adapter *priv, struct i2c_kdepl *deploy)
{
    int ret = 0;

    i2c_adapter_set_default(priv);

    ret = i2c_master_initialize(priv, deploy);
    if (ret) {
        i2c_adapter_set_default(priv);
    }

    return ret;
}

static int i2c_config_io_reuse(i2c_ids i2c_id)
{
    struct i2c_iomux_info data_info;
    struct i2c_iomux_info clk_info;

    data_info = g_i2c_iomux_data_tbl[i2c_id];
    clk_info = g_i2c_iomux_clk_tbl[i2c_id];

    if (data_info.flag) {
        io_config_t_read_write(data_info.offset, data_info.mask, data_info.val);
    } else {
        io_config_r_read_write(data_info.offset, data_info.mask, data_info.val);
    }

    if (clk_info.flag) {
        io_config_t_read_write(clk_info.offset, clk_info.mask, clk_info.val);
    } else {
        io_config_r_read_write(clk_info.offset, clk_info.mask, clk_info.val);
    }

    return 0;
}

static int i2c_config_io_clock(i2c_ids i2c_id)
{
    uint clk_value;
    clk_value = BIT(i2c_id + I2C_LPSC_CLKEN_CTRL0_OFFSET);

    do {
        lp_subsys_reg_read_write(LP_SUBSYS_CSR_LPSC_CLKEN_CTRL0_OFFSET_ADDR, clk_value, clk_value);
    } while (!(i2c_readl(LP_SUBSYS_CSR_BASE_ADDR + LP_SUBSYS_CSR_LPSC_CLKSTA_CTRL0_OFFSET_ADDR, 0) & clk_value));

    return 0;
}

static void i2c_iomux_lp_config(i2c_ids i2c_id)
{
    /* Setting the IOMUX */
    i2c_config_io_reuse(i2c_id);

    /* Enable the clock gating of the I2C controller */
    i2c_config_io_clock(i2c_id);
}

static int hi309x_i2c_init(struct udevice *bus)
{
    struct hi309x_i2c_adapter *priv = dev_get_priv(bus);

    if (priv->init_flag) {
        return 0;
    }

    struct i2c_kdepl deploy = { 0 };
    deploy.flags |= (I2C_KDEPLOY_FORCE | I2C_KDEPLOY_MASTER);

    if (priv->speed == I2C_SPEED_100K) {
        deploy.flags |= I2C_KDEPLOY_SPEED_MODE_STD;
    } else if (priv->speed == I2C_SPEED_400K) {
        deploy.flags |= I2C_KDEPLOY_SPEED_MODE_FAST;
    } else {
        printf("i2c speed is inval\n");
        return -EINVAL;
    }

    i2c_iomux_lp_config(priv->i2c_id);
    if (i2c_master_init(priv, &deploy)) {
        return -EINVAL;
    }

    priv->init_flag = 1;
    return 0;
}

/* dm_i2c_ops */
static int hi309x_i2c_xfer(struct udevice *bus, struct i2c_msg *msg, int nmsgs)
{
    struct hi309x_i2c_adapter *priv = dev_get_priv(bus);
    int ret;

    debug("i2c_xfer: %d message\n", nmsgs);
    for (; nmsgs > 0; nmsgs--, msg++) {
        debug("i2c_xfer: chip=0x%x, len=0x%x\n", msg->addr, msg->len);
        if (i2c_check_xfer_info(msg)) {
            return -EINVAL;
        }

        debug("%s msg->flags %x\n", __func__, msg->flags);
        if (msg->flags & I2C_M_RD) {
            ret = i2c_read_data(priv, msg->addr, msg->buf, msg->len);
        } else {
            ret = i2c_write_data(priv, msg->addr, msg->buf, msg->len);
        }

        if (ret) {
            printf("i2c_write: error sending\n");
            return -EREMOTEIO;
        }
    }

    return 0;
}

static void set_i2c_adapter(struct hi309x_i2c_adapter *priv, struct hi309x_i2c_plat *plat)
{
    priv->i2c_id = plat->i2c_id;
    priv->regbase = plat->regbase;
    priv->irqn = plat->irqn;
    priv->speed = plat->speed;
    priv->init_flag = plat->init_flag;
}

static int hi309x_i2c_set_bus_speed(struct udevice *bus, uint speed)
{
    struct hi309x_i2c_adapter *priv = dev_get_priv(bus);
    int ret = 0;

    priv->speed = speed;
    priv->init_flag = 0;
    ret = hi309x_i2c_init(bus);

    return ret;
}

static int hi309x_i2c_probe(struct udevice *bus)
{
    struct hi309x_i2c_plat *plat = dev_get_plat(bus);
    struct hi309x_i2c_adapter *priv = dev_get_priv(bus);
    int ret = 0;

    set_i2c_adapter(priv, plat);
    ret = hi309x_i2c_init(bus);

    return ret;
}

static const struct dm_i2c_ops hi309x_i2c_ops = {
	.xfer		= hi309x_i2c_xfer,
	.set_bus_speed	= hi309x_i2c_set_bus_speed,
};

U_BOOT_DRIVER(i2c_hi309x) = {
	.name	= "i2c_hi309x",
	.id	= UCLASS_I2C,
	.probe	= hi309x_i2c_probe,
	.priv_auto	= sizeof(struct hi309x_i2c_adapter),
	.ops	= &hi309x_i2c_ops,
};

static const struct hi309x_i2c_plat hi309x_i2c[] = {
    {I2C_ID_0, 0x8707000, 104, I2C_SPEED_100K, 0},
    {I2C_ID_1, 0x8708000, 105, I2C_SPEED_100K, 0},
    {I2C_ID_2, 0x8709000, 106, I2C_SPEED_100K, 0},
    {I2C_ID_3, 0x870a000, 107, I2C_SPEED_100K, 0},
    {I2C_ID_4, 0x870b000, 108, I2C_SPEED_100K, 0},
    {I2C_ID_5, 0x870c000, 109, I2C_SPEED_100K, 0},
    {I2C_ID_6, 0x870d000, 110, I2C_SPEED_100K, 0},
    {I2C_ID_7, 0x870e000, 111, I2C_SPEED_100K, 0},
    {I2C_ID_8, 0x8750000, 112, I2C_SPEED_100K, 0},
    {I2C_ID_9, 0x8751000, 113, I2C_SPEED_100K, 0},
    {I2C_ID_10, 0x8752000, 114, I2C_SPEED_100K, 0},
    {I2C_ID_11, 0x8753000, 115, I2C_SPEED_100K, 0},
    {I2C_ID_12, 0x8754000, 116, I2C_SPEED_100K, 0},
    {I2C_ID_13, 0x8755000, 117, I2C_SPEED_100K, 0},
    {I2C_ID_14, 0x8756000, 118, I2C_SPEED_100K, 0},
    {I2C_ID_15, 0x8757000, 119, I2C_SPEED_100K, 0},
};

U_BOOT_DRVINFOS(hi309x_i2cs) = {
    { "i2c_hi309x", &hi309x_i2c[0] },
    { "i2c_hi309x", &hi309x_i2c[1] },
    { "i2c_hi309x", &hi309x_i2c[2] },
    { "i2c_hi309x", &hi309x_i2c[3] },
    { "i2c_hi309x", &hi309x_i2c[4] },
    { "i2c_hi309x", &hi309x_i2c[5] },
    { "i2c_hi309x", &hi309x_i2c[6] },
    { "i2c_hi309x", &hi309x_i2c[7] },
    { "i2c_hi309x", &hi309x_i2c[8] },
    { "i2c_hi309x", &hi309x_i2c[9] },
    { "i2c_hi309x", &hi309x_i2c[11] },
    { "i2c_hi309x", &hi309x_i2c[12] },
    { "i2c_hi309x", &hi309x_i2c[13] },
    { "i2c_hi309x", &hi309x_i2c[14] },
    { "i2c_hi309x", &hi309x_i2c[15] },
};