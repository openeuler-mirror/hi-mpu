/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: hi1711_pm
 * Author: xxx
 * Create: 2022-11-16
 */

#include <arch_helpers.h>
#include <drivers/console.h>
#include <common/debug.h>
#include <drivers/arm/gicv3.h>
#include <lib/mmio.h>
#include <lib/psci/psci.h>
#include <plat/common/platform.h>
#include "drivers/pwrc/hisi_pwrc.h"
#include "hi1711_private.h"
#include "hi1711_def.h"

#define CORE_PWR_STATE(state)       ((state)->pwr_domain_state[MPIDR_AFFLVL0])
#define CLUSTER_PWR_STATE(state)    ((state)->pwr_domain_state[MPIDR_AFFLVL1])
#define SYSTEM_PWR_STATE(state)     ((state)->pwr_domain_state[PLAT_MAX_PWR_LVL])

static uintptr_t g_hi1711_sec_entrypoint;
static unsigned char g_cluster_powerdown_gic_disable_flag = 0x0;

#define GIC0_BASE   0x109000000
#define GICD_OFFSET 0x0

#define GICD_ISPENDRLOW                         0x0200
#define GICD_ISPENDR_SP                         0x0204
#define GICD_ICFGR_LOW_SGI                      0x00C00UL

static void Hi1711_pwr_domain_standby(plat_local_state_t cpu_state)
{
    unsigned long scr;
    unsigned int coreid;

    if (cpu_state != PLAT_MAX_RET_STATE) {
        ERROR("[%s]: cpu_state is %u, error! \n\r", __func__, cpu_state);
        return;
    }

    scr = read_scr_el3();

    /* Enable Physical IRQ and FIQ to wake the CPU. */
    write_scr_el3(scr | SCR_IRQ_BIT | SCR_FIQ_BIT);
    wfi();

    coreid = plat_my_core_pos();

    INFO("[%s] Core %d has been wakeup. \n\r", __func__, coreid);

    /*
    * Restore SCR to the original value, synchronisazion of
    * scr_el3 is done by eret while el3_exit to save some
    * execution cycles.
 */
    write_scr_el3(scr);
}

/*******************************************************************************
 * Private Hi1711 function to program the mailbox for a cpu before it is released
 * from reset.
 ******************************************************************************/
static void Hi1711_program_mailbox(uint64_t mpidr, uint64_t address)
{
    uint64_t linear_id;

    linear_id = (uint64_t)plat_core_pos_by_mpidr(mpidr);
    if (linear_id >= PLATFORM_CORE_MAX_NUM) {
        NOTICE("[%s]:[%dL] Error core id %lx.\n", __func__, __LINE__, linear_id);
        return;
    }

    set_a55_coren_rvba(linear_id, (uint32_t)address);

    return;
}

/* st: A55 CPUECTLR_EL1 L3PCTL[12:10]bit置为0b'100
core下电后上电需要配置A55 core的此寄存器 */
static void Hi1711_a55_l3pctl_set(void)
{
    volatile uint64_t sysreg = 0;

    __asm__ __volatile__("mrs %0, S3_0_C15_C1_4"
                         : "=r"(sysreg));
    sysreg |= (1 << 12); /* 1 << 12 */
    sysreg &= (~(1 << 11)); /* 1 << 11 */
    sysreg &= (~(1 << 10)); /* 1 << 10 */
    __asm__ __volatile__("msr S3_0_C15_C1_4, %0"
                         :
                         : "r"(sysreg));
    isb();
    dsb();
}

#define GIC_PERI_SUBCTRL_BASE 0x10C000000
#define GIC_RESET_ST          0x5C08
#define GIC_RESET_DREQ        0xC0C
#define GIC_CLOCK_EN          0x330
#define GIC_CLOCK_DIS         0x334
#define GIC_CLOCK_STATE       0x5330

#define SDMA_CLK_ENABLE 0x5B0
#define SDMA_RESET_DREQ 0xCA4

#define CPU_CLST_SUBCTRL_BASE 0x1000E0000
#define CORE_STATE            0x7100

/* function: gic software reset , return 0 success, else fail. */
static int Hi1711_gic_disreset(void)
{
    volatile uint32_t reset_value;
    volatile uint32_t state;
    volatile uint32_t timeout = 0xFFF;

    reset_value = mmio_read_32(GIC_PERI_SUBCTRL_BASE + GIC_RESET_ST);
    /* 处于软复位状态 */
    if (reset_value) {
        /* 1.step disable gic clock */
        mmio_write_32(GIC_PERI_SUBCTRL_BASE + GIC_CLOCK_DIS, 0x1);

        /* 2. 撤销软复位有效 */
        mmio_write_32(GIC_PERI_SUBCTRL_BASE + GIC_RESET_DREQ, 0x1);

        /* 3. 软复位有效状态 */
        do {
            state = mmio_read_32(GIC_PERI_SUBCTRL_BASE + GIC_RESET_ST);
            if (state == 0) {
                /* 4. enable GIC clock */
                mmio_write_32(GIC_PERI_SUBCTRL_BASE + GIC_CLOCK_EN, 0x1);
            }

            timeout--;
            if (timeout == 0) {
                ERROR("GIC reset timeout\n");
                return 2; /* return 2 */
            }
        } while (state != 0x0);
    }

    /* gic clock check */
    timeout = 0xFFF;
    do {
        state = mmio_read_32(GIC_PERI_SUBCTRL_BASE + GIC_CLOCK_STATE);

        timeout--;
        if (timeout == 0x0) {
            ERROR("GIC clock not ready!!!\n");
            return 1;
        }
    } while (state != 0x1);

    return 0;
}

static int Hi1711_pwr_domain_on(u_register_t mpidr)
{
    unsigned int coreid;
    unsigned int cluster;

    /* 不考虑cluster不上电情况. */
    cluster = MPIDR_AFFLVL1_VAL(mpidr);
    coreid = MPIDR_AFFLVL0_VAL(mpidr);

    /* Setup mailbox with address for CPU entrypoint when it next powers up. */
    Hi1711_program_mailbox(mpidr, g_hi1711_sec_entrypoint);

    /* 配置状态机pdc给core上电. */
    hi1711_powerup_core(cluster, coreid);

    return PSCI_E_SUCCESS;
}

static void Hi1711_pwr_domain_on_finish(const psci_power_state_t *target_state)
{
    unsigned int coreid = plat_my_core_pos();
    unsigned long mpidr = read_mpidr_el1();

    Hi1711_a55_l3pctl_set();

    Hi1711_program_mailbox(mpidr, 0);

    gicv3_rdistif_init(coreid);
    gicv3_cpuif_enable(coreid);
}

static void enable_gicd(void)
{
    uint32_t value;

    value = mmio_read_32(GIC0_BASE + GICD_OFFSET);
    value = value | (0x7);
    mmio_write_32(GIC0_BASE + GICD_OFFSET, value);
}

static void Hi1711_pwr_domain_off(const psci_power_state_t *target_state)
{
    unsigned long mpidr;
    unsigned int coreid;
    unsigned int cluster;

    mpidr = read_mpidr_el1();
    cluster = MPIDR_AFFLVL1_VAL(mpidr);
    coreid = MPIDR_AFFLVL0_VAL(mpidr);

    if (CORE_PWR_STATE(target_state) != PLAT_MAX_OFF_STATE) {
        return;
    }

    if (CORE_PWR_STATE(target_state) == PLAT_MAX_OFF_STATE) {
        /* 此处代码暂且屏蔽，地址值待确定，在上电时指定 */
        com_core_pwdown_a55_init(coreid, 0);
        hi1711_disable_core(cluster, coreid);

        /* 写状态机下电请求寄存器 */
        hi1711_powerdn_core(cluster, coreid);

        gicv3_cpuif_disable(coreid);
    }

    return;
}

static int Hi1711_validate_power_state_main(psci_power_state_t *req_state, unsigned int pwr_lvl, unsigned int pstate)
{
    int i;

    for (i = MPIDR_AFFLVL0; i <= pwr_lvl; i++) {
        req_state->pwr_domain_state[i] = PLAT_MAX_OFF_STATE;
    }

    pstate = pstate >> 24; /* pstate >> 24 */
    /* Suspend */
    if (!(pstate & 0x1) && (MPIDR_AFFLVL0 != pwr_lvl)) {
        req_state->pwr_domain_state[pwr_lvl] = PLAT_MAX_CLUSTER_IDLE_STATE;
    }

    return 0;
}

static int Hi1711_validate_power_state_standby(psci_power_state_t *req_state, unsigned int pwr_lvl)
{
    /*
     * It's possible to enter standby only on power level 0
     * Ignore any other power level.
     */
    if (pwr_lvl != MPIDR_AFFLVL0) {
        return PSCI_E_INVALID_PARAMS;
    }

    req_state->pwr_domain_state[MPIDR_AFFLVL0] = PLAT_MAX_RET_STATE;

    return 0;
}

static int Hi1711_validate_power_state(unsigned int power_state, psci_power_state_t *req_state)
{
    unsigned int pwr_lvl = psci_get_pstate_pwrlvl(power_state);
    unsigned int pstate = psci_get_pstate_type(power_state);
    int ret;

    if (!req_state) {
        ERROR("[%s]: req_state error! \n\r", __func__);
        return PSCI_E_INVALID_PARAMS;
    }

    if (pwr_lvl > PLAT_MAX_PWR_LVL) {
        return PSCI_E_INVALID_PARAMS;
    }

    /* Sanity check the requested state */
    ret = (pstate == PSTATE_TYPE_STANDBY) ?
          Hi1711_validate_power_state_standby(req_state, pwr_lvl) :
          Hi1711_validate_power_state_main(req_state, pwr_lvl, pstate);
    if (ret) {
        return ret;
    }

    /*
     * We expect the 'state id' to be zero.
     */
    if (psci_get_pstate_id(power_state)) {
        return PSCI_E_INVALID_PARAMS;
    }

    return PSCI_E_SUCCESS;
}

static int Hi1711_validate_ns_entrypoint(uintptr_t entrypoint)
{
    /*
	 * Check if the non secure entrypoint lies within the non
	 * secure DRAM.
	 */
    (void)entrypoint;

    return PSCI_E_SUCCESS;
}

static void Hi1711_pwr_domain_suspend(const psci_power_state_t *target_state)
{
    if (CORE_PWR_STATE(target_state) != PLAT_MAX_OFF_STATE) { /* 下电状态-PLAT_MAX_OFF_STATE */
        return;
    }

    /*
	 * Setup mailbox with address for CPU entrypoint when it next powers up.
	 */
    Hi1711_program_mailbox(read_mpidr_el1(), g_hi1711_sec_entrypoint);

    Hi1711_pwr_domain_off(target_state);
}

#define STATUS_BASE  0x10c000000
#define SDMAREG_BASE 0x102080000

#define AA_PERI_BASE     0x1000F0000
#define AA_IO_BASE       0x100100000
#define AA_DVPP_BASE     0x100110000
#define AA_A55_BASE      0x100120000
#define AA_AI0_BASE      0x100130000
#define AA_AI1_BASE      0x100140000
#define DISP_REG_BASE    0x102060000
#define SYSCTRL_REG_BASE 0x1100C0000

#define SYSSOFTRST_CTRL  0x200
#define SYSSOFTRST_VALUE 0x55555555

#define DAW_MSD_CTRL 0xC0
#define DAW_EN       0xD0

#define TS_SYSCTRL0_BASE 0x1F0030000
#define TS_SECURE0_CTROL 0x7000

static void Hi1711_pwr_domain_suspend_finish(const psci_power_state_t *target_state)
{
    volatile uint32_t temp;

    /* Nothing to be done on waking up from retention from CPU level */
    if (CORE_PWR_STATE(target_state) != PLAT_MAX_OFF_STATE) {
        return;
    }

    Hi1711_a55_l3pctl_set();

    if (CLUSTER_PWR_STATE(target_state) == PLAT_MAX_OFF_STATE) {
        /* 防挂死特性，需要把periph subsys、IO subsys、DVPP subsys、
        CPU Cluster的DAW_EN配置为0x3ffff，aicore的DAW_EN配置为0x3ec00 */
        mmio_write_32(AA_PERI_BASE + DAW_MSD_CTRL, 0x3);
        mmio_write_32(AA_PERI_BASE + DAW_EN, 0x3FFFF);
        __asm__ volatile("dsb sy");
        __asm__ volatile("isb");

        mmio_write_32(AA_IO_BASE + DAW_MSD_CTRL, 0x3);
        mmio_write_32(AA_IO_BASE + DAW_EN, 0x3FFFF);

        mmio_write_32(AA_DVPP_BASE + DAW_MSD_CTRL, 0x3);
        mmio_write_32(AA_DVPP_BASE + DAW_EN, 0x3FFFF);
        __asm__ volatile("dsb sy");

        mmio_write_32(AA_A55_BASE + DAW_MSD_CTRL, 0x3);
        mmio_write_32(AA_A55_BASE + DAW_EN, 0x3FFFF);
        __asm__ volatile("dsb sy");

        mmio_write_32(AA_AI0_BASE + DAW_MSD_CTRL, 0x3);
        mmio_write_32(AA_AI0_BASE + DAW_EN, 0x3EC00);
        __asm__ volatile("dsb sy");

        if (IsAsicBoot() == 0x1) {
            mmio_write_32(AA_AI1_BASE + DAW_MSD_CTRL, 0x3);
            mmio_write_32(AA_AI1_BASE + DAW_EN, 0x3EC00);
            __asm__ volatile("dsb sy");
        }

        /* st: GIC restore */
        /* GIC、ITS解复位前需要配置PERISUB_DISP的DAW窗口使能，访问GIC需要配置为0xef，
        访问ITS需要配置为0x9f，两者同时访问需要配置为0xff */
        mmio_write_32(DISP_REG_BASE + DAW_EN, 0xff);
        __asm__ volatile("dsb sy");
        if (Hi1711_gic_disreset() == 0) {
            plat_arm_gic_resume();
        } else {
            ERROR("GIC resume error!!!\n");
            panic();
        }

        /* 配置TS 非安全权限使能 */
        temp = mmio_read_32(TS_SYSCTRL0_BASE + TS_SECURE0_CTROL);
        temp = temp | 0x1;
        mmio_write_32(TS_SYSCTRL0_BASE + TS_SECURE0_CTROL, temp);

        /* 解复位SDMA */
        mmio_write_32(GIC_PERI_SUBCTRL_BASE + SDMA_CLK_ENABLE, 0x1);
        mmio_write_32(GIC_PERI_SUBCTRL_BASE + SDMA_RESET_DREQ, 0x1);
    }

    gicv3_cpuif_enable(plat_my_core_pos());

    if (g_cluster_powerdown_gic_disable_flag) {
        enable_gicd();
        g_cluster_powerdown_gic_disable_flag = 0x0;
    }
}

#define SYS_REF_CLOCK_FREQ 25000000

#define WATCHDOG_DEV_MAX_NUM                6
#define WDG5_HI1711_BASE                    0x08768000
#define SYSCTRL_HI1711_BASE                 0x08740000
#define WDG_CSR_LOCK_OFFSET_ADDR            0xC00
#define WDG_CSR_CONTROL_OFFSET_ADDR         0x8
#define WDG_CSR_LOAD_OFFSET_ADDR            0x0
#define SYSCTRL_CSR_WD_CTRL_REG_OFFSET_ADDR 0x48
#define SYSCTRL_CSR_WD_DLY_CFG_OFFSET_ADDR  0x4C

void watchdog_sysctrl_set(uint32_t wdg_id, uint32_t wdg_mask, uint32_t rst_ctrl,
                          uint32_t rst_output, uint32_t delay_time)
{
    uint32_t bits;
    uint32_t mask;
    uint32_t tmp_value;

    /* rst_output : bit[3]
       rst_ctrl   : bit[2:1]
       mask       : bit[0] */
    bits = (wdg_mask & 0x1) | ((rst_ctrl & 0x3) << 1) | ((rst_output & 0x1) << 3); /* rst_output : bit[3] */

    bits = (bits << (wdg_id * 0x4));
    mask = (0xfu << (wdg_id * 0x4));

    /* set reset area and delay time */
    tmp_value = mmio_read_32(SYSCTRL_HI1711_BASE + SYSCTRL_CSR_WD_CTRL_REG_OFFSET_ADDR);
    tmp_value &= ~mask;
    tmp_value |= (bits & mask);
    mmio_write_32(SYSCTRL_HI1711_BASE + SYSCTRL_CSR_WD_CTRL_REG_OFFSET_ADDR, tmp_value);
    /* 配置复位延迟时间 */
    mmio_write_32(SYSCTRL_HI1711_BASE + SYSCTRL_CSR_WD_DLY_CFG_OFFSET_ADDR,
                  delay_time * (SYS_REF_CLOCK_FREQ / 1000000)); /* 1000000 = 1M */

    return;
}

void watchdog_lock(uint32_t wdg_id, uint32_t lock)
{
    /* 禁止WDG的寄存器写访问 */
    if (lock) {
        mmio_write_32(WDG5_HI1711_BASE + WDG_CSR_LOCK_OFFSET_ADDR, 0);
    } else { /* 允许WDG的寄存器写访问 */
        mmio_write_32(WDG5_HI1711_BASE + WDG_CSR_LOCK_OFFSET_ADDR, 0x1acce551);
    }

    return;
}

void watchdog_stop(void)
{
    uint32_t tmp_value;
    /* 关闭wdg */
    tmp_value = mmio_read_32(WDG5_HI1711_BASE + WDG_CSR_CONTROL_OFFSET_ADDR);
    tmp_value = tmp_value & (~(0x3));
    mmio_write_32(WDG5_HI1711_BASE + WDG_CSR_CONTROL_OFFSET_ADDR, tmp_value);

    return;
}

int watchdog_reboot(uint32_t wdg_id, uint32_t delay_time, uint32_t rst_area)
{
    int ret = 0;
    uint32_t tmp_value;

    if (wdg_id >= WATCHDOG_DEV_MAX_NUM) {
        ERROR("watchdog reboot wdg_id(0x%x) is inval.\n", wdg_id);
        return -2; /* -2 failed */
    }

    watchdog_lock(wdg_id, 0);
    watchdog_stop();
    /* 配置wdg计数器初始值 */
    mmio_write_32(WDG5_HI1711_BASE + WDG_CSR_LOAD_OFFSET_ADDR, 0);
    /* 配置系统复位域和延迟复位时间 */
    watchdog_sysctrl_set(wdg_id, 0, rst_area, 0, delay_time);
    /* 打开wdg */
    tmp_value = mmio_read_32(WDG5_HI1711_BASE + WDG_CSR_CONTROL_OFFSET_ADDR);
    tmp_value = tmp_value | (0x3);
    mmio_write_32(WDG5_HI1711_BASE + WDG_CSR_CONTROL_OFFSET_ADDR, tmp_value);
    watchdog_lock(wdg_id, 1);

    return ret;
}

static void __dead2 developer_system_reset(void)
{
    int ret = 0;

    /* set reboot delay 200 ms */
    ret = watchdog_reboot(5, 200000, 2);
    if (ret) {
        ERROR("watchdog reboot setting error %d\n", ret);
    }
    NOTICE("watchdog reboot setting success.\n");

    __asm__ volatile("isb");
    __asm__ volatile("dsb sy");
    __asm__ volatile("wfi");

    /* avoid print panic info */
    while (1) {}

    panic();
}

static void Hi1711_get_sys_suspend_power_state(psci_power_state_t *req_state)
{
    unsigned int i;

    for (i = MPIDR_AFFLVL0; i <= PLAT_MAX_PWR_LVL; i++) {
        req_state->pwr_domain_state[i] = PLAT_MAX_OFF_STATE;
    }
}

static const plat_psci_ops_t g_hikey960_psci_ops = {
    .cpu_standby = Hi1711_pwr_domain_standby,
    .pwr_domain_on = Hi1711_pwr_domain_on,                         /* 主core唤醒从core流程 */
    .pwr_domain_on_finish = Hi1711_pwr_domain_on_finish,           /* 从core上电后启动流程 */
    .pwr_domain_off = Hi1711_pwr_domain_off,                       /* 4个core下电流程，cluster下电流程/cluster不下电 */
    .pwr_domain_suspend = Hi1711_pwr_domain_suspend,               /* 1个core下电流程--包括4个core下电，cluster不下电 */
    .pwr_domain_suspend_finish = Hi1711_pwr_domain_suspend_finish, /* core唤醒后恢复上下文流程 */
    .system_off = NULL,
    .system_reset = developer_system_reset,
    .validate_power_state = Hi1711_validate_power_state,
    .validate_ns_entrypoint = Hi1711_validate_ns_entrypoint,
    .get_sys_suspend_power_state = Hi1711_get_sys_suspend_power_state,
};

int plat_setup_psci_ops(uintptr_t sec_entrypoint, const plat_psci_ops_t **psci_ops)
{
    g_hi1711_sec_entrypoint = sec_entrypoint;

    /*
     * Initialize PSCI ops struct
     */
    *psci_ops = &g_hikey960_psci_ops;
    return 0;
}
