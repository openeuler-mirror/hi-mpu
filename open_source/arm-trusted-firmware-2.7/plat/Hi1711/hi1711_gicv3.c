/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: hi1711_gicv3
 * Author: xxx
 * Create: 2022-11-16
 */

#include <platform_def.h>
#include <arch_helpers.h>

#include <drivers/arm/gicv3.h>
#include <plat/common/platform.h>
#include <lib/mmio.h>
#include <common/debug.h>

#include "hi1711_private.h"

#pragma weak plat_arm_gic_driver_init
#pragma weak plat_arm_gic_init
#pragma weak plat_arm_gic_cpuif_enable
#pragma weak plat_arm_gic_cpuif_disable
#pragma weak plat_arm_gic_pcpu_init
#pragma weak plat_arm_gic_redistif_on
#pragma weak plat_arm_gic_redistif_off

static uintptr_t g_rdistif_base_addrs[PLATFORM_CORE_COUNT];

static const interrupt_prop_t g_g0_interrupt_props[] = {
    INTR_PROP_DESC(PERI_WDT5_S, GIC_HIGHEST_SEC_PRIORITY,
                   INTR_GROUP0, GIC_INTR_CFG_LEVEL),
    INTR_PROP_DESC(FIQ_SGI_S, GIC_HIGHEST_SEC_PRIORITY,
                   INTR_GROUP0, GIC_INTR_CFG_LEVEL),
};

/*
 * We save and restore the GICv3 context on system suspend. Allocate the
 * data in the designated EL3 Secure carve-out memory
 */
static gicv3_redist_ctx_t rdist_ctx __section("arm_el3_tzc_dram");
static gicv3_dist_ctx_t dist_ctx __section("arm_el3_tzc_dram");
static gicv3_its_ctx_t its_ctx __section("arm_el3_tzc_dram");

const gicv3_driver_data_t g_arm_gic_data = {
    .gicd_base = GICD_BASE,
    .gicr_base = GICR_BASE,
    .rdistif_num = PLATFORM_CORE_COUNT,
    .rdistif_base_addrs = g_rdistif_base_addrs,
    .mpidr_to_core_pos = plat_arm_calc_core_pos,
    .interrupt_props = g_g0_interrupt_props,
    .interrupt_props_num = ARRAY_SIZE(g_g0_interrupt_props),
};

void plat_arm_gic_driver_init(void)
{
#if (defined(AARCH32) && defined(IMAGE_BL32)) || (defined(IMAGE_BL31) && !defined(AARCH32))
    gicv3_driver_init(&g_arm_gic_data);
#endif
}

void plat_arm_gic_cpuif_enable(void)
{
    unsigned int proc_num;

    proc_num = plat_my_core_pos();
    gicv3_cpuif_enable(proc_num);

    return;
}

void plat_arm_gic_init(void)
{
    unsigned long long router;
    gicv3_distif_init();

    gicv3_disable_interrupt(PERI_WDT5_S, 0);
    /* Set interrupt configuration */
    gicd_set_icfgr(GICD_BASE, PERI_WDT5_S, 0x3);
    /* set wdg 5 int router to any core */
    router = mmio_read_64(GICD_BASE + GICD_IROUTER + (PERI_WDT5_S << 3)); /* PERI_WDT5_S << 3 */
    router |= 1U << IROUTER_IRM_SHIFT;
    mmio_write_64(GICD_BASE + GICD_IROUTER + (PERI_WDT5_S << 3), router); /* PERI_WDT5_S << 3 */
    mmio_read_64(GICD_BASE + GICD_IROUTER + (PERI_WDT5_S << 3)); /* PERI_WDT5_S << 3 */
    gicv3_enable_interrupt(PERI_WDT5_S, 0);

    gicv3_rdistif_init(plat_my_core_pos());
    gicv3_cpuif_enable(plat_my_core_pos());

    return;
}

void plat_arm_gic_cpuif_disable(void)
{
    unsigned int proc_num;

    proc_num = plat_my_core_pos();
    gicv3_cpuif_disable(proc_num);

    return;
}

void plat_arm_gic_pcpu_init(void)
{
    unsigned int proc_num;

    proc_num = plat_my_core_pos();
    gicv3_rdistif_init(proc_num);

    return;
}

void plat_arm_gic_redistif_on(void)
{
    unsigned int proc_num;

    proc_num = plat_my_core_pos();
    gicv3_rdistif_on(proc_num);

    return;
}

void plat_arm_gic_redistif_off(void)
{
    unsigned int proc_num;

    proc_num = plat_my_core_pos();
    gicv3_rdistif_off(proc_num);

    return;
}

void plat_arm_gic_save(void)
{
    /*
     * If an ITS is available, save its context before
     * the Redistributor using:
     * gicv3_its_save_disable gits_base, &its_ctx[i]
     * Additionnaly, an implementation-defined sequence may
     * be required to save the whole ITS state.
    */
    gicv3_its_save_disable(0x108000000, &its_ctx);

    gicv3_rdistif_save(plat_my_core_pos(), &rdist_ctx);

    gicv3_distif_save(&dist_ctx);
}

void plat_arm_gic_resume(void)
{
    gicv3_distif_init_restore(&dist_ctx);

    gicv3_rdistif_init_restore(plat_my_core_pos(), &rdist_ctx);

    gicv3_its_restore(0x108000000, &its_ctx); /* ITS restore */
}

void NMI_init_func()
{
    INFO("[%s] [%d]\n", __func__, __LINE__);
}

/*******************************************************************************
 * When the Nmi Interrupt happen, write the context of ELR_EL3,
 * and exit the exception through eret to the OS handler.
 ******************************************************************************/
uint64_t NMI_interrupt_handler(uint32_t id, uint32_t flags, void *handle, void *cookie)
{
    uint32_t linear_id = plat_core_pos_by_mpidr(read_mpidr());

    (void)flags;
    (void)handle;
    (void)cookie;

    INFO("core %d interrupt id: %d. \n", linear_id, id);

    dsb();

    return 0;
}

interrupt_handler_table_t g_interrupt_handler_table_g0s[] = {
    { PERI_smmu_spi0_s,    NMI_interrupt_handler, NMI_init_func },
    { PERI_smmu_spi1_s,    NMI_interrupt_handler, NMI_init_func },
    { PERI_smmu_spi2_s,    NMI_interrupt_handler, NMI_init_func },
    { PERI_s_wdg0_ns_spi0, NMI_interrupt_handler, NMI_init_func },
    { PERI_s_wdg0_ns_spi1, NMI_interrupt_handler, NMI_init_func },
    { IO_smmu_spi0_s,      NMI_interrupt_handler, NMI_init_func },
    { IO_smmu_spi1_s,      NMI_interrupt_handler, NMI_init_func },
    { IO_smmu_spi2_s,      NMI_interrupt_handler, NMI_init_func },
    { DVPP_smmu0_spi0_s,   NMI_interrupt_handler, NMI_init_func },
    { DVPP_smmu0_spi1_s,   NMI_interrupt_handler, NMI_init_func },
    { DVPP_smmu0_spi2_s,   NMI_interrupt_handler, NMI_init_func },
    { DVPP_smmu1_spi0_s,   NMI_interrupt_handler, NMI_init_func },
    { DVPP_smmu1_spi1_s,   NMI_interrupt_handler, NMI_init_func },
    { DVPP_smmu1_spi2_s,   NMI_interrupt_handler, NMI_init_func },
    { AICORE0_smmu_spi0_s, NMI_interrupt_handler, NMI_init_func },
    { AICORE0_smmu_spi1_s, NMI_interrupt_handler, NMI_init_func },
    { AICORE0_smmu_spi2_s, NMI_interrupt_handler, NMI_init_func },
    { AICORE1_smmu_spi0_s, NMI_interrupt_handler, NMI_init_func },
    { AICORE1_smmu_spi1_s, NMI_interrupt_handler, NMI_init_func },
    { AICORE1_smmu_spi2_s, NMI_interrupt_handler, NMI_init_func },
    { PERI_WDT5_S,         ext_int0_int_handler,  NMI_init_func },
    { FIQ_SGI_S,           ext_int0_int_handler,  NMI_init_func },
    { INTR_ID_UNAVAILABLE, NULL,                  NULL }
};

uint64_t el3_interrupt_handler(uint32_t id, uint32_t flags, void *handle, void *cookie)
{
    uint32_t gic_interrupt_ID;
    interrupt_type_handler_t interrupt_handler = NULL;
    interrupt_handler_table_t *interrupt_handler_table = NULL;

    INFO("[%s]:[%dL] FIQ Enter!\n", __func__, __LINE__);

#if ARM_GIC_ARCH == 2
    gic_interrupt_ID = gicc_read_IAR(GICC_BASE);
#else
    gic_interrupt_ID = read_icc_iar0_el1();
#endif

    INFO("[%s]:[%dL] Interrupt ID:%d.\n\r", __func__, __LINE__, gic_interrupt_ID);

    interrupt_handler_table = g_interrupt_handler_table_g0s;
    for (; interrupt_handler_table->source != INTR_ID_UNAVAILABLE;) {
        if (gic_interrupt_ID == interrupt_handler_table->source) {
            interrupt_handler = interrupt_handler_table->handler;
            break;
        }
        interrupt_handler_table++;
    }

    if (interrupt_handler != NULL) {
        (void)interrupt_handler(gic_interrupt_ID, flags, handle, cookie);
    } else {
        ERROR("[%s]:[%dL] interrupt_handler is NULL!\n\r",
              __func__, __LINE__);
        goto EXIT;
    }

EXIT:
    /*
     * A processor writes EIOR to inform the CPU interface
     * that it has completed the processing of the specified
     * Group0 interrupts
 */
#if ARM_GIC_ARCH == 2
    INFO("gicc_write_EOIR(GICD_BASE,0x%x)\n", gic_interrupt_ID);
    gicc_write_EOIR(GICC_BASE, gic_interrupt_ID);
#else
    write_icc_eoir0_el1(gic_interrupt_ID);
#endif
    dsb();
    INFO("[%s]:[%dL] FIQ exit!\n", __func__, __LINE__);

    return 0;
}

