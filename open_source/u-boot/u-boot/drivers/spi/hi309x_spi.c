/*
 * SPDX-License-Identifier: GPL-2.0
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2021-2031. All rights reserved.
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <spi.h>
#include <log.h>
#include <asm/io.h>
#include <clk.h>
#include <reset.h>
#include <asm/arch-hi309x/spi.h>
#include <linux/delay.h>
#include <linux/err.h>

static int spi_wait_idle(ulong regbase)
{
    unsigned int u_timeout = SPI_TRANSFER_TIMEOUT * 1000; // 1000 us

    /* Check whether the SPI is in the idle state. */
    while ((u_timeout != 0) && (spi_readl(regbase, SPI_SR_OFFSET_ADDR) & BIT(0))) {
        u_timeout--;
        udelay(1); /* usleep 1~2 us */
    }

    if (u_timeout == 0) {
        printf("wait spi idle timeout\n");
        return -ETIMEDOUT;
    }

    return 0;
}

static int exception_tackle(uint *timeout)
{
    *timeout = *timeout - 1;
    udelay(1); /* usleep 1~2 us */

    if (*timeout == 0) {
        printf("spi exception\n");
        return -ETIMEDOUT;
    }

    return 0;
}

static int spi_read(struct hi309x_spi_adapter *priv, void *data_addr, uint len, uint cs)
{
    uint word_num = len;
    uint u_timeout = SPI_TRANSFER_TIMEOUT * 1000; // 1000 us
    uint rx_temp;
    int ret;
    (void)cs;

    while (word_num) {
        if (exception_tackle(&u_timeout)) {
            return -ETIMEDOUT;
        }

        rx_temp = spi_readl(priv->regbase, SPI_RXFLR_OFFSET_ADDR);
        if (rx_temp == 0) {
            continue;
        }

        if (priv->wordlen > 8) {
           ((ushort *)data_addr)[len - word_num] = spi_readl(priv->regbase, SPI_DR0_OFFSET_ADDR) & 0xffff;
        } else {
            ((uchar *)data_addr)[len - word_num] = spi_readl(priv->regbase, SPI_DR0_OFFSET_ADDR) & 0xff;
        }

        word_num--;
    }

    ret = spi_wait_idle(priv->regbase);

    spi_disable(priv->regbase);    /* clear tx and rx fifo */
    spi_enable(priv->regbase);

    return ret;
}

static int spi_write(struct hi309x_spi_adapter *priv, const void *data_addr, uint len, uint cs)
{
    uint word_num = len;
    uint tx_temp_val;
    int ret = 0;

    spi_writel(priv->regbase, (1 << cs), SPI_SER_OFFSET_ADDR);
    while (word_num) {
        if (priv->wordlen > 8) {
            tx_temp_val = 0xffff & ((ushort *)data_addr)[len - word_num];
            spi_writel(priv->regbase, tx_temp_val, SPI_DR0_OFFSET_ADDR);
            udelay(1000);
        } else {
            tx_temp_val = 0xffff & ((uchar *)data_addr)[len - word_num];
            spi_writel(priv->regbase, tx_temp_val, SPI_DR0_OFFSET_ADDR);
            udelay(1000);
        }

        word_num--;
    }
    spi_writel(priv->regbase, SER_REG_VALUE_NON_SLAVE, SPI_SER_OFFSET_ADDR);

    ret = spi_wait_idle(priv->regbase);
    return ret;
}

static int hi309x_spi_xfer(struct udevice *dev, uint bitlen, const void *dout, void *din, ulong flags)
{
    struct dm_spi_slave_plat *plat = dev_get_parent_plat(dev);
    struct udevice *bus = dev->parent;
    struct hi309x_spi_adapter *priv = dev_get_priv(bus);
    uint len;
    int ret;

    if (bitlen % (priv->wordlen  + 1)) {
        printf("bitlen must be a multiple of the data frame length.\n");
        return -EINVAL;
    }
    len = bitlen / (priv->wordlen  + 1);

    if (dout != NULL && din != NULL) {
        ret = spi_write(priv, dout, len, plat->cs);
        if (ret) {
            return ret;
        }
        ret = spi_read(priv, din, len, plat->cs);

    } else if (dout != NULL) {
        ret = spi_write(priv, dout, len, plat->cs);

    } else if (din != NULL) {
        ret = spi_read(priv, din, len, plat->cs);

    } else {
        printf("dout and bin are both null pointers.");
        return -EINVAL;
    }

    return ret;
}

static void spi_config_cs_iomux(spi_ids spi_id, uint cs)
{
    if (spi_id == SPI_ID_0) {
        io_config_r_read_write(g_spi_cs_iomux_tbl[spi_id * SPI_CS_PIN_NUM + cs].offset,
        g_spi_cs_iomux_tbl[spi_id * SPI_CS_PIN_NUM + cs].mask, g_spi_cs_iomux_tbl[spi_id * SPI_CS_PIN_NUM + cs].val);
    } else {
#ifdef USE_SPI1_SFC_PIN
        io_config_t_read_write(g_spi_cs_iomux_tbl[spi_id * SPI_CS_PIN_NUM + cs].offset,
        g_spi_cs_iomux_tbl[spi_id * SPI_CS_PIN_NUM + cs].mask, g_spi_cs_iomux_tbl[spi_id * SPI_CS_PIN_NUM + cs].val);
#else
        io_config_r_read_write(g_spi_cs_iomux_tbl[spi_id * SPI_CS_PIN_NUM + cs].offset,
        g_spi_cs_iomux_tbl[spi_id * SPI_CS_PIN_NUM + cs].mask, g_spi_cs_iomux_tbl[spi_id * SPI_CS_PIN_NUM + cs].val);
#endif
    }
}

static int hi309x_spi_claim_bus(struct udevice *dev)
{
    return 0;
}

static int hi309x_spi_release_bus(struct udevice *dev)
{
    return 0;
}

static int hi309x_spi_cs_info(struct udevice *bus, uint cs, struct spi_cs_info *info)
{
    struct hi309x_spi_adapter *priv = dev_get_priv(bus);

    if (cs >= SPI_MAX_CTL) {
        printf(" Can only connect up to two slave devices at most.\n");
        return -EINVAL;
    }

    spi_config_cs_iomux(priv->spi_id, cs);

    return 0;
}

static void spi_ctrl_reg_config(ulong regbase, spi_config_items item, uint val)
{
    unsigned int tmp_value;

    /* SPI CTRLR0 control register */
    tmp_value = spi_readl(regbase, SPI_CTRLR0_OFFSET_ADDR);
    /* Returning the forbidden energy */
    tmp_value = drv_32bit_set_field(tmp_value, CTRLR0_SRL_FIELD_BIT, CTRLR0_SRL_FIELD_BIT, SPI_NOT_LOOP_BACK);

    if (item == FRM_FORMAT) {
        tmp_value =
        drv_32bit_set_field(tmp_value, CTRLR0_FRF_FIELD_START_BIT, CTRLR0_FRF_FIELD_END_BIT, val);
    } else if (item == FRM_LEN) {
        tmp_value = drv_32bit_set_field(tmp_value, CTRLR0_DFS_FIELD_START_BIT, CTRLR0_DFS_FIELD_END_BIT, val);
    } else if (item == EDGE) {
        tmp_value = drv_32bit_set_field(tmp_value, CTRLR0_SCPH_FIELD_BIT, CTRLR0_SCPH_FIELD_BIT, val);
    } else if (item == POL) {
        tmp_value = drv_32bit_set_field(tmp_value, CTRLR0_SCPOL_FIELD_BIT, CTRLR0_SCPOL_FIELD_BIT, val);
    } 

    spi_writel(regbase, tmp_value, SPI_CTRLR0_OFFSET_ADDR);
}

static int hi309x_spi_set_wordlen(struct udevice *dev, uint wordlen)
{
    struct udevice *bus = dev_get_parent(dev);
    struct hi309x_spi_adapter *priv = dev_get_priv(bus);

    if (wordlen != HISPI_WORDLEN_8_BIT && wordlen != HISPI_WORDLEN_16_BIT) {
        printf("The driver only supports 8-bit and 16-bit word lengths.\n");
        return -EINVAL;
    }

    spi_disable(priv->regbase);
    spi_ctrl_reg_config(priv->regbase, FRM_LEN, wordlen);
    priv->wordlen = wordlen;
    spi_enable(priv->regbase);

    return 0;
}

static int hi309x_spi_set_speed(struct udevice *bus, uint hz)
{
    struct hi309x_spi_adapter *priv = dev_get_priv(bus);
    uint freq_div;

    if (!hz) {
        printf("baud_rate(%u) must not be zero\n", hz);
        return -EINVAL;
    }

    if (hz > SPI_MAX_SPEED) {
        printf("the max baud_rate(%u) is 20 MHz\n", hz);
        return -EINVAL;
    }

    /* Sets the SPI bus clock frequency. */
    freq_div = SYS_AHB_CLOCK_FREQ / hz;
    if ((freq_div < SPI_MIN_DIV) || (freq_div > SPI_MAX_DIV)) {
        printf("freq_div out of range.\n");
        return -EINVAL;
    }

    if ((freq_div & 0x1) != 0) {    /* freq_div must be an even number */
        freq_div++;
    }
    spi_disable(priv->regbase);
    spi_writel(priv->regbase, freq_div, SPI_BAUDR_OFFSET_ADDR);
    spi_enable(priv->regbase);

    return 0;
}

static int hi309x_spi_set_mode(struct udevice *bus, uint mode)
{
    struct hi309x_spi_adapter *priv = dev_get_priv(bus);
    uint clk_edge;
    uint clk_pol;

    clk_edge = (mode & BIT(1)) >> 1;
    clk_pol = mode & BIT(0);

    spi_disable(priv->regbase);
    spi_ctrl_reg_config(priv->regbase, EDGE, clk_edge);
    spi_ctrl_reg_config(priv->regbase, POL, clk_pol);
    spi_enable(priv->regbase);

    return 0;
}

static void spi_ctrl_config(struct hi309x_spi_adapter *priv)
{
    spi_disable(priv->regbase);

    spi_ctrl_reg_config(priv->regbase, FRM_FORMAT, SPI_DEFAULT_FRAME_FORMAT);
    spi_ctrl_reg_config(priv->regbase, FRM_LEN, priv->wordlen);

    spi_writel(priv->regbase, 0, SPI_CTRLR1_OFFSET_ADDR);
    /* Modifying the RX Interrupt Waterline Depth */
    spi_writel(priv->regbase, 0, SPI_RXFTLR_OFFSET_ADDR);
    spi_writel(priv->regbase, 0, SPI_TXFTLR_OFFSET_ADDR);
    /* Do not select any slave server. */
    spi_writel(priv->regbase, SER_REG_VALUE_NON_SLAVE, SPI_SER_OFFSET_ADDR);

    /* mask all interrupts */
    spi_mask_irq_signal(priv->regbase);

    spi_enable(priv->regbase);
}

static void spi_config_ctl_iomux(spi_ids spi_id)
{
    for (int i = 0; i < SPI_CTL_PIN_NUM; i++) {
        if (spi_id == SPI_ID_0) {
            io_config_r_read_write(g_spi_iomux_tbl[spi_id * SPI_CTL_PIN_NUM + i].offset,
            g_spi_iomux_tbl[spi_id * SPI_CTL_PIN_NUM + i].mask, g_spi_iomux_tbl[spi_id * SPI_CTL_PIN_NUM + i].val);
        } else {
#ifdef USE_SPI1_SFC_PIN
            io_config_t_read_write(g_spi_iomux_tbl[spi_id * SPI_CTL_PIN_NUM + i].offset,
            g_spi_iomux_tbl[spi_id * SPI_CTL_PIN_NUM + i].mask, g_spi_iomux_tbl[spi_id * SPI_CTL_PIN_NUM + i].val);
#else
            io_config_r_read_write(g_spi_iomux_tbl[spi_id * SPI_CTL_PIN_NUM + i].offset,
            g_spi_iomux_tbl[spi_id * SPI_CTL_PIN_NUM + i].mask, g_spi_iomux_tbl[spi_id * SPI_CTL_PIN_NUM + i].val);
#endif
        }
    }
}

static void spi_iomux_lp_config(spi_ids spi_id)
{
    spi_config_ctl_iomux(spi_id);

    /* set lp_subsys */
    uint lp_spi_val;
    lp_subsys_spi_reg spi_reg;
    spi_reg.val = 0;

    if (spi_id == SPI_ID_0) {
        spi_reg.bits.spi0_in_n = 1;
        spi_reg.bits.spi0_rx_only = 1;
        lp_spi_val = BIT(0);
    } else {
        spi_reg.bits.spi1_in_n = 1;
        spi_reg.bits.spi1_rx_only = 1;
        lp_spi_val = BIT(5); /* 5: bit */
    }

    lp_subsys_reg_read_write(LP_SUBSYS_CSR_LPSC_SPI_CTRL_OFFSET_ADDR, spi_reg.val, lp_spi_val);
#ifdef USE_SPI1_SFC_PIN
    lp_subsys_reg_set_bits(LP_SUBSYS_CSR_LPSC_IOMUX_CTRL_OFFSET_ADDR, 1, 1);
#else
    lp_subsys_reg_set_bits(LP_SUBSYS_CSR_LPSC_IOMUX_CTRL_OFFSET_ADDR, 1, 0);
#endif
}

static void hi309x_spi_init(struct hi309x_spi_adapter *priv)
{
    spi_iomux_lp_config(priv->spi_id);
    spi_ctrl_config(priv);
}

static int hi309x_spi_probe(struct udevice *bus)
{
    struct hi309x_spi_plat *plat = dev_get_plat(bus);
    struct hi309x_spi_adapter *priv = dev_get_priv(bus);

    priv->spi_id = plat->spi_id;
    priv->regbase = plat->regbase;
    priv->wordlen = plat->wordlen;

    hi309x_spi_init(priv);

    return 0;
}

static const struct dm_spi_ops hi309x_spi_ops = {
	.claim_bus	= hi309x_spi_claim_bus,
	.release_bus	= hi309x_spi_release_bus,
    .cs_info = hi309x_spi_cs_info,
    .set_wordlen = hi309x_spi_set_wordlen,
	.xfer		= hi309x_spi_xfer,
	.set_speed	= hi309x_spi_set_speed,
	.set_mode	= hi309x_spi_set_mode,
};

U_BOOT_DRIVER(spi_hi309x) = {
    .name = "spi_hi309x",
    .id = UCLASS_SPI,
    .probe = hi309x_spi_probe,
    .priv_auto = sizeof(struct hi309x_spi_adapter),
    .ops = &hi309x_spi_ops,
};

static const struct hi309x_spi_plat hi309x_spi[] = {
    { 0, 0x8713000, HISPI_WORDLEN_8_BIT },
    { 1, 0x8715000, HISPI_WORDLEN_8_BIT },
};

U_BOOT_DRVINFOS(hi309x_spis) = {
    { "spi_hi309x", &hi309x_spi[0] },
    { "spi_hi309x", &hi309x_spi[1] },
};
