// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * localbus module sdk interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/mm.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/mtd/mtd.h>

#include "local_bus_addr_define.h"
#include "comm_knl_api.h"
#include "local_bus_usr_api.h"
#include "bdinfo.h"
#include "board_info.h"
#include "local_bus_ker.h"

static localbus_mgr_s *g_localbus_mgr = NULL;

static void localbus_irq_enable(u32 enable)
{
    u_lb_intmsk localbus_intmsk;

    if (enable) {
        localbus_intmsk.val = 0xF0F0;
    } else {
        localbus_intmsk.val = 0;
    }

    iowrite32(localbus_intmsk.val, g_localbus_mgr->regbase + LB_INTMSK_OFFSET_ADDR);

    return;
}

static u32 localbus_err_type_check(void)
{
    u_lb_intsts int_status;
    u_lb_rint clean_rint;

    clean_rint.val = 0;
    int_status.val = ioread32(g_localbus_mgr->regbase + LB_INTSTS_OFFSET_ADDR);
    clean_rint.val = int_status.val;

    if (int_status.bits.cf_wr_prot_intsts_3) {
        LOG(LOG_ERROR, "ERROR: Write to the address3 protected by write protection");
    }

    if (int_status.bits.cf_wr_prot_intsts_2) {
        LOG(LOG_ERROR, "ERROR: Write to the address2 protected by write protection");
    }

    if (int_status.bits.cf_wr_prot_intsts_1) {
        LOG(LOG_ERROR, "ERROR: Write to the address1 protected by write protection");
    }

    if (int_status.bits.cf_wr_prot_intsts_0) {
        LOG(LOG_ERROR, "ERROR: Write to the address0 protected by write protection");
    }

    if (int_status.bits.cf_rdy_overtime_intsts_3) {
        LOG(LOG_ERROR, "ERROR: cs3's external device response timeout");
    }

    if (int_status.bits.cf_rdy_overtime_intsts_2) {
        LOG(LOG_ERROR, "ERROR: cs2's external device response timeout");
    }

    if (int_status.bits.cf_rdy_overtime_intsts_1) {
        LOG(LOG_ERROR, "ERROR: cs1's external device response timeout");
    }

    if (int_status.bits.cf_rdy_overtime_intsts_0) {
        LOG(LOG_ERROR, "ERROR: cs0's external device response timeout");
    }

    return clean_rint.val;
}

static irqreturn_t localbus_interrupt_handle(int irq, void *arg)
{
    u_lb_rint clean_rint;

    /* Check the interrupt status and print the cause of the reported interrupt. */
    clean_rint.val = localbus_err_type_check();

    /* Interrupt clear. Writing 1 clears the interrupt. */
    iowrite32(clean_rint.val, g_localbus_mgr->regbase + LB_RINT_OFFSET_ADDR);

    return IRQ_HANDLED;
}

static s32 localbus_mgr_width_base_set(void)
{
    u32 i;
    /* The bit width obtained by CS1 is valid in bit 5 and bit 6, and the mask is 3. */
    g_localbus_mgr->width[CS1] = (ioread32(g_localbus_mgr->regbase + LB_CS1_CTRL_OFFSET_ADDR) >> 5) & 0x3;
    /* The bit width obtained by CS1 is valid in bit 5 and bit 6, and the mask is 3. */
    g_localbus_mgr->width[CS2] = (ioread32(g_localbus_mgr->regbase + LB_CS2_CTRL_OFFSET_ADDR) >> 5) & 0x3;
    /* The bit width obtained by CS1 is valid in bit 5 and bit 6, and the mask is 3. */
    g_localbus_mgr->width[CS3] = (ioread32(g_localbus_mgr->regbase + LB_CS3_CTRL_OFFSET_ADDR) >> 5) & 0x3;

    g_localbus_mgr->cs_io_addr[CS1] = (u8 *)ioremap(CS1_PHY_BASE_ADDR, CS1_MAP_LEN);
    g_localbus_mgr->cs_io_addr[CS2] = (u8 *)ioremap(CS2_PHY_BASE_ADDR, CS2_MAP_LEN);
    g_localbus_mgr->cs_io_addr[CS3] = (u8 *)ioremap(CS3_PHY_BASE_ADDR, CS3_MAP_LEN);

    for (i = 0; i < CS_NUM; i++) {
        if (g_localbus_mgr->cs_io_addr[i] == NULL) {
            LOG(LOG_ERROR, "ioremap err");
            return -1;
        }
    }

    return 0;
}

static void localbus_get_offset_bit(u32 mask, u32 * const offset)
{
    u32 i = 0;

    while (i < MASK_SIZE) {
        if (((mask >> i) & 0x1) == 1) {
            *offset = i;
            break;
        }

        i++;
    };

    return;
}

s32 localbus_set_cs_width_offset(u32 cs, u32 width, u32 addr_offset)
{
    u_lb_cs1_ctrl cs1_ctrl;

    if (cs > CS_NUM) {
        LOG(LOG_ERROR, "cs should not be greater than 3");
        return -EINVAL;
    }

    if (width > CS_WIDTH_32) {
        LOG(LOG_ERROR, "width(%u) out of range(0~2)", width);
        return -EINVAL;
    }

    if ((addr_offset != CF_ADDR_OFFSET0) && (addr_offset != CF_ADDR_OFFSET1)) {
        LOG(LOG_ERROR, "offset mast be 0(no offset) or 1(has offset)");
        return -EINVAL;
    }

    /* If the CS bit width is not the target value, change it to the target bit width. */
    cs1_ctrl.val = ioread32(g_localbus_mgr->regbase + LB_CS0_CTRL_OFFSET_ADDR + cs * OFFSET_SIZE);
    cs1_ctrl.bits.cf_data_wid_1 = width;
    cs1_ctrl.bits.cf_addr_offset_1 = addr_offset;
    iowrite32(cs1_ctrl.val, g_localbus_mgr->regbase + LB_CS0_CTRL_OFFSET_ADDR + cs * OFFSET_SIZE);

    return 0;
}
EXPORT_SYMBOL(localbus_set_cs_width_offset);

static s32 localbus_type_mask_check(u32 cs, u32 type, size_t count, u32 mask)
{
    u32 tmp;
    u32 data_len;

    if (type != OP_TYPE_BITS) {
        return 0;
    }

    if (g_localbus_mgr->width[cs] == CS_WIDTH_8) {
        tmp = mask & 0xff;
        data_len = WIDTH8_BYTE_NUM;
    } else if (g_localbus_mgr->width[cs] == CS_WIDTH_16) {
        tmp = mask & 0xffff;
        data_len = WIDTH16_BYTE_NUM;
    } else {
        tmp = mask;
        data_len = WIDTH32_BYTE_NUM;
    }

    if (tmp == 0) {
        LOG(LOG_ERROR, "ERROR mask:0x%x", mask);
        return -EINVAL;
    }

    if (count != data_len) {
        LOG(LOG_ERROR, "ERROR count:0x%x", count);
        return -EINVAL;
    }

    return 0;
}

static s32 localbus_addr_len_alignment_check(u32 cs, size_t len, loff_t pos)
{
    if ((g_localbus_mgr->width[cs] == CS_WIDTH_16) || (g_localbus_mgr->width[cs] == CS_WIDTH_32)) {
        /* The length of the access must be aligned. */
        if ((((u32)len) % (g_localbus_mgr->width[cs] * 2)) != 0) { // mul 2
            LOG(LOG_ERROR, "ERROR (len = %u, invalid)", len);
            return -EINVAL;
        }

        if ((((u32)pos) % (g_localbus_mgr->width[cs] * 2)) != 0) { // mul 2
            LOG(LOG_ERROR, "ERROR (pos = %u invalid)", (size_t)pos);
            return -EINVAL;
        }
    }

    return 0;
}

static s32 localbus_para_check(l_bus_opt_s *l_bus_opt, s32 mode)
{
    s32 ret;

    if ((mode != L_BUS_USER_MODE) && (mode != L_BUS_KERNEL_MODE)) {
        LOG(LOG_ERROR, "ERROR mode invalid");
        return -EINVAL;
    }

    if (l_bus_opt == NULL) {
        LOG(LOG_ERROR, "ERROR l_bus_opt cant be null");
        return -EINVAL;
    }

    if (l_bus_opt->cs >= CS_NUM) {
        LOG(LOG_ERROR, "ERROR cs%u", l_bus_opt->cs);
        return -EINVAL;
    }

    if (l_bus_opt->type > OP_TYPE_BITS) {
        LOG(LOG_ERROR, "ERROR type%u", l_bus_opt->type);
        return -EINVAL;
    }

    if ((l_bus_opt->len == 0) || (l_bus_opt->len > CS_MAX_DATA_LEN) || (l_bus_opt->offset > CS_MAP_LEN)) {
        LOG(LOG_ERROR, "ERROR (len = %u, pos = %u, out of range)", l_bus_opt->len, (size_t)l_bus_opt->offset);
        return -EINVAL;
    }

    if ((l_bus_opt->len + l_bus_opt->offset) > CS_MAP_LEN) {
        LOG(LOG_ERROR, "ERROR (count = %u, pos = %u invalid)", l_bus_opt->len, (size_t)l_bus_opt->offset);
        return -EINVAL;
    }

    ret = localbus_addr_len_alignment_check(l_bus_opt->cs, l_bus_opt->len, l_bus_opt->offset);
    if (ret) {
        LOG(LOG_ERROR, "ERROR addr(%u) or len(%u) is not aligned", (size_t)l_bus_opt->offset, l_bus_opt->len);
        return ret;
    }

    if (l_bus_opt->data == NULL) {
        LOG(LOG_ERROR, "ERROR buf is null");
        return -EINVAL;
    }

    return localbus_type_mask_check(l_bus_opt->cs, l_bus_opt->type, l_bus_opt->len, l_bus_opt->mask);
}

static void localbus_read_write(u32 cs, size_t count, const u8 *buf, const void __iomem *offset, bool read)
{
    u32 i = 0;
    u8 *l_puc = NULL;
    u16 *l_pus = NULL;
    u32 *l_pul = NULL;
    void __iomem *addr = (void __iomem *)offset;

    /* 8-bit width access */
    if (g_localbus_mgr->width[cs] == CS_WIDTH_8) {
        l_puc = (u8 *)buf;
        while (i < count) {
            if (read) {
                *l_puc = ioread8(addr);
            } else {
                iowrite8(*l_puc, addr);
            }
            i += sizeof(u8);
            addr += sizeof(u8);
            l_puc++;
        }
    } else if (g_localbus_mgr->width[cs] == CS_WIDTH_16) {
        /* 16-bit width access */
        l_pus = (u16 *)buf;
        while (i < count) {
            if (read) {
                *l_pus = (u16)ioread16(addr);
            } else {
                iowrite16(*l_pus, addr);
            }
            i += sizeof(u16);
            addr += sizeof(u16);
            l_pus++;
        }
    } else {
        /* 32-bit bit width access */
        l_pul = (u32 *)buf;
        while (i < count) {
            if (read) {
                *l_pul = ioread32(addr);
            } else {
                iowrite32(*l_pul, addr);
            }
            i += sizeof(u32);
            addr += sizeof(u32);
            l_pul++;
        }
    }

    return;
}

static void localbus_write_according_width(u32 cs, size_t count, const u8 *buf, const void __iomem *offset)
{
    localbus_read_write(cs, count, buf, offset, false);

    return;
}

static void localbus_read_according_width(u32 cs, size_t count, const u8 *buf, const void __iomem *offset)
{
    localbus_read_write(cs, count, buf, offset, true);

    return;
}

s32 localbus_get_data(l_bus_opt_s *l_bus_opt, s32 mode)
{
    s32 ret;
    void __iomem *offset;
    u8 *tmpbuf = NULL;
    u32 offset_bit = 0;

    ret = localbus_para_check(l_bus_opt, mode);
    if (ret != 0) {
        LOG(LOG_ERROR, "localbus_para_check err");
        return ret;
    }

    if ((ret = down_interruptible(&g_localbus_mgr->localbus_access_sem[l_bus_opt->cs])) != 0) {
        LOG(LOG_ERROR, "ERROR down_interruptible");
        return ret;
    }

    offset = g_localbus_mgr->cs_io_addr[l_bus_opt->cs] + l_bus_opt->offset;
    tmpbuf = (u8 *)kzalloc(l_bus_opt->len, GFP_KERNEL);
    if (!tmpbuf) {
        LOG(LOG_ERROR, "kzalloc err");
        ret = -ENOMEM;
        goto EXIT;
    }

    localbus_read_according_width(l_bus_opt->cs, l_bus_opt->len, tmpbuf, offset);

    if (l_bus_opt->type == OP_TYPE_BITS) {
        localbus_get_offset_bit(l_bus_opt->mask, &offset_bit);

        if (g_localbus_mgr->width[l_bus_opt->cs] == CS_WIDTH_8) {
            *(u8 *)tmpbuf = ((*(u8 *)tmpbuf) & (u8)l_bus_opt->mask) >> offset_bit;
        } else if (g_localbus_mgr->width[l_bus_opt->cs] == CS_WIDTH_16) {
            *(u16 *)tmpbuf = ((*(u16 *)tmpbuf) & (u16)l_bus_opt->mask) >> offset_bit;
        } else {
            *(u32 *)tmpbuf = ((*(u32 *)tmpbuf) & (u32)l_bus_opt->mask) >> offset_bit;
        }
    }

    /* Copy the data in the local bus to the user. */
    if (mode == L_BUS_USER_MODE) {
        if (copy_to_user(l_bus_opt->data, tmpbuf, l_bus_opt->len)) {
            LOG(LOG_ERROR, "ERROR copy_to_user");
            ret = -EFAULT;
        }
    } else {
        if (memcpy_s(l_bus_opt->data, l_bus_opt->len, tmpbuf, l_bus_opt->len)) {
            LOG(LOG_ERROR, "memcpy_s failed");
            ret = -EFAULT;
        }
    }

EXIT:
    up(&g_localbus_mgr->localbus_access_sem[l_bus_opt->cs]);

    if (tmpbuf != NULL) {
        kfree(tmpbuf);
    }

    return ret;
}
EXPORT_SYMBOL(localbus_get_data);

static void localbus_prepare_data_according_type(u32 cs, u32 type, const u8 *buf, u32 mask, const void __iomem *offset)
{
    u8 reg_value8;
    u16 reg_value16;
    u32 reg_value32;
    u32 offset_bit = 0;

    if (type != OP_TYPE_BITS) {
        return;
    }

    localbus_get_offset_bit(mask, &offset_bit);
    if (g_localbus_mgr->width[cs] == CS_WIDTH_8) {
        reg_value8 = (u8)ioread8(offset);
        reg_value8 = reg_value8 & (~(u8)mask);

        *(u8 *)buf = (((*(u8 *)buf) << offset_bit) & (u8)mask) | reg_value8;
    } else if (g_localbus_mgr->width[cs] == CS_WIDTH_16) {
        reg_value16 = (u16)ioread16(offset);
        reg_value16 = reg_value16 & (~(u16)mask);

        *(u16 *)buf = (((*(u16 *)buf) << offset_bit) & (u16)mask) | reg_value16;
    } else {
        reg_value32 = ioread32(offset);
        reg_value32 = reg_value32 & (~(u32)mask);

        *(u32 *)buf = (((*(u32 *)buf) << offset_bit) & (u32)mask) | reg_value32;
    }

    return;
}

s32 localbus_write_data(l_bus_opt_s *l_bus_opt, s32 mode)
{
    void __iomem *offset;
    s32 ret;
    u8 *tmpbuf = NULL;

    ret = localbus_para_check(l_bus_opt, mode);
    if (ret != 0) {
        LOG(LOG_ERROR, "localbus_para_check err");
        return ret;
    }

    offset = g_localbus_mgr->cs_io_addr[l_bus_opt->cs] + l_bus_opt->offset;
    ret = down_interruptible(&g_localbus_mgr->localbus_access_sem[l_bus_opt->cs]);
    if (ret) {
        LOG(LOG_ERROR, "ERROR down_interruptible");
        goto EXIT0;
    }

    tmpbuf = (u8 *)kzalloc(l_bus_opt->len, GFP_KERNEL);
    if (!tmpbuf) {
        LOG(LOG_ERROR, "kzalloc err");
        ret = -ENOMEM;
        goto EXIT1;
    }

    if (mode == L_BUS_USER_MODE) {
        if (copy_from_user(tmpbuf, l_bus_opt->data, l_bus_opt->len)) {
            LOG(LOG_ERROR, "copy_from_user err");
            ret = -EFAULT;
            goto EXIT1;
        }
    } else {
        if (memcpy_s(tmpbuf, l_bus_opt->len, l_bus_opt->data, l_bus_opt->len)) {
            LOG(LOG_ERROR, "memcpy_s err");
            ret = -EFAULT;
            goto EXIT1;
        }
    }

    localbus_prepare_data_according_type(l_bus_opt->cs, l_bus_opt->type, tmpbuf, l_bus_opt->mask, offset);
    localbus_write_according_width(l_bus_opt->cs, l_bus_opt->len, tmpbuf, offset);

EXIT1:
    up(&g_localbus_mgr->localbus_access_sem[l_bus_opt->cs]);

EXIT0:
    if (tmpbuf != NULL) {
        kfree(tmpbuf);
    }

    return ret;
}
EXPORT_SYMBOL(localbus_write_data);

static s32 localbus_set_timing(u32 offset, u32 mask, u32 val)
{
    u32 value;
    u32 debug2;

    if ((offset > LB_CS3_T_SET2_OFFSET_ADDR) || (offset < LB_CS0_T_SET0_OFFSET_ADDR)) {
        LOG(LOG_ERROR, "offset(%u) is out of range(0x40~0x6c)", offset);
        return -EINVAL;
    }

    iowrite32(0, g_localbus_mgr->regbase + LB_CFG_T_UPDATE_EN_OFFSET_ADDR);
    value = ioread32(g_localbus_mgr->regbase + offset);
    value &= ~mask;
    value |= (val & mask);
    iowrite32(value, g_localbus_mgr->regbase + offset);
    debug2 = ioread32(g_localbus_mgr->regbase + LB_DBG_T_CFG_OFFSET_ADDR);
    if (!debug2) {
        iowrite32(1, g_localbus_mgr->regbase + LB_CFG_T_UPDATE_EN_OFFSET_ADDR);
    } else {
        LOG(LOG_ERROR, "set addr to data num error, (debug2(%u)), using default value", debug2);
        return -EAGAIN;
    }

    return 0;
}

static s32 localbus_cmd_rw_cpld(u32 cmd, unsigned long arg)
{
    s32 ret;
    l_bus_opt_s l_bus_opt;

    if (copy_from_user(&l_bus_opt, (void *)arg, sizeof(l_bus_opt_s))) {
        LOG(LOG_ERROR, "copy_from_user err");
        return -EFAULT;
    }

    switch (cmd) {
        case L_BUS_CMD_READ:
            ret = localbus_get_data(&l_bus_opt, L_BUS_USER_MODE);
            break;
        case L_BUS_CMD_WRITE:
            ret = localbus_write_data(&l_bus_opt, L_BUS_USER_MODE);
            break;
        default:
            LOG(LOG_ERROR, "cmd invalid fail cmd = 0x%x", cmd);
            return -EFAULT;
    }

    return ret;
}

static s32 localbus_analysis_timing_para(localbus_timing_s *para, localbus_reg_wr_s *wr_str)
{
    if ((para == NULL) || (wr_str == NULL)) {
        LOG(LOG_ERROR, "timing para or reg structure can not be NULL");
        return -EINVAL;
    }

    if (para->cs >= CS_NUM) {
        LOG(LOG_ERROR, "cs should not be greater than 3");
        return -EINVAL;
    }

    switch (para->type) {
        case L_BUS_LALE:
            if (para->value > 0x1F) {
                LOG(LOG_ERROR, "[LALE] value should not be greater than 0x1f");
                return -EINVAL;
            }
            wr_str->offset = para->cs * 12 + LB_CS0_T_SET1_OFFSET_ADDR; // mul 12
            wr_str->mask = (0x1F << ALE_BIT);
            wr_str->value = ((u32)para->value << ALE_BIT);
            break;
        case L_BUS_ADDR2DATA_NUM:
            if (para->value > 0x7) {
                LOG(LOG_ERROR, "[ADDR2DATA_NUM] value should not be greater than 0x7");
                return -EINVAL;
            }
            wr_str->offset = para->cs * 12 + LB_CS0_T_SET2_OFFSET_ADDR; // mul 12
            wr_str->mask = ((u32)0x7 << ADDR2DATA_BIT);
            wr_str->value = ((u32)para->value << ADDR2DATA_BIT);
            break;
        default:
            LOG(LOG_ERROR, "timing para type is out of range:%u", para->type);
            return -EINVAL;
    }

    return 0;
}

static s32 localbus_cmd_set_timing(u32 cmd, unsigned long arg)
{
    localbus_timing_s timing_para;
    localbus_reg_wr_s timing_reg;
    s32 ret;

    if (copy_from_user(&timing_para, (void *)arg, sizeof(localbus_timing_s))) {
        LOG(LOG_ERROR, "copy_from_user err");
        return -EFAULT;
    }

    ret = localbus_analysis_timing_para(&timing_para, &timing_reg);
    if (ret) {
        LOG(LOG_ERROR, "timing parameter analysis fail(%d)", ret);
        return ret;
    }

    return localbus_set_timing(timing_reg.offset, timing_reg.mask, timing_reg.value);
}

static s32 localbus_cmd_set_bitwide(u32 cmd, unsigned long arg)
{
    localbus_set_bitwide_s bitwide_para;

    if (copy_from_user(&bitwide_para, (void *)arg, sizeof(localbus_set_bitwide_s))) {
        LOG(LOG_ERROR, "copy_from_user err");
        return -EFAULT;
    }

    /* Set the bit width and offset as required. */
    return localbus_set_cs_width_offset(bitwide_para.cs, bitwide_para.width, bitwide_para.cf_addr_offset);
}

static s32 localbus_open(struct inode *pnode, struct file *pfile)
{
    return 0;
}

static s32 localbus_release(struct inode *pnode, struct file *pfile)
{
    return 0;
}

static long localbus_ioctl(struct file *pfile, u32 cmd, unsigned long arg)
{
    s32 ret = 0;

    switch (cmd) {
        case L_BUS_CMD_READ:
        case L_BUS_CMD_WRITE:
            ret = localbus_cmd_rw_cpld(cmd, arg);
            break;
        case L_BUS_CMD_SET_TIMING:
            ret = localbus_cmd_set_timing(cmd, arg);
            break;
        case L_BUS_CMD_SET_WIDTH_OFFSET:
            ret = localbus_cmd_set_bitwide(cmd, arg);
            break;
        default:
            ret = -EINVAL;
    }

    return ret;
}

const struct file_operations g_localbus_fops = {
    .owner = THIS_MODULE,
    .open = localbus_open,
    .release = localbus_release,
    .unlocked_ioctl = localbus_ioctl
};

static void localbus_cs_iounmap(void)
{
    u32 i;

    for (i = 0; i < CS_NUM; i++) {
        if (g_localbus_mgr->cs_io_addr[i] != NULL) {
            iounmap(g_localbus_mgr->cs_io_addr[i]);
            g_localbus_mgr->cs_io_addr[i] = NULL;
        }
    }

    return;
}

static void localbus_board_adapter(void)
{
    u32 plat_id;
    u_lb_cs1_t_set1 lb_t_set;
    u32 debug2;

    lb_t_set.val = 0;
    plat_id = bdinfo_get_platform_id();
    if (plat_id == BOARD_PLATM_PANGEA_V6) {
        iowrite32(0, g_localbus_mgr->regbase + LB_CFG_T_UPDATE_EN_OFFSET_ADDR);
        lb_t_set.val = ioread32(g_localbus_mgr->regbase + LB_CS1_T_SET1_OFFSET_ADDR);
        lb_t_set.bits.cf_t_lale_1 = 0x14;
        iowrite32(lb_t_set.val, g_localbus_mgr->regbase + LB_CS1_T_SET1_OFFSET_ADDR);
        debug2 = ioread32(g_localbus_mgr->regbase + LB_DBG_T_CFG_OFFSET_ADDR);
        if (!debug2) {
            iowrite32(1, g_localbus_mgr->regbase + LB_CFG_T_UPDATE_EN_OFFSET_ADDR);
        } else {
            LOG(LOG_ERROR, "set ALE error, (debug2(%u)), using default value", debug2);
        }
    }

    return;
}

static s32 localbus_irq_init(struct device_node *node)
{
    s32 ret;

    if (!node) {
        LOG(LOG_ERROR, "node is invalid");
        return -EINVAL;
    }

    g_localbus_mgr->localbus_virq = irq_of_parse_and_map(node, 0);
    if (g_localbus_mgr->localbus_virq == 0) {
        LOG(LOG_ERROR, "get irq and map error");
        return -EINVAL;
    }

    ret = request_irq(g_localbus_mgr->localbus_virq, localbus_interrupt_handle, 0, "localbus", NULL);
    if (ret) {
        g_localbus_mgr->localbus_virq = 0;
        LOG(LOG_ERROR, "localbus's irq(0x%x) already in use", g_localbus_mgr->localbus_virq);
        return ret;
    }

    return 0;
}

static s32 localbus_probe(struct platform_device *pdev)
{
    s32 ret;
    u32 i;
    struct device_node *node = pdev->dev.of_node;

    g_localbus_mgr = (localbus_mgr_s *)kzalloc(sizeof(localbus_mgr_s), GFP_KERNEL);
    if (g_localbus_mgr == NULL) {
        LOG(LOG_ERROR, "g_localbus_mgr err!");
        return -ENOMEM;
    }

    ret = comm_init_dev(&g_localbus_mgr->localbus_dev, &g_localbus_fops, DEV_L_BUS_NAME);
    if (ret != 0) {
        LOG(LOG_ERROR, "comm_init_dev err");
        goto ERR_EXIT0;
    }

    g_localbus_mgr->regbase = of_iomap(node, 0);
    if (g_localbus_mgr->regbase == NULL) {
        LOG(LOG_ERROR, "error l_bus_io_addr");
        goto ERR_EXIT1;
    }

    ret = localbus_set_cs_width_offset(1, CS_WIDTH_8, CF_ADDR_OFFSET0);
    if (ret) {
        goto MAP_ERR;
    }

    localbus_board_adapter();

    ret = localbus_mgr_width_base_set();
    if (ret) {
        goto MAP_ERR;
    }

    for (i = 0; i < CS_NUM; i++) {
        init_mutex(&(g_localbus_mgr->localbus_access_sem[i]));
    }

    ret = localbus_irq_init(node);
    if (ret) {
        LOG(LOG_ERROR, "localbus init irq failed(%d)", ret);
        goto MAP_ERR;
    }

    localbus_irq_enable(ENABLE);

    return 0;

MAP_ERR:
    localbus_cs_iounmap();
    iounmap(g_localbus_mgr->regbase);

ERR_EXIT1:
    comm_cleanup_dev(&g_localbus_mgr->localbus_dev);

ERR_EXIT0:
    kfree(g_localbus_mgr);
    g_localbus_mgr = NULL;

    return -ENOMEM;
}

static s32 localbus_remove(struct platform_device *pdev)
{
    if (!g_localbus_mgr) {
        return 0;
    }

    free_irq(g_localbus_mgr->localbus_virq, NULL);
    g_localbus_mgr->localbus_virq = 0;

    localbus_cs_iounmap();

    if (&g_localbus_mgr->localbus_dev != NULL) {
        comm_cleanup_dev(&g_localbus_mgr->localbus_dev);
    }

    if (g_localbus_mgr->regbase) {
        iounmap(g_localbus_mgr->regbase);
    }

    kfree(g_localbus_mgr);
    g_localbus_mgr = NULL;

    return 0;
}

static const struct of_device_id g_localbus_match[] = {
    { .compatible = "hisilicon, hi1711-localbus" }, {}
};

static struct platform_driver g_localbus_driver = {
    .driver = {
        .name = "localbus",
        .of_match_table = g_localbus_match,
    },

    .probe = localbus_probe,
    .remove = localbus_remove,
};
module_platform_driver(g_localbus_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("l_bus driver");