/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Hi309x dual boot APIs implementations
 * Create: 2018-05-10
 */
#include <common.h>
#include <dm.h>
#include <asm/io.h>
#include <hi309x_common.h>
#include <command.h>
#include <hi309x_mmc.h>
#include <linux/delay.h>
#include "autoboot.h"
#include "cli.h"
#include <configs/hi309x.h>

void dboot_info_show(void)
{
    unsigned int dboot_status;

#ifdef DEBUG_EN
    printf("ChipID:%08X\n", readl(SYSCTL_CHIP_ID_BASE));
#endif
    printf("Boot Strap:%08X\n", readl(SYSCTL_STRAP_STATUS_BASE));
    printf("Reboot Control:%08X\n", readl(SYSCTL_REBOOT_CTRL_BASE));

    dboot_status = readl(SYSCTL_DBOOT_STATUS_BASE);
    printf("Boot from Area %u(boot_cnt0[%u] boot_cnt1[%u])\n",
           dboot_status & 0x1, (dboot_status >> 8) & 0xFF, /* boot_cnt0 offset [8,15], size 8bit; boot area 1bit */
           (dboot_status >> 16) & 0xFF); /* boot_cnt1 offset [16, 23], 0XFF size 8bit */
    return;
}

int print_cpuinfo(void)
{
    return 0;
}

extern unsigned int g_init_flag;

int mmc_non_dma_read(unsigned char *buf, unsigned int start, unsigned int num, unsigned int retry);

unsigned long get_cpuId(void)
{
    unsigned long cpuId;
    asm("mrs %0, mpidr_el1" : "=r"(cpuId));
    cpuId = cpuId << 48;
    cpuId = cpuId >> 56;
    return cpuId;
}

static inline void *get_gd(void)
{
	void *gd_ptr;

#ifdef CONFIG_ARM64
	/*
	 * Make will already error that reserving x18 is not supported at the
	 * time of writing, clang: error: unknown argument: '-ffixed-x18'
	 */
	__asm__ volatile("mov %0, x18\n" : "=r" (gd_ptr));
#else
	__asm__ volatile("mov %0, r9\n" : "=r" (gd_ptr));
#endif

	return gd_ptr;
}

#define STEP_DEBUG(_step, _val) *(unsigned int *)(CPU_DFX_ADDR + 4 * _step) = _val //0x8727FFD0 + 4 * _step
typedef void (*slave_func_call)(void);
void slave_entry(void)
{
	dsb();
    unsigned long cpuId = get_cpuId();
    slave_func_call func = NULL;
    if (cpuId == 1) {
        func = (slave_func_call)(uintptr_t)HI_CORE1_BASE_ADDR;
        func();
    } else if (cpuId == 2) {
        func = (slave_func_call)(uintptr_t)HI_CORE2_BASE_ADDR;
        func();
    } else if (cpuId == 3) {
        func = (slave_func_call)(uintptr_t)HI_CORE3_BASE_ADDR;
        func();
    } else {
    }
    while (1) {
    }
}

#define WRITE_CPU_ADDR(addr, value) (*(unsigned int *)addr = value)
#define READ_CPU_ADDR(addr) (*(unsigned int *)addr)
void master_export_data(void)
{
	u32 vbar = 0;
	WRITE_CPU_ADDR(CPU_RELEASE_GDTBL, (unsigned int)(uintptr_t)get_gd());
	asm("mrs %0, vbar_el1" : "=r"(vbar));
	WRITE_CPU_ADDR(CPU_RELEASE_VBAR, vbar);
}

static void load_bins_process(void)
{
	unsigned int start0 = EMMC_BIN_UPDATE_ADDR + 0 * USER_BIN_BLOCK_NUM;
    unsigned int start1 = EMMC_BIN_UPDATE_ADDR + 1 * USER_BIN_BLOCK_NUM; // each bin 512K
	unsigned int start2 = EMMC_BIN_UPDATE_ADDR + 2 * USER_BIN_BLOCK_NUM; // each bin 512K
    unsigned int start3 = EMMC_BIN_UPDATE_ADDR + 3 * USER_BIN_BLOCK_NUM; // each bin 512K
    master_export_data();
	unsigned char *master = (unsigned char *)HI_CORE0_BASE_ADDR;
	unsigned char *slave1 = (unsigned char *)HI_CORE1_BASE_ADDR;
	unsigned char *slave2 = (unsigned char *)HI_CORE2_BASE_ADDR;
	unsigned char *slave3 = (unsigned char *)HI_CORE3_BASE_ADDR;
    if (g_init_flag == 0) {
        emmc_init();
    }
	mmc_non_dma_change_area(0, 3);
	mmc_non_dma_read(master, start0, env_get_hex("bin0size", 1024), 2);
	mmc_non_dma_read(slave1, start1, env_get_hex("bin1size", 1024), 2);
	mmc_non_dma_read(slave2, start2, env_get_hex("bin2size", 1024), 2);
	mmc_non_dma_read(slave3, start3, env_get_hex("bin3size", 1024), 2);

	printf("copy slave to ddr ready\n");
	*(unsigned long long *)(CPU_RELEASE_ADDR) = 1;
}

int load_bins_and_run(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	writel(WDG_UNLOCK_VALUE, WDG5_LOCK_BASE);
	writel(0, WDG5_CONTROL_BASE);
	writel(WDG_LOCK_VALUE, WDG5_LOCK_BASE);
    load_bins_process();

	udelay(5);
	smp_kick_all_cpus();

    slave_func_call func = (slave_func_call)(uintptr_t)HI_CORE0_BASE_ADDR;
    func();
    return 0;
}
static const char *s_temp;
void main_loop_remain(void)
{
    printf("**************uboot start***********\n");
	autoboot_command(s_temp);
	cli_loop();
	panic("No CLI available");
}

static int do_set_boot_cmd(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
    if (argc != 2) {
        return -1;
    }
    if (strcmp(argv[1], "baremetal") == 0) {
        env_set("hi3093_baremetal", "run_all_cores");
        env_set("bootcmd", "run hi3093_baremetal");
    } else if (strcmp(argv[1], "linux") == 0) {
        env_set("hi309x_linux", "run load_ext4_kernel;run load_ext4_dtb;run load_ext4_initrd;run run_linux");
        env_set("bootcmd", "run hi309x_linux");  
    }
    run_command("saveenv", 0);
    return 0;
}

extern void master_switch_el(void (*func)(void));

void hi309x_master_main(const char *s)
{
#if defined(CONFIG_ARMV8_SWITCH_TO_EL1)
    s_temp = s;
	master_switch_el(main_loop_remain);
#endif
}

#ifndef CONFIG_HI3093_SPI
U_BOOT_CMD(
    run_all_cores, 8, 0, load_bins_and_run,
    "run all cores",
    "run all cores"
);
#endif

U_BOOT_CMD(
    setbootcmd, 8, 0, do_set_boot_cmd,
    "run all cores",
    "run all cores"
);