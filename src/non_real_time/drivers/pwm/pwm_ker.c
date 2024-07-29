// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * pwm module sdk interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <asm/io.h>
#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/sysrq.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/slab.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/interrupt.h>
#include <linux/interrupt.h>

#include "pwm_csr_addr_define.h"
#include "comm_knl_api.h"
#include "pwm_usr_api.h"
#include "lp_subsys_csr_addr_define.h"
#include "lp_subsys.h"
#include "bdinfo.h"
#include "board_info.h"
#include "pwm_ker.h"

static fan_pwm_mgr *g_fan_pwm_mgr = NULL;

static u32 g_pwm_pad_cfg[PWM_MAX_CHANNEL] = { 11, 12, 13, 14, 15, 16, 17, 18, 19, 5, 20, 21 };
static u32 g_fan_pad_cfg[FAN_MAX_CAHNNEL] = { 0, 1, 2, 3, 4, 6, 7, 8, 9, 10, 22, 23, 24, 25, 26, 27 };

static s32 fan_pwm_mgr_pad_modify(u32 *fan_pad, u32 *pwm_pad)
{
    u32 i;

    for (i = 0; i < PWM_MAX_CHANNEL; i++) {
        g_fan_pwm_mgr->pwm_fan_pad.pwm_pad[i] = pwm_pad[i];
    }

    for (i = 0; i < FAN_MAX_CAHNNEL; i++) {
        g_fan_pwm_mgr->pwm_fan_pad.fan_pad[i] = fan_pad[i];
    }

    return 0;
}

static s32 pwm_fan_pad_check_connect(u32 pad_num, u32 *connect_info)
{
    u32 connect_temp;

    if ((pad_num >= (FAN_MAX_CAHNNEL + PWM_MAX_CHANNEL)) || (connect_info == NULL)) {
        LOG(LOG_ERROR, "pad 0x%x is out of range or para err", pad_num);
        return -EINVAL;
    }
    connect_temp = *connect_info;

    if (connect_temp & (1u << pad_num)) {
        LOG(LOG_ERROR, "pad 0x%x is repeat", pad_num);
        return -EFAULT;
    }
    connect_temp |= (1u << pad_num);
    *connect_info = connect_temp;

    return 0;
}

static s32 pwm_fan_check_pad_cfg(u32 *fan_pad, u32 *pwm_pad)
{
    s32 ret;
    u32 fan_pwm_pad_connect = 0;
    u32 i;

    for (i = 0; i < FAN_MAX_CAHNNEL; i++) {
        ret = pwm_fan_pad_check_connect(fan_pad[i], &fan_pwm_pad_connect);
        if (ret) {
            LOG(LOG_ERROR, "fan 0x%x pad check err", i);
            return ret;
        }
    }

    for (i = 0; i < PWM_MAX_CHANNEL; i++) {
        ret = pwm_fan_pad_check_connect(pwm_pad[i], &fan_pwm_pad_connect);
        if (ret) {
            LOG(LOG_ERROR, "pwm 0x%x pad check err", i);
            return ret;
        }
    }

    if (fan_pwm_pad_connect != PWM_FAN_PAD_CONNECT_FULL) {
        LOG(LOG_ERROR, "connect info 0x%x not right", fan_pwm_pad_connect);
        return -EINVAL;
    }

    return ret;
}

static s32 fan_pwm_mgr_check_init(u32 *fan_pad, u32 *pwm_pad)
{
    u32 board_id;
    u32 plat_id;
    u32 board_type;

    board_id = bdinfo_get_board_id();
    plat_id = bdinfo_get_platform_id();
    board_type = bdinfo_get_board_type();

    g_fan_pwm_mgr->module_id[0] = PWM_FAN_MODULE0;
    g_fan_pwm_mgr->module_id[1] = PWM_FAN_MODULE1;

    if ((plat_id == BOARD_PLATM_TAISHAN) && (board_id == TAISHAN_BOARD_ID_2480) && (board_type == BOARD_TYPE_HOST)) {
        pwm_pad[7] = 22; // pwm 7 to pad 22
        pwm_pad[8] = 23; // pwm 8 to pad 23
        fan_pad[10] = 18; // fan 10 to pad 18
        fan_pad[11] = 19; // fan 11 to pad 19
    }

    if ((plat_id == BOARD_PLATM_PANGEA_V6) && ((board_id == PANGEA_V6_BOARD_ID_MANAGEBOARD) ||
        (board_id == PANGEA_V6_BOARD_ID_STL6MGTD10))) {
        pwm_pad[1] = 1; // pwm 1 to pad 1
        pwm_pad[3] = 3; // pwm 3 to pad 3
        fan_pad[1] = 12; // fan 1 to pad 12
        fan_pad[3] = 14; // fan 3 to pad 14
    }

    /* Check the configuration information. */
    if (pwm_fan_check_pad_cfg(fan_pad, pwm_pad)) {
        LOG(LOG_ERROR, " pwm_fan_check_pad_cfg failed!");
        return -EFAULT;
    }

    return fan_pwm_mgr_pad_modify(fan_pad, pwm_pad);
}

static void __iomem *pwm_fan_baseoffset_convert(u32 type, u32 chan)
{
    u32 dest_chan = PWM_FIRST_HALF;

    if (type == FAN_TYPE) {
        dest_chan = FAN_FIRST_HALF;
    }

    if (chan > dest_chan) {
        return (g_fan_pwm_mgr->infra_pwm_map_addr + FAN_PWM_SECOND_BASE_OFFSET);
    }

    return g_fan_pwm_mgr->infra_pwm_map_addr;
}

/*
 * Convert the global channel numbers 0-11 and 0-15 to the internal channel numbers 0-5 and 0-7 of each module.
 */
static u32 pwm_fan_chan_convert(u32 type, u32 ori_chan)
{
    u32 dest_chan = PWM_FIRST_HALF;
    u32 res_chan;

    if (type == FAN_TYPE) {
        dest_chan = FAN_FIRST_HALF;
    }

    if (ori_chan > dest_chan) {
        res_chan = ori_chan - dest_chan;
        res_chan -= 1;
        return res_chan;
    }

    return ori_chan;
}

static u32 pwm_get_prd(u32 ori_chan)
{
    void __iomem *pwm_map_addr;
    u32 offset;
    u32 prd;

    pwm_map_addr = pwm_fan_baseoffset_convert(PWM_TYPE, ori_chan);

    offset = PWM_CSR_PRD0_OFFSET_ADDR + (pwm_fan_chan_convert(PWM_TYPE, ori_chan) * PWM_CH_OFFSET);
    prd = ioread32(pwm_map_addr + offset);

    return (prd + 1);
}

static void pwm_set_prd(u32 chan, u32 prd)
{
    void __iomem *pwm_map_addr;
    u32 offset;

    pwm_map_addr = pwm_fan_baseoffset_convert(PWM_TYPE, chan);
    if (prd & 0xF0000000) {
        LOG(LOG_WARN, "warning:pwm prd val fault,ignore bit31-28");
    }

    offset = PWM_CSR_PRD0_OFFSET_ADDR + (pwm_fan_chan_convert(PWM_TYPE, chan) * PWM_CH_OFFSET);
    iowrite32((prd - 1) & 0x0FFFFFFF, pwm_map_addr + offset);

    return;
}

static irqreturn_t pwm_interrupt_handle(s32 irq, void *arg)
{
    u32 module_id;

    if (arg == NULL) {
        LOG(LOG_ERROR, "arg is null");
        return IRQ_HANDLED;
    }

    module_id = *((u32 *)arg);

    LOG(LOG_PRINT, "irq module id 0x%x coming", module_id);
    iowrite32(0x3, g_fan_pwm_mgr->infra_pwm_map_addr + module_id * FAN_PWM_SECOND_BASE_OFFSET + INT_IRQ);

    return IRQ_HANDLED;
}

static s32 pwm_set_duty_ratio(const pwm_info_s *pwm_info, u32 precision)
{
    u32 offset;
    u32 base;
    void __iomem *pwm_map_addr;
    u32 prd;

    if (pwm_info->channel_id >= PWM_MAX_CHANNEL) {
        LOG(LOG_ERROR, "pwm chanel %u out of change ( 0--11 )", pwm_info->channel_id);
        return -EINVAL;
    }

    if (pwm_info->ratio > precision) {
        LOG(LOG_ERROR, "pwm chanel precision %u out of change", pwm_info->ratio);
        return -EINVAL;
    }

    if ((precision != LOW_RATIO) && (precision != HIGH_RATIO)) {
        LOG(LOG_ERROR, "pwm chanel precision %u is invalid", precision);
        return -EINVAL;
    }

    pwm_map_addr = pwm_fan_baseoffset_convert(PWM_TYPE, pwm_info->channel_id);

    base = PWM_CSR_PRD0_OFFSET_ADDR;
    offset = base + (pwm_fan_chan_convert(PWM_TYPE, pwm_info->channel_id) * PWM_CH_OFFSET);
    prd = pwm_get_prd(pwm_info->channel_id);
    iowrite32(0, pwm_map_addr + offset + PWM_PWH_OFFSET);

    /* pwl 100/1000-User-configured duty cycle because of the negation logic */
    iowrite32(prd - prd * pwm_info->ratio / precision, pwm_map_addr + offset + PWM_PWL_OFFSET);

    return 0;
}

static s32 pwm_get_duty_ratio(pwm_info_s *pwm_info, u32 precision)
{
    u32 offset;
    u32 base;
    u32 high_level_time;
    void __iomem *pwm_map_addr;
    u32 prd;

    if (pwm_info->channel_id >= PWM_MAX_CHANNEL) {
        LOG(LOG_ERROR, "pwm chanel id %u out of change ( 0--11 )", pwm_info->channel_id);
        return -EINVAL;
    }

    pwm_map_addr = pwm_fan_baseoffset_convert(PWM_TYPE, pwm_info->channel_id);
    base = PWM_CSR_PRD0_OFFSET_ADDR;
    offset = base + (pwm_fan_chan_convert(PWM_TYPE, pwm_info->channel_id) * PWM_CH_OFFSET);
    offset = offset + PWM_PWL_OFFSET;
    high_level_time = ioread32(pwm_map_addr + offset);

    prd = pwm_get_prd(pwm_info->channel_id);
    if (!prd) {
        LOG(LOG_ERROR, "pwm cycle prd = %u is invalid", prd);
        return -EINVAL;
    }

    high_level_time = (prd - high_level_time) * precision / prd;
    pwm_info->ratio = high_level_time;

    return 0;
}

/*
 * To calculate the actual speed of the fan (FFAN) based on the pulse count M read from register FAN_SRx:
 * which, M represent the number of pulses output within a certain time period T, the time window size is calculated as
 * T = (N + 1) / 8, where N is the value of the register SURE_TIME[13:0] (physical address 0x2001A108),
 * with a default value of 3999.
 * The fan generates 2 pulses per revolution. Therefore, the fan speed value is calculated as:
 * S = M×60/T/(number of pulses per fan revolution), with the unit is rad/min.
 */
static s32 pwm_get_fan_speed(fan_info_s *fan_info)
{
    u32 offset;
    u32 base;
    s32 ready_flag; /* Ready signal for saving the fan speed value each time */
    s32 times_i = 0;
    u32 speed;
    u32 time_reg;
    void __iomem *pwm_map_addr;

    base = FAN_INFO_OFFSET;
    if (fan_info->channel_id >= FAN_MAX_CAHNNEL) {
        LOG(LOG_ERROR, "pwm chanel %u out of change ( 0--15 )", fan_info->channel_id);
        return -EINVAL;
    }

    pwm_map_addr = pwm_fan_baseoffset_convert(FAN_TYPE, fan_info->channel_id);
    offset = base + (pwm_fan_chan_convert(FAN_TYPE, fan_info->channel_id) * FAN_REG_OFFSET);

    /* The fan speed can be read only when the fan speed is stable. */
    for (;;) {
        if (times_i > 0x500) {
            LOG(LOG_ERROR, "get fan speed time out");
            return -ETIME;
        }
        speed = ioread32(pwm_map_addr + offset);
        ready_flag = (speed >> FAN_SRD_BIT) & 0x1;
        if (ready_flag == 1) {
            break;
        }
        times_i++;
        msleep(1);
    }

    time_reg = ioread32(pwm_map_addr + PWM_CSR_MEASURE_TIME_OFFSET_ADDR);
    speed = speed & 0xFF;

    /* 1000, 8, 60, and 2 are the data involved in the calculation formula. */
    speed = speed * (1000 * 8 / (time_reg + 1)) * 60 / 2;
    fan_info->speed = speed;

    return 0;
}

static void pwm_all_prd_init(void)
{
    u32 i;
    u32 board_id;
    u32 plat_id;

    for (i = 0; i < PWM_MAX_CHANNEL; i++) {
        /*
         * Note that the value of Cycling_NUM should not be too large.
         * Otherwise, overflow may occur when the duty cycle is calculated.
         */
        pwm_set_prd(i, CYCLE_NUM);
    }

    plat_id = bdinfo_get_platform_id();
    board_id = bdinfo_get_board_id();
    if ((plat_id == BOARD_PLATM_PANGEA_V6) && ((board_id == PANGEA_V6_BOARD_ID_MANAGEBOARD) ||
        (board_id == PANGEA_V6_BOARD_ID_STL6MGTD10))) {
        pwm_set_prd(PWM3, PWM_CLK_FRZ);
    }
}

static s32 pwm_set_reset_low_limit_radio(u32 high_radio)
{
    s32 ret;

    if (high_radio > HIGH_RATIO) {
        LOG(LOG_INFO, "save radio 0x%x too big!, fix to max value.", high_radio);
        high_radio = HIGH_RATIO;
    }

    ret = lp_subsys_reg_write(LP_SUBSYS_CSR_LPSC_ECO_NONUSE5_OFFSET_ADDR, high_radio);
    if (ret) {
        LOG(LOG_ERROR, "set radio 0x%x err", high_radio);
    }

    return ret;
}

static s32 pwm_set_radio_hold(u32 en)
{
    s32 ret;

    ret = lp_subsys_reg_read_write(LP_SUBSYS_CSR_LPSC_RESET_SEL_OFFSET_ADDR, 0x1, (en ? 0 : 1));
    if (ret) {
        LOG(LOG_ERROR, "set reset en 0x%x err", en);
    }

    return ret;
}

static s32 pwm_hold_cur_duty_ratio(u32 cmd, unsigned long arg)
{
    s32 ret;
    pwm_rst_ctrl_s bmc_rst_ctrl = { 0 };

    if (copy_from_user((void *)&bmc_rst_ctrl, (void __user *)arg, sizeof(pwm_rst_ctrl_s))) {
        LOG(LOG_ERROR, "pwm_hold_cur_duty_ratio from_user fail!");
        return -EFAULT;
    }

    ret = pwm_set_reset_low_limit_radio(bmc_rst_ctrl.ratio);
    if (ret) {
        return ret;
    }

    ret = pwm_set_radio_hold(bmc_rst_ctrl.rst_hold_en);

    return ret;
}

static s32 pwm_cmd_get_duty_ratio(u32 cmd, unsigned long arg)
{
    s32 ret;
    pwm_info_s pwm_info = { 0 };

    if (copy_from_user(&pwm_info, (void *)arg, sizeof(pwm_info_s))) {
        LOG(LOG_ERROR, "copy_from_user errs");
        return -EINVAL;
    }

    switch (cmd) {
        case PWM_CMD_GET_DUTY_RATIO:
            ret = pwm_get_duty_ratio(&pwm_info, LOW_RATIO);
            break;
        case PWM_CMD_GET_HIGH_DUTY_RATIO:
            ret = pwm_get_duty_ratio(&pwm_info, HIGH_RATIO);
            break;
        default:
            return -EINVAL;
    }

    if (ret < 0) {
        LOG(LOG_ERROR, "err(%d) pwm get duty ratio, cmd:%u", ret, _IOC_NR(cmd));
        return ret;
    }

    if (copy_to_user((void *)arg, &pwm_info, sizeof(pwm_info_s))) {
        LOG(LOG_ERROR, "pwm get low duty ratio copy_to_user err,cmd:%u", _IOC_NR(cmd));
        return -EINVAL;
    }

    return ret;
}

static s32 pwm_cmd_set_duty_ratio(u32 cmd, unsigned long arg)
{
    s32 ret;
    pwm_info_s pwm_info = { 0 };

    if (copy_from_user(&pwm_info, (void *)arg, sizeof(pwm_info_s))) {
        LOG(LOG_ERROR, "copy_from_user err");
        return -EINVAL;
    }

    switch (cmd) {
        case PWM_CMD_SET_DUTY_RATIO:
            ret = pwm_set_duty_ratio(&pwm_info, LOW_RATIO);
            break;
        case PWM_CMD_SET_HIGH_DUTY_RATIO:
            ret = pwm_set_duty_ratio(&pwm_info, HIGH_RATIO);
            break;
        default:
            return -EINVAL;
    }

    return ret;
}

static s32 pwm_cmd_get_fan_speed(u32 cmd, unsigned long arg)
{
    s32 ret;
    fan_info_s fan_info = { 0 };

    if (copy_from_user(&fan_info, (void *)arg, sizeof(fan_info_s))) {
        LOG(LOG_ERROR, "copy_from_user err");
        return -EINVAL;
    }

    ret = pwm_get_fan_speed(&fan_info);
    if (ret < 0) {
        LOG(LOG_ERROR, "err(%d) pwm_get_fan_speed", ret);
        return ret;
    }

    if (copy_to_user((void *)arg, &fan_info, sizeof(fan_info_s))) {
        LOG(LOG_ERROR, "copy_to_user err");
        return -EINVAL;
    }

    return ret;
}

static long pwm_ioctl(struct file *pfile, u32 cmd, unsigned long arg)
{
    s32 ret;

    ret = comm_ioc_parm_check(cmd, (void __user *)arg, PWM_IOC_MAGIC, PWM_CMD_MAXNR);
    if (ret) {
        LOG(LOG_ERROR, "pwm cmd(0x%x) ioc parm check failed!", cmd);
        return -EFAULT;
    }

    switch (cmd) {
        case PWM_CMD_GET_DUTY_RATIO:
        case PWM_CMD_GET_HIGH_DUTY_RATIO:
            ret = pwm_cmd_get_duty_ratio(cmd, arg);
            break;

        /*
         * Set the initial pulse value of each channel and the time when the high and low levels start.
         * Set the fan speed by the exhaust ratio.
         */
        case PWM_CMD_SET_DUTY_RATIO:
        case PWM_CMD_SET_HIGH_DUTY_RATIO:
            ret = pwm_cmd_set_duty_ratio(cmd, arg);
            break;

        /* Obtains the fan speed. */
        case PWM_CMD_GET_FAN_SPEED:
            ret = pwm_cmd_get_fan_speed(cmd, arg);
            break;

        case PWM_CMD_ENABLE_RESET_SPEED:
            ret = pwm_hold_cur_duty_ratio(cmd, arg);
            break;

        default:
            LOG(LOG_ERROR, "[pwm_ioctl]param cmd invalid cmd = 0x%x", cmd);
            return -EINVAL;
    }

    return ret;
}

void pwm_dfx_dump(void)
{
    u32 i;
    u32 prd;
    u32 ratio;
    u32 high_level_time;
    u32 offset;
    u32 base;
    void __iomem *pwm_map_addr;

    for (i = 0; i < PWM_MAX_CHANNEL; i++) {
        pwm_map_addr = pwm_fan_baseoffset_convert(PWM_TYPE, i);
        base = PWM_CSR_PRD0_OFFSET_ADDR;
        offset = base + (pwm_fan_chan_convert(PWM_TYPE, i) * 12); // mul 12
        prd = ioread32(pwm_map_addr + offset) + 1;
        offset = offset + 4; // offset 4
        high_level_time = ioread32(pwm_map_addr + offset);
        ratio = (prd - high_level_time) * LOW_RATIO / prd;
        pwm_counter_msg("pwm num: %u, period: 0x%x, high level: 0x%x, ratio: %u.\n", i, prd, high_level_time, ratio);
    }

    return;
}
EXPORT_SYMBOL(pwm_dfx_dump);

static void pwm_free_allirq(u32 virq0, u32 virq1)
{
    free_irq(virq0, &g_fan_pwm_mgr->module_id[0]);
    free_irq(virq1, &g_fan_pwm_mgr->module_id[1]);

    return;
}

static void fan_pwm_map_pad_init(void)
{
    u32 chan;
    u32 pad_pwm_oen = 0;
    u32 pad_offset;

    for (chan = 0; chan < PWM_MAX_CHANNEL; chan++) {
        pad_offset = g_fan_pwm_mgr->pwm_fan_pad.pwm_pad[chan];
        (void)lp_subsys_reg_write(LP_SUBSYS_CSR_LPSC_PAD_PWM_SEL0_OFFSET_ADDR + pad_offset * PWM_PAD_WIDTH, chan);
        pad_pwm_oen |= (1UL << g_fan_pwm_mgr->pwm_fan_pad.pwm_pad[chan]);
    }

    for (chan = 0; chan < FAN_MAX_CAHNNEL; chan++) {
        (void)lp_subsys_reg_write(LP_SUBSYS_CSR_LPSC_PAD_FAN_SEL0_OFFSET_ADDR + chan * PWM_PAD_WIDTH,
                                  g_fan_pwm_mgr->pwm_fan_pad.fan_pad[chan]);
    }

    (void)lp_subsys_reg_write(LP_SUBSYS_CSR_LPSC_PAD_PWM_OEN_OFFSET_ADDR, pad_pwm_oen);

    return;
}

static s32 pwm_fan_pad_check_init(u32 *fan_pad, u32 *pwm_pad)
{
    s32 ret;

    if ((fan_pad == NULL) || (pwm_pad == NULL)) {
        LOG(LOG_ERROR, "pwm or fan para is null");
        return -EINVAL;
    }

    ret = fan_pwm_mgr_check_init(fan_pad, pwm_pad);
    if (ret) {
        LOG(LOG_ERROR, "fan pwm mgr check and init fail(%d)", ret);
        return -EINVAL;
    }

    fan_pwm_map_pad_init();

    return 0;
}

static s32 pwm_init_irq(struct device_node *node)
{
    s32 id;
    s32 ret;

    for (id = 0; id < PWM_FAN_MODULE_NUM; id++) {
        g_fan_pwm_mgr->virq[id] = irq_of_parse_and_map(node, id);
        if (!g_fan_pwm_mgr->virq[id]) {
            ret = -EINVAL;
            LOG(LOG_ERROR, "pwm irq_of_parse_and_map error");
            goto ERR_EXIT;
        }

        ret = request_irq(g_fan_pwm_mgr->virq[id], pwm_interrupt_handle, 0, DEV_PWM_NAME_KER,
                          &g_fan_pwm_mgr->module_id[id]);
        if (ret) {
            g_fan_pwm_mgr->virq[id] = 0;
            LOG(LOG_ERROR, "can't get assigned irq%d, Return code is %d", id, ret);
            goto ERR_EXIT;
        }
    }

    return 0;

ERR_EXIT:
    while (id) {
        id--;
        free_irq(g_fan_pwm_mgr->virq[id], &g_fan_pwm_mgr->module_id[id]);
        g_fan_pwm_mgr->virq[id] = 0;
    }

    return ret;
}

static void pwm_ioremap_exit(void)
{
    if (g_fan_pwm_mgr->infra_pwm_map_addr) {
        iounmap(g_fan_pwm_mgr->infra_pwm_map_addr);
        g_fan_pwm_mgr->infra_pwm_map_addr = NULL;
    }

    return;
}

static s32 pwm_ioremap(struct device_node *node)
{
    g_fan_pwm_mgr->infra_pwm_map_addr = of_iomap(node, 0);
    if (!g_fan_pwm_mgr->infra_pwm_map_addr) {
        LOG(LOG_ERROR, "g_infra_pwm_map_addr error!");
        return -EIO;
    }

    return 0;
}

static const struct file_operations g_infra_pwm_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = pwm_ioctl
};

static s32 pwm_probe(struct platform_device *pdev)
{
    s32 ret;
    struct device_node *node = pdev->dev.of_node;

    ret = pwm_set_radio_hold(0);
    if (ret) {
        LOG(LOG_ERROR, "set radio hold failed");
    }

    g_fan_pwm_mgr = (fan_pwm_mgr *)kzalloc(sizeof(fan_pwm_mgr), GFP_KERNEL);
    if (g_fan_pwm_mgr == NULL) {
        LOG(LOG_ERROR, " kzalloc failed!");
        return -ENOMEM;
    }

    ret = comm_init_dev(&g_fan_pwm_mgr->infra_pwm_dev, &g_infra_pwm_fops, DEV_PWM_NAME_KER);
    if (ret) {
        kfree(g_fan_pwm_mgr);
        g_fan_pwm_mgr = NULL;
        LOG(LOG_ERROR, "call comm_init_dev failed!");
        return -ENOMEM;
    }

    if (pwm_fan_pad_check_init(g_fan_pad_cfg, g_pwm_pad_cfg)) {
        LOG(LOG_ERROR, " pwm_fan_pad_check_init failed!");
        ret = -EFAULT;
        goto EXIT0;
    }

    ret = pwm_ioremap(node);
    if (ret) {
        LOG(LOG_ERROR, "pwm of_iomap failed");
        goto EXIT0;
    }

    ret = pwm_init_irq(node);
    if (ret) {
        LOG(LOG_ERROR, "request interrupt failed");
        goto EXIT1;
    }

    pwm_all_prd_init();

    /* Set the time window size of the fan to 1999 and the corresponding time value to 250 ms. */
    iowrite32(FAN_MEASURE_TIME_VALUE, g_fan_pwm_mgr->infra_pwm_map_addr + PWM_CSR_MEASURE_TIME_OFFSET_ADDR);
    iowrite32(FAN_MEASURE_TIME_VALUE,
              g_fan_pwm_mgr->infra_pwm_map_addr + FAN_PWM_SECOND_BASE_OFFSET + PWM_CSR_MEASURE_TIME_OFFSET_ADDR);

    LOG(LOG_INFO, "pwm_init ok!");
    return 0;

EXIT1:
    pwm_ioremap_exit();

EXIT0:
    comm_cleanup_dev(&g_fan_pwm_mgr->infra_pwm_dev);
    kfree(g_fan_pwm_mgr);
    g_fan_pwm_mgr = NULL;

    return ret;
}

static s32 pwm_remove(struct platform_device *pdev)
{
    if (g_fan_pwm_mgr) {
        pwm_free_allirq(g_fan_pwm_mgr->virq[PWM_FAN_MODULE0], g_fan_pwm_mgr->virq[PWM_FAN_MODULE1]);
        pwm_ioremap_exit();

        comm_cleanup_dev(&g_fan_pwm_mgr->infra_pwm_dev);

        kfree(g_fan_pwm_mgr);
        g_fan_pwm_mgr = NULL;
    }

    LOG(LOG_INFO, "pwm_clean ok!");

    return 0;
}

static const struct of_device_id g_pwm_match[] = {
    { .compatible = "hisilicon, hi1711-pwm" }, {}
};

static struct platform_driver g_pwm_driver = {
    .driver = {
        .name = DEV_PWM_NAME_KER,
        .of_match_table = g_pwm_match,
    },

    .probe = pwm_probe,
    .remove = pwm_remove,
};
module_platform_driver(g_pwm_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("pwm driver");