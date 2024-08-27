// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * spi module interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include "comm_knl_api.h"
#include "lp_subsys.h"
#include "lp_subsys_csr_addr_define.h"
#include "lp_subsys_c_union_define.h"
#include "hispi_usr_api.h"
#include "hispi.h"

#define SPI_BUFFER_MAX_SIZE 24
#define SPI_MAX_SPEED 20000000   // 20MHz
#define SPI_TRANSFER_WAIT_TIMEOUT (500 * HZ / 1000)
#define BYTES_PER_16BIT_FRAME 2

#define HIGH_BYTE_FROM_16BIT(value) ((value) >> 8)

enum spi_slave_sel {
    SPI_SLAVE_0 = 0,
    SPI_SLAVE_1 = 1
};

struct hi309x_spi_attributes {
    enum spi_slave_sel slave_select;
    u32 pro_type;
    u32 baud_rate;
    u32 frame_len;
    u32 clk_edge;
    u32 clk_pol;
};

struct hi309x_spi {
    struct spi_controller *ctrl;
    u32 id;
    u32 v_irq;
    u32 clock_sel;
    phys_addr_t phys_base;
    unsigned long phys_size;
    void __iomem *virt_base;
    s8 irq_name[SPI_IRQ_NAME_LEN];
    wait_queue_head_t w_wait_queue;
    wait_queue_head_t r_wait_queue;
    volatile u32 w_wait_flag;
    volatile u32 r_wait_flag;
    u8 *rx_buf;
    u32 rx_len;
    struct hi309x_spi_attributes spi_attributes;
};

static void hispi_disable(void __iomem *base_addr)
{
    writel(SPI_DISEN, base_addr + SPI_SSIENR_REG_OFFSET);
    return;
}

static void hispi_enable(void __iomem *base_addr)
{
    writel(SPI_EN, base_addr + SPI_SSIENR_REG_OFFSET);
    return;
}

static void hispi_enable_intr(void __iomem *base_addr, u32 mask)
{
    u32 temp;

    temp = readl(base_addr + SPI_IMR_REG_OFFSET);
    temp |= mask;
    writel(temp, (base_addr + SPI_IMR_REG_OFFSET));
    return;
}

static void hispi_disable_intr(void __iomem *base_addr, u32 mask)
{
    u32 temp;

    temp = readl(base_addr + SPI_IMR_REG_OFFSET);
    temp &= ~mask;
    writel(temp, (base_addr + SPI_IMR_REG_OFFSET));
    return;
}

static s32 hi309x_spi_parse_dts(struct platform_device *pdev, struct hi309x_spi *hispi)
{
    s32 ret, irq;
    struct device_node *np = pdev->dev.of_node;
    struct resource res;

    ret = of_property_read_u32(np, "ctlid", &hispi->id);
    if (ret) {
        LOG(LOG_ERROR, "read ctl_id err %d", ret);
        return ret;
    }

    ret = of_property_read_u32(np, "clock-sel", &hispi->clock_sel);
    if (ret) {
        LOG(LOG_ERROR, "read clock_sel err %d", ret);
        return ret;
    }

    ret = of_address_to_resource(np, 0, &res);
    if (ret) {
        LOG(LOG_ERROR, "read phys_base err %d", ret);
        return ret;
    }

    irq = platform_get_irq(pdev, 0);
    if (irq < 0) {
        return irq;
    }

    hispi->v_irq = irq;
    hispi->phys_base = res.start;
    hispi->phys_size = resource_size(&res);

    return 0;
}

static s32 hi309x_spi_ctrl_init(struct platform_device *pdev, struct hi309x_spi *hispi)
{
    s32 ret = 0;
    hispi->w_wait_flag = 0;
    hispi->r_wait_flag = 0;

    if (hispi->id >= HISI_SPI_MAX_CTL) {
        LOG(LOG_ERROR, "ctrl_id(%u), out of range(0~1)", hispi->id);
        return -EINVAL;
    }

    /* Mapping virtual address. */
    if (hispi->virt_base == NULL) {
        LOG(LOG_ERROR, "hispi_setup_device id 0x%x!", hispi->id);
        hispi->virt_base = ioremap(hispi->phys_base, hispi->phys_size);
        if (hispi->virt_base == NULL) {
            LOG(LOG_ERROR, "virt_base ioremap error!");
            ret = -EIO;
        }
    }

    return ret;
}

static irqreturn_t h309x_spi_irq(s32 irq, void *dev_id)
{
    struct hi309x_spi *hispi = dev_id;
    void __iomem *base_addr = hispi->virt_base;
    u32 irq_status = readl(base_addr + SPI_ISR_REG_OFFSET);
    u32 rx_tmp_value;

    if ((irq_status & ISR_TX_EMPTY) == ISR_TX_EMPTY) {
        hispi_disable_intr(base_addr, INT_MASK_TX_EMPTY);
        hispi->w_wait_flag = 0;
        wake_up_interruptible(&(hispi->w_wait_queue));

    } else if (((irq_status & ISR_RX_FULL) == ISR_RX_FULL) || ((irq_status & ISR_RX_OVERFLOW) == ISR_RX_OVERFLOW)) {
        hispi_disable_intr(base_addr, INT_MASK_RX_FULL | INT_MASK_RX_OVER);
        hispi->r_wait_flag = 0;
        wake_up_interruptible(&(hispi->w_wait_queue));

    } else {
        LOG(LOG_ERROR, "unknow state irq_status = 0x%x", irq_status);
        hispi_disable_intr(base_addr, irq_status);
        rx_tmp_value = readl(base_addr + SPI_CLEAN_TX_ISR_REG);    /* Clears the TX FIFO overflow interrupt. */
        (void)rx_tmp_value;
    }

    return IRQ_HANDLED;
}

static s32 hi309x_spi_irq_init(struct platform_device *pdev, struct hi309x_spi *hispi)
{
    s32 ret;
    struct device *dev = &pdev->dev;

    ret = devm_request_irq(dev, hispi->v_irq, h309x_spi_irq, 0, dev_name(dev), hispi);
    if (ret < 0) {
        dev_err(dev, "failed to get IRQ=%d, ret=%d\n", hispi->v_irq, ret);
        return ret;
    }

    init_waitqueue_head(&hispi->w_wait_queue);
    init_waitqueue_head(&hispi->r_wait_queue);

    return 0;
}

static s32 hi309x_baudrate_cfg(void __iomem *base_addr, u32 baud_rate)
{
    u32 freq_div;

    if (!baud_rate) {
        LOG(LOG_ERROR, "baud_rate(%u) must not be zero", baud_rate);
        return -EINVAL;
    }

    freq_div = SYS_AHB_CLOCK_FREQ / baud_rate;
    if ((freq_div & 0x1) != 0) {
        freq_div++;
    }
    /* Sets the SPI bus clock frequency. */
    if ((freq_div < SPI_MIN_DIV) || (freq_div > SPI_MAX_DIV)) {
        LOG(LOG_ERROR, "freq_div out of range");
        return -EINVAL;
    }
    freq_div &= 0xffff;
    writel(freq_div, base_addr + SPI_BAUDR_REG_OFFSET);

    return 0;
}

static s32 hi309x_spi_ctrl_config(struct hi309x_spi *hispi)
{
    s32 reg_value;
    u_lpsc_spi_ctrl lpsc_per10;
    u32 lp_spi_val;
    struct hi309x_spi_attributes *spi_attr = &hispi->spi_attributes;
    void __iomem *base_addr = hispi->virt_base;

    hispi_disable(base_addr);

    /* Initialize the SPI transfer config. */
    reg_value = readl(base_addr + SPI_CTRLR0_REG_OFFSET);
    reg_value = drv_32bit_set_field(reg_value, CTRLR0_SRL_FIELD_BIT, CTRLR0_SRL_FIELD_BIT, SPI_NOT_LOOP_BACK);
    reg_value = drv_32bit_set_field(reg_value, CTRLR0_FRF_FIELD_START_BIT, CTRLR0_FRF_FIELD_END_BIT,
                                    SPI_DEFAULT_FRAME_FORMAT);
    reg_value = drv_32bit_set_field(reg_value, CTRLR0_DFS_FIELD_START_BIT, CTRLR0_DFS_FIELD_END_BIT,
                                    (spi_attr->frame_len - 1));
    reg_value = drv_32bit_set_field(reg_value, CTRLR0_SCPOL_FIELD_BIT, CTRLR0_SCPOL_FIELD_BIT, spi_attr->clk_pol);
    reg_value = drv_32bit_set_field(reg_value, CTRLR0_SCPH_FIELD_BIT, CTRLR0_SCPH_FIELD_BIT, spi_attr->clk_edge);
    writel(reg_value, base_addr + SPI_CTRLR0_REG_OFFSET);

    if (hi309x_baudrate_cfg(base_addr, spi_attr->baud_rate)) {
        LOG(LOG_ERROR, "Failed to set SPI baud rate");
        return -EINVAL;
    }
    hispi_disable_intr(base_addr, 0xffffffff);  // Disable all interrupts

    /* Set the lp_subsys to control SPI. */
    lpsc_per10.val = 0;
    if (hispi->id == 0) {
        lpsc_per10.bits.spi0_in_n = 1;
        lpsc_per10.bits.spi0_rx_only = 1;
        lp_spi_val = BIT(0); // bit 0
    } else {
        lpsc_per10.bits.spi1_in_n = 1;
        lpsc_per10.bits.spi1_rx_only = 1;
        lp_spi_val = BIT(5); // bit 5
    }
    if (lp_subsys_reg_read_write(LP_SUBSYS_CSR_LPSC_SPI_CTRL_OFFSET_ADDR, lpsc_per10.val, lp_spi_val)) {
        LOG(LOG_ERROR, "lp_subsys_reg_read_write spi csf err!");
        return -ENODEV;
    }

    hispi_enable(base_addr);

    return 0;
}

static s32 hi309x_setup_para_check(struct spi_device *spi, u32 mode_bits)
{
    /* Check if the max speed is supported */
    if (spi->max_speed_hz > SPI_MAX_SPEED) {
        LOG(LOG_ERROR, "ERROR: The SPI support max speed: %uHz\n", SPI_MAX_SPEED);
        return -EINVAL;
    }

    /* Check if the mode is supported */
    if (spi->mode & ~mode_bits) {
        LOG(LOG_ERROR, "ERROR: SPI controller does not support mode bits 0x%X\n", spi->mode & ~mode_bits);
        return -EINVAL;
    }

    return 0;
}

static s32 hi309x_spi_setup(struct spi_device *spi)
{
    struct spi_controller *ctrl = spi->controller;
    struct hi309x_spi *hispi = spi_controller_get_devdata(ctrl);
    struct hi309x_spi_attributes *spi_attr = &hispi->spi_attributes;

    s32 ret;
    u32 tmp_clk_pol = spi_attr->clk_pol;
    u32 tmp_clk_edge = spi_attr->clk_edge;
    u32 tmp_baud_rate = spi_attr->baud_rate;
    u32 tmp_frame_len = spi_attr->frame_len;

    ret = hi309x_setup_para_check(spi, ctrl->mode_bits);
    if (ret) {
        return ret;
    }

    /* Set the SPI transfer mode. */
    if (spi->mode & SPI_CPOL) {
        spi_attr->clk_pol = HISPI_IDLE_CLOCK_POLARITYHIGH;
    } else {
        spi_attr->clk_pol = HISPI_IDLE_CLOCK_POLARITYLOW;
    }
    if (spi->mode & SPI_CPHA) {
        spi_attr->clk_edge = HISPI_CLOCK_EDGEDOWN;
    } else {
        spi_attr->clk_edge = HISPI_CLOCK_EDGEUP;
    }

    spi_attr->baud_rate = spi->max_speed_hz;
    spi_attr->frame_len = spi->bits_per_word;

    ret = hi309x_spi_ctrl_config(hispi);
    if (ret) {
        goto err;
    }
    return 0;

err:
    spi_attr->baud_rate = tmp_baud_rate;
    spi_attr->frame_len = tmp_frame_len;
    spi_attr->clk_pol = tmp_clk_pol;
    spi_attr->clk_edge = tmp_clk_edge;
    return ret;
}

static void hispi_unactivate_all_cs(void __iomem *base_addr)
{
    writel(SER_REG_VALUE_NON_SLAVE, base_addr + SPI_SER_REG_OFFSET);
    return;
}

static void hispi_activate_cs(void __iomem *base_addr, u8 cs)
{
    if (cs == SPI_SLAVE_0) {
        writel(SER_REG_VALUE_SLAVE_0, base_addr + SPI_SER_REG_OFFSET);
    } else if (cs == SPI_SLAVE_1) {
        writel(SER_REG_VALUE_SLAVE_1, base_addr + SPI_SER_REG_OFFSET);
    }
    return;
}

static void hi309x_spi_set_cs(struct spi_device *spi, bool enable)
{
    struct spi_controller *ctrl = spi->controller;
    struct hi309x_spi *hispi = spi_controller_get_devdata(ctrl);
    struct hi309x_spi_attributes *spi_attr = &hispi->spi_attributes;
    void __iomem *base_addr = hispi->virt_base;
    u32 cs = spi->chip_select;

    if (spi->chip_select == 0) {
        spi_attr->slave_select = SPI_SLAVE_0;
    } else {
        spi_attr->slave_select = SPI_SLAVE_1;
    }

    if (!enable) {
        hispi_activate_cs(base_addr, cs);
    } else {
        hispi_unactivate_all_cs(base_addr);
    }

    return;
}

static u32 get_frame_num(u32 frame_len, u32 len)
{
    return ((frame_len - 1) == HISPI_FRAME_LENGTH_16_BIT) ? (len / BYTES_PER_16BIT_FRAME ) : len;
}

static s32 spi_wait_spi_idle(void __iomem *base_addr)
{
    unsigned long timeout = jiffies + msecs_to_jiffies(50);

    /* Detects that the SPI is in idle state. */
    while (readl(base_addr + SPI_SR_REG_OFFSET) & BIT(0)) {
        if (time_after(jiffies, timeout)) {
            printk_ratelimited("wait spi idle timeout\n");
            return -ETIME;
        }
        usleep_range(1, 2); /* usleep 1~2 us */
    };

    return 0;
}

static void hi309x_spi_write_config(void __iomem *base_addr)
{
    hispi_disable(base_addr);
    drv_32bit_reg_set_field(base_addr + SPI_CTRLR0_REG_OFFSET, CTRLR0_TMOD_FIELD_START_BIT, CTRLR0_TMOD_FIELD_END_BIT,
                            SPI_TRAS_MODE_SEND);
    writel(SEND_LINE_VAL, base_addr + SPI_TXFTLR);
    hispi_enable(base_addr);

    return;
}

static s32 hi309x_spi_write(struct spi_device *spi, const u8 *tx_buf, u32 tx_frame_num)
{
    struct spi_controller *ctrl = spi->controller;
    struct hi309x_spi *hispi = spi_controller_get_devdata(ctrl);
    void __iomem *base_addr = hispi->virt_base;

    u32 tx_num_tmp;
    u32 len;
    u32 tx_tmp_value;
    u32 ret;
    u8 frame_len;

    hispi_unactivate_all_cs(base_addr);
    hispi->w_wait_flag = 1;
    tx_num_tmp = readl(base_addr + SPI_TX_NUM);
    len = ((TX_FIFO_SIZE - tx_num_tmp) > tx_frame_num) ? tx_frame_num : (TX_FIFO_SIZE - tx_num_tmp);
    /* Send data. */
    frame_len = readl(base_addr + SPI_CTRLR0_REG_OFFSET) & 0xf;
    while (len > 0) {
        if (frame_len == 0xf) {
            tx_tmp_value = (tx_buf == NULL) ? SPI_START_RECEIVE : ((*(u16 *)tx_buf) & 0xffff);
            if (tx_buf) {
                tx_buf = tx_buf + BYTES_PER_16BIT_FRAME;
            }
        } else {
            tx_tmp_value = (tx_buf == NULL) ? SPI_START_RECEIVE : ((*tx_buf) & 0xff);
            if (tx_buf) {
                tx_buf++;
            }
        }
        writel(tx_tmp_value, base_addr + SPI_DR0_REG_OFFSET);
        len--;
    }

    /* Start SPI transfer. */
    hispi_enable_intr(base_addr, INT_MASK_TX_EMPTY);
    hispi_activate_cs(base_addr, spi->chip_select);

    /* Wait until the interrupt processing is complete and set w_wait_flag to 0. */
    ret = wait_event_interruptible_timeout(hispi->w_wait_queue,
                                           (hispi->w_wait_flag == 0), SPI_TRANSFER_WAIT_TIMEOUT);
    if (!ret) {
        if (hispi->w_wait_flag) {
            LOG(LOG_ERROR, "wait_event_interruptible_timeout timeout fail(%d)!", ret);
            return -ETIMEDOUT;
        }
    } else if (ret == -ERESTARTSYS) {
        return -EINTR;
    }

    return spi_wait_spi_idle(base_addr);
}

static void spi_clear_rxfifo(void __iomem *base_addr)
{
    u32 i = 0;
    u32 rx_num_tmp;
    u32 rx_tmp_value;

    rx_num_tmp = readl(base_addr + SPI_RX_NUM);
    while (rx_num_tmp && (i < 100)) { // 100 cycles
        i++;
        rx_num_tmp = readl(base_addr + SPI_RX_NUM);
        (void)rx_num_tmp;
        rx_tmp_value = readl(base_addr + SPI_DR0_REG_OFFSET);
        (void)rx_tmp_value;
    }

    return;
}

static void hi309x_spi_read_config(void __iomem *base_addr, u32 rx_frame_num)
{
    hispi_disable(base_addr);

    /* Set the SPI mode to receive and transmit mode. */
    drv_32bit_reg_set_field((base_addr + SPI_CTRLR0_REG_OFFSET),
                            CTRLR0_TMOD_FIELD_START_BIT, CTRLR0_TMOD_FIELD_END_BIT, SPI_TRAS_MODE_DUPLEX);
    writel(rx_frame_num - 1, base_addr + SPI_CTRLR1_REG_OFFSET);

    /* Modifying the RX Interrupt Waterline Depth */
    writel(rx_frame_num - 1, base_addr + SPI_RXFTLR);
    writel(SEND_LINE_VAL, base_addr + SPI_TXFTLR);

    hispi_disable_intr(base_addr, INT_MASK_TX_EMPTY | INT_MASK_RX_FULL | INT_MASK_RX_OVER);
    hispi_enable(base_addr);

    return;
}

static s32 hi309x_spi_write_data(struct spi_controller *ctrl,
                                    struct spi_device *spi,
                                    struct spi_transfer *xfer)
{
    struct hi309x_spi *hispi = spi_controller_get_devdata(ctrl);
    void __iomem *base_addr = hispi->virt_base;
    const u8 *tx_buf = xfer->tx_buf;
    u32 tx_frame_num = get_frame_num(xfer->bits_per_word, xfer->len);

    hi309x_spi_write_config(base_addr);

    return hi309x_spi_write(spi, tx_buf, tx_frame_num);
}

static s32 hi309x_spi_read_data(struct spi_controller *ctrl,
                            struct spi_device *spi,
                            struct spi_transfer *xfer)
{
    s32 ret;
    u32 i;
    u32 rx_tmp_value;
    u8 frame_len;
    struct hi309x_spi *hispi = spi_controller_get_devdata(ctrl);
    void __iomem *base_addr = hispi->virt_base;
    u32 rx_frame_num = get_frame_num(xfer->bits_per_word, xfer->len);

    hi309x_spi_read_config(base_addr, rx_frame_num);
    spi_clear_rxfifo(base_addr);

    /* Connect to receive data. */
    hi309x_spi_write(spi, NULL, rx_frame_num);

    hispi->rx_len = xfer->len;
    hispi->rx_buf = xfer->rx_buf;
    hispi->r_wait_flag = 1;

    hispi_enable_intr(base_addr, INT_MASK_RX_FULL | INT_MASK_RX_OVER);

    /* Wait until the interrupt is processed to transmit the clock and receive data, and set r_wait_flag to 0. */
    ret = wait_event_interruptible_timeout(hispi->r_wait_queue, (hispi->r_wait_flag == 0), SPI_TRANSFER_WAIT_TIMEOUT);
    if (!ret) {
        if (hispi->r_wait_flag) {
            LOG(LOG_ERROR, "wait_event_interruptible_timeout timeout fail!");
            return -ETIMEDOUT;
        }
    } else if (ret == -ERESTARTSYS) {
        return -EINTR;
    }

    /* Read the data from the FIFO reg. */
    frame_len = (readl(base_addr + SPI_CTRLR0_REG_OFFSET)) & 0xf;
    if (frame_len == 0xf) {
        for (i = 0; i < hispi->rx_len; i = i + 2) { // offset 2
            rx_tmp_value = readl(base_addr + SPI_DR0_REG_OFFSET);
            hispi->rx_buf[i] = rx_tmp_value & 0xff;
            hispi->rx_buf[i + 1] = HIGH_BYTE_FROM_16BIT(rx_tmp_value) & 0xff; // >> 8
        }
    } else {
        for (i = 0; i < hispi->rx_len; i++) {
            rx_tmp_value = readl(base_addr + SPI_DR0_REG_OFFSET);
            hispi->rx_buf[i] = rx_tmp_value & 0xff;
        }
    }

    hispi->rx_buf += hispi->rx_len;
    hispi->rx_len = 0;
    spi_clear_rxfifo(base_addr);

    return 0;
}

static s32 spi_transfer_para_check(struct spi_transfer *xfer, u32 bits_per_word_mask, u32 max_speed_hz)
{
    if (bits_per_word_mask) {
        if (!(bits_per_word_mask & SPI_BPW_MASK(xfer->bits_per_word))) {
            LOG(LOG_ERROR, "SPI ERROR: Transfer bits_per_word (%d) not supported by the controller");
            return -EINVAL;
        }
    }

    if (xfer->len > SPI_BUFFER_MAX_SIZE) {  // buffer max len: 24 bytes
        LOG(LOG_ERROR, "SPI ERROR: Transfer length exceeds the maximum allowed (24 bytes)!");
        return -EINVAL;
    }

    printk("SPI: Transfer speed_hz: %uHz\n", xfer->speed_hz);
    if (xfer->speed_hz > max_speed_hz) {
        LOG(LOG_ERROR, "SPI ERROR: Transfer speed_hz is greater than the max_speed_hz!");
        return -EINVAL;
    }

    return 0;
}

static s32 spi_transfer_config(struct spi_controller *ctrl,
                                struct spi_device *spi,
                                struct spi_transfer *xfer)
{
    s32 ret;
    struct hi309x_spi *hispi = spi_controller_get_devdata(ctrl);
    void __iomem *base_addr = hispi->virt_base;
    u32 reg_value;

    ret = spi_transfer_para_check(xfer, ctrl->bits_per_word_mask, spi->max_speed_hz);
    if (ret) {
        return -EINVAL;
    }

    /* Set the SPI transfer config. */
    hispi_disable(base_addr);

    reg_value = readl(base_addr + SPI_CTRLR0_REG_OFFSET);
    reg_value = drv_32bit_set_field(reg_value, CTRLR0_DFS_FIELD_START_BIT, CTRLR0_DFS_FIELD_END_BIT,
                                    (xfer->bits_per_word - 1));
    writel(reg_value, base_addr + SPI_CTRLR0_REG_OFFSET);

    ret = hi309x_baudrate_cfg(base_addr, xfer->speed_hz);
    if (ret) {
        LOG(LOG_ERROR, "SPI: Failed to set speed_hz!");
        return -EINVAL;
    }

    hispi_enable(base_addr);

    return 0;
}

static s32 spi_config_reset(struct spi_controller *ctrl)
{
    s32 ret = 0;
    struct hi309x_spi *hispi = spi_controller_get_devdata(ctrl);
    struct hi309x_spi_attributes *spi_attr = &hispi->spi_attributes;
    void __iomem *base_addr = hispi->virt_base;
    u32 reg_value;

    hispi_disable(base_addr);

    /* Reset the SPI transfer config. */
    reg_value = readl(base_addr + SPI_CTRLR0_REG_OFFSET);
    reg_value = drv_32bit_set_field(reg_value, CTRLR0_DFS_FIELD_START_BIT, CTRLR0_DFS_FIELD_END_BIT,
                                    (spi_attr->frame_len - 1));
    reg_value = drv_32bit_set_field(reg_value, CTRLR0_SCPOL_FIELD_BIT, CTRLR0_SCPOL_FIELD_BIT, spi_attr->clk_pol);
    reg_value = drv_32bit_set_field(reg_value, CTRLR0_SCPH_FIELD_BIT, CTRLR0_SCPH_FIELD_BIT, spi_attr->clk_edge);
    writel(reg_value, base_addr + SPI_CTRLR0_REG_OFFSET);

    ret = hi309x_baudrate_cfg(base_addr, spi_attr->baud_rate);
    if (ret) {
        LOG(LOG_ERROR, "Failed to set SPI baud rate");
        goto err_out;
    }

    hispi_enable(base_addr);

err_out:
    return ret;
}

static s32 hi309x_spi_transfer_one(struct spi_controller *ctrl,
                                   struct spi_device *spi,
                                   struct spi_transfer *xfer)
{
    s32 ret;

    ret = spi_transfer_config(ctrl, spi, xfer);
    if (ret) {
        spi_config_reset(ctrl);
        return ret;
    }

    if (xfer->tx_buf && xfer->rx_buf) {
        ret = hi309x_spi_write_data(ctrl, spi, xfer);
        if (ret) {
            return ret;
        }
        ret = hi309x_spi_read_data(ctrl, spi, xfer);
    } else if (xfer->rx_buf) {
        ret = hi309x_spi_read_data(ctrl, spi, xfer);
    } else if (xfer->tx_buf) {
        ret = hi309x_spi_write_data(ctrl, spi, xfer);
    } else {
        ret = -EINVAL;
    }

    spi_config_reset(ctrl);
    return ret;
}

static void hi309x_spi_handle_err(struct spi_controller *ctrl, struct spi_message *msg)
{
    struct hi309x_spi *hispi = spi_controller_get_devdata(ctrl);
    void __iomem *base_addr = hispi->virt_base;

    hispi_disable(base_addr);
    hispi_disable_intr(base_addr, 0xffffffff);   // Disable all interrupts
    hispi_enable(base_addr);

    return;
}

static s32 hisi_spi_register_controller(struct device *dev, struct spi_controller *ctrl)
{
    struct hi309x_spi *hispi = spi_controller_get_devdata(ctrl);
    s32 ret;

    hispi->ctrl = ctrl;

    ctrl->mode_bits = SPI_CPOL | SPI_CPHA | SPI_LOOP;
    ctrl->bits_per_word_mask = SPI_BPW_MASK(8) | SPI_BPW_MASK(16);
    ctrl->bus_num = (s16)hispi->id;
    ctrl->num_chipselect = SPI_CHIP_SELCET_TOTAL_NUM;
    ctrl->setup = hi309x_spi_setup;
    ctrl->set_cs = hi309x_spi_set_cs;
    ctrl->transfer_one = hi309x_spi_transfer_one;
    ctrl->handle_err = hi309x_spi_handle_err;
    ctrl->max_speed_hz = SPI_MAX_SPEED;
    ctrl->dev.of_node = dev->of_node;

    ret = spi_register_controller(ctrl);
    if (ret) {
        goto err_register_controller;
    }

    return 0;

err_register_controller:
    dev_err(&ctrl->dev, "register spi ctrl err(%d)", ret);
    spi_unregister_controller(ctrl);
    return ret;
}

static s32 hi309x_spi_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct hi309x_spi *hispi;
    struct spi_controller *ctrl;
    s32 ret = 0;

    ctrl = devm_spi_alloc_master(dev, sizeof(*hispi));
    if (!ctrl) {
        return -ENOMEM;
    }
    platform_set_drvdata(pdev, ctrl);
    hispi = spi_controller_get_devdata(ctrl);

    ret = hi309x_spi_parse_dts(pdev, hispi);
    if (ret) {
        dev_err(dev, "Failed to parse device tree node\n");
        return ret;
    }

    ret = hi309x_spi_ctrl_init(pdev, hispi);
    if (ret) {
        goto err_out;
    }

    ret = hi309x_spi_irq_init(pdev, hispi);
    if (ret) {
        goto err_out;
    }

    ret = hisi_spi_register_controller(&pdev->dev, ctrl);
    if (ret) {
        goto err_out;
    }

    return 0;

err_out:
    iounmap((void __iomem *)hispi->virt_base);
    hispi->virt_base = NULL;
    return ret;
}

static s32 hi309x_spi_remove(struct platform_device *pdev)
{
    struct spi_controller *ctrl = platform_get_drvdata(pdev);
    struct hi309x_spi *hispi = spi_controller_get_devdata(ctrl);

    iounmap((void __iomem *)hispi->virt_base);
    hispi->virt_base = NULL;
    spi_unregister_controller(ctrl);

    return 0;
}

static struct of_device_id hi309x_spi_of_match[] = {
    { .compatible = "hisilicon,hisi-spi" },
    {}
};
MODULE_DEVICE_TABLE(of, hi309x_spi_of_match);

static struct platform_driver hi309x_spi_driver = {
    .driver = {
        .name = "hisi-spi",
        .of_match_table = hi309x_spi_of_match,
    },
    .probe = hi309x_spi_probe,
    .remove = hi309x_spi_remove,
};

module_platform_driver(hi309x_spi_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("spi driver");
