// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * pci driver
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "pci_fix_ker.h"
#include "sysctrl.h"
#include "host_subsys.h"
#include <linux/spinlock.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>

static int g_gen_mode = 2;
module_param(g_gen_mode, int, 0444);
MODULE_PARM_DESC(g_gen_mode, "pcie gen mode, 1 for gen1; 2 for gen2!");

unsigned int g_fix_bit = 0;
module_param(g_fix_bit, uint, 0444);
MODULE_PARM_DESC(g_fix_bit, "pcie fix bit request for linkdown isr, bit0~1 is valide, 0: don't request; 1: request !");

struct hisi_pcie_event g_slv_err_event = {0};
struct hisi_pcie_event g_ecc_err_event = {0};

struct host_all g_host_all = {0};

unsigned int g_pci0_id = 0;
unsigned int g_pci1_id = 1;

static int read_data_lock(void __iomem *pmisc)
{
    PCIE_PHYCTL pcie_phyctl;
    PCIE_PHYSTAT pcie_phystat;
    unsigned int data_out;

    pcie_phyctl.val = readl(pmisc + PCIE_PHYCTL3);
    pcie_phyctl.bits.phy_cr_read = 1;
    writel(pcie_phyctl.val, pmisc + PCIE_PHYCTL3);

    udelay(1);

    pcie_phystat.val= readl(pmisc + PCIE_PHYSTAT0);
    if (pcie_phystat.bits.phy_cr_ack == 1) {
        pcie_phystat.val = readl(pmisc + PCIE_PHYSTAT0);

        data_out = pcie_phystat.bits.phy_data_out;

        pcie_phyctl.val = readl(pmisc + PCIE_PHYCTL3);
        pcie_phyctl.bits.phy_cr_read = 0;
        writel(pcie_phyctl.val, pmisc + PCIE_PHYCTL3);
        udelay(1);
        pcie_phystat.val = readl(pmisc + PCIE_PHYSTAT0);
        if (pcie_phystat.bits.phy_cr_ack) {
            LOG(LOG_ERROR, "interface exception! ");
            return -1;
        }
        return data_out;
    }

    LOG(LOG_ERROR, "interface exception! ");

    return -1;
}

static int write_addr_lock(unsigned int cr_addr_phy, void __iomem *pmisc)
{
    PCIE_PHYCTL pcie_phyctl;
    PCIE_PHYSTAT pcie_phystat;

    pcie_phyctl.val = readl(pmisc + PCIE_PHYCTL3);
    pcie_phyctl.bits.phy_cr_data_in = cr_addr_phy;
    writel(pcie_phyctl.val, pmisc + PCIE_PHYCTL3);

    pcie_phyctl.val = readl(pmisc + PCIE_PHYCTL3);
    pcie_phyctl.bits.phy_cr_cap_addr = 1;
    writel(pcie_phyctl.val, pmisc + PCIE_PHYCTL3);
    udelay(1);

    pcie_phystat.val = readl(pmisc + PCIE_PHYSTAT0);
    if (pcie_phystat.bits.phy_cr_ack == 1) {
        pcie_phyctl.val = readl(pmisc + PCIE_PHYCTL3);
        pcie_phyctl.bits.phy_cr_cap_addr = 0;
        writel(pcie_phyctl.val, pmisc + PCIE_PHYCTL3);
        udelay(1);
    } else {
        LOG(LOG_ERROR, "interface exception! ");
        return -1;
    }

    pcie_phystat.val = readl(pmisc + PCIE_PHYSTAT0);
    if (pcie_phystat.bits.phy_cr_ack == 0) {
        return 0;
    } else if (pcie_phystat.bits.phy_cr_ack == 1) {
        LOG(LOG_ERROR, "interface exception! ");
        return -1;
    }

    return 0;
}

static int write_data_lock(u32 data, void __iomem *pmisc)
{
    PCIE_PHYCTL pcie_phyctl;
    PCIE_PHYSTAT pcie_phystat;

    pcie_phyctl.val = readl(pmisc + PCIE_PHYCTL3);
    pcie_phyctl.bits.phy_cr_data_in = data;
    writel(pcie_phyctl.val, pmisc + PCIE_PHYCTL3);

    pcie_phyctl.bits.phy_cr_cap_data = 1;
    writel(pcie_phyctl.val, pmisc + PCIE_PHYCTL3);
    udelay(1);

    pcie_phystat.val = readl(pmisc + PCIE_PHYSTAT0);
    if (pcie_phystat.bits.phy_cr_ack == 1) {
        pcie_phyctl.val = readl(pmisc + PCIE_PHYCTL3);
        pcie_phyctl.bits.phy_cr_cap_data = 0;
        writel(pcie_phyctl.val, pmisc + PCIE_PHYCTL3);
        udelay(1);
    } else {
        LOG(LOG_ERROR, "interface exception! ");
        return -1;
    }

    pcie_phystat.val = readl(pmisc + PCIE_PHYSTAT0);
    if (pcie_phystat.bits.phy_cr_ack == 0) {
        pcie_phyctl.val = readl(pmisc + PCIE_PHYCTL3);
        pcie_phyctl.bits.phy_cr_write = 1;
        writel(pcie_phyctl.val, pmisc + PCIE_PHYCTL3);
        udelay(1);
    } else if (pcie_phystat.bits.phy_cr_ack == 1) {
        LOG(LOG_ERROR, "interface exception! ");
        return -1;
    }

    pcie_phystat.val = readl(pmisc + PCIE_PHYSTAT0);
    if (pcie_phystat.bits.phy_cr_ack == 1) {
        pcie_phyctl.val = readl(pmisc + PCIE_PHYCTL3);
        pcie_phyctl.bits.phy_cr_write = 0;
        writel(pcie_phyctl.val, pmisc + PCIE_PHYCTL3);
        udelay(1);
    } else if (pcie_phystat.bits.phy_cr_ack == 0) {
        LOG(LOG_ERROR, "interface exception! ");
        return -1;
    }

    pcie_phystat.val = readl(pmisc + PCIE_PHYSTAT0);
    if (pcie_phystat.bits.phy_cr_ack == 1) {
        LOG(LOG_ERROR, "interface exception! ");
        return -1;
    }

    return 0;
}

int pcie_cr_write(u32 id, u32 cr_addr_phy, u32 cr_data)
{
    int ret;
    void __iomem *pmisc;

    if (id == 0) {
        pmisc = g_host_all.pci0.misc;
    } else {
        pmisc = g_host_all.pci1.misc;
    }

    ret = write_addr_lock(cr_addr_phy, pmisc);
    if (ret) {
        LOG(LOG_ERROR, "write_addr lock failed!");
        return -1;
    }

    ret = write_data_lock(cr_data, pmisc);
    if (ret) {
        LOG(LOG_ERROR, "write_data lock failed!");
        return -1;
    }

    return 0;
}
EXPORT_SYMBOL_GPL(pcie_cr_write);

int pcie_cr_read(u32 id, u32 cr_addr_phy)
{
    int val;
    void __iomem *pmisc;

    if (id == 0) {
        pmisc = g_host_all.pci0.misc;
    } else {
        pmisc = g_host_all.pci1.misc;
    }

    val = write_addr_lock(cr_addr_phy, pmisc);
    if (val) {
        LOG(LOG_ERROR, "write_addr lock failed!");
        return -1;
    }

    val = read_data_lock(pmisc);
    if (val == -1) {
        LOG(LOG_ERROR, "write_addr lock failed!");
        return -1;
    }

    return val;
}
EXPORT_SYMBOL_GPL(pcie_cr_read);

void pcie_dfx_regs_collect(void)
{
    unsigned int reg_val = 0;

    /* read host offset 0x24 reg */
    (void)host_subsys_reg_read(GPU_DFX_CFG_0, &reg_val);
    PCIE_COUNTER_MSG("HOST_SUBSYS(GPU_DFX_CFG_0) = 0x%x\n", reg_val);
    /* read host offset 0x28 reg */
    (void)host_subsys_reg_read(GPU_DFX_CFG_1, &reg_val);
    PCIE_COUNTER_MSG("HOST_SUBSYS(GPU_DFX_CFG_1) = 0x%x\n", reg_val);
    /* read host offset 0x194 reg */
    (void)host_subsys_reg_read(CFG_RST_FIX_EN, &reg_val);
    PCIE_COUNTER_MSG("HOST_SUBSYS(CFG_RST_FIX_EN) = 0x%x\n", reg_val);

    reg_val = readl(g_host_all.pci0.misc + PCIE_CTRL0);
    PCIE_COUNTER_MSG("PCIE0_MISC(PCIE_CTRL0), PCIe0 slv device type = 0x%x\n", reg_val);

    reg_val = readl(g_host_all.pci1.misc + PCIE_CTRL0);
    PCIE_COUNTER_MSG("PCIE1_MISC(PCIE_CTRL1), PCIe1 slv device type = 0x%x\n", reg_val);

    reg_val = readl(g_host_all.pci0.misc + PCIE_PHYCTL0);
    PCIE_COUNTER_MSG("PCIE0_MISC(PCIE_PHYCTL0), PCIe0 phy deemph and swing = 0x%x\n", reg_val);

    reg_val = readl(g_host_all.pci1.misc + PCIE_PHYCTL0);
    PCIE_COUNTER_MSG("PCIE1_MISC(PCIE_PHYCTL0), PCIe1 phy deemph and swing = 0x%x\n", reg_val);

    reg_val = readl(g_host_all.pci0.misc + PCIE_PHYCTL2);
    PCIE_COUNTER_MSG("PCIE0_MISC(PCIE_PHYCTL2), PCIe0 phy rx_eq = 0x%x\n", reg_val);

    reg_val = readl(g_host_all.pci1.misc + PCIE_PHYCTL2);
    PCIE_COUNTER_MSG("PCIE1_MISC(PCIE_PHYCTL2), PCIe1 phy rx_eq = 0x%x\n", reg_val);

    reg_val = readl(g_host_all.pci1.misc + PCIE_CTRL8);
    PCIE_COUNTER_MSG("PCIE1_MISC(PCIE_CTRL8), PCIe1 credit protect = 0x%x\n", reg_val);
}
EXPORT_SYMBOL_GPL(pcie_dfx_regs_collect);

static void deal_ram_ecc_int(void __iomem *pmisc, unsigned int *id)
{
    unsigned int value;

    value = readl(pmisc + PCIE_ECC_INT);
    writel(value, pmisc + PCIE_ECC_INT);
    g_ecc_err_event.cnt = g_ecc_err_event.cnt + 1;
    if ((g_ecc_err_event.cnt <= MAX_LOG_PRT_CNT) && (time_after(jiffies, g_ecc_err_event.timeout))) {
        g_ecc_err_event.timeout = jiffies + HZ; // 1 second
        LOG(LOG_ERROR, "pcie%u ram ecc 0x%x int counts %u", *id, value, g_ecc_err_event.cnt);
    }
}

static void deal_slv_err_int(void __iomem *pmisc, unsigned int *id)
{
    unsigned int value;

    value = readl(pmisc + PCIE_CTRL11);
    value |= (1 << SHIFT_SLV_ERR_INT);
    writel(value, (pmisc + PCIE_CTRL11));
    value &= ~(1 << SHIFT_SLV_ERR_INT);
    writel(value, (pmisc + PCIE_CTRL11));

    g_slv_err_event.cnt = g_slv_err_event.cnt + 1;
    if ((g_slv_err_event.cnt <= MAX_LOG_PRT_CNT) && (time_after(jiffies, g_slv_err_event.timeout))) {
        g_slv_err_event.timeout = jiffies + HZ; // 1 second
        LOG(LOG_ERROR, "pcie%u slv err int counts %u", *id, g_slv_err_event.cnt);
    }
}

static unsigned int able_ltssm(void __iomem *pmisc, unsigned int en)
{
    unsigned int value;
    unsigned int tmp;

    value = readl(pmisc + PCIE_CTRL7);
    tmp = value;
    if (en) {
        value |= (1 << SHIFT_LTSSM_ENABLE);
    } else {
        value &= (~(1 << SHIFT_LTSSM_ENABLE));
    }
    writel(value, (pmisc + PCIE_CTRL7));

    return (tmp >> SHIFT_LTSSM_ENABLE) & 0x1;
}

static void deal_bridge_flush(void __iomem *pmisc, unsigned int *id)
{
    unsigned int value;
    unsigned int i;

    value = readl(pmisc + PCIE_STATUS1);
    if (value & (1 << SHIFT_BRIDGE_FLUSH_NREG)) {
        value = readl(pmisc + PCIE_CTRL11);
        value |= (1 << SHIFT_BRIDGE_FLUSH_NCLR);
        writel(value, (pmisc + PCIE_CTRL11));
        value &= ~(1 << SHIFT_BRIDGE_FLUSH_NCLR);
        writel(value, (pmisc + PCIE_CTRL11));
    }

    for (i = 0; i < BIT_OFFSET_3; i++) {
        value = readl(pmisc + PCIE_STATUS1);
        if (value & (1 << SHIFT_BRIDGE_FLUSH_NREG)) {
            break;
        } else {
            value = readl(pmisc + PCIE_CTRL11);
            value |= (1 << SHIFT_BRIDGE_FLUSH_NCLR);
            writel(value, (pmisc + PCIE_CTRL11));
            value &= ~(1 << SHIFT_BRIDGE_FLUSH_NCLR);
            writel(value, (pmisc + PCIE_CTRL11));
        }
    }
    if (i == BIT_OFFSET_3) {
        LOG(LOG_ERROR, "pcie%u bridge_flush_not is 0 for 3 counts", *id);
    }
}

static void deal_reset_pcie(void __iomem *pmisc, unsigned int *id)
{
    unsigned int value;

    if (*id == 0) {
        value = readl(pmisc + PCIE_CTRL8);
        value |= (1 << SHIFT_POWER_UP_RSTEN);
        writel(value, (pmisc + PCIE_CTRL8));
        value &= ~(1 << SHIFT_POWER_UP_RSTEN);
        writel(value, (pmisc + PCIE_CTRL8));

    } else {
        value = readl(pmisc + PCIE_CTRL8);
        value |= (1 << SHIFT_POWER_UP_RSTEN);
        writel(value, (pmisc + PCIE_CTRL8));
        value &= ~(1 << SHIFT_POWER_UP_RSTEN);
        /* disable credit protect */
        value |= (0x1 << SHIFT_PCIE_NO_CREDIT_PRO_DIS);
        writel(value, (pmisc + PCIE_CTRL8));
    }
}

static void clear_linkdown_status(void __iomem *pmisc)
{
    unsigned int value;

    value = readl(pmisc + PCIE_CTRL11);
    value |= (1 << SHIFT_LINK_REQ_NCLR);
    writel(value, (pmisc + PCIE_CTRL11));
    value &= ~(1 << SHIFT_LINK_REQ_NCLR);
    writel(value, (pmisc + PCIE_CTRL11));
}

static void set_gen_mode(void __iomem *pdm)
{
    unsigned int value;

    if (GEN_1 == g_gen_mode) {
        value = readl(pdm + DM_LINK_CTRL);
        value &= (~0xF);
        value |= 0x1;
        writel(value, (pdm + DM_LINK_CTRL));
    } else if (GEN_2 == g_gen_mode) {
        LOG(LOG_PRINT, "Current mode is gen2!");
    } else {
        LOG(LOG_ERROR, "Unsupported gen mode, using default gen2");
    }
}

static unsigned int check_working_mode(unsigned int portid)
{
    int ret;
    unsigned int val;

    if (portid == 0) {
        return MODE_LEP;
    } else {
        /* PCIe1 is BMC RST domain means RC mode */
        ret = sysctrl_reg_read(MISC_CTRL, &val);
        if (ret) {
            LOG(LOG_ERROR, "sysctrl_reg_read MISC_CTRL err:%d", ret);
            /* return default value */
            return MODE_LEP;
        }
        if (val & PCIE1_RST_DOMAIN) {
            return MODE_RC;
        } else {
            return MODE_LEP;
        }
    }
}

static irqreturn_t pci_isr(int irq, void *dev_id)
{
    unsigned int status;
    unsigned int *id = NULL;
    void __iomem *pmisc;
    void __iomem *pdm;
    unsigned int mode;
    unsigned int ltssm_back;

    id = (unsigned int *)dev_id;
    LOG(LOG_PRINT, "pcie%u linkdown", (u32)*id);
    mode = check_working_mode(*id);
    if (mode == MODE_RC) {
        LOG(LOG_ERROR, "pcie%u is RC mode", (u32)*id);
        return IRQ_HANDLED;
    }

    if (*id == 0) {
        pmisc = g_host_all.pci0.misc;
        pdm = g_host_all.pci0.dm;
    } else {
        pmisc = g_host_all.pci1.misc;
        pdm = g_host_all.pci1.dm;
    }
    status = readl(pmisc + PCIE_LINKDOWN_STATUS);
    if (status & PCIE_RAM_ECC_INT) {
        deal_ram_ecc_int(pmisc, id);
    }

    if (status & PCIE_SLV_ERR_INT) {
        deal_slv_err_int(pmisc, id);
    }

    if (status & PCIE_LINK_REQ_RST_NOT) {
        ltssm_back = able_ltssm(pmisc, DISABLE_LTSSM);   /* LTSSM disable */
        deal_bridge_flush(pmisc, id);       /* brigde_flush_not:1 no more data; 0 has data */
        deal_reset_pcie(pmisc, id);         /* reset pcie */
        clear_linkdown_status(pmisc);       /* clear pcie linkdown status */
        set_gen_mode(pdm);                  /* set gen mode */
        (void)able_ltssm(pmisc, ltssm_back);    /* restore LTSSM */
    }

    return IRQ_HANDLED;
}

static void check_and_unmap(void __iomem **addr)
{
    if (*addr) {
        iounmap(*addr);
    }
    *addr = NULL;
}

static int deal_ioremap_nocache(void)
{
    // pcie_MISC
    g_host_all.pci0.misc = ioremap(HI1711_PCI0_MISC_BASE, 0x100);
    if (g_host_all.pci0.misc == NULL) {
        LOG(LOG_ERROR, "pci0.misc ioremap err! ");
        goto ERR_PCI0_MISC;
    }

    g_host_all.pci1.misc = ioremap(HI1711_PCI1_MISC_BASE, 0x100);
    if (g_host_all.pci1.misc == NULL) {
        LOG(LOG_ERROR, "pci1.misc ioremap err! ");
        goto ERR_PCI1_MISC;
    }

    // pcie_DM
    g_host_all.pci0.dm = ioremap(HI1711_PCI0_DM_BASE, 0x1000);
    if (g_host_all.pci0.dm == NULL) {
        LOG(LOG_ERROR, "pci0.dm ioremap err! ");
        goto ERR_PCI0_DM;
    }

    g_host_all.pci1.dm = ioremap(HI1711_PCI1_DM_BASE, 0x1000);
    if (g_host_all.pci1.dm == NULL) {
        LOG(LOG_ERROR, "pci1.dm ioremap err! ");
        goto ERR_PCI1_DM;
    }

    return 0;

ERR_PCI1_DM:
    check_and_unmap(&g_host_all.pci0.dm);
ERR_PCI0_DM:
    check_and_unmap(&g_host_all.pci1.misc);
ERR_PCI1_MISC:
    check_and_unmap(&g_host_all.pci0.misc);
ERR_PCI0_MISC:

    return -ENOMEM;
}

static int pci_fix_ker_probe(struct platform_device *pdev)
{
    int ret;
    unsigned int virq0;
    unsigned int virq1;
    struct device_node *node = pdev->dev.of_node;

    (void)memset_s(&g_host_all, sizeof(struct host_all), 0, sizeof(struct host_all));

    ret = deal_ioremap_nocache();
    if (ret < 0) {
        goto EXIT;
    }

    if (g_fix_bit & PCI_0) {
        virq0 = irq_of_parse_and_map(node, 0);
        ret = request_irq(virq0, pci_isr, 0, DEV_PCIE0_NAME, &g_pci0_id);
        if (ret) {
            LOG(LOG_ERROR, "request_irq%u failed %d", virq0, ret);
            goto OUT;
        }
    }

    if (g_fix_bit & PCI_1) {
        virq1 = irq_of_parse_and_map(node, 1);
        ret = request_irq(virq1, pci_isr, 0, DEV_PCIE1_NAME, &g_pci1_id);
        if (ret) {
            LOG(LOG_ERROR, "request_irq%u failed %d", virq1, ret);
            goto ERR_IRQ_REQUEST;
        }
    }

    LOG(LOG_ERROR, "probe OK");
    return 0;

ERR_IRQ_REQUEST:
    if (g_fix_bit & 0x1) {
        free_irq(virq0, &g_pci0_id);
    }
OUT:
    check_and_unmap(&g_host_all.pci0.misc);
    check_and_unmap(&g_host_all.pci1.misc);
    check_and_unmap(&g_host_all.pci0.dm);
    check_and_unmap(&g_host_all.pci1.dm);
EXIT:

    return ret;
}

static int pci_fix_ker_remove(struct platform_device *pdev)
{
    unsigned int virq0;
    unsigned int virq1;
    struct device_node *node = pdev->dev.of_node;

    LOG(LOG_ERROR, "driver remove");

    virq0 = irq_of_parse_and_map(node, 0);
    virq1 = irq_of_parse_and_map(node, 1);

    if (g_fix_bit & 0x1) {
        free_irq(virq0, &g_pci0_id);
    }
    if (g_fix_bit & 0x2) {
        free_irq(virq1, &g_pci1_id);
    }

    check_and_unmap(&g_host_all.pci0.misc);
    check_and_unmap(&g_host_all.pci1.misc);
    check_and_unmap(&g_host_all.pci0.dm);
    check_and_unmap(&g_host_all.pci1.dm);

    (void)memset_s(&g_host_all, sizeof(struct host_all), 0, sizeof(struct host_all));

    return 0;
}

static const struct of_device_id g_pci_fix_match[] = {
    {.compatible = "hisilicon,hi1711-pci_fix"},
    {}
};

static struct platform_driver g_pci_fix_driver = {
    .driver = {
        .name = DEV_PCI_FIX_NAME_KER,
        .of_match_table = g_pci_fix_match,
    },

    .probe  = pci_fix_ker_probe,
    .remove = pci_fix_ker_remove,
};

module_platform_driver(g_pci_fix_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("pci_fix driver");