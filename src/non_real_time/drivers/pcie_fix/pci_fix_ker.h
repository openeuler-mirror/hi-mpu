// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * pci driver header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _PCI_FIX_KER_H
#define _PCI_FIX_KER_H

#include "comm_knl_api.h"

#define DEV_PCI_FIX_NAME_KER "pci_fix"

#define MAX_LOG_PRT_CNT 20
#define DEV_PCIE0_NAME             "pcie0_fix"
#define DEV_PCIE1_NAME             "pcie1_fix"

#define PCI_0 0x1
#define PCI_1 0x2

#define HI1711_HP_SUB_BASE 0xCD0C000
#define HI1711_HOST_SUBSYS_BASE 0x8B20000
#define HI1711_SYSCTRL_BASE 0x8740000

#define HI1711_PCI0_MISC_BASE 0x8B21000
#define HI1711_PCI1_MISC_BASE 0xCD0E000

#define HI1711_PCI0_DM_BASE 0x8B10000
#define HI1711_PCI1_DM_BASE 0xCD10000

#define HI1711_PCI0_PCS_BASE 0x8B06000
#define HI1711_PCI1_PCS_BASE 0xCD08000

#define PCIE0_LINK_DOWN_INT_IRQ_NUM 240
#define PCIE1_LINK_DOWN_INT_IRQ_NUM 248

#define PCIE_CTRL0 0x0
#define SHIFT_SLV_DEVICE_TYPE 28

#define PCIE_ECC_INT 0x50
#define PCIE_LINKDOWN_STATUS 0x70

#define PCIE_LINK_REQ_RST_NOT 1U
#define PCIE_SLV_ERR_INT BIT(1)
#define PCIE_RAM_ECC_INT BIT(2)
#define PCIE_STATUS1 0x84
#define SHIFT_BRIDGE_FLUSH_NREG 20

#define PCIE_CTRL11 0x2C
#define SHIFT_SLV_ERR_INT 14
#define SHIFT_LINK_REQ_NCLR 12
#define SHIFT_BRIDGE_FLUSH_NCLR 11

#define PCIE_CTRL8 0x20
#define SHIFT_POWER_UP_RSTEN 12
#define SHIFT_PCIE_NO_CREDIT_PRO_DIS 17

#define PCIE_CTRL7 0x1C
#define SHIFT_LTSSM_ENABLE 11

#define HOST_RST_ENABLE 0xC
#define HOST_CLK_DISABLE 0x4

#define HP_SRST_ST2 0x4

#define DM_LINK_CTRL 0xa0
#define DM_PCIHDR_ID 0

#define MISC_CTRL 0x200
#define PCIE1_RST_DOMAIN BIT(3)

#define GEN_1 1
#define GEN_2 2

#define DISABLE_LTSSM 0
#define ENABLE_LTSSM 1

#define PCIE_PHYCTL3     (0x3c)
#define PCIE_PHYSTAT0    (0xb0)

#define BIT_OFFSET_3 3

#define GPU_DFX_CFG_0         (0x24)
#define GPU_DFX_CFG_1         (0x28)
#define CFG_RST_FIX_EN        (0x194)

#define PCIE_PHYCTL0           (0x30)
#define PCIE_PHYCTL1           (0x34)
#define PCIE_PHYCTL2           (0x38)

#define PCIE_COUNTER_MSG(format, args...) KBOX_MSG(KBOX_COUNTER, format, ##args)

enum PCIE_MODE {
    MODE_EEP = 0,
    MODE_LEP = 1,
    MODE_RC = 4
};

struct hisi_pcie_event {
    unsigned long timeout;
    unsigned int cnt;
};

struct pci_host {
    void __iomem    *misc;
    void __iomem    *dm;
    void __iomem    *pcs;
};

struct host_all {
    struct pci_host pci0;
    struct pci_host pci1;
};

typedef union {
    /* Define the struct bits */
    struct {
        u32 phy_cr_data_in               : 16; /* [15:0]  */
        u32 phy_cr_cap_data              : 1; /* [16]  */
        u32 phy_cr_cap_addr              : 1; /* [17]  */
        u32 phy_cr_write                 : 1; /* [18]  */
        u32 phy_cr_read                  : 1; /* [19]  */
        u32 pciephy_cfg_ssc_range        : 3; /* [20]  */
        u32 pciephy_cfg_ssc_ref_clk_sel  : 9; /* [31:21]  */
    } bits;

    /* Define an unsigned member */
    u32 val;
} PCIE_PHYCTL;

typedef union {
    /* Define the struct bits */
    struct {
        u32 phy_data_out          : 16; /* [15:0]  */
        u32 phy_cr_ack            : 1;  /* [16]  */
        u32 reserved              : 14; /* [30:17]  */
        u32 pciephy_rtune_ack     : 1;  /* [31]  */
    } bits;

    /* Define an unsigned member */
    u32 val;
} PCIE_PHYSTAT;

int pcie_cr_write(u32 id, u32 cr_addr_phy, u32 cr_data);
int pcie_cr_read(u32 id, u32 cr_addr_phy);
void pcie_dfx_regs_collect(void);
#endif /* _PCI_FIX_KER_H */