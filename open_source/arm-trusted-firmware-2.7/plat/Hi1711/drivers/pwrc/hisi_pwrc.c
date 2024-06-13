/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: hi1711_pwrc
 * Author: xxx
 * Create: 2022-11-16
 */

#include "hisi_pwrc.h"

#include <arch_helpers.h>
#include <hisi_ipc.h>

#include <plat/common/platform.h>
#include <common/debug.h>
#include <lib/spinlock.h>
#include <lib/mmio.h>

#include "fcm_pmctrl_csr_addr_define.h"
#include "../hi1711_private.h"


#define __PDC_ENABLE__ 1
#define TS_AICPU_STATUS_BASE_VA 0x1F0030000
#define TS_SC_SEMAREG0          (TS_AICPU_STATUS_BASE_VA + 0xc20)
spinlock_t g_pm_ctrl_lock;

void modify_any_reg(uint32_t base, uint32_t offset, uint32_t mask, uint32_t val)
{
    uint32_t temp_val;

    temp_val = mmio_read_32(base + offset);
    temp_val &= (~mask);
    temp_val |= (mask & val);
    mmio_write_32((base + offset), temp_val);

    return;
}

void modify_pmctrl_reg(uint32_t offset, uint32_t mask, uint32_t val)
{
    uint32_t pm_reg_base = FCM_PMCTRL_CSR_BASE_ADDR;

    modify_any_reg(pm_reg_base, offset, mask, val);

    return;
}

uint64_t get_actlr_el3(void)
{
    uint64_t actlr_val = 0xDEADBEEF;
    __asm__ volatile("mrs %0, actlr_el3"
                     : "=r"(actlr_val));
    return actlr_val;
}

void set_actlr_el3(uint64_t actlr_val)
{
    __asm__ volatile("msr actlr_el3, %0"
                     :
                     : "r"(actlr_val));
    return;
}

unsigned long get_actlr_el2(void)
{
    unsigned long actlr_val = 0xDEADBEEF;
    __asm__ volatile("mrs %0, actlr_el2"
                     : "=r"(actlr_val));
    return actlr_val;
}

void set_actlr_el2(unsigned long actlr_val)
{
    __asm__ volatile("msr actlr_el2, %0"
                     :
                     : "r"(actlr_val));
    return;
}

void modify_actlr_el3(uint64_t mask, uint64_t val)
{
    uint64_t actlr;

    actlr = get_actlr_el3();
    actlr &= (~mask);
    actlr |= (mask & val);
    set_actlr_el3(actlr);

    return;
}

void modify_actlr_el2(unsigned long mask, unsigned long val)
{
    unsigned long actlr;

    actlr = get_actlr_el2();
    actlr &= (~mask);
    actlr |= (mask & val);
    set_actlr_el2(actlr);

    return;
}

/* hard step1 first half，-soft- step1 */
void com_core_pwdown_a55_init(uint32_t cpu, uint32_t reset_addr)
{
    (void)cpu;
    (void)reset_addr;

    return;
}

void com_core_pwdown_enter_wfi(void)
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

void pchan_switch(uint32_t cpu, uint32_t enable)
{
    /* enable pchan */
    modify_pmctrl_reg(FCM_PMCTRL_CSR_PMC_CORE0_PCTRL_OFFSET_ADDR + cpu * 4, /* 4cpu */
                      (1ul << 31), (enable << 31)); /* <<31 */
    return;
}

/* 参数为0x100000的地址 */
void set_a55_coren_rvba(uint32_t cpu, uint32_t rvba_addr)
{
    /* rvba_addr >> 2 */
    mmio_write_32((A55_CSR_BASE_ADDR + A55_CORE0_RVBA_OFFSET + CRG_RVBAR(0, cpu)), (rvba_addr >> 2));
    return;
}

#ifdef __PDC_ENABLE__
static void hi1711_pdc_enable(unsigned int core)
{
    if (core >= MAX_1711_CORE_NUM) {
        return;
    }

    /* 2.PMC_CORE0_PCTRL寄存器31比特置1,
       使能Ccore0 P-Channel控制器. */
    pchan_switch(core, 1);

    /* 3.使能核上下电控制器 */
    /* Core0 PDC使能信号：
        1 - 使能PDC 0 - 禁用PDC */
    pdc_switch(core, 1);

    return;
}
#endif

static void resource_lock(void)
{
    spin_lock(&g_pm_ctrl_lock);
}

static void resource_unlock(void)
{
    spin_unlock(&g_pm_ctrl_lock);
}

void pdc_switch(uint32_t cpu, uint32_t enable)
{
    resource_lock();
    modify_pmctrl_reg(FCM_PMCTRL_CSR_PMC_CORE_PDC_CTRL_OFFSET_ADDR, (1ul << cpu), (enable << cpu));
    resource_unlock();

    return;
}

void hi1711_powerup_core(unsigned int cluster, unsigned int core)
{
    (void)cluster;

#ifdef __PDC_ENABLE__
    /* 配置一次即可,可以不需要进行多次配置 */
    hi1711_pdc_enable(core);

    /* 1.增加Down REQ 先清零. */
    resource_lock();
    /* core0_p_channel_enable */
#ifdef CLR_REQ
    modify_pmctrl_reg(FCM_PMCTRL_CSR_PMC_CORE_POWER_DOWN_REQ_OFFSET_ADDR, BIT(core), 0);
#endif

    /* 2.软件发送上电请求 */
    modify_pmctrl_reg(FCM_PMCTRL_CSR_PMC_CORE_POWER_UP_REQ_OFFSET_ADDR, BIT(core), BIT(core));

    __asm__ volatile("dsb sy");
    resource_unlock();

#endif

    return;
}

void hi1711_powerdn_core(unsigned int cluster, unsigned int core)
{
    (void)cluster;

#ifdef __PDC_ENABLE__
    /* 配置一次即可,可以不需要进行多次配置. */
    hi1711_pdc_enable(core);

    /* 增加UP REQ 先清零. */
    resource_lock();
#ifdef CLR_REQ
    /* 原davi这一步不需要，原设计必须这样做，1711可以不需要 */
    modify_pmctrl_reg(FCM_PMCTRL_CSR_PMC_CORE_POWER_UP_REQ_OFFSET_ADDR, BIT(core), 0);
#endif
    /* 软件发送单核下电请求. */
    modify_pmctrl_reg(FCM_PMCTRL_CSR_PMC_CORE_POWER_DOWN_REQ_OFFSET_ADDR, BIT(core), BIT(core));
    resource_unlock();
#endif

    return;
}

void hi1711_disable_core(unsigned int cluster, unsigned int core)
{
    volatile uint64_t sysreg = 0;
    (void)cluster;

    /* Step 2. CPUPWRCTLR_EL1寄存器(S3_0_C15_C2_7)0bit置位，
    指示PMC在Core进入WFI模式后通过PACTIVE申请Core下电 */
    __asm__ __volatile__("mrs %0, S3_0_C15_C2_7"
                         : "=r"(sysreg));
    sysreg |= (1 << 0);
    __asm__ __volatile__("msr S3_0_C15_C2_7, %0"
                         :
                         : "r"(sysreg));
    isb();
    dsb();

    return;
}

void hi1711_disable_cluster(unsigned int cluster, unsigned int core)
{
    volatile uint64_t sysreg = 0;
    (void)cluster;

    /* Step 1. CLUSTERPWRDN_EL1寄存器0比特置位，
    指示所有Core下电后Cluster可以下电 */
    __asm__ __volatile__("mrs %0, S3_0_C15_C3_6"
                         : "=r"(sysreg));
    sysreg &= (~(1 << 0));
    __asm__ __volatile__("msr S3_0_C15_C3_6, %0"
                         :
                         : "r"(sysreg));
    isb();
    dsb();

    return;
}

void hi1711_powerdn_cluster(unsigned int cluster, unsigned int core)
{
    hisi_ipc_pm_on_off(core, cluster, PM_OFF);

    return;
}
