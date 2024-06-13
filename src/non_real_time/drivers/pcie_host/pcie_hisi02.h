// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * PCIe host controller driver for Hisilicon Hip02 SoCs
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _PCIE_HISI_H
#define _PCIE_HISI_H
#include "pcie_hip02_reg.h"

#define MAX_LOG_PRT_CNT 20
#define ENABLE  1
#define DISABLE 0

#define PCIE_HOST_HIP   0x1710
#define PCIE_HOST_HIP05 0x660
#define PCIE_HOST_HIP06 0x1610

#define PCIE_ASSERT_RESET_ON   true
#define PCIE_DEASSERT_RESET_ON false
#define PCIE_CLOCK_ON          true
#define PCIE_CLOCK_OFF         false

#define SLEEP_TIME_10    10
#define SLEEP_TIME_100   100
#define SLEEP_TIME_1000  1000

#define PCS_READ_COUNTS  10
#define MISC_READ_COUNTS 10

#define TARGET_LINK_GEN2 2
#define TARGET_LINK_GEN1 1

#define BYTE_BIT_CNT      8
#define ALIGN_TWO_BOTTOM  0x3
#define ALIGN_FOUR_BOTTOM 0xf

#define ADDR_MASK_OFFSET_3 3
#define ADDR_MASK_OFFSET_2 2

#define PORT_ID_LIMIT      3

#define DEC_NUM_3 3

#define PCIE_LINK_REQ_RST_NOT  1u
#define PCIE_SLV_ERR_INT      BIT(1)
#define PCIE_RAM_ECC_INT      BIT(2)
#define PCIE_DM_FOR_BRDG_CTL_REG 0x3c
#define PCIE_SERR_ENABLE         BIT(17)

#define PCIE_DM_CTL_1_OFF_REG 0x8bc
#define PCIE_DBI_RO_RW_EN_BIT 1u
enum pcie_mac_phy_rate_e {
    PCIE_GEN1 = 0, /* PCIE 1.0 */
    PCIE_GEN2 = 1, /* PCIE 2.0 */
    PCIE_GEN3 = 2  /* PCIE 3.0 */
};

#define to_hisi_pcie(x) container_of(x, struct hisi_pcie, pp)

struct hisi_pcie;

/*
 * In order to improve the maintainability, we could change the
 * configurations and acquire the state of the PCIe controller
 * by sysfs.
 */
struct pcie_ctrl_attr {
    struct attribute attr;
    ssize_t (*show)(struct hisi_pcie *, char *);
    ssize_t (*store)(struct hisi_pcie *, const char *, size_t);
};

struct pcie_ctrl {
    struct hisi_pcie *pcie;
    struct kobject kobj;
};

#define to_pcie_ctrl_attr(x) container_of(x, struct pcie_ctrl_attr, attr)
#define to_pcie_ctrl(x)      container_of(x, struct pcie_ctrl, kobj)

struct pcie_err_poll_info {
    struct task_struct *err_thread;
    u32 interval;
    u32 index;
};

struct pcie_soc_ops {
    int (*hisi_pcie_link_up)(struct hisi_pcie *pcie);
    void (*hisi_pcie_config_context)(struct hisi_pcie *pcie);
    void (*hisi_pcie_enable_ltssm)(struct hisi_pcie *pcie, bool on);
    void (*hisi_pcie_gen3_dfe_enable)(struct hisi_pcie *pcie);
    void (*hisi_pcie_init_pcs)(struct hisi_pcie *pcie);
    void (*pcie_equalization)(struct hisi_pcie *pcie);
    void (*hisi_pcie_mode_set)(struct hisi_pcie *pcie);
    void (*hisi_hip_pcie_portnum_set)(struct hisi_pcie *pcie, u32 num);
    void (*hisi_pcie_lane_reversal_set)(struct hisi_pcie *pcie);
    void (*hisi_pcie_gen3_config)(struct hisi_pcie *pcie);
    void (*hisi_pcie_clr_extra_aer)(struct hisi_pcie *pcie, int where,
                                    int size);
    void (*hisi_pcie_enable_aer)(struct hisi_pcie *pcie);
    void (*populate_ctrl_sysfs)(struct hisi_pcie *pcie);
    void (*create_err_thread)(struct hisi_pcie *pcie);
    u32 aer_cap;
    u32 pcie_pcs_core_reset;
    u32 pcie_clk_ctrl;
    u32 pcs_serdes_status;
    u32 soc_type;
};

struct hisi_pcie {
    void __iomem *misc_base;
    void __iomem *dm_base;  // dbi_base reg_base
    void __iomem *psc_base;
    void __iomem *rsv_base;

    phys_addr_t misc_pa;
    phys_addr_t dm_pa;
    phys_addr_t psc_pa;
    phys_addr_t rsv_pa;

    u32 port_id;
    struct hip_pcie_port pp;
    u64 msi_addr;
    struct pcie_soc_ops *soc_ops;
    u32 smmu_ro;
    struct pcie_err_poll_info err_poll;
};

struct hisi_pcie_event {
    unsigned long timeout;
    unsigned int cnt;
};
#define DEV_PCIE_RC_NAME "pcie_rc_fix"

void hisi_pcie_change_dbi_mode(struct hisi_pcie *pcie, u32 mode);
int hisi_pcie_probe(struct platform_device *pdev);
int hisi_pcie_remove(struct platform_device *platdev);

static inline void hisi_pcie_misc_writel(struct hisi_pcie *pcie,
                                         u32 val, u32 reg)
{
    writel(val, pcie->misc_base + reg);
}

static inline u32 hisi_pcie_misc_readl(struct hisi_pcie *pcie, u32 reg)
{
    return readl(pcie->misc_base + reg);
}

static inline void hisi_pcie_dm_writel(struct hisi_pcie *pcie,
                                       u32 val, u32 reg)
{
    hisi_pcie_change_dbi_mode(pcie, PCIE_SLV_DBI_MODE);
    writel(val, pcie->dm_base + reg);
    hisi_pcie_change_dbi_mode(pcie, PCIE_SLV_NONE_DBI_MODE);
}

static inline u32 hisi_pcie_dm_readl(struct hisi_pcie *pcie, u32 reg)
{
    hisi_pcie_change_dbi_mode(pcie, PCIE_SLV_DBI_MODE);
    return readl(pcie->dm_base + reg);
}

static inline void hisi_pcie_pcs_writel(struct hisi_pcie *pcie, u32 val, u32 reg)
{
    writel(val, pcie->psc_base + reg);
}

static inline u32 hisi_pcie_pcs_readl(struct hisi_pcie *pcie, u32 reg)
{
    return readl(pcie->psc_base + reg);
}

static inline ssize_t pcie_ctrl_attr_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
    struct pcie_ctrl_attr *ctrl_attr = to_pcie_ctrl_attr(attr);
    struct pcie_ctrl *ctrl = to_pcie_ctrl(kobj);
    struct hisi_pcie *pcie = ctrl->pcie;

    return ctrl_attr->show ? ctrl_attr->show(pcie, buf) : -EIO;
}

static inline ssize_t pcie_ctrl_attr_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t len)
{
    struct pcie_ctrl_attr *ctrl_attr = to_pcie_ctrl_attr(attr);
    struct pcie_ctrl *ctrl = to_pcie_ctrl(kobj);
    struct hisi_pcie *pcie = ctrl->pcie;

    return ctrl_attr->store ? ctrl_attr->store(pcie, buf, len) : -EIO;
}

#endif
