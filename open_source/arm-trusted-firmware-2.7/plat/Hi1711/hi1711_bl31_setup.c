/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: bl31 setup
 * Author: xxx
 * Create: 2022-11-16
 */

#include <arch_helpers.h>
#include <common/bl_common.h>
#include <common/debug.h>
#include <drivers/generic_delay_timer.h>
#include <lib/mmio.h>
#include <lib/xlat_tables/xlat_tables.h>
#include <platform_def.h>
#include <../../drivers/arm/8250/uart8250.h>
#include "hi1711_def.h"
#include "hi1711_private.h"

#define BL31_COHERENT_RAM_BASE  (unsigned long)(&__COHERENT_RAM_START__)
#define BL31_COHERENT_RAM_LIMIT (unsigned long)(&__COHERENT_RAM_END__)

#define BL31_RO_BASE  (unsigned long)(&__RO_START__)
#define BL31_RO_LIMIT (unsigned long)(&__RO_END__)
#define BL31_END      (unsigned long)(&__BL31_END__)

#define SYSCTL_INITIAL_INF_REG 0x8740008UL

static const mmap_region_t g_mmap[] = {
    {IO_BASE, IO_BASE, IO_END - IO_BASE, MT_DEVICE | MT_RW | MT_SECURE},
    {0}
};

static entry_point_info_t g_bl32_ep_info;
static entry_point_info_t g_bl33_ep_info;
static console_8250_t g_console;

entry_point_info_t *bl31_plat_get_next_image_ep_info(unsigned int type)
{
    entry_point_info_t *next_image_info;

    next_image_info = (type == NON_SECURE) ? &g_bl33_ep_info : &g_bl32_ep_info;

    /* Description: In Hi1711, we  need to jump to the bl33, we need not  to return the uboot */
    return next_image_info;
}

void bl31_early_platform_setup2(hi1711_bl31_params_t *from_bl2, void *plat_params_from_bl2)
{
    generic_delay_timer_init();

    /* Initialize the console to provide early debug support,already init in  Uboot? */
    (void)console_8250_register(HI1711_UART_BASE, HI1711_UART_CLK_IN_HZ, Hi1711_BAUDRATE, &g_console);

    g_bl32_ep_info = *from_bl2->bl32_ep_info;
    g_bl33_ep_info = *from_bl2->bl33_ep_info;
}

static uint64_t get_rtos_size(void)
{
    uint32_t ddr_size;
    uint64_t rtos_size = 0;
    uint32_t other_fw_size = RTOS_BASE - DDR_BASE;

    ddr_size = (mmio_read_32(SYSCTL_INITIAL_INF_REG) >> 26) & 0x3; /* ddr_size: bit 26-27 */
    switch (ddr_size) {
        case DDR_SIZE_512M:
            rtos_size = SIZE_512M - other_fw_size;
            break;
        case DDR_SIZE_1G:
            rtos_size = SIZE_1G - other_fw_size;
            break;
        case DDR_SIZE_2G:
            rtos_size = SIZE_2G - other_fw_size;
            break;
        case DDR_SIZE_4G:
            rtos_size = SIZE_4G - other_fw_size;
            break;
        default:
            ERROR("bug:%s %d\n", __func__, __LINE__);
            break;
    }

    NOTICE("ddr_size=0x%x\n", ddr_size);

    return rtos_size;
}

static void Hi1711_setup_page_tables(void)
{
    mmap_add_region(BL31_BASE, BL31_BASE,
        BL31_LIMIT - BL31_BASE,
        MT_MEMORY | MT_RW | MT_SECURE);

    mmap_add_region(BL31_RO_BASE, BL31_RO_BASE,
        BL31_RO_LIMIT - BL31_RO_BASE,
        MT_CODE | MT_SECURE);

    mmap_add_region(BL31_COHERENT_RAM_BASE, BL31_COHERENT_RAM_BASE,
        BL31_COHERENT_RAM_LIMIT - BL31_COHERENT_RAM_BASE,
        MT_DEVICE | MT_RW | MT_SECURE);

    mmap_add_region(RTOS_BASE, RTOS_BASE,
        get_rtos_size(),
        MT_DEVICE | MT_RW | MT_SECURE);

    mmap_add(g_mmap);

    init_xlat_tables();
    enable_mmu_el3(0);
}

void bl31_plat_arch_setup(void)
{
    NOTICE("Before BL31 init mmu\n");
    Hi1711_setup_page_tables();
    NOTICE("BL31 init mmu done\n");
}

void bl31_platform_setup(void)
{
    /* Initialize the GIC driver, cpu and distributor interfaces */
    plat_arm_gic_driver_init();
    plat_arm_gic_init();
}

void bl31_plat_runtime_setup(void)
{
    uint64_t rc;
    uint64_t flags = 0;

    set_interrupt_rm_flag(flags, NON_SECURE);
    rc = register_interrupt_type_handler(INTR_TYPE_EL3, el3_interrupt_handler, flags);
    if (rc) {
        ERROR("register interrupt error!\n");
        return;
    }

    NOTICE("fiq init done !\n");
    return;
}
