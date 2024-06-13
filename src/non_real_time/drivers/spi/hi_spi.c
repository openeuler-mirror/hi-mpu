// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * spi module interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/semaphore.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

#include <asm/io.h>

#include "comm_knl_api.h"
#include "lp_subsys.h"
#include "lp_subsys_csr_addr_define.h"
#include "lp_subsys_c_union_define.h"
#include "hispi_usr_api.h"
#include "hispi.h"
#include "spi_addr_define.h"

static int g_transmode = 1;
module_param(g_transmode, int, 0444);
MODULE_PARM_DESC(g_transmode, "SPI trans mode [0~1]!");

static struct hisi_spi_mgr g_hispi_mgr;
static cdev_st *g_infra_spi_dev = NULL;

static void hispi_enable_intr(void __iomem *base_addr, u32 mask)
{
    u32 temp;

    temp = readl(base_addr + SPI_IMR_REG_ADDR);
    temp |= mask;
    writel(temp, (base_addr + SPI_IMR_REG_ADDR));

    return;
}

static void hispi_disable_intr(void __iomem *base_addr, u32 mask)
{
    u32 temp;

    temp = readl(base_addr + SPI_IMR_REG_ADDR);
    temp &= ~mask;
    writel(temp, (base_addr + SPI_IMR_REG_ADDR));

    return;
}

static u32 hispi_get_frame_num(u32 ctrl_id, u32 len)
{
    u32 frame_len;

    frame_len = (readl(g_hispi_mgr.hispi[ctrl_id].virt_base + SPI_CTRLR0_REG_ADDR)) & 0xf;
    return (frame_len == 0xf) ? (len / 2) : len; // div 2
}

static s32 hispi_data_len_check(u32 ctrl_id, u32 length, u32 max_size)
{
    u32 frame_len;

    if ((length == 0) || (length > max_size)) {
        LOG(LOG_ERROR, "input length = %u is outof size(1 ~ %u)", length, max_size);
        return -EINVAL;
    }

    frame_len = (readl(g_hispi_mgr.hispi[ctrl_id].virt_base + SPI_CTRLR0_REG_ADDR)) & 0xf;
    if (frame_len == 0xf) {
        /* When the frame length is 16 bits, the number of input bytes must be an integral multiple of 2. */
        if (length % 2) {
            LOG(LOG_ERROR, "frame length is 16bit, input length(%u) must be multiple of 2", length);
            return -EINVAL;
        }
    }

    return 0;
}

static long hispi_ioctl(struct file *pfile, u32 cmd, unsigned long arg)
{
    s32 ret;

    ret = spi_process(cmd, arg);
    if (ret) {
        LOG(LOG_ERROR, "hispi_ioctl cmd %u failed", _IOC_NR(cmd));
    }

    return ret;
}

static s32 hispi_read_para_check(dev_spi_id *spi_id, u8 *send_cmd, u32 cmd_len, u8 *p_data, u32 data_len)
{
    s32 ret;
    u32 ctl_id = spi_id->ctrl_id;
    u32 cs = spi_id->cs_id;

    if ((cs >= SPI_CHIP_SELCET_TOTAL_NUM) || (ctl_id >= HISI_SPI_MAX_CTL)) {
        LOG(LOG_ERROR, "parameter ctrlid 0x%x, cs = 0x%x, out of range!", ctl_id, cs);
        return -EINVAL;
    }

    if (send_cmd == NULL) {
        LOG(LOG_ERROR, "read cmd buffer err(NULL)");
        return -EINVAL;
    }

    if (p_data == NULL) {
        LOG(LOG_ERROR, "data para buffer err(NULL)");
        return -EINVAL;
    }

    ret = hispi_data_len_check(ctl_id, cmd_len, TX_DATA_SIZE);
    if (ret) {
        LOG(LOG_ERROR, "cmd lenth(%u) is invalid", cmd_len);
        return -EINVAL;
    }

    ret = hispi_data_len_check(ctl_id, data_len, RX_DATA_SIZE);

    return ret;
}

static s32 hisi_spi_mgr_parse_of_node(struct device_node *np, u32 *ctl_id, s32 *v_irq, phys_addr_t *phys_base,
                                      unsigned long *phys_size)
{
    s32 ret;
    struct resource res;

    ret = of_property_read_u32(np, "ctlid", ctl_id);
    if (ret) {
        LOG(LOG_ERROR, "read ctl_id err %d", ret);
        return ret;
    }

    ret = of_address_to_resource(np, 0, &res);
    if (ret) {
        LOG(LOG_ERROR, "read phys_base err %d", ret);
        return ret;
    }

    *phys_base = res.start;
    *phys_size = resource_size(&res);

    *v_irq = irq_of_parse_and_map(np, 0);
    if (*v_irq == 0) {
        LOG(LOG_ERROR, "map hw_irq err");
        return -EINVAL;
    }

    return 0;
}

static s32 hisi_spi_mgr_init(s32 ctrl_id)
{
    s32 ret;
    struct hisi_spi *hispi = NULL;

    if (ctrl_id >= HISI_SPI_MAX_CTL) {
        return -EINVAL;
    }

    hispi = &g_hispi_mgr.hispi[ctrl_id];

    if (memset_s(hispi, sizeof(struct hisi_spi), 0, sizeof(struct hisi_spi))) {
        LOG(LOG_ERROR, "hispi_init bus %d memset failed", ctrl_id);
        return -EINVAL;
    }

    hispi->id = ctrl_id;
    ret = sprintf_s(hispi->irq_name, sizeof(hispi->irq_name), "hispi_irq%d", ctrl_id);
    if (ret == -1) {
        LOG(LOG_ERROR, "spi copy irq name failed(%d)", ret);
        return -EINVAL;
    }

    hispi->clock_sel = 2; // clock_sel is 2
    hispi->w_wait_flags = 0;
    hispi->r_wait_flags = 0;
    hispi->virt_base = NULL;
    hispi->rx_text_pos = NULL;
    hispi->rx_text_len = 0;

    g_hispi_mgr.init_flag[ctrl_id] = 1;

    return 0;
}

static void hisi_spi_mgr_deinit(u32 ctrl_id)
{
    struct hisi_spi *hispi = NULL;

    hispi = &g_hispi_mgr.hispi[ctrl_id];
    g_hispi_mgr.init_flag[ctrl_id] = 0;

    memset_s(hispi, sizeof(struct hisi_spi), 0, sizeof(struct hisi_spi));

    return;
}

static void hispi_free_irq(struct hisi_spi *hispi)
{
    if (g_infra_spi_dev) {
        free_irq(hispi->v_irq, hispi);
    }

    return;
}

static void hispi_disable(void __iomem *base_addr)
{
    writel(SPI_DISEN, base_addr + SPI_SSIENR_OFFSET_ADDR);

    return;
}

static void hispi_enable(void __iomem *base_addr)
{
    writel(SPI_EN, base_addr + SPI_SSIENR_OFFSET_ADDR);

    return;
}

static void hispi_clear_rxfifo(u32 ctrl_id)
{
    u32 i = 0;
    u32 rx_num_tmp;
    u32 rx_tmp_value;
    void __iomem *base_addr;

    base_addr = g_hispi_mgr.hispi[ctrl_id].virt_base;
    rx_num_tmp = readl(base_addr + SPI_RX_NUM);
    while (rx_num_tmp && (i < 100)) { // 100 cycles
        i++;
        rx_num_tmp = readl(base_addr + SPI_RX_NUM);
        (void)rx_num_tmp;
        rx_tmp_value = readl(base_addr + SPI_DR0_REG_ADDR);
        (void)rx_tmp_value;
    }

    return;
}

static s32 hispi_baudrate_cfg(u32 ctrl_id, u32 baud_rate)
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
    freq_div &= 0xffff;
    if ((freq_div < SPI_MIN_DIV) || (freq_div > SPI_MAX_DIV)) {
        LOG(LOG_ERROR, "freq_div out of range");
        return -EINVAL;
    }

    writel(freq_div, g_hispi_mgr.hispi[ctrl_id].virt_base + SPI_BAUDR_REG_ADDR);

    return 0;
}

static s32 hispi_ctrl_config(dev_spi_ioctl_std_init *ioctl_init)
{
    u32 tmp_value;
    u_lpsc_spi_ctrl lpsc_per10;
    u32 lp_spi_val;
    s32 ret;
    void __iomem *base_addr;
    u32 ctrl_id;

    ctrl_id = ioctl_init->ctrl_id;
    base_addr = g_hispi_mgr.hispi[ctrl_id].virt_base;
    hispi_disable(base_addr);

    /* Setting SPI CTRLR0 Control Sending */
    tmp_value = readl(base_addr + SPI_CTRLR0_OFFSET_ADDR);
    tmp_value = drv_32bit_set_field(tmp_value, CTRLR0_SRL_FIELD_BIT, CTRLR0_SRL_FIELD_BIT, SPI_NOT_LOOP_BACK);
    tmp_value = drv_32bit_set_field(tmp_value, CTRLR0_FRF_FIELD_START_BIT, CTRLR0_FRF_FIELD_END_BIT,
                                    SPI_DEFAULT_FRAME_FORMAT);
    tmp_value = drv_32bit_set_field(tmp_value, CTRLR0_DFS_FIELD_START_BIT, CTRLR0_DFS_FIELD_END_BIT,
                                    ioctl_init->frame_len);
    tmp_value = drv_32bit_set_field(tmp_value, CTRLR0_SCPOL_FIELD_BIT, CTRLR0_SCPOL_FIELD_BIT, ioctl_init->clk_pol);
    tmp_value = drv_32bit_set_field(tmp_value, CTRLR0_SCPH_FIELD_BIT, CTRLR0_SCPH_FIELD_BIT, ioctl_init->clk_edge);

    writel(tmp_value, base_addr + SPI_CTRLR0_REG_ADDR);

    ret = hispi_baudrate_cfg(ctrl_id, ioctl_init->baud);
    if (ret) {
        return ret;
    }

    hispi_disable_intr(base_addr, 0xffffffff);

    lpsc_per10.val = 0;
    if (ctrl_id == 0) {
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

    writel(SER_REG_VALUE_NON_SLAVE, base_addr + SPI_SER_REG_ADDR);

    /* Sets the slave device. The SPI module sends the chip select signal. */
    /* Transmits the data in the TX FIFO. */
    if (ioctl_init->cs_id == SPI_SLAVE_0_E) {
        writel(SER_REG_VALUE_SLAVE_0, base_addr + SPI_SER_REG_ADDR);
    } else if (ioctl_init->cs_id == SPI_SLAVE_1_E) {
        writel(SER_REG_VALUE_SLAVE_1, base_addr + SPI_SER_REG_ADDR);
    } else {
        LOG(LOG_ERROR, "no more chip!");
        return -EINVAL;
    }

    return 0;
}

static void hispi_read_config(u32 ctrl_id, u32 cs_id, u32 read_mode, u32 frame_num)
{
    void __iomem *base_addr;

    base_addr = g_hispi_mgr.hispi[ctrl_id].virt_base;
    hispi_disable(base_addr);

    /* Set the SPI module to receive and transmit mode. */
    if (read_mode == SPI_NON_EEPROM_RD_E) {
        /* Read a non-EEPOM device and set the SPI to the duplex mode. */
        drv_32bit_reg_set_field((base_addr + SPI_CTRLR0_REG_ADDR),
                                CTRLR0_TMOD_FIELD_START_BIT, CTRLR0_TMOD_FIELD_END_BIT, SPI_TRAS_MODE_DUPLEX);
    } else {
        /* Reads the EEPOM device and sets the SPI to the EEPROM read mode. */
        drv_32bit_reg_set_field((base_addr + SPI_CTRLR0_REG_ADDR),
                                CTRLR0_TMOD_FIELD_START_BIT, CTRLR0_TMOD_FIELD_END_BIT, SPI_TRAS_MODE_EEPROM_RD);
    }

    writel(frame_num - 1, base_addr + SPI_CTRLR1_OFFSET_ADDR);

    /* Modifying the RX Interrupt Waterline Depth */
    writel(frame_num - 1, base_addr + SPI_RXFTLR_OFFSET_ADDR);
    writel(SEND_LINE_VAL, base_addr + SPI_TXFTLR_OFFSET_ADDR);

    hispi_disable_intr(base_addr, INT_MASK_TX_EMPTY | INT_MASK_RX_FULL | INT_MASK_RX_OVER);

    hispi_enable(base_addr);

    return;
}

static void hispi_write_config(u32 ctl_id, u32 cs_id)
{
    void __iomem *base_addr;

    base_addr = g_hispi_mgr.hispi[ctl_id].virt_base;
    hispi_disable(base_addr);

    /* Set the SPI to send-only mode. */
    drv_32bit_reg_set_field(base_addr + SPI_CTRLR0_OFFSET_ADDR, CTRLR0_TMOD_FIELD_START_BIT, CTRLR0_TMOD_FIELD_END_BIT,
                            SPI_TRAS_MODE_SEND);

    writel(SEND_LINE_VAL, base_addr + SPI_TXFTLR_OFFSET_ADDR);
    hispi_enable(base_addr);

    return;
}

static s32 hispi_read(u32 ctrl_id, u32 cs_id, u8 *rx_buf, u32 rx_len)
{
    void __iomem *base_addr;
    s32 ret;
    u32 timeout = 500 * HZ / 1000;

    g_hispi_mgr.hispi[ctrl_id].rx_text_pos = rx_buf;
    g_hispi_mgr.hispi[ctrl_id].rx_text_len = rx_len;
    g_hispi_mgr.hispi[ctrl_id].r_wait_flags = 1;

    base_addr = g_hispi_mgr.hispi[ctrl_id].virt_base;
    hispi_enable_intr(base_addr, INT_MASK_RX_FULL | INT_MASK_RX_OVER);

    /* Wait until the interrupt is processed to transmit the clock and receive data, and set r_wait_flags to 0. */
    ret = wait_event_interruptible_timeout(g_hispi_mgr.hispi[ctrl_id].r_select_wait,
                                           (g_hispi_mgr.hispi[ctrl_id].r_wait_flags == 0), (long)timeout);
    if (!ret) {
        if (g_hispi_mgr.hispi[ctrl_id].r_wait_flags) {
            LOG(LOG_ERROR, "wait_event_interruptible_timeout timeout fail!");
            return -ETIMEDOUT;
        }
    } else if (ret == -ERESTARTSYS) {
        return -EINTR;
    }

    return 0;
}

static s32 hispi_wait_spi_idle(u32 ctrl_id)
{
    unsigned long timeout = jiffies + msecs_to_jiffies(50);
    void __iomem *base_addr = g_hispi_mgr.hispi[ctrl_id].virt_base;

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

static s32 hispi_write(u32 ctrl_id, u32 cs_id, u8 *tx_buf, u32 tx_len)
{
    void __iomem *base_addr;
    s32 ret;
    u32 tx_num_tmp;
    u32 len;
    u32 tx_tmp_value;
    u32 timeout = 500 * HZ / 1000;
    u32 frame_len;

    base_addr = g_hispi_mgr.hispi[ctrl_id].virt_base;
    writel(SER_REG_VALUE_NON_SLAVE, base_addr + SPI_SER_REG_ADDR);

    g_hispi_mgr.hispi[ctrl_id].w_wait_flags = 1;

    tx_num_tmp = readl(base_addr + SPI_TX_NUM);
    len = ((TX_FIFO_SIZE - tx_num_tmp) > tx_len) ? tx_len : (TX_FIFO_SIZE - tx_num_tmp);
    frame_len = (readl(base_addr + SPI_CTRLR0_REG_ADDR)) & 0xf;

    while (len > 0) {
        if (frame_len == 0xf) {
            tx_tmp_value = (tx_buf == NULL) ? SPI_START_RECEIVE : ((*(u16 *)tx_buf) & 0xffff);
            if (tx_buf) {
                tx_buf = tx_buf + 2; // add 2
            }
        } else {
            tx_tmp_value = (tx_buf == NULL) ? SPI_START_RECEIVE : ((*tx_buf) & 0xff);
            if (tx_buf) {
                tx_buf++;
            }
        }
        writel(tx_tmp_value, base_addr + SPI_DR0_REG_ADDR);
        len--;
    }

    hispi_enable_intr(base_addr, INT_MASK_TX_EMPTY);
    writel(BIT(cs_id), base_addr + SPI_SER_REG_ADDR);

    /* Wait until the interrupt processing is complete and set w_wait_flags to 0. */
    ret = wait_event_interruptible_timeout(g_hispi_mgr.hispi[ctrl_id].w_select_wait,
                                           (g_hispi_mgr.hispi[ctrl_id].w_wait_flags == 0), (long)timeout);
    if (!ret) {
        if (g_hispi_mgr.hispi[ctrl_id].w_wait_flags) {
            LOG(LOG_ERROR, "wait_event_interruptible_timeout timeout fail(%d)!", ret);
            return -ETIMEDOUT;
        }
    } else if (ret == -ERESTARTSYS) {
        return -EINTR;
    }

    return hispi_wait_spi_idle(ctrl_id);
}

static s32 hispi_data_read(dev_spi_id *spi_id, u8 *tx_buf, u32 tx_len, u8 *rx_buf, u32 rx_len)
{
    s32 ret;
    u32 rx_frame_num, tx_frame_num;
    u32 ctrl_id = spi_id->ctrl_id;
    u32 cs_id = spi_id->cs_id;

    rx_frame_num = hispi_get_frame_num(ctrl_id, rx_len);
    tx_frame_num = hispi_get_frame_num(ctrl_id, tx_len);

    hispi_read_config(ctrl_id, cs_id, g_transmode, rx_frame_num);

    ret = hispi_write(ctrl_id, cs_id, tx_buf, tx_frame_num);
    if (ret) {
        return ret;
    }

    /* TX rxlen 0x5a5a */
    if (g_transmode == SPI_NON_EEPROM_RD_E) {
        ret = hispi_write(ctrl_id, cs_id, NULL, rx_frame_num);
        if (ret) {
            return ret;
        }
    }

    ret = hispi_read(ctrl_id, cs_id, rx_buf, rx_len);

    hispi_clear_rxfifo(ctrl_id);

    return ret;
}

static s32 hispi_data_write(u32 ctrl_id, u32 cs_id, u8 *tx_buf, u32 tx_len)
{
    u32 tx_frame_num;

    hispi_write_config(ctrl_id, cs_id);

    tx_frame_num = hispi_get_frame_num(ctrl_id, tx_len);

    return hispi_write(ctrl_id, cs_id, tx_buf, tx_frame_num);
}

static s32 hispi_init_param_check(dev_spi_ioctl_std_init *ioctl_init)
{
    u32 ctrl_id = ioctl_init->ctrl_id;
    u32 cs_id = ioctl_init->cs_id;
    u32 protocol_type = ioctl_init->pro_type;
    u32 frame_len = ioctl_init->frame_len;
    u32 clk_edge = ioctl_init->clk_edge;
    u32 clk_pol = ioctl_init->clk_pol;

    if ((ctrl_id >= HISI_SPI_MAX_CTL) || (cs_id >= SPI_CHIP_SELCET_TOTAL_NUM)) {
        LOG(LOG_ERROR, "ctrl_id(%u) cs_id(%u) error", ctrl_id, cs_id);
        return -EINVAL;
    }

    if ((protocol_type != HISPI_TYPE_ADSPI) && (protocol_type != HISPI_TYPE_SSPI)) {
        LOG(LOG_ERROR, "parameter error: pro_type = %u", protocol_type);
        return -EINVAL;
    }

    if ((frame_len < HISPI_FRAME_LENGTH_4_BIT) || (frame_len > HISPI_FRAME_LENGTH_16_BIT)) {
        LOG(LOG_ERROR, "parameter error: frame_len(%u) out of range", frame_len);
        return -EINVAL;
    }

    if (clk_edge >= HISPI_CLOCK_EDGEERR) {
        LOG(LOG_ERROR, "parameter error: clk_edge = %u", clk_edge);
        return -EINVAL;
    }

    if (clk_pol >= HISPI_IDLE_CLOCK_POLARITYERR) {
        LOG(LOG_ERROR, "parameter error: clk_pol = %u", clk_pol);
        return -EINVAL;
    }

    return 0;
}

s32 kernspi_init(dev_spi_ioctl_std_init *ioctl_init)
{
    s32 ret;
    u32 ctrl_id;

    if (!ioctl_init) {
        LOG(LOG_ERROR, "Error: ioctl_init is NULL");
        return -EINVAL;
    }

    ret = hispi_init_param_check(ioctl_init);
    if (ret) {
        return ret;
    }

    ctrl_id = ioctl_init->ctrl_id;

    ret = down_interruptible(&g_hispi_mgr.hispi[ctrl_id].spi_bus_sem);
    if (ret) {
        LOG(LOG_ERROR, "down_interruptible sem error!");
        return ret;
    }

    ret = hispi_ctrl_config(ioctl_init);
    up(&g_hispi_mgr.hispi[ctrl_id].spi_bus_sem);

    return ret;
}
EXPORT_SYMBOL(kernspi_init);

s32 kernspi_read_data(dev_spi_id *spi_id, u8 *cmd, u32 cmd_len, u8 *data, u32 data_len)
{
    s32 ret;
    u32 ctrl_id;

    if (!spi_id) {
        LOG(LOG_ERROR, "Error: spi_id is NULL");
        return -EINVAL;
    }

    ret = hispi_read_para_check(spi_id, cmd, cmd_len, data, data_len);
    if (ret) {
        LOG(LOG_ERROR, "read hispi_read_para_check err!");
        return ret;
    }

    ctrl_id = spi_id->ctrl_id;

    /* Obtain the semaphore. */
    ret = down_interruptible(&g_hispi_mgr.hispi[ctrl_id].spi_bus_sem);
    if (ret) {
        LOG(LOG_ERROR, "down_interruptible sem error!");
        return ret;
    }

    ret = hispi_data_read(spi_id, cmd, cmd_len, data, data_len);
    if (ret < 0) {
        LOG(LOG_ERROR, "call 0x%x hispi_data_read() failed!", ctrl_id);
    }

    /* Release the semaphore */
    up(&g_hispi_mgr.hispi[ctrl_id].spi_bus_sem);

    return ret;
}
EXPORT_SYMBOL(kernspi_read_data);

s32 kernspi_write_data(dev_spi_id *spi_id, u8 *cmd, u32 cmd_len)
{
    s32 ret;
    u32 ctrl_id;
    u32 cs_id;

    if (!spi_id) {
        LOG(LOG_ERROR, "Error: spi_id is NULL");
        return -EINVAL;
    }

    ctrl_id = spi_id->ctrl_id;
    cs_id = spi_id->cs_id;

    if (cs_id >= SPI_CHIP_SELCET_TOTAL_NUM || ctrl_id >= HISI_SPI_MAX_CTL) {
        LOG(LOG_ERROR, "invalid parameter:cs_id:%u, ctrl_id:%u", cs_id, ctrl_id);
        return -EINVAL;
    }

    if (cmd == NULL) {
        LOG(LOG_ERROR, "cmd err len = %u!", cmd_len);
        return -EINVAL;
    }

    ret = hispi_data_len_check(ctrl_id, cmd_len, (TX_DATA_SIZE + TX_CMD_SIZE));
    if (ret) {
        LOG(LOG_ERROR, "cmdlen(%u) is invaild", cmd_len);
        return -EINVAL;
    }

    /* Obtain the semaphore. */
    ret = down_interruptible(&g_hispi_mgr.hispi[ctrl_id].spi_bus_sem);
    if (ret) {
        LOG(LOG_ERROR, "down_interruptible sem error (%d)! cs_id = %u, out of range!", ret, cs_id);
        return ret;
    }

    ret = hispi_data_write(ctrl_id, cs_id, cmd, cmd_len);
    if (ret < 0) {
        LOG(LOG_ERROR, "ctrl_id %u hispi_data_write failed(%d)!!", ctrl_id, ret);
    }

    /* Release the semaphore */
    up(&g_hispi_mgr.hispi[ctrl_id].spi_bus_sem);

    return ret;
}
EXPORT_SYMBOL(kernspi_write_data);

static const struct file_operations g_infra_spi_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = hispi_ioctl,
};

static s32 hispi_dev_init(void)
{
    s32 ret;
    if (!g_infra_spi_dev) {
        g_infra_spi_dev = (cdev_st *)kzalloc(sizeof(cdev_st), GFP_KERNEL);
        if (!g_infra_spi_dev) {
            return -ENOMEM;
        }

        ret = comm_init_dev(g_infra_spi_dev, &g_infra_spi_fops, DEV_SPI_NAME);
        if (ret) {
            kfree(g_infra_spi_dev);
            g_infra_spi_dev = NULL;
            LOG(LOG_ERROR, "spi_init dev fail!");
            return ret;
        }
    }

    return 0;
}

static void hispi_dev_exit(void)
{
    if (g_infra_spi_dev) {
        comm_cleanup_dev(g_infra_spi_dev);
        kfree(g_infra_spi_dev);
        g_infra_spi_dev = NULL;
    }

    return;
}

static irqreturn_t hispi_irq_handler(s32 irq, void *dev_id)
{
    u32 irq_en_tmp;
    u32 rx_tmp_value;
    u32 i;
    u32 frame_len;
    void __iomem *base_addr;

    struct hisi_spi *hispi = (struct hisi_spi *)dev_id;
    if (hispi == NULL) {
        LOG(LOG_ERROR, "hispi null isr sta ");
        return IRQ_HANDLED;
    }

    base_addr = hispi->virt_base;
    frame_len = (readl(base_addr + SPI_CTRLR0_REG_ADDR)) & 0xf;
    irq_en_tmp = readl(base_addr + SPI_ISR_REG_OFFSET);
    if ((irq_en_tmp & ISR_TX_EMPTY) == ISR_TX_EMPTY) {
        hispi_disable_intr(base_addr, INT_MASK_TX_EMPTY);
        hispi->w_wait_flags = 0;
        wake_up_interruptible(&(hispi->w_select_wait));
    } else if (((irq_en_tmp & ISR_RX_FULL) == ISR_RX_FULL) || ((irq_en_tmp & ISR_RX_OVERFLOW) == ISR_RX_OVERFLOW)) {
        if (frame_len == 0xf) {
            for (i = 0; i < hispi->rx_text_len; i = i + 2) { // offset 2
                rx_tmp_value = readl(base_addr + SPI_DR0_REG_ADDR);
                hispi->rx_text_pos[i] = rx_tmp_value & 0xff;
                hispi->rx_text_pos[i + 1] = (rx_tmp_value >> 8) & 0xff; // >> 8
            }
        } else {
            for (i = 0; i < hispi->rx_text_len; i++) {
                rx_tmp_value = readl(base_addr + SPI_DR0_REG_ADDR);
                hispi->rx_text_pos[i] = rx_tmp_value & 0xff;
            }
        }

        hispi->rx_text_pos += hispi->rx_text_len;
        hispi->rx_text_len = 0;

        hispi_disable_intr(base_addr, INT_MASK_RX_FULL | INT_MASK_RX_OVER);
        hispi->r_wait_flags = 0;
        wake_up_interruptible(&hispi->r_select_wait);
    } else {
        LOG(LOG_ERROR, "unknow state irq_en_tmp = 0x%x", irq_en_tmp);
        hispi_disable_intr(base_addr, irq_en_tmp);

        /* Clears the TX FIFO overflow interrupt. */
        rx_tmp_value = readl(base_addr + SPI_CLEAN_TX_ISR_REG);
        (void)rx_tmp_value;
    }

    return IRQ_HANDLED;
}

static s32 hispi_probe(struct hisi_spi *hispi)
{
    s32 ret;

    ret = hispi_dev_init();
    if (ret) {
        LOG(LOG_ERROR, "spi_init fail!");
        return ret;
    }

    ret = request_irq(hispi->v_irq, hispi_irq_handler, 0, hispi->irq_name, (void *)hispi);
    if (ret) {
        LOG(LOG_ERROR, "request_irq fail!");
        return ret;
    }

    init_waitqueue_head(&hispi->w_select_wait);
    init_waitqueue_head(&hispi->r_select_wait);

    return 0;
}

static s32 hispi_setup_device(struct hisi_spi *hispi, s32 v_irq, phys_addr_t phys_base, unsigned long phys_size)
{
    s32 ret;

    if (hispi == NULL || phys_base == 0) {
        return -EINVAL;
    }

    hispi->v_irq = v_irq;
    hispi->phys_base = phys_base;

    ret = hispi_probe(hispi);
    if (ret) {
        LOG(LOG_ERROR, "hispi_probe error!");
        return -EIO;
    }

    if (hispi->virt_base == NULL) {
        LOG(LOG_ERROR, "hispi_setup_device id 0x%x!", hispi->id);
        hispi->virt_base = ioremap(phys_base, phys_size);
        if (hispi->virt_base == NULL) {
            LOG(LOG_ERROR, "virt_base ioremap error!");
            ret = -EIO;
            goto err_out;
        }
    }

    init_mutex(&g_hispi_mgr.hispi[hispi->id].spi_bus_sem);

    return 0;

err_out:
    if (hispi->virt_base) {
        iounmap(hispi->virt_base);
        hispi->virt_base = NULL;
    }

    hispi_free_irq(hispi);

    return ret;
}

static s32 hispi_probe_driver(struct platform_device *pdev)
{
    s32 ret;
    u32 ctl_id = 0;
    s32 v_irq = 0;
    phys_addr_t phys_base = 0;
    unsigned long phys_size = 0;
    struct hisi_spi *hispi = NULL;

    ret = hisi_spi_mgr_parse_of_node(pdev->dev.of_node, &ctl_id, &v_irq, &phys_base, &phys_size);
    if (ret) {
        LOG(LOG_ERROR, "hisi_spi_mgr_parse_of err(%d)", ret);
        return -ENODEV;
    }

    if (ctl_id >= HISI_SPI_MAX_CTL) {
        LOG(LOG_ERROR, "ctrl_id(%u), out of range(0~1)", ctl_id);
        return -EINVAL;
    }

    hispi = &g_hispi_mgr.hispi[ctl_id];

    ret = hisi_spi_mgr_init(ctl_id);
    if (ret) {
        LOG(LOG_ERROR, "hisi_spi_mgr_init of spi %u err %d", ctl_id, ret);
        return ret;
    }

    hispi->dev = &pdev->dev;
    if (hispi->dev == NULL) {
        LOG(LOG_ERROR, "init of spi %u err %d", ctl_id, ret);
    }

    ret = hispi_setup_device(hispi, v_irq, phys_base, phys_size);
    if (ret) {
        hisi_spi_mgr_deinit(ctl_id);
        hispi_dev_exit();
    }

    return ret;
}

static s32 hispi_remove_driver(struct platform_device *dev)
{
    u32 i;

    for (i = 0; i < HISI_SPI_MAX_CTL; i++) {
        if (g_hispi_mgr.hispi[i].virt_base) {
            iounmap((void __iomem *)g_hispi_mgr.hispi[i].virt_base);
            g_hispi_mgr.hispi[i].virt_base = NULL;
        }

        hispi_free_irq(&g_hispi_mgr.hispi[i]);
        hisi_spi_mgr_deinit(i);
    }

    hispi_dev_exit();

    return 0;
}

static const struct of_device_id hisi_spi_of_match[] = {
    { .compatible = "hisilicon,hisi-spi" }, {}
};

static struct platform_driver infra_spi_driver = {
    .driver = {
        .name = "Hi1711-of-spi",
        .owner = THIS_MODULE,
        .of_match_table = hisi_spi_of_match,
    },
    .probe = hispi_probe_driver,
    .remove = hispi_remove_driver,
};

module_platform_driver(infra_spi_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("spi driver");