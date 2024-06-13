// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * pcie module interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pci.h>
#include <linux/irqreturn.h>
#include "pcie_framework.h"
#include "pcie_hisi02.h"
#include "sysctrl.h"

#define MISC_CTRL_OFFSET 0x200
#define PCIE_RST_DOMAIN_BIT 3

#define HIP05_PCIE_CLK_CTRL     0x3
#define HIP05_PCS_SERDES_STATUS 0x8108
#define HIP05_PCIE_CORE_RESET   0x1
#define LOOP_COUNT_LIMITE       300
#define RCVRY_DELAY_TIME        500
static int hisi_pcie_link_up_hip(struct hisi_pcie *pcie)
{
    u32 val;
    u32 count = 0;

    /*
     * There are three possible states of the link when
     * this code is called:
     * 1) The link is UP stably (in L0 state);
     * 2) The link is UP, but still in LTSSM training
     *     Wait for the training to finish, which should take a very short
     *     time. If the training does not finish, we return 0
     *     to indicate linkdown state. If the training does finish,
     *     the link is up and operating correctly.
     * 3) The link is down.
     */
    while (1) {
        val = hisi_pcie_misc_readl(pcie, PCIE_STATUS4);
        val = val & PCIE_LTSSM_STATE_MASK;

        /* The longest time for recovery stage could be 150ms,
         * time delay in each while loop is 500us, therefore the loop count is limited to 300.
         * Wait for training to the L0 state in the following states:
         * RCVRY_LOCK/RCVRY_SPEED/RCVRY_CFG/RCVRY_IDLE/RCVRY_EQ0/RCVRY_EQ1/RCVRY_EQ2/RCVRY_EQ3;
         */
        if (val == PCIE_LTSSM_LINKUP_STATE) {
            return 1;
        } else if (count >= LOOP_COUNT_LIMITE) {
            return 0;
        } else if (((val >= PCIE_LTSSM_RCVRY_LOCK_STATE) && (val <= PCIE_LTSSM_RCVRY_IDLE_STATE)) ||
                   ((val >= PCIE_LTSSM_RCVRY_EQ0_STATE) && (val <= PCIE_LTSSM_RCVRY_EQ3_STATE))) {
            udelay(RCVRY_DELAY_TIME);
            count++;
            continue;
        } else {
            return 0;
        }
    }
}

static void hisi_pcie_enable_ltssm_hip(struct hisi_pcie *pcie, bool on)
{
    u32 val;

    val = hisi_pcie_misc_readl(pcie, PCIE_CTRL7);
    if (on) {
        val |= PCIE_LTSSM_ENABLE_SHIFT;
    } else {
        val &= ~(PCIE_LTSSM_ENABLE_SHIFT);
    }
    hisi_pcie_misc_writel(pcie, val, PCIE_CTRL7);
}

static void hisi_pcie_mode_set_hip(struct hisi_pcie *pcie)
{
    u32 val;

    val = hisi_pcie_misc_readl(pcie, PCIE_CTRL0);
    val &= (~PCIE_SC_SLV_DEVICE_TYPE_MASK);
    val |= PCIE_SC_SLV_DEVICE_TYPE_HOST;
    hisi_pcie_misc_writel(pcie, val, PCIE_CTRL0);
}

static struct pcie_soc_ops g_hip_ops = {
    &hisi_pcie_link_up_hip,
    NULL,
    &hisi_pcie_enable_ltssm_hip,
    NULL,
    NULL,
    NULL,
    &hisi_pcie_mode_set_hip,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    0,
    HIP_PCIE_CORE_RESET,
    HIP05_PCIE_CLK_CTRL,
    HIP05_PCS_SERDES_STATUS,
    PCIE_HOST_HIP
};

static const struct of_device_id g_hisi_pcie_of_match_hip[] = {
    {
        .compatible = "hisilicon,hip-pcie",
        .data = (void *)&g_hip_ops,
    },
    {},
};

MODULE_DEVICE_TABLE(of, g_hisi_pcie_of_match_hip);

static struct platform_driver g_hisi_pcie_driver_hip = {
    .driver = {
        .name = "hisi-pcie-hip",
        .owner = THIS_MODULE,
        .of_match_table = g_hisi_pcie_of_match_hip,
    },
    .probe = hisi_pcie_probe,
    .remove = hisi_pcie_remove,
};

static int __init hisi_pcie_init_hip(void)
{
    /* Set PCIe reset domain to BMC */
    int ret;
    u32 val;

    ret = sysctrl_reg_read(MISC_CTRL_OFFSET, &val);
    if (ret) {
        return ret;
    }
    val |= (1 << PCIE_RST_DOMAIN_BIT);
    (void)sysctrl_reg_write(val, MISC_CTRL_OFFSET);

    return platform_driver_register(&g_hisi_pcie_driver_hip);
}

module_init(hisi_pcie_init_hip);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("pcie host driver");