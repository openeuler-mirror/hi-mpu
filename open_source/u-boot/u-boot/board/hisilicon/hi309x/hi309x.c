/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Hi309x uboot platform specific APIs
 * Create: 2018-05-10
 */
#include <common.h>
#include <dm.h>
#include <asm/armv8/mmu.h>
#include <asm/io.h>
#include <hi309x_common.h>
#include "timer_core.h"
#include "bd_info.h"
#include "env.h"
#define SYSCNT_ENABLE_VAL ((1UL << 8) | 0x1)

/* 对内存重新划分，在board cfg里面新增加了bmc类型，需要兼容在此之前的单板，
下面2个数组列出了在此之前的所有单板 */
#define HOST_PCHPROTO_ESPI 0x1
#define HI309x_ENV_SIZE 512
#define PTE_BLOCK_AP_R (2 << 6)
#define PTE_BLOCK_AP_RW (0 << 6)
#define MMU_ATTR_DEVICE_NGNRNE (PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE))
#define MMU_ATTR_DEVICE (PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRE))
#define MMU_ATTR_UNCACHE_UNSHARE (PTE_BLOCK_MEMTYPE(MT_NORMAL_NC) | PTE_BLOCK_NON_SHARE)
#define MMU_ATTR_UNCACHE_SHARE (PTE_BLOCK_MEMTYPE(MT_NORMAL_NC) | PTE_BLOCK_INNER_SHARE)
#define MMU_ATTR_CACHE_UNSHARE (PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_NON_SHARE)
#define MMU_ATTR_CACHE_SHARE (PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_INNER_SHARE)
#define MMU_ACCESS_NONE (PTE_BLOCK_AP_RW)
#define MMU_ACCESS_R (PTE_BLOCK_AF | PTE_BLOCK_UXN | PTE_BLOCK_PXN | PTE_BLOCK_AP_R)
#define MMU_ACCESS_RW (PTE_BLOCK_AF | PTE_BLOCK_UXN | PTE_BLOCK_PXN | PTE_BLOCK_AP_RW)
#define MMU_ACCESS_RWX (PTE_BLOCK_AF | PTE_BLOCK_AP_RW)
#define MMU_ACCESS_RX (PTE_BLOCK_AF | PTE_BLOCK_AP_R)
#define PCIE_CTRL8 0xcd0e020
#define PCIE_CREDIT_CLOSE 0x20000

#define BOOT_FROM_SFC  0x0
#define BOOT_FROM_LBUS 0x1
#define BOOT_FROM_EMMC 0x2

static struct mm_region g_hi309x_mem_map[] = {
    {
        /* BOOT AREA */
        .virt = 0x0,
        .phys = 0x0,
        .size = 0x08000000,
        .attrs = MMU_ATTR_DEVICE_NGNRNE | MMU_ACCESS_RWX,
    },
    {
        /* ROM */
        .virt = 0x08000000,
        .phys = 0x08000000,
        .size = 0x100000,
        .attrs = MMU_ATTR_CACHE_SHARE | MMU_ACCESS_R,
    },
    {
        /* SRAM */
        .virt = 0x08100000,
        .phys = 0x08100000,
        .size = 0x100000,
        .attrs = MMU_ATTR_CACHE_SHARE | MMU_ACCESS_RWX,
    },
    {
        /* Secure SRAM */
        .virt = 0x08200000,
        .phys = 0x08200000,
        .size = 0x100000,
        .attrs = MMU_ATTR_CACHE_SHARE | MMU_ACCESS_RWX,
    },
    {
        /* Registers */
        .virt = 0x08300000,
        .phys = 0x08300000,
        .size = 0x27D00000,
        .attrs = MMU_ATTR_DEVICE_NGNRNE | MMU_ACCESS_RW,
    },
    {
        /* Local bus ,SFC */
        .virt = 0x30000000,
        .phys = 0x30000000,
        .size = 0x50000000,
        .attrs = MMU_ATTR_DEVICE_NGNRNE | MMU_ACCESS_RWX,
    },
    {
        /* DDR */
        /* List terminator */
        0,
    }
};

struct mm_region *mem_map = g_hi309x_mem_map;

/* disable  L3cache */
static void disable_l3_pctl(void)
{
    asm volatile("mrs x0, S3_0_c15_c1_4"
                 :
                 :
                 : "x0");
    asm volatile("mov x1, #0xc00"
                 :
                 :
                 : "x1");
    asm volatile("bic x0, x0, #0xc00"
                 :
                 :
                 : "x0");
    asm volatile("orr x0, x0, #0x1000"
                 :
                 :
                 : "x0");
    asm volatile("msr S3_0_c15_c1_4, x0"
                 :
                 :
                 : "x0");
}

static void close_pci_credit(void)
{
    writel(PCIE_CREDIT_CLOSE, PCIE_CTRL8);
}


/* Check if init_flag is set, if set, return 1 else return 0 */
int sys_init_flag_check(unsigned int init_flag)
{
    unsigned int sys_init_flag;

    sys_init_flag = readl(SYSCTL_A55_INFO0_BASE);
    if (sys_init_flag & init_flag) {
        return 1;
    }

    return 0;
}

/* set init_flag */
void sys_init_flag_set(unsigned int init_mask, unsigned int init_flag)
{
    unsigned int sys_init_flag;

    sys_init_flag = readl(SYSCTL_A55_INFO0_BASE);
    sys_init_flag &= ~init_mask;
    sys_init_flag |= init_flag;
    writel(sys_init_flag, SYSCTL_A55_INFO0_BASE);
}

/* get init_flag */
unsigned int sys_inif_flag_get(unsigned int mask, unsigned int bit_off)
{
    unsigned int sys_init_flag;

    sys_init_flag = readl(SYSCTL_A55_INFO0_BASE);
    sys_init_flag &= mask;
    sys_init_flag >>= bit_off;
    return sys_init_flag;
}

/* delay with timer */
void early_udelay(unsigned long usec)
{
    unsigned int count;

    count = usec * TIMER_1US - 1; /* 定时器中断计数一次0.01us 100倍表示us */
    timer_cnt(CONFIG_SYS_TIMER_NUM, count);

    return;
}

/* check boot mode,if low_temp return 1; else return 0 */
static int is_low_temp_routine(void)
{
    unsigned int ppll0_ctrl1_reg;

    ppll0_ctrl1_reg = readl(SYSCTL_PPLL0_CTRL1_BASE);
    ppll0_ctrl1_reg &= 0xFFF;

    /*
     * the value of fbdiv indicates that:
     * 0x50 -> APB 100Mhz
     * 0x3c -> APB 75Mhz (low temperature routine)
     */
    if (ppll0_ctrl1_reg == 0x3C) {
        return 1;
    }

    return 0;
}

static void low_temperature_process(void)
{
    u32 reg_val;

    if (!is_low_temp_routine()) {
        return;
    }

    /* PPLL0 works on 1.5Ghz so that lp_clk is 300Mhz, fix clock div for special modules */
    reg_val = readl(SYSCTL_USB_REF_CLK_CFG_BASE);
    reg_val &= ~(USB_PHY_CLK_DIV_MASK);
    reg_val |= (USB_PHY_CLK_DIV_LOW_TEMP);
    writel(reg_val, SYSCTL_USB_REF_CLK_CFG_BASE);
}

void system_counter_enable(void)
{
    writel(CONFIG_SYS_REF_CLOCK_FREQ, SYSCNT_FREQ_TBL_REG_BASE);
    writel(SYSCNT_ENABLE_VAL, SYSCNT_CTL_REG_BASE);
}

void system_critical_module_dis_reset(void)
{
    /* host rtc, mctp,gpu, vce, pcie1,pcie0 */
    writel(0x7f, HOST_RST_DISABLE_BASE);
}

/* SOC level init */
int mach_cpu_init(void)
{
    system_counter_enable();
    system_critical_module_dis_reset();
    return 0;
}

/* pll init */
static void cpll_init(void)
{
    /*
     * Default 0, 800Mhz for core_clk, just used for debug
     * Should be removed after AT
     */
    int cpll_fbdiv_id = 1;
    unsigned int cpll_fbdiv_tbl[] = {
        0x40, /* A55 core 800Mhz -> 25Mhz * 64 / 2 */
        0x50, /* A55 core 1Ghz -> 25Mhz * 80 / 2 */
        0x20, /* A55 core 400Mhz -> 25Mhz * 32 / 2 */
        0x10, /* A55 core 200Mhz */
    };
    unsigned int pll_clk_bypass;
    unsigned int cpll_ctrl;
    unsigned int pll_lock_st;
    unsigned int pll_lock_timeout = PLL_LOCK_TIMEOUT_US;

    /*
     * Bypass enable: SYSCTL_CLK_BYPASS_CTRL_REG
     * Set bit4(cpll_out_bypass) and bit0(core_clk_bypass)
     */
    pll_clk_bypass = readl(SYSCTL_CLK_BYPASS_CTRL_BASE);
    pll_clk_bypass |= CPLL_CLK_BYPASS_MASK;
    writel(pll_clk_bypass, SYSCTL_CLK_BYPASS_CTRL_BASE);

    /* Power down cpll: set bit 23 of cpll_ctrl_reg_1 */
    cpll_ctrl = readl(SYSCTL_CPLL_CTRL1_BASE);
    cpll_ctrl |= CPLL_PD_MASK;
    /* FBDIV set to proper value bit[11:0] */
    cpll_ctrl &= ~CPLL_FBDIV_MASK;
    cpll_ctrl |= cpll_fbdiv_tbl[cpll_fbdiv_id];
    writel(cpll_ctrl, SYSCTL_CPLL_CTRL1_BASE);

    early_udelay(5); /* delay 5 us */
    /* Power up */
    cpll_ctrl &= ~CPLL_PD_MASK;
    writel(cpll_ctrl, SYSCTL_CPLL_CTRL1_BASE);
    early_udelay(100); /* delay 100 us */

    /* Loop while PLL_LOCK_STATUS is 0 */
    do {
        pll_lock_st = readl(SYSCTL_PLL_LOCK_STATUS_BASE);
        if (pll_lock_st & CPLL_LOCK_MASK) {
            /* Pll locked */
            break;
        }

        pll_lock_timeout--;
    } while (pll_lock_timeout);

    if (pll_lock_timeout == 0) {
        uart_early_puts(CONFIG_SYS_UART_NUM, "CPLL lock timeout!\r\n");
        sys_init_flag_set(CPLL_LOCK_TIMEOUT, CPLL_LOCK_TIMEOUT);
    }

    /* Switch to PLL output */
    pll_clk_bypass &= ~CPLL_CLK_OUT_BYPASS;
    writel(pll_clk_bypass, SYSCTL_CLK_BYPASS_CTRL_BASE);
    pll_clk_bypass &= ~CPLL_CORE_CLK_BYPASS;
    writel(pll_clk_bypass, SYSCTL_CLK_BYPASS_CTRL_BASE);
}

static void ppll0_init(void)
{
    int ppll0_fbdiv_id = 0;
    unsigned int ppll0_fbdiv_tbl[] = {
        0x50, /* lp_clk 400Mhz -> 25Mhz * 80 / 5 = 400Mhz */
        0x28, /* lp_clk 200Mhz -> 25Mhz * 40 / 5 = 200Mhz */
    };
    unsigned int pll_clk_bypass;
    unsigned int ppll0_ctrl;
    unsigned int pll_lock_st;
    unsigned int pll_lock_timeout = PLL_LOCK_TIMEOUT_US;

    if (get_boot_core() == BOOT_CORE_SECURE_M3) {
        /*
         * Secure M3 has already initialized PPLL0
         * It must not be initailized twice
         */
        return;
    }

    /*
     * Bypass enable: SYSCTL_CLK_BYPASS_CTRL_REG
     * Set bit5(ppll0_out_bypass) and bit1(bus_clk_bypass)
     */
    pll_clk_bypass = readl(SYSCTL_CLK_BYPASS_CTRL_BASE);
    pll_clk_bypass |= PPLL0_CLK_BYPASS_MASK;
    writel(pll_clk_bypass, SYSCTL_CLK_BYPASS_CTRL_BASE);

    /* Power down ppll0: set bit 23 of ppll0_ctrl_reg_1 */
    ppll0_ctrl = readl(SYSCTL_PPLL0_CTRL1_BASE);
    ppll0_ctrl |= PPLL0_PD_MASK;
    /* FBDIV set to proper value bit[11:0] */
    ppll0_ctrl &= ~PPLL0_FBDIV_MASK;
    ppll0_ctrl |= ppll0_fbdiv_tbl[ppll0_fbdiv_id];
    writel(ppll0_ctrl, SYSCTL_PPLL0_CTRL1_BASE);

    early_udelay(5); /* delay 5 us */
    /* Power up */
    ppll0_ctrl &= ~PPLL0_PD_MASK;
    writel(ppll0_ctrl, SYSCTL_PPLL0_CTRL1_BASE);
    early_udelay(100); /* delay 100 us */

    /* Loop while PLL_LOCK_STATUS is 0 */
    do {
        pll_lock_st = readl(SYSCTL_PLL_LOCK_STATUS_BASE);
        if (pll_lock_st & PPLL0_LOCK_MASK) {
            /* Pll locked */
            break;
        }

        pll_lock_timeout--;
    } while (pll_lock_timeout);

    if (pll_lock_timeout == 0) {
        uart_early_puts(CONFIG_SYS_UART_NUM, "PPLL0 lock timeout!\r\n");
        sys_init_flag_set(PPLL0_LOCK_TIMEOUT, PPLL0_LOCK_TIMEOUT);
    }

    /* Switch to PLL output */
    pll_clk_bypass &= ~PPLL0_CLK_OUT_BYPASS;
    writel(pll_clk_bypass, SYSCTL_CLK_BYPASS_CTRL_BASE);
    pll_clk_bypass &= ~PPLL0_BUS_CLK_BYPASS;
    writel(pll_clk_bypass, SYSCTL_CLK_BYPASS_CTRL_BASE);
}

static void ppll1_init(void)
{
    /* PPLL1 fbdiv table: 600Mhz, 300Mhz */
    int ppll1_freq_id = 1;
    unsigned int ppll1_ctrl0_tbl[] = {
        0xBEB851F, /* 533Mhz */
        0x9000000, /* 600Mhz */
        0xC000000, /* 400Mhz */
        0xBF5C28F, /* 466Mhz */
    };
    unsigned int ppll1_ctrl1_tbl[] = {
        0x107F,   /* 533Mhz */
        0x401030, /* 600Mhz */
        0x401080, /* 400Mhz */
        0x106F,   /* 466Mhz */
    };
    unsigned int pll_clk_bypass;
    unsigned int ppll1_ctrl;
    unsigned int pll_lock_st;
    unsigned int pll_lock_timeout = PLL_LOCK_TIMEOUT_US;

    /*
     * Bypass enable: SYSCTL_CLK_BYPASS_CTRL_REG
     * Set bit2(ddrc_clk_bypass) and bit6(ppll1_out_bypass)
     */
    pll_clk_bypass = readl(SYSCTL_CLK_BYPASS_CTRL_BASE);
    pll_clk_bypass |= PPLL1_CLK_BYPASS_MASK;
    writel(pll_clk_bypass, SYSCTL_CLK_BYPASS_CTRL_BASE);

    /* Power down ppll1: set bit 23 of ppll1_ctrl_reg_1 */
    ppll1_ctrl = ppll1_ctrl1_tbl[ppll1_freq_id];
    ppll1_ctrl |= PPLL1_PD_MASK;
    writel(ppll1_ctrl, SYSCTL_PPLL1_CTRL1_BASE);
    /* Setup ppll1_ctrl_reg0 */
    writel(ppll1_ctrl0_tbl[ppll1_freq_id], SYSCTL_PPLL1_CTRL0_BASE);

    early_udelay(5); /* delay 5 us */
    /* Power up */
    ppll1_ctrl &= ~PPLL1_PD_MASK;
    writel(ppll1_ctrl, SYSCTL_PPLL1_CTRL1_BASE);
    early_udelay(100); /* delay 100 us */

    /* Loop while PLL_LOCK_STATUS is 0 */
    do {
        pll_lock_st = readl(SYSCTL_PLL_LOCK_STATUS_BASE);
        if (pll_lock_st & PPLL1_LOCK_MASK) {
            /* Pll locked */
            break;
        }

        pll_lock_timeout--;
    } while (pll_lock_timeout);

    if (pll_lock_timeout == 0) {
        uart_early_puts(CONFIG_SYS_UART_NUM, "PPLL1 lock timeout!\r\n");
        sys_init_flag_set(PPLL1_LOCK_TIMEOUT, PPLL1_LOCK_TIMEOUT);
    }

    /* Switch to PLL output */
    pll_clk_bypass &= ~PPLL1_CLK_OUT_BYPASS;
    writel(pll_clk_bypass, SYSCTL_CLK_BYPASS_CTRL_BASE);
    pll_clk_bypass &= ~PPLL1_DDRC_CLK_BYPASS;
    writel(pll_clk_bypass, SYSCTL_CLK_BYPASS_CTRL_BASE);
}

static void ppll2_init(void)
{
    unsigned int pll_clk_bypass;
    unsigned int value;

    /* set gpu_bypass_en */
    value = readl(SYSCTL_REG_BASE + 0x200);
    value |= (0x1 << 2); /* reg bit2 set 1 ，set gpu_bypass_en up */
    writel(value, SYSCTL_REG_BASE + 0x200);

    /* Only set GPU clk bypass to 0 */
    pll_clk_bypass = readl(SYSCTL_CLK_BYPASS_CTRL_BASE);
    pll_clk_bypass &= ~PPLL2_GPU_CLK_BYPASS;
    writel(pll_clk_bypass, SYSCTL_CLK_BYPASS_CTRL_BASE);
}

/* Initialize CPLL(for A55 Cluster) and PPLL1(for ddrc_clk) */
void sys_pll_init(void)
{
    if (sys_init_flag_check(PLL_INIT_DONE)) {
        /* Already initialized, skip pll configuration */
        return;
    }

    uart_early_puts(CONFIG_SYS_UART_NUM, "unPPLL init!\n");
    cpll_init();
    ppll0_init();
    ppll1_init();
    ppll2_init();
    sys_init_flag_set(PLL_INIT_MASK, PLL_INIT_DONE);
}

/*
 * BIT0 represents booting core 0 - A55 1 - secure M3
 * If booting from Secure M3
 * env bootcmd is set to run_linux(booti kernel_addr initrd_addr dtb_addr)
 * since kernel, initrd and dtb is already loaded at DDR by secure M3
 *
 */
int get_boot_core(void)
{
    unsigned int secure_en;
    unsigned int boot_strap;
    unsigned int boot_medium;
    unsigned int boot_core;

    boot_strap = readl(SYSCTL_STRAP_STATUS_BASE);
    /* Boot strap bit[1:0] indicates whether the system is booted from sfc or localbus or emmc */
    boot_medium = boot_strap & 0x3;
    /* Boot strap bit2 indicates whether the system is booted from Secure M3(1) or A55(0)
     * if strap bit[1:0] is not 10(boot from emmc) */
    boot_core = boot_strap & 0x4;
    /* Boot define bit0 indicates whether efuse secure bits are burned */
    secure_en = readl(SYSCTL_BOOT_DEFINE_BASE) & 0x1;
    if (secure_en || boot_core || (boot_medium == BOOT_FROM_EMMC)) {
        return BOOT_CORE_SECURE_M3;
    }

    return BOOT_CORE_A55;
}

unsigned int get_boot_media_type(void)
{
    unsigned int boot_media;

    boot_media = readl(SYSCTL_STRAP_STATUS_BASE) & 0x3;
    /* define BOOT mode equal reg bit01 */
    return boot_media;
}

int misc_init_r(void)
{
    int boot_core;
    unsigned int boot_media = get_boot_media_type();

    boot_core = get_boot_core();
    /* The boot core is safe m3 and boot media is sfc, use cramfs parsing */
    if ((boot_core == BOOT_CORE_SECURE_M3) && (boot_media == BOOT_MODE_SFC)) {
        printf("sfc boot mode\n");
        env_set("bootcmd", "run hi309x_sfc_fitimage_boot");
    }

    return 0;
}

void sys_reset_reason(void)
{
    return;
}

/* init uart */
#ifdef CONFIG_BOARD_EARLY_INIT_F
void uart_early_init(void)
{
    unsigned int uart_src_clk = CONFIG_SYS_APB_CLOCK_FREQ;
    struct uart_cfg cfg = {
        8,  /* Data length: 8 */
        0,  /* Stop bit: 1 */
        0,  /* Parity Enable: 0 */
        0,  /* Parity Select: NA */
        CONFIG_BAUDRATE
    };
    unsigned int lpsc_crg_div_ctrl;
    unsigned int apb_div_cnt;

    lpsc_crg_div_ctrl = readl(LP_SUBSYS_CRG_DIV_CTRL_BASE);
    lpsc_crg_div_ctrl &= ~LPSC_APB_DIV_MASK;
    apb_div_cnt = ((lpsc_crg_div_ctrl >> 20) & 0xF);  /* bit[23:20] -> apb_clk_div_cnt, 0XF is 4bit */

    low_temperature_process();
    if (sys_init_flag_check(PLL_INIT_DONE) && is_low_temp_routine()) {
        /* If chip is used under low-temperature condition and apb_div_cnt is 0x4 APB is 75Mhz */
        if (apb_div_cnt == 0x4) {
            uart_src_clk = CONFIG_SYS_APB_CLOCK_FREQ_LOW_TEMP;
        }
    }

    /* Write IOCFG to 0 for uart2 */
    writel(0x0, 0x874506c);
    writel(0x0, 0x8745070);

    (void)uart_init(CONFIG_SYS_UART_NUM, &cfg, uart_src_clk);
}

/* init timer */
static void timer_early_init(void)
{
    timer_cfg cfg;
    cfg.id = CONFIG_SYS_TIMER_NUM;
    cfg.load_l = 0;
    cfg.load_h = 0;
    cfg.bgload_l = 0;
    cfg.bgload_h = 0;
    cfg.mode = 0x02; /* Run in oneshot mode */
    cfg.prescaler = 0;                  /* Don't frequency division */
    cfg.interrupten = 0;                /* Trigger interrupt enable */
    cfg.size = 0;                       /* 1 for 64bit, 0 for 32bit */

    (void)timer_core_init(&cfg);
}

int board_early_init_f(void)
{
    uart_early_init();
	timer_early_init();
    return 0;
}
#endif
void hi309x_pcie_init(void);
int board_init(void)
{
    hi309x_pcie_init();
    disable_l3_pctl();
    close_pci_credit();
    dboot_info_show();
    sys_reset_reason();

    bdinfo_init();

    return 0;
}

/* close chip watchdog */
void wdg_disable(void)
{
    unsigned int wd_ctrl;

    /* disable all watchdog */
    writel(0x0, WDG0_CONTROL_BASE);
    writel(0x0, WDG1_CONTROL_BASE);
    writel(0x0, WDG2_CONTROL_BASE);
    writel(0x0, WDG3_CONTROL_BASE);
    writel(0x0, WDG4_CONTROL_BASE);

    /* unlock wdg5 */
    writel(WDG_UNLOCK_VALUE, WDG5_LOCK_BASE);
    /* set wdg5 timeout 60s */
    writel(WDG5_UBOOT_TIMEOUT, WDG5_FEED_BASE);
    /* enable wdg5 */
    writel(WDG_ENABLE_VALUE, WDG5_CONTROL_BASE);
    /* lock wdg5 */
    writel(WDG_LOCK_VALUE, WDG5_LOCK_BASE);

    if (BOOT_CORE_A55 == get_boot_core()) {
        /*
         * If booted from A55, sec watchdog
         * heart beat output must be disabled
         * since sec watchdog timeout will pull
         * up the heart beat outpu signal
         */
        wd_ctrl = readl(SYSCTL_WDG_CTRL_BASE);
        wd_ctrl &= ~WDG6_OUT_EN_MASK;
        writel(wd_ctrl, SYSCTL_WDG_CTRL_BASE);
    }

    return;
}

/* load uboot */
void prepare_uboot(void)
{
    unsigned long long run_addr = CONFIG_SYS_TEXT_BASE;

    if (BOOT_CORE_SECURE_M3 == get_boot_core()) {
        return;
    }
    uart_early_puts(CONFIG_SYS_UART_NUM, "Booted from A55 core\r\n");
    /*lint -e418*/
    (void)memcpy((void *)run_addr, (void *)UBOOT_COPY_ADDR, UBOOT_COPY_SIZE);

    return;
}

/* reset cpu */
void reset_cpu(void)
{
    uart_early_puts(CONFIG_SYS_UART_NUM, "reset cpu\r\n");

    /* unlock wdg5 */
    writel(WDG_UNLOCK_VALUE, WDG5_LOCK_BASE);
    /* disable wdg5 */
    writel(0, WDG5_CONTROL_BASE);
    /* set wdg5 timeout 0s */
    writel(0, WDG5_FEED_BASE);
    /* enable wdg5 */
    writel(WDG_ENABLE_VALUE, WDG5_CONTROL_BASE);
    /* lock wdg5 */
    writel(WDG_LOCK_VALUE, WDG5_LOCK_BASE);

    return;
}
