// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Analog-to-Digital Converter(ADC) driver
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/spinlock.h>
#include <linux/proc_fs.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>

#include "lp_subsys_csr_addr_define.h"
#include "lp_subsys.h"
#include "lp_subsys_c_union_define.h"
#include "comm_knl_api.h"
#include "adc_usr_api.h"
#include "adc_ker.h"

static struct adc_host *g_adc_mgr = NULL;

void adc_ioremap_exit(void)
{
    u32 i;

    for (i = 0; i < ADC_GROUP_MAX; i++) {
        if (g_adc_mgr->regbase[i]) {
            iounmap(g_adc_mgr->regbase[i]);
            g_adc_mgr->regbase[i] = NULL;
        }
    }

    return;
}

s32 adc_ioremap(struct device_node *node)
{
    u32 i;

    for (i = 0; i < ADC_GROUP_MAX; i++) {
        g_adc_mgr->regbase[i] = of_iomap(node, i);
        if (!g_adc_mgr->regbase[i]) {
            LOG(LOG_ERROR, "adc%u ioremap failed!", i);
            goto ERR_EXIT;
        }
    }

    return 0;

ERR_EXIT:
    adc_ioremap_exit();
    return -EIO;
}

static irqreturn_t adc_irq_handler(s32 irq, void *arg)
{
    unsigned long flags = 0;

    u32 adc_id = *((u32 *)arg);

    /* protect g_adc_mgr->g_alarm_msg without sleeping. */
    spin_lock_irqsave(&g_adc_mgr->adc_lock, flags);

    g_adc_mgr->g_alarm_msg[adc_id] = readl(&g_adc_mgr->regbase[adc_id]->adc_irq_st);
    if (g_adc_mgr->g_alarm_msg[adc_id]) {
        /* clear interrupts. */
        writel(g_adc_mgr->g_alarm_msg[adc_id], &g_adc_mgr->regbase[adc_id]->adc_irq_st);
    }

    /* disable interrupts, as voltage exceeding a certain value may lead to a persistent high level. */
    writel(ADC_IRQ_DISABLE, &g_adc_mgr->regbase[adc_id]->adc_irq_en);

    spin_unlock_irqrestore(&g_adc_mgr->adc_lock, flags);

    return IRQ_HANDLED;
}

s32 adc_init_irq(struct device_node *node)
{
    s32 ret;
    u32 id;

    /* assign interrupt names to ADC0 and ADC1. */
    g_adc_mgr->adc_irq_name[0] = "ADC0";
    g_adc_mgr->adc_irq_name[1] = "ADC1";

    for (id = 0; id < ADC_GROUP_MAX; id++) {
        g_adc_mgr->irq_flag[id] = id;
        g_adc_mgr->adc_virq[id] = irq_of_parse_and_map(node, id);
        if (!g_adc_mgr->adc_virq[id]) {
            LOG(LOG_ERROR, "irq of parse map fail %u", id);
            ret = -EINVAL;
            goto ERR_EXIT;
        }

        ret = request_irq(g_adc_mgr->adc_virq[id], adc_irq_handler, 0, g_adc_mgr->adc_irq_name[id],
                          &g_adc_mgr->irq_flag[id]);
        if (ret) {
            g_adc_mgr->adc_virq[id] = 0;
            LOG(LOG_ERROR, "can't get assigned irq ADC%u, Return code is %d", id, ret);
            goto ERR_EXIT;
        }
    }

    return 0;

ERR_EXIT:
    while (id) {
        id--;
        free_irq(g_adc_mgr->adc_virq[id], &g_adc_mgr->irq_flag[id]);
        g_adc_mgr->adc_virq[id] = 0;
    }

    return ret;
}

static void adc_alarm_config(const struct adc_alarm *alarm)
{
    u32 tmp;
    u32 adc_id;
    u32 chan_id;

    /* ADC1 channels 0 to 7 correspond to user input channels 8 to 15. */
    adc_id = alarm->chan_id / ADC_CTRL_CH_NUM;
    chan_id = alarm->chan_id % ADC_CTRL_CH_NUM;

    tmp = (alarm->top_limit) & (ADC_ALARM_TOP_MASK);
    tmp |= (alarm->lower_limit) << ADC_ALARM_LOWER_SHIFT;

    /* configure upper and lower limits. */
    writel(tmp, g_adc_mgr->regbase[adc_id]->adc_alarm_regs + chan_id);
    writel(0xff, &g_adc_mgr->regbase[adc_id]->adc_irq_st);

    /* clear the previous alarm information. */
    g_adc_mgr->g_alarm_msg[adc_id] = 0;

    /* enable interrupts after configuring upper and lower limits. */
    writel(ADC_IRQ_ENABLE, &g_adc_mgr->regbase[adc_id]->adc_irq_en);

    return;
}

/**
 * @brief: ADC configuration function, including reference voltage source (external or internal),
 *         reference voltage value, and scan rate.
 *                                   adc_mode_config
 *                                   /             \
 *                            EXTERNAL_VOLT    INTERNAL_VOLT
 *                            /           \              /
 *                       scan rate  reference voltage scan rate
 * @param: struct adc_config *config
 * @retval: 0 is success, other is failure
 */
static s32 adc_mode_config(struct adc_config *config)
{
    s32 ret;
    u_lpsc_adc_ctrl adc_ctrl;

    if ((config->voltage_ref_source != INTERNAL_VOLT) && (config->voltage_ref_source != EXTERNAL_VOLT)) {
        LOG(LOG_ERROR, "err voltage source");
        return -EINVAL;
    }
    /* disable ADC1 control. */
    writel(ADC_DISABLE, &g_adc_mgr->regbase[1]->adc_ctrl);
    /* disable ADC0 control. */
    writel(ADC_DISABLE, &g_adc_mgr->regbase[0]->adc_ctrl);

    /* disable ADC0&ADC1. */
    (void)lp_subsys_reg_read(LP_SUBSYS_CSR_LPSC_ADC_CTRL_OFFSET_ADDR, &adc_ctrl.val);
    adc_ctrl.bits.adc0_enadc = ADC_DISABLE;
    adc_ctrl.bits.adc1_enadc = ADC_DISABLE;
    (void)lp_subsys_reg_write(LP_SUBSYS_CSR_LPSC_ADC_CTRL_OFFSET_ADDR, adc_ctrl.val);
    (void)lp_subsys_reg_read(LP_SUBSYS_CSR_LPSC_ADC_CTRL_OFFSET_ADDR, &adc_ctrl.val);

    /* configure voltage reference mode. */
    if (config->voltage_ref_source == EXTERNAL_VOLT) {
        g_adc_mgr->ref_voltage = ADC_EXTERNEL_REF;
        adc_ctrl.bits.adc0_selbg = ADC_EXTER_VOLTAGE;
        adc_ctrl.bits.adc1_selbg = ADC_EXTER_VOLTAGE;
        adc_ctrl.bits.adc0_selref = ADC_EXTER_VOLTAGE;
        adc_ctrl.bits.adc1_selref = ADC_EXTER_VOLTAGE;
    } else {
        g_adc_mgr->ref_voltage = ADC_INTERAL_REF;
        adc_ctrl.bits.adc0_selbg = ADC_INTER_VOLTAGE;
        adc_ctrl.bits.adc1_selbg = ADC_INTER_VOLTAGE;
        adc_ctrl.bits.adc0_selref = ADC_INTER_VOLTAGE;
        adc_ctrl.bits.adc1_selref = ADC_INTER_VOLTAGE;
    }

    adc_ctrl.bits.adc0_enadc = ADC_ENABLE;
    adc_ctrl.bits.adc1_enadc = ADC_ENABLE;

    (void)lp_subsys_reg_write(LP_SUBSYS_CSR_LPSC_ADC_CTRL_OFFSET_ADDR, adc_ctrl.val);

    /* delay 2us. */
    udelay(2);

    /* reset ADC0. */
    ret = m_reset_handle_atomic(LP_SUBSYS_CSR_LPSC_SRST_REQ1_OFFSET_ADDR, MODULE_RST_MASK_ADC0);
    if (ret) {
        LOG(LOG_ERROR, "adc0 reset failed!");
        return ret;
    }
    /* reset ADC1. */
    ret = m_reset_handle_atomic(LP_SUBSYS_CSR_LPSC_SRST_REQ2_OFFSET_ADDR, MODULE_RST_MASK_ADC1);
    if (ret) {
        LOG(LOG_ERROR, "adc1 reset failed!");
        return ret;
    }

    /* configure scan period. */
    writel(config->scan_rate, &g_adc_mgr->regbase[0]->adc_scan_rate);
    writel(config->scan_rate, &g_adc_mgr->regbase[1]->adc_scan_rate);

    /* enable ADC1 control. */
    writel(ADC_ENABLE, &g_adc_mgr->regbase[1]->adc_ctrl);
    /* enable ADC0 control. */
    writel(ADC_ENABLE, &g_adc_mgr->regbase[0]->adc_ctrl);

    return ret;
}

static void adc_voltage_get(struct adc_voltage * const voltage)
{
    u32 channel_reg;
    u32 channel_id;
    u32 channel_offset;
    u32 tmp_val;
    u32 adc_id;

    /* ADC1 channels 0 to 7 correspond to user input channels 8 to 15. */
    adc_id = voltage->chan_id / ADC_CTRL_CH_NUM;
    channel_id = voltage->chan_id % ADC_CTRL_CH_NUM;

    channel_reg = channel_id / ADC_VOL_REG_DIVISION;
    channel_offset = channel_id % ADC_VOL_REG_DIVISION;
    tmp_val = readl(g_adc_mgr->regbase[adc_id]->adc_voltage_regs + channel_reg);
    tmp_val = (tmp_val >> (channel_offset * ADC_VOL_REG_SHIFT));

    voltage->voltage_val = (ADC_VOLTAGE_CHANNEL_MASK) & (tmp_val);
    voltage->voltage_val = (voltage->voltage_val) * (g_adc_mgr->ref_voltage) / ADC_12BIT_BASE_NUM;

    return;
}

static s32 adc_cmd_alarm_config(unsigned long arg)
{
    s32 status;
    unsigned long lockflags = 0;
    struct adc_alarm alarm = { 0 };

    status = copy_from_user(&alarm, (struct adc_alarm *)arg, sizeof(struct adc_alarm));
    if (status) {
        LOG(LOG_ERROR, "error: copy alarm data from user failed");
        return -EAGAIN;
    }

    if (alarm.chan_id >= ADC_CHANNEL_NUM) {
        LOG(LOG_ERROR, "id must smaller than 15，only 16 channels");
        return -EFAULT;
    }

    if (alarm.top_limit <= alarm.lower_limit) {
        LOG(LOG_ERROR, "top_limit must greater than lower_limit");
        return -EFAULT;
    }

    if (alarm.top_limit > ADC_ALARM_TOP_MASK) {
        LOG(LOG_ERROR, "top_limit must lower than 0xfff");
        return -EFAULT;
    }

    spin_lock_irqsave(&g_adc_mgr->adc_lock, lockflags);
    adc_alarm_config(&alarm);
    spin_unlock_irqrestore(&g_adc_mgr->adc_lock, lockflags);

    return 0;
}

static s32 adc_cmd_config(unsigned long arg)
{
    s32 status;
    unsigned long lockflags = 0;
    struct adc_config config = { 0 };

    status = copy_from_user(&config, (struct adc_config *)arg, sizeof(struct adc_config));
    if (status) {
        LOG(LOG_ERROR, "error: copy config data from user failed");
        return -EAGAIN;
    }

    if ((config.voltage_ref_source != ADC_INTER_VOLTAGE) && (config.voltage_ref_source != ADC_EXTER_VOLTAGE)) {
        LOG(LOG_ERROR, "voltage_ref_source must be 1(inner ref sorurce) or 0(external ref source)");
        return -EFAULT;
    }

    if ((config.voltage_ref_source == ADC_INTER_VOLTAGE) && (config.ref_voltage != ADC_INTERAL_REF)) {
        LOG(LOG_ERROR, "inner voltage_ref_source must be 1.2V, force config.ref_voltage to 1.2V");
        return -EFAULT;
    }
    /* The scan rate register size is limited to 0xFFFFFF, writing a larger value will result in data loss.
     * The driver needs to include this limitation.
     */
    if ((config.scan_rate == 0) || (config.scan_rate > ADC_SCAN_RATE_LIMITIED)) {
        LOG(LOG_ERROR, "scan_rate must greater than 0 & lower than 0xffffff");
        return -EFAULT;
    }

    spin_lock_irqsave(&g_adc_mgr->adc_lock, lockflags);
    status = adc_mode_config(&config);
    spin_unlock_irqrestore(&g_adc_mgr->adc_lock, lockflags);

    return status;
}

static s32 adc_cmd_get_alarm_msg(unsigned long arg)
{
    u32 alarm_msg;
    s32 status;
    unsigned long lockflags = 0;

    /* Alarm_msg represents the interrupt status, where bit[16] indicates the presence of an interrupt,
     * and bit[0-15] indicates which channel has an interrupt.
     * The interrupt status for channels 8~15 is represented by ADC1, channels 0~7 is represented by ADC0.
     * Bit 8 of ADC0 and ADC1 collectively indicate whether an interrupt has occurred
     */
    alarm_msg = (((((g_adc_mgr->g_alarm_msg[1]) & 0x100) | ((g_adc_mgr->g_alarm_msg[0]) & 0x100)) << ADC_CTRL_CH_NUM) |
                 (((g_adc_mgr->g_alarm_msg[1]) & 0xFF) << ADC_CTRL_CH_NUM) | ((g_adc_mgr->g_alarm_msg[0]) & 0xFF));

    status = copy_to_user((u32 *)(uintptr_t)arg, &alarm_msg, sizeof(u32));
    if (status) {
        LOG(LOG_ERROR, "error copying chan config to user space %d", status);
        status = -EAGAIN;
    }
    LOG(LOG_INFO, "g_alarm_msg = 0x%x", alarm_msg);

    spin_lock_irqsave(&g_adc_mgr->adc_lock, lockflags);
    g_adc_mgr->g_alarm_msg[0] = 0;
    g_adc_mgr->g_alarm_msg[1] = 0;
    spin_unlock_irqrestore(&g_adc_mgr->adc_lock, lockflags);

    return status;
}

static s32 adc_cmd_get_voltage(unsigned long arg)
{
    s32 status;
    unsigned long lockflags = 0;
    struct adc_voltage voltage = { 0 };

    status = copy_from_user(&voltage, (struct adc_voltage *)arg, sizeof(struct adc_voltage));
    if (status) {
        LOG(LOG_ERROR, "error chan copying data from user space");
        return -EAGAIN;
    }

    if (voltage.chan_id >= ADC_CHANNEL_NUM) {
        LOG(LOG_ERROR, "arg err,chan_id(%d) must be 0 ~ 15", voltage.chan_id);
        return -EINVAL;
    }

    spin_lock_irqsave(&g_adc_mgr->adc_lock, lockflags);
    adc_voltage_get(&voltage);
    spin_unlock_irqrestore(&g_adc_mgr->adc_lock, lockflags);

    status = copy_to_user((struct adc_voltage *)arg, &voltage, sizeof(struct adc_voltage));
    if (status) {
        LOG(LOG_ERROR, "error copying chan config to user space %d", status);
        status = -EAGAIN;
    }

    return status;
}

static long adc_ioctl(struct file *files, u32 cmd, unsigned long arg)
{
    s32 status;

    /* ADC command parameter check. */
    status = comm_ioc_parm_check(cmd, (void __user *)arg, ADC_IOC_MAGIC, ADC_IOC_MAXNR);
    if (status) {
        LOG(LOG_ERROR, "adc cmd(0x%x) ioc parm check failed!", cmd);
        return -EFAULT;
    }

    switch (cmd) {
        case ADC_CMD_ALARM_CONFIG:
            status = adc_cmd_alarm_config(arg);
            break;

        case ADC_CMD_CONFIG:
            status = adc_cmd_config(arg);
            break;

        case ADC_CMD_GET_ALARM_MSG:
            status = adc_cmd_get_alarm_msg(arg);
            break;

        case ADC_CMD_VOLTAGE_GET:
            status = adc_cmd_get_voltage(arg);
            break;

        default:
            return -EINVAL;
    }

    return status;
}

static const struct file_operations g_adc_fops = {
    .unlocked_ioctl = adc_ioctl,
    .owner = THIS_MODULE,
};

s32 adc_module_init(void)
{
    s32 ret;
    u_lpsc_adc_ctrl adc_ctrl;

    /* configure reference voltage mode and resolution, default to using internal voltage. */
    adc_ctrl.val = 0;
    adc_ctrl.bits.adc0_selbg = ADC_INTER_VOLTAGE;
    adc_ctrl.bits.adc0_selref = ADC_INTER_VOLTAGE;
    adc_ctrl.bits.adc1_selbg = ADC_INTER_VOLTAGE;
    adc_ctrl.bits.adc1_selref = ADC_INTER_VOLTAGE;
    adc_ctrl.bits.adc0_selres = ADC_12BIT_RES;  /* default to using 12-bit resolution mode. */
    adc_ctrl.bits.adc1_selres = ADC_12BIT_RES;
    adc_ctrl.bits.adc0_enadc = ADC_ENABLE;
    adc_ctrl.bits.adc1_enadc = ADC_ENABLE;

    ret = lp_subsys_reg_write(LP_SUBSYS_CSR_LPSC_ADC_CTRL_OFFSET_ADDR, adc_ctrl.val);
    if (ret) {
        LOG(LOG_ERROR, "adc module init failed!");
        return ret;
    }

    udelay(2);

    ret = m_reset_handle(LP_SUBSYS_CSR_LPSC_SRST_REQ1_OFFSET_ADDR, MODULE_RST_MASK_ADC0);
    if (ret) {
        LOG(LOG_ERROR, "adc0 module reset failed!");
        return ret;
    }

    ret = m_reset_handle(LP_SUBSYS_CSR_LPSC_SRST_REQ2_OFFSET_ADDR, MODULE_RST_MASK_ADC1);
    if (ret) {
        LOG(LOG_ERROR, "adc1 module reset failed!");
        return ret;
    }

    writel(ADC_ENABLE, &g_adc_mgr->regbase[1]->adc_ctrl);
    writel(ADC_ENABLE, &g_adc_mgr->regbase[0]->adc_ctrl);

    return ret;
}

static s32 adc_probe(struct platform_device *pdev)
{
    s32 ret;
    struct device_node *node = pdev->dev.of_node;

    g_adc_mgr = (struct adc_host *)kzalloc(sizeof(struct adc_host), GFP_KERNEL);
    if (!g_adc_mgr) {
        LOG(LOG_ERROR, "adc_host kzalloc err");
        return -ENOMEM;
    }

    ret = comm_init_dev(&g_adc_mgr->adc_dev, &g_adc_fops, DEV_ADC_NAME);
    if (ret) {
        kfree(g_adc_mgr);
        g_adc_mgr = NULL;
        LOG(LOG_ERROR, "adc dev init failed!");
        return ret;
    }

    spin_lock_init(&g_adc_mgr->adc_lock);

    /* address mapping. */
    ret = adc_ioremap(node);
    if (ret) {
        goto ERR_EXIT;
    }

    /* default to internal reference voltage of 1.2V. */
    g_adc_mgr->ref_voltage = ADC_INTERAL_REF;

    ret = adc_module_init();
    if (ret) {
        goto ERR_EXIT;
    }

    /* interrupt initialization. */
    ret = adc_init_irq(node);
    if (ret) {
        goto ERR_EXIT;
    }

    return ret;

ERR_EXIT:
    adc_ioremap_exit();
    comm_cleanup_dev(&g_adc_mgr->adc_dev);

    if (g_adc_mgr) {
        kfree(g_adc_mgr);
        g_adc_mgr = NULL;
    }

    return ret;
}

static s32 adc_remove(struct platform_device *pdev)
{
    u32 i;

    if (g_adc_mgr) {
        /* release interrupt handler. */
        for (i = 0; i < ADC_GROUP_MAX; i++) {
            free_irq(g_adc_mgr->adc_virq[i], &g_adc_mgr->irq_flag[i]);
            g_adc_mgr->adc_virq[i] = 0;
        }

        adc_ioremap_exit();

        /* character device cleanup. */
        comm_cleanup_dev(&g_adc_mgr->adc_dev);

        kfree(g_adc_mgr);
        g_adc_mgr = NULL;
    }

    return 0;
}

static const struct of_device_id g_adc_match[] = {
    { .compatible = "hisilicon, hi1711-adc" }, {}
};

static struct platform_driver g_adc_driver = {
    .driver = {
        .name = DEV_ADC_NAME,
        .of_match_table = g_adc_match,
    },

    .probe = adc_probe,
    .remove = adc_remove,
};
module_platform_driver(g_adc_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("adc driver");
