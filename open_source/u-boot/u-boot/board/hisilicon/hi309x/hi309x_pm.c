/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Hi309x CPU power management APIs
 * Create: 2018-05-10
 */

#include "hi309x_pm.h"
#include <config.h>
#include <hi309x_common.h>
#include <common.h>
#include <div64.h>
#include <dm.h>
#include <jffs2/jffs2.h>
#include <linux/mtd/mtd.h>
#include <asm/io.h>

/* use rw_exclusive make sure uboot powerdown */
static int hi309x_rw_exclusive_lock(u32 cpu)
{
    u32 sta;
    u32 time_out = 400000000;

    while (time_out--) {
        sta = readl(0x874021c);
        if ((sta & 0xF) == cpu) {
            writel(0, (unsigned long)(SLAVE_PD_LOCK_TIMEOUT_ADDR + cpu * 0x4));
            return 0;
        }
        writel((0xFFFFFFF0 | cpu), 0x874021c);
    }

    writel(cpu, (unsigned long)(SLAVE_PD_LOCK_TIMEOUT_ADDR + cpu * 0x4));

    return 1;
}

static void hi309x_rw_exclusive_unlock(void)
{
    writel(0xFFFFFFF0, 0x874021c);

    return;
}

static u64 hi309x_get_actlr_el3(void)
{
    u64 actlr_val = 0xDEADBEEF;
    __asm__ volatile("mrs %0, actlr_el3"
                     : "=r"(actlr_val));
    return actlr_val;
}

static void hi309x_set_actlr_el3(u64 actlr_val)
{
    __asm__ volatile("msr actlr_el3, %0"
                     :
                     : "r"(actlr_val));
    return;
}
static unsigned long hi309x_get_actlr_el2(void)
{
    unsigned long actlr_val = 0xDEADBEEF;
    __asm__ volatile("mrs %0, actlr_el2"
                     : "=r"(actlr_val));
    return actlr_val;
}

static void hi309x_set_actlr_el2(unsigned long actlr_val)
{
    __asm__ volatile("msr actlr_el2, %0"
                     :
                     : "r"(actlr_val));
    return;
}
static void hi309x_modify_actlr_el3(u64 mask, u64 val)
{
    u64 actlr;

    actlr = hi309x_get_actlr_el3();
    actlr &= (~mask);
    actlr |= (mask & val);
    hi309x_set_actlr_el3(actlr);

    return;
}

static void hi309x_modify_actlr_el2(unsigned long mask, unsigned long val)
{
    unsigned long actlr;

    actlr = hi309x_get_actlr_el2();
    actlr &= (~mask);
    actlr |= (mask & val);
    hi309x_set_actlr_el2(actlr);

    return;
}

static unsigned int hi309x_get_cpupwrctlr_el1(void)
{
    unsigned int val = 0xDEADBEEF;
    __asm__ volatile("mrs %0, S3_0_C15_C2_7"
                     : "=r"(val));
    return val;
}

static void hi309x_set_cpupwrctlr_el1(unsigned int val)
{
    __asm__ volatile("msr S3_0_C15_C2_7, %0"
                     :
                     : "r"(val));
    return;
}

static void hi309x_modify_cpupwrctlr_el1(unsigned int mask, unsigned int val)
{
    unsigned int temp;

    temp = hi309x_get_cpupwrctlr_el1();
    temp &= (~mask);
    temp |= (mask & val);
    hi309x_set_cpupwrctlr_el1(temp);

    return;
}

static void hi309x_modify_any_reg(unsigned long base, u32 offset, u32 mask, u32 val)
{
    u32 temp_val;

    temp_val = readl((unsigned long)(base + offset));
    temp_val &= (~mask);
    temp_val |= (mask & val);
    writel(temp_val, base + offset);

    return;
}

static void hi309x_modify_pmctrl_reg(u32 offset, u32 mask, u32 val)
{
    unsigned long pm_reg_base = FCM_PMCTRL_CSR_BASE_ADDR;

    hi309x_modify_any_reg(pm_reg_base, offset, mask, val);

    return;
}
static void hi309x_set_a55_coren_rvba(u32 cpu, u32 rvba_addr)
{
    if (rvba_addr & 0x3) {
        uart_early_puts(CONFIG_SYS_UART_NUM, "warning cpu , rvba not allign\r\n");
    }
    writel((rvba_addr >> 0x2), (unsigned long)(A55_CSR_BASE_ADDR + A55_CORE0_RVBA_OFFSET + 0x4 * cpu));

    return;
}

static void hi309x_com_core_pwdown_a55_init(u32 cpu, u32 reset_addr)
{
    hi309x_modify_actlr_el3(ACTLR_ELX_PWREN, ACTLR_ELX_PWREN);
    hi309x_modify_actlr_el2(ACTLR_ELX_PWREN, ACTLR_ELX_PWREN);
    hi309x_set_a55_coren_rvba(cpu, reset_addr); /* 重新上电从备用uboot启动 */

    return;
}
static void hi309x_com_core_pwdown_enter_wfi(void)
{
    asm volatile("isb"
                 :
                 :
                 : "memory");
    asm volatile("dsb sy"
                 :
                 :
                 : "memory");
    asm volatile("wfi"
                 :
                 :
                 : "memory");
    return;
}

static void hi309x_pchan_switch(u32 cpu, u32 enable)
{
    hi309x_modify_pmctrl_reg(FCM_PMCTRL_CSR_PMC_CORE0_PCTRL_OFFSET_ADDR + cpu * sizeof(u32),
                             (1ul << 31), (enable << 31)); /* bit 32 is pchan, so left shift 31 */

    return;
}

static void hi309x_pdc_switch(u32 cpu, u32 enable)
{
    hi309x_modify_pmctrl_reg(FCM_PMCTRL_CSR_PMC_CORE_PDC_CTRL_OFFSET_ADDR, (1ul << cpu), (enable << cpu));

    return;
}

static int hi309x_master_wait_reg_state_ready(unsigned long reg_addr, u32 reg_bit_mask, u32 obj_val, u32 wait_cnt)
{
    s32 ret;
    u32 cnt = 0;
    char err_msg[PM_ERR_MSG_LEN] = { 0 }; /* msg_buff is 256 byte */
    u32 reg_act_val = readl(reg_addr);

    reg_act_val &= reg_bit_mask;
    while ((cnt < wait_cnt) && (reg_act_val != obj_val)) {
        cnt++;
        reg_act_val = readl(reg_addr);
        reg_act_val &= reg_bit_mask;
    }
    if (cnt == wait_cnt) {
        ret = sprintf(err_msg, "master wait reg_addr 0x%x bitmask 0x%x obj 0x%x, act 0x%x timeout\n",
            (u32)reg_addr, reg_bit_mask, obj_val, reg_act_val);
        if (ret < 0) {
            return -1;
        }
        uart_early_puts(CONFIG_SYS_UART_NUM, err_msg);
        return -1;
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : pmc_wait_all_slave_pwr_done
 功能描述  : 等待所有的从核下电完成，并清除中断，当前ATF中不会使用该中断
 输入参数  : uint32_t cpu
 输出参数  : 无
 返 回 值  : uint32_t 真:对应状态完成，假:对应状态未发生
 要求软件不能随意操作中断，用于pll配置之前
*****************************************************************************/
int pmc_wait_all_slave_pwr_done(void)
{
    u32 cpu_psta;
    char err_msg[PM_ERR_MSG_LEN] = { 0 }; /* msg_buff is 256 byte */
    u32 wait_cnt = CONFIG_SYS_CPU_FREQ / 2; /* divide 2, wait 500ms */
    int ret;

    ret = hi309x_master_wait_reg_state_ready(FCM_PMCTRL_CSR_PMC_CORE_PWR_DONE_INT_ADDR,
                                             SLACE_PD_SUCCESS_FLAG, SLACE_PD_SUCCESS_FLAG, wait_cnt);
    if (ret) {
        cpu_psta = readl(FCM_PMCTRL_CSR_PMC_CORE_POWER_STATE_ADDR);
        ret = sprintf(err_msg, "pstate 0x%x, wait_cnt 0x%x\n", cpu_psta, wait_cnt);
        if (ret > 0) {
            uart_early_puts(CONFIG_SYS_UART_NUM, err_msg);
        }

        /* Just print DFX message and continue */
        ret = sprintf(err_msg, "Wait slave cpus power down timeout: lock_flag: 0x%08x 0x%08x 0x%08x\r\n",
                        readl(SLAVE_PD_LOCK_TIMEOUT_ADDR + 0x4),
                        readl(SLAVE_PD_LOCK_TIMEOUT_ADDR + 0x8),
                        readl(SLAVE_PD_LOCK_TIMEOUT_ADDR + 0xc));
        if (ret > 0) {
            uart_early_puts(CONFIG_SYS_UART_NUM, err_msg);
        }
        ret = -1;
    }

    /* clear cpu1、2、3 pd int */
    writel(FCM_PMCTRL_CSR_PMC_CORE_PWR_DONE_INT_ADDR, SLACE_PD_SUCCESS_FLAG);

    return ret;
}

int hi309x_core_hd_pwdown(u32 cpu)
{
    int ret = 0;

    if (cpu >= MAX_309x_CORE_NUM) {
        uart_early_puts(CONFIG_SYS_UART_NUM, "cpu num para err\r\n");
        return -EINVAL;
    }

    (void)hi309x_rw_exclusive_lock(cpu);

    /* step 1 当前为el3修改访问权限可以不修改 */
    hi309x_com_core_pwdown_a55_init(cpu, 0);

    hi309x_pchan_switch(cpu, 1);
    hi309x_pdc_switch(cpu, 1);
    /* gic cfg no use */
    /* step 3 开启a55可下电使能 */
    hi309x_modify_cpupwrctlr_el1((1ul << 0), (1ul << 0));

    /* step 4 软件发起下电准备 */
    hi309x_modify_pmctrl_reg(FCM_PMCTRL_CSR_PMC_CORE_POWER_DOWN_REQ_OFFSET_ADDR, (1ul << cpu), (1ul << cpu));

    hi309x_rw_exclusive_unlock();

    /* step 5 软件执行指令，core[n]进入WFI */
    hi309x_com_core_pwdown_enter_wfi();

    /* step 6 硬件自动完成P-Channel握手，执行一系列下电动作 */
    /* no code */
    return ret;
}
