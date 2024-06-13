// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * PCIe host controllers common functions for HiSilicon SoCs drivers
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/version.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/irqdomain.h>
#include <linux/module.h>
#include <linux/of.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#include <linux/pci.h>
#else
#include <linux/of_pci.h>
#endif

#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/of_device.h>

#include "pcie_framework.h"
#include "pcie_hisi02.h"

struct hisi_pcie_event g_slv_err_event = { 0 };
struct hisi_pcie_event g_ecc_err_event = { 0 };

/*
 * Change mode to indicate the same reg_base to base of PCIe host configure
 * registers, base of RC configure space or base of vmid/asid context table
 */
void hisi_pcie_change_dbi_mode(struct hisi_pcie *pcie, u32 mode)
{
    u32 val;

    if (mode == PCIE_SLV_DBI_MODE) {
        /* AW pcie_slv_awmisc_info */
        val = hisi_pcie_misc_readl(pcie, PCIE_CTRL0);
        val |= PCIE_SC_SLV_DBI_ENABLE;
        hisi_pcie_misc_writel(pcie, val, PCIE_CTRL0);

        /* AR pcie_slv_armisc_info  */
        val = hisi_pcie_misc_readl(pcie, PCIE_CTRL1);
        val |= PCIE_SC_SLV_DBI_ENABLE;
        hisi_pcie_misc_writel(pcie, val, PCIE_CTRL1);
    } else if (mode == PCIE_SLV_NONE_DBI_MODE) {
        /* AW pcie_slv_awmisc_info  */
        val = hisi_pcie_misc_readl(pcie, PCIE_CTRL0);
        val &= ~PCIE_SC_SLV_DBI_ENABLE;
        hisi_pcie_misc_writel(pcie, val, PCIE_CTRL0);

        /* AR pcie_slv_armisc_info  */
        val = hisi_pcie_misc_readl(pcie, PCIE_CTRL1);
        val &= ~PCIE_SC_SLV_DBI_ENABLE;
        hisi_pcie_misc_writel(pcie, val, PCIE_CTRL1);
    }
}

static void hisi_pcie_core_reset_ctrl(struct hisi_pcie *pcie, bool reset_on)
{
    u32 value;
    u32 reg_reset_ctrl;
    u32 reg_dereset_ctrl;

    reg_reset_ctrl = PCIE_CTRL8;
    reg_dereset_ctrl = PCIE_CTRL8;

    value = hisi_pcie_misc_readl(pcie, reg_reset_ctrl);

    if (reset_on) {
        /* if core is link up, stop the ltssm state machine first */
        if (pcie->soc_ops->hisi_pcie_link_up(pcie)) {
            pcie->soc_ops->hisi_pcie_enable_ltssm(pcie, false);
        }
        hisi_pcie_misc_writel(pcie, (HIP_PCIE_CORE_RESET | value), reg_reset_ctrl);
    } else {
        hisi_pcie_misc_writel(pcie, (~HIP_PCIE_CORE_RESET) & value, reg_dereset_ctrl);
    }
}

static void hisi_pcie_assert_core_reset(struct hisi_pcie *pcie)
{
    hisi_pcie_core_reset_ctrl(pcie, PCIE_ASSERT_RESET_ON);
}

static void hisi_pcie_deassert_core_reset(struct hisi_pcie *pcie)
{
    hisi_pcie_core_reset_ctrl(pcie, PCIE_DEASSERT_RESET_ON);
}

static void hisi_pcie_spd_set(struct hisi_pcie *pcie, u32 spd)
{
    u32 val;

    val = hisi_pcie_dm_readl(pcie, PCIE_STATUS7);
    val &= ~(ALIGN_FOUR_BOTTOM);
    val |= spd;
    hisi_pcie_dm_writel(pcie, val, PCIE_STATUS7);
}

static void hisi_pcie_spd_control(struct hisi_pcie *pcie)
{
    u32 val;

    /* set link width speed control register */
    val = hisi_pcie_dm_readl(pcie, PCIE_LINK_WIDTH_SPEED_CONTROL);
    /*
     * set the Directed Speed Change field of the Link Width and Speed
     * Change Control register
     */
    val |= PORT_LOGIC_SPEED_CHANGE;
    hisi_pcie_dm_writel(pcie, val, PCIE_LINK_WIDTH_SPEED_CONTROL);
}

static void hisi_pcie_serr_enable(struct hisi_pcie *pcie, u32 enable)
{
    u32 val;

    val = hisi_pcie_dm_readl(pcie, PCIE_DM_FOR_BRDG_CTL_REG);

    if (enable) {
        val |= PCIE_SERR_ENABLE;
    } else {
        val &= ~(PCIE_SERR_ENABLE);
    }
    hisi_pcie_dm_writel(pcie, val, PCIE_DM_FOR_BRDG_CTL_REG);
}

static void hisi_pcie_reg_ro(struct hisi_pcie *pcie, u32 ro)
{
    u32 val;

    val = hisi_pcie_dm_readl(pcie, PCIE_DM_CTL_1_OFF_REG);

    if (ro) {
        val &= ~(PCIE_DBI_RO_RW_EN_BIT);
    } else {
        val |= PCIE_DBI_RO_RW_EN_BIT;
    }
    hisi_pcie_dm_writel(pcie, val, PCIE_DM_CTL_1_OFF_REG);
}

void mask_linkdown_isr(struct hisi_pcie *pcie, bool on)
{
    u32 val;

    val = hisi_pcie_misc_readl(pcie, PCIE_CTRL10);
    if (on) {
        val |= (1 << SHIFT_LINK_REQ_RST_NOT_MASK);
    } else {
        val &= ~(1 << SHIFT_LINK_REQ_RST_NOT_MASK);
    }
    hisi_pcie_misc_writel(pcie, val, PCIE_CTRL10);
}

int check_link_up(struct hisi_pcie *pcie)
{
    u32 val;
    u32 count = 0;

    while (!hip_pcie_link_up(&pcie->pp)) {
        mdelay(SLEEP_TIME_100);
        count++;
        if (count == MISC_READ_COUNTS) {
            val = hisi_pcie_misc_readl(pcie, PCIE_STATUS5);
            dev_info(pcie->pp.dev, "PCIe Link Failed! ltssm log: 0x%x\n", val);
            return 1;
        }
    }
    dev_info(pcie->pp.dev, "Link up\n");
    return 0;
}

void prepare_cfg(struct hisi_pcie *pcie)
{
    u32 val;

    val = hisi_pcie_misc_readl(pcie, PCIE_CTRL7);
    val |= PCIE_ERR_RRESP_ENABLE | PCIE_ERR_BRESP_ENABLE;
    hisi_pcie_misc_writel(pcie, val, PCIE_CTRL7);

    val = hisi_pcie_misc_readl(pcie, PCIE_CTRL11);
    val |= PCIE_BRIDGE_FLUSH_NOT_CLR;
    hisi_pcie_misc_writel(pcie, val, PCIE_CTRL11);
    val = hisi_pcie_misc_readl(pcie, PCIE_CTRL11);
    val &= ~PCIE_BRIDGE_FLUSH_NOT_CLR;
    hisi_pcie_misc_writel(pcie, val, PCIE_CTRL11);
}

void hisi_pcie_establish_link(struct hisi_pcie *pcie)
{
    int res;

    /* mask linkdown isr */
    mask_linkdown_isr(pcie, true);

    /* assert LTSSM enable */
    pcie->soc_ops->hisi_pcie_enable_ltssm(pcie, false);
    /* assert reset signals */
    hisi_pcie_assert_core_reset(pcie);

    msleep(SLEEP_TIME_10);

    hisi_pcie_deassert_core_reset(pcie);

    msleep(SLEEP_TIME_100);

    /* initialize phy */
    if (pcie->soc_ops->hisi_pcie_init_pcs) {
        pcie->soc_ops->hisi_pcie_init_pcs(pcie);
    }

    /* set target link speed */
    hisi_pcie_spd_set(pcie, TARGET_LINK_GEN2);

    /*
     * set pcie port num 0 for all controller, must be set 0 here.
     * there is a bug about ITS. ITS uses request_id(BDF) + MSI_vector to
     * establish ITS table for PCIe devices. However, PCIe host send
     * port_id + request_id + MSI_vector to ITS TRANSLATION register
     */
    if (pcie->soc_ops->hisi_hip_pcie_portnum_set) {
        pcie->soc_ops->hisi_hip_pcie_portnum_set(pcie, 0);
    }
    /*
     * Enable PCIe lane revesal to fix potential broken linkup
     * for some EP devices.
     */
    if (pcie->soc_ops->hisi_pcie_lane_reversal_set) {
        pcie->soc_ops->hisi_pcie_lane_reversal_set(pcie);
    }
    /* set link speed control */
    hisi_pcie_spd_control(pcie);

    /* setup root complex */
    hip_pcie_setup_rc(&pcie->pp);

    /* unmask linkdown isr */
    mask_linkdown_isr(pcie, false);

    /* assert LTSSM enable */
    pcie->soc_ops->hisi_pcie_enable_ltssm(pcie, true);

    if (pcie->soc_ops->hisi_pcie_gen3_config) {
        pcie->soc_ops->hisi_pcie_gen3_config(pcie);
    }
    /* check if the link is up or not */
    res = check_link_up(pcie);
    if (res) {
        return;
    }
    /*
     * add a 1s delay between linkup and enumeration, make sure
     * the EP device's configuration registers are prepared well
     */
    prepare_cfg(pcie);

    mdelay(SLEEP_TIME_1000);
}

static void hisi_pcie_set_db2_enable(struct hisi_pcie *pcie, u32 enable)
{
    u32 dbi_ctrl;

    dbi_ctrl = hisi_pcie_misc_readl(pcie, PCIE_CTRL7);
    if (enable) {
        dbi_ctrl |= PCIE_DB2_ENABLE_SHIFT;
    } else {
        dbi_ctrl &= ~PCIE_DB2_ENABLE_SHIFT;
    }
    hisi_pcie_misc_writel(pcie, dbi_ctrl, PCIE_CTRL7);
}

static void hisi_pcie_set_barmask(struct hisi_pcie *pcie)
{
    hisi_pcie_set_db2_enable(pcie, PCIE_DBI_CS2_ENABLE);
    hisi_pcie_dm_writel(pcie, pcie->pp.mem_size, PCIE_CFG_BAR0BASE);
    hisi_pcie_dm_writel(pcie, 0, PCIE_CFG_BAR1BASE);
    hisi_pcie_set_db2_enable(pcie, PCIE_DBI_CS2_DISABLE);
}

static int hisi_pcie_cfg_write_local(struct hip_pcie_port *pp, int where, int size, u32 val)
{
    u32 reg_val;
    u32 reg;
    struct hisi_pcie *pcie = to_hisi_pcie(pp);
    void *walker = &reg_val;

    walker += ((u32)where & ALIGN_TWO_BOTTOM);
    reg = (u32)where & ~ALIGN_TWO_BOTTOM;
    if (size == LOW_BYTE_4) {
        hisi_pcie_dm_writel(pcie, val, reg);
    } else if (size == LOW_BYTE_2) {
        reg_val = hisi_pcie_dm_readl(pcie, reg);
        *(u16 __force *)walker = val;
        hisi_pcie_dm_writel(pcie, reg_val, reg);
    } else if (size == LOW_BYTE_1) {
        reg_val = hisi_pcie_dm_readl(pcie, reg);
        *(u8 __force *)walker = val;
        hisi_pcie_dm_writel(pcie, reg_val, reg);
    } else {
        return PCIBIOS_BAD_REGISTER_NUMBER;
    }

    /*
     * This is a quirk. For some PCIe controller(eg, hip06), we need to
     * operate some private regs to clear aer interrupt. The AER driver
     * didn't provide interface to do this, we do this after the AER dirver
     * clear aer interrupt.
     */
    if (pcie->soc_ops->hisi_pcie_clr_extra_aer) {
        pcie->soc_ops->hisi_pcie_clr_extra_aer(pcie, where, size);
    }
    return PCIBIOS_SUCCESSFUL;
}

static void hisi_pcie_write_rc(struct hip_pcie_port *pp, u32 val, void __iomem *dbi_base)
{
    struct hisi_pcie *pcie = to_hisi_pcie(pp);
    hisi_pcie_change_dbi_mode(pcie, PCIE_SLV_DBI_MODE);
    writel(val, dbi_base);
    hisi_pcie_change_dbi_mode(pcie, PCIE_SLV_NONE_DBI_MODE);
}

int hisi_pcie_cfg_read_local(struct hip_pcie_port *pp, int where, int size, u32 *val)
{
    u32 reg;
    struct hisi_pcie *pcie = to_hisi_pcie(pp);

    reg = (u32)where & ~ALIGN_TWO_BOTTOM;

    *val = hisi_pcie_dm_readl(pcie, reg);

    if (size == LOW_BYTE_1) {
        *val = (*val >> (BYTE_BIT_CNT * ((u32)where & ADDR_MASK_OFFSET_3))) & 0xff;
    } else if (size == LOW_BYTE_2) {
        *val = (*val >> (BYTE_BIT_CNT * ((u32)where & ADDR_MASK_OFFSET_3))) & 0xffff;
    } else if (size != LOW_BYTE_4) {
        return PCIBIOS_BAD_REGISTER_NUMBER;
    }
    return PCIBIOS_SUCCESSFUL;
}

void hisi_pcie_read_rc(struct hip_pcie_port *pp, void __iomem *dbi_base, u32 *val)
{
    struct hisi_pcie *pcie = to_hisi_pcie(pp);


    hisi_pcie_change_dbi_mode(pcie, PCIE_SLV_DBI_MODE);
    *val = readl(dbi_base);
    hisi_pcie_change_dbi_mode(pcie, PCIE_SLV_NONE_DBI_MODE);
}

static void hisi_pcie_scan_bus(struct hip_pcie_port *pp)
{
    struct hisi_pcie *hisi_pcie = to_hisi_pcie(pp);
    struct pci_bus *b = NULL;
    struct pci_dev *dev;

    b = pci_find_next_bus(b);
    while (b != NULL) {
        if (pp == b->sysdata) {
            break;
        }
        b = pci_find_next_bus(b);
    }

    if (!b) {
        return;
    }

    dev = pci_get_slot(b, 0);
    if (!dev) {
        return;
    }

    hisi_pcie->soc_ops->aer_cap = pci_find_ext_capability(dev, PCI_EXT_CAP_ID_ERR);
    pci_dev_put(dev);
}

int hisi_pcie_link_up(struct hip_pcie_port *pp)
{
    struct hisi_pcie *hisi_pcie = to_hisi_pcie(pp);

    return hisi_pcie->soc_ops->hisi_pcie_link_up(hisi_pcie);
}

static void hisi_pcie_prog_outbound_atu(struct hisi_pcie *pcie, int index, int type, struct pcie_atu_info *pcie_atu)
{
    hisi_pcie_dm_writel(pcie, HIP02_PCIE_ATU_REGION_OUTBOUND | index, HIP02_PCIE_ATU_VIEWPORT);
    hisi_pcie_dm_writel(pcie, lower_32_bits(pcie_atu->cpu_addr), HIP02_PCIE_ATU_LOWER_BASE);
    hisi_pcie_dm_writel(pcie, upper_32_bits(pcie_atu->cpu_addr), HIP02_PCIE_ATU_UPPER_BASE);
    hisi_pcie_dm_writel(pcie, lower_32_bits(pcie_atu->cpu_addr + pcie_atu->size - 1), HIP02_PCIE_ATU_LIMIT);
    hisi_pcie_dm_writel(pcie, lower_32_bits(pcie_atu->pci_addr), HIP02_PCIE_ATU_LOWER_TARGET);
    hisi_pcie_dm_writel(pcie, upper_32_bits(pcie_atu->pci_addr), HIP02_PCIE_ATU_UPPER_TARGET);
    hisi_pcie_dm_writel(pcie, type, HIP02_PCIE_ATU_CR1);
    hisi_pcie_dm_writel(pcie, HIP02_PCIE_ATU_ENABLE, HIP02_PCIE_ATU_CR2);
}

int hisi_pcie_cfg_read(const void __iomem *addr, int where, int size, u32 *val)
{
    *val = readl(addr);

    if (size == LOW_BYTE_1) {
        *val = (*val >> (BYTE_BIT_CNT * ((u32)where & ADDR_MASK_OFFSET_3))) & 0xff;
    } else if (size == LOW_BYTE_2) {
        *val = (*val >> (BYTE_BIT_CNT * ((u32)where & ADDR_MASK_OFFSET_3))) & 0xffff;
    } else if (size != LOW_BYTE_4) {
        return PCIBIOS_BAD_REGISTER_NUMBER;
    }
    return PCIBIOS_SUCCESSFUL;
}

static void hisi_pcie_rw_other_conf_prepare(struct hip_pcie_port *pp, struct pci_bus *bus,
    u32 devfn, struct pcie_atu_info *pcie_atu, void __iomem **va_cfg_base)
{
    int type;

    pcie_atu->pci_addr = HIP02_PCIE_ATU_BUS(bus->number) | HIP02_PCIE_ATU_DEV(PCI_SLOT(devfn)) |
                        HIP02_PCIE_ATU_FUNC(PCI_FUNC(devfn));
    if (bus->parent->number == pp->root_bus_nr) {
        type = HIP02_PCIE_ATU_TYPE_CFG0;
        pcie_atu->cpu_addr = pp->cfg0_base;
        pcie_atu->size = pp->cfg0_size;
        *va_cfg_base = pp->va_cfg0_base;
    } else {
        type = HIP02_PCIE_ATU_TYPE_CFG1;
        pcie_atu->cpu_addr = pp->cfg1_base;
        pcie_atu->size = pp->cfg1_size;
        *va_cfg_base = pp->va_cfg1_base;
    }

    hisi_pcie_prog_outbound_atu(to_hisi_pcie(pp), HIP02_PCIE_ATU_REGION_INDEX1, type, pcie_atu);
}

static void hisi_pcie_rw_other_conf_end(struct hip_pcie_port *pp, struct pcie_atu_info *pcie_atu)
{
    if (pp->num_viewport <= VIEW_PORT_COUNT) {
        pcie_atu->cpu_addr = pp->io_base;
        pcie_atu->pci_addr = pp->io_bus_addr;
        pcie_atu->size = pp->io_size;
        hisi_pcie_prog_outbound_atu(to_hisi_pcie(pp), HIP02_PCIE_ATU_REGION_INDEX1, HIP02_PCIE_ATU_TYPE_IO, pcie_atu);
    }
}

static int hisi_pcie_rd_other_conf(struct hip_pcie_port *pp, struct pci_bus *bus,
                            u32 devfn, struct pcie_cfg_unit *cfg_unit, u32 *val)
{
    int ret;
    u32 address;
    void __iomem *va_cfg_base;
    struct hisi_pcie *pcie;
    struct pcie_atu_info pcie_atu;

    pcie = to_hisi_pcie(pp);
    address = (u32)(cfg_unit->where) & ~ALIGN_TWO_BOTTOM;

    hisi_pcie_rw_other_conf_prepare(pp, bus, devfn, &pcie_atu, &va_cfg_base);
    ret = hisi_pcie_cfg_read(va_cfg_base + address, cfg_unit->where, cfg_unit->size, val);
    hisi_pcie_rw_other_conf_end(pp, &pcie_atu);

    return ret;
}

int hisi_pcie_cfg_write(void __iomem *addr, int where, int size, u32 val)
{
    if (size == LOW_BYTE_4) {
        writel(val, addr);
    } else if (size == LOW_BYTE_2) {
        writew(val, addr + ((u32)where & ADDR_MASK_OFFSET_2));
    } else if (size == LOW_BYTE_1) {
        writeb(val, addr + ((u32)where & ADDR_MASK_OFFSET_3));
    } else {
        return PCIBIOS_BAD_REGISTER_NUMBER;
    }
    return PCIBIOS_SUCCESSFUL;
}

static int hisi_pcie_wr_other_conf(struct hip_pcie_port *pp, struct pci_bus *bus,
                                   u32 devfn, struct pcie_cfg_unit *cfg_unit, u32 val)
{
    int ret;
    u32 address;
    void __iomem *va_cfg_base;
    struct hisi_pcie *pcie;
    struct pcie_atu_info pcie_atu;

    pcie = to_hisi_pcie(pp);
    address = (u32)(cfg_unit->where) & ~ALIGN_TWO_BOTTOM;

    hisi_pcie_rw_other_conf_prepare(pp, bus, devfn, &pcie_atu, &va_cfg_base);
    ret = hisi_pcie_cfg_write(va_cfg_base + address, cfg_unit->where, cfg_unit->size, val);
    hisi_pcie_rw_other_conf_end(pp, &pcie_atu);

    return ret;
}

static void hisi_pcie_host_init(struct hip_pcie_port *pp)
{
    struct hisi_pcie *pcie = to_hisi_pcie(pp);
    struct pcie_atu_info pcie_atu;

    /* set controller to RC mode */
    pcie->soc_ops->hisi_pcie_mode_set(pcie);
    hisi_pcie_establish_link(pcie);

    /*
     * Some register configurations are moved from hip_pcie_host_init()
     * to hip_pcie_setup_rc(). If the PCIe link is establised by BIOS,
     * the hip_pcie_setup_rc() won't be called in hisi_pcie_establish_link(),
     * add hip_pcie_setup_rc() to be called to ensure the registers are
     * set as expected.
     * The speed-change bit need not to be set after linkup.
     */
    hip_pcie_setup_rc_base(pp);

    pcie_atu.cpu_addr = pp->mem_base;
    pcie_atu.pci_addr = pp->mem_base;
    pcie_atu.size = pp->mem_size;
    hisi_pcie_prog_outbound_atu(pcie, HIP02_PCIE_ATU_REGION_INDEX0, HIP02_PCIE_ATU_TYPE_MEM, &pcie_atu);
    if (!pp->ops->rd_other_conf) {
        pp->ops->rd_other_conf = hisi_pcie_rd_other_conf;
    }
    if (!pp->ops->wr_other_conf) {
        pp->ops->wr_other_conf = hisi_pcie_wr_other_conf;
    }
    /*
     * The default size of BAR0 in p660 host bridge is 0x10000000,
     * which will bring problem when most resource has been allocated
     * to BAR0 in host bridge.However, we need not use BAR0 in host bridge
     * in RC mode. Here we just disable it
     */
    hisi_pcie_set_barmask(pcie);
    /*
     * We need to initialize some private regs to enable aer interrupt.
     */
    if (pcie->soc_ops->hisi_pcie_enable_aer) {
        pcie->soc_ops->hisi_pcie_enable_aer(pcie);
    }
}

static int hisi_add_hip_pcie_port(struct hip_pcie_port *pp, struct platform_device *pdev)
{
    int ret;
    u32 port_id = 0;
    struct hisi_pcie *hisi_pcie = to_hisi_pcie(pp);

    if (of_property_read_u32(pdev->dev.of_node, "port-id", &port_id)) {
        dev_err(&pdev->dev, "failed to read port-id\n");
        return -EINVAL;
    }
    if (port_id > PORT_ID_LIMIT) {
        dev_err(&pdev->dev, "Invalid port-id: %u\n", port_id);
        return -EINVAL;
    }
    hisi_pcie->port_id = port_id;

    ret = hip_pcie_host_init(pp);
    if (ret) {
        dev_err(&pdev->dev, "failed to initialize host\n");
        return ret;
    }

    if (hisi_pcie->soc_ops->populate_ctrl_sysfs) {
        hisi_pcie->soc_ops->populate_ctrl_sysfs(hisi_pcie);
    }
    if (hisi_pcie->soc_ops->create_err_thread) {
        hisi_pcie->soc_ops->create_err_thread(hisi_pcie);
    }

    return 0;
}

void deal_ram_ecc(struct hisi_pcie *phisi_pcie)
{
    unsigned int value;

    value = hisi_pcie_misc_readl(phisi_pcie, PCIE_ECC_INT);
    hisi_pcie_misc_writel(phisi_pcie, value, PCIE_ECC_INT);

    g_ecc_err_event.cnt = g_ecc_err_event.cnt + 1;
    if ((g_ecc_err_event.cnt <= MAX_LOG_PRT_CNT) && (time_after(jiffies, g_ecc_err_event.timeout))) {
        g_ecc_err_event.timeout = jiffies + HZ;  /* 1 second  */
        dev_err(phisi_pcie->pp.dev, "pcie_rc ram ecc 0x%x int counts %u", value, g_ecc_err_event.cnt);
    }
}

void deal_slv_err_int(struct hisi_pcie *phisi_pcie)
{
    unsigned int value;

    value = hisi_pcie_misc_readl(phisi_pcie, PCIE_CTRL11);
    value |= (1 << SHIFT_SLV_ERR_INT);
    hisi_pcie_misc_writel(phisi_pcie, value, PCIE_CTRL11);
    value &= ~(1 << SHIFT_SLV_ERR_INT);
    hisi_pcie_misc_writel(phisi_pcie, value, PCIE_CTRL11);
    g_slv_err_event.cnt = g_slv_err_event.cnt + 1;
    if ((g_slv_err_event.cnt <= MAX_LOG_PRT_CNT) && (time_after(jiffies, g_slv_err_event.timeout))) {
        g_slv_err_event.timeout = jiffies + HZ;  /* 1 second  */
        dev_err(phisi_pcie->pp.dev, "pcie_rc slv err int counts %u", g_slv_err_event.cnt);
    }
}

unsigned int able_ltssm(struct hisi_pcie *phisi_pcie, int en)
{
    unsigned int value;
    unsigned int tmp;

    value = hisi_pcie_misc_readl(phisi_pcie, PCIE_CTRL7);
    tmp = value;
    if (en) {
        value |= (1 << SHIFT_LTSSM_ENABLE);
    } else {
        value &= (~(1 << SHIFT_LTSSM_ENABLE));
    }
    hisi_pcie_misc_writel(phisi_pcie, value, PCIE_CTRL7);

    return (tmp >> SHIFT_LTSSM_ENABLE) & 0x1;
}

void deal_bridge_flush(struct hisi_pcie *phisi_pcie)
{
    unsigned int value;
    unsigned int i;

    value = hisi_pcie_misc_readl(phisi_pcie, PCIE_STATUS1);
    if (value & (1 << SHIFT_BRIDGE_FLUSH_NREG)) {
        value = hisi_pcie_misc_readl(phisi_pcie, PCIE_CTRL11);
        value |= (1 << SHIFT_BRIDGE_FLUSH_NCLR);
        hisi_pcie_misc_writel(phisi_pcie, value, PCIE_CTRL11);
        value &= ~(1 << SHIFT_BRIDGE_FLUSH_NCLR);
        hisi_pcie_misc_writel(phisi_pcie, value, PCIE_CTRL11);
    }

    for (i = 0; i < DEC_NUM_3; i++) {
        value = hisi_pcie_misc_readl(phisi_pcie, PCIE_STATUS1);
        if (value & (1 << SHIFT_BRIDGE_FLUSH_NREG)) {
            break;
        }

        value = hisi_pcie_misc_readl(phisi_pcie, PCIE_CTRL11);
        value |= (1 << SHIFT_BRIDGE_FLUSH_NCLR);
        hisi_pcie_misc_writel(phisi_pcie, value, PCIE_CTRL11);
        value &= ~(1 << SHIFT_BRIDGE_FLUSH_NCLR);
        hisi_pcie_misc_writel(phisi_pcie, value, PCIE_CTRL11);
    }
    if (i == DEC_NUM_3) {
        dev_err(phisi_pcie->pp.dev, "pcie_rc bridge_flush_not is 0 for 3 counts");
    }
}

void deal_rest_pcie(struct hisi_pcie *phisi_pcie)
{
    unsigned int value;

    value = hisi_pcie_misc_readl(phisi_pcie, PCIE_CTRL8);
    value |= (1 << SHIFT_POWER_UP_RSTEN);
    hisi_pcie_misc_writel(phisi_pcie, value, PCIE_CTRL8);
    value &= ~(1 << SHIFT_POWER_UP_RSTEN);
    hisi_pcie_misc_writel(phisi_pcie, value, PCIE_CTRL8);
}

void clear_linkdown_status(struct hisi_pcie *phisi_pcie)
{
    unsigned int value;

    value = hisi_pcie_misc_readl(phisi_pcie, PCIE_CTRL11);
    value |= (1 << SHIFT_LINK_REQ_RST_NOT_MASK);
    hisi_pcie_misc_writel(phisi_pcie, value, PCIE_CTRL11);
    value &= ~(1 << SHIFT_LINK_REQ_RST_NOT_MASK);
    hisi_pcie_misc_writel(phisi_pcie, value, PCIE_CTRL11);
}

static irqreturn_t pci_linkdown_isr(int irq, void *dev_id)
{
    unsigned int status;
    struct hisi_pcie *phisi_pcie;
    unsigned int ltssm_back;

    phisi_pcie = (struct hisi_pcie *)dev_id;

    status = hisi_pcie_misc_readl(phisi_pcie, PCIE_LINKDOWN_STATUS);
    if (status & PCIE_RAM_ECC_INT) {
        deal_ram_ecc(phisi_pcie);
    }

    if (status & PCIE_SLV_ERR_INT) {
        deal_slv_err_int(phisi_pcie);
    }

    if (status & PCIE_LINK_REQ_RST_NOT) {
        mask_linkdown_isr(phisi_pcie, true);    /* mask linkdown isr */
        ltssm_back = able_ltssm(phisi_pcie, 0); /* LTSSM disable */
        deal_bridge_flush(phisi_pcie);       /* bfigde_flush_not:1 no more data; 0 has data */
        deal_rest_pcie(phisi_pcie);          /* reset pcie */
        clear_linkdown_status(phisi_pcie);   /* clear pcie linkdown status */
        mask_linkdown_isr(phisi_pcie, false);    /* unmask linkdown isr */
        (void)able_ltssm(phisi_pcie, ltssm_back); /* restore LTSSM */
    }

    return IRQ_HANDLED;
}

int init_pcie_ops(struct platform_device *pdev, struct hip_pcie_port *pp)
{
    pp->ops = devm_kzalloc(&pdev->dev, sizeof(struct pcie_host_ops), GFP_KERNEL);
    if (!pp->ops) {
        return -ENOMEM;
    }
    pp->ops->host_init = hisi_pcie_host_init;
    pp->ops->link_up = hisi_pcie_link_up;
    pp->ops->readl_rc = hisi_pcie_read_rc;
    pp->ops->writel_rc = hisi_pcie_write_rc;
    pp->ops->wr_own_conf = hisi_pcie_cfg_write_local;
    pp->ops->rd_own_conf = hisi_pcie_cfg_read_local;
    pp->ops->scan_bus = hisi_pcie_scan_bus;

    return 0;
}

int init_pcie_misc(struct platform_device *pdev, struct hisi_pcie *hisi_pcie, struct hip_pcie_port *pp)
{
    struct resource *misc;

    misc = platform_get_resource_byname(pdev, IORESOURCE_MEM, "misc");
    if (!misc) {
        dev_err(pp->dev, "miss subctrl base\n");
        return -EINVAL;
    }
    hisi_pcie->misc_base = devm_ioremap(&pdev->dev, misc->start, resource_size(misc));
    if (!hisi_pcie->misc_base) {
        dev_err(pp->dev, "cannot get misc base\n");
        return -ENOMEM;
    }
    hisi_pcie->misc_pa = misc->start;

    return 0;
}

int init_pcie_dm(struct platform_device *pdev, struct hisi_pcie *hisi_pcie, struct hip_pcie_port *pp)
{
    struct resource *reg;

    reg = platform_get_resource_byname(pdev, IORESOURCE_MEM, "rc_dbi");
    if (!reg) {
        dev_err(pp->dev, "platform get rc_dbi resource byname failed\n");
        return -EINVAL;
    }

    hisi_pcie->dm_base = devm_ioremap_resource(&pdev->dev, reg);
    if (IS_ERR(hisi_pcie->dm_base)) {
        dev_err(pp->dev, "cannot get dm base\n");
        return PTR_ERR(hisi_pcie->dm_base);
    }
    hisi_pcie->dm_pa = reg->start;
    hisi_pcie->pp.dbi_base = hisi_pcie->dm_base;

    return 0;
}

int init_pcie_phy(struct platform_device *pdev, struct hisi_pcie *hisi_pcie, struct hip_pcie_port *pp)
{
    struct resource *phy;

    phy = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pcs");
    if (!phy) {
        dev_err(pp->dev, "platform get pcs resource byname failed\n");
        return -EINVAL;
    }

    hisi_pcie->psc_base = devm_ioremap_resource(&pdev->dev, phy);
    if (IS_ERR(hisi_pcie->psc_base)) {
        dev_err(pp->dev, "cannot get pcs base\n");
        return PTR_ERR(hisi_pcie->psc_base);
    }
    hisi_pcie->psc_pa = phy->start;

    return 0;
}

void check_soc_type(struct hisi_pcie *hisi_pcie, struct hip_pcie_port *pp)
{
    if (hisi_pcie->soc_ops->soc_type == PCIE_HOST_HIP) {
        dev_err(pp->dev, "Hi1711 soc type\n");
    } else {
        dev_err(pp->dev, "unsopported soc type\n");
    }
}

int init_pci_irq(struct platform_device *pdev, struct hisi_pcie *hisi_pcie, struct hip_pcie_port *pp)
{
    unsigned int virq;
    int ret;
    struct device_node *node = pdev->dev.of_node;

    virq = irq_of_parse_and_map(node, 0);
    if (virq <= 0) {
        dev_err(pp->dev, "pcie: unable to get irq from dt0, virq=%d!", virq);
        return -EFAULT;
    }

    ret = devm_request_irq(&pdev->dev, virq, pci_linkdown_isr, 0, DEV_PCIE_RC_NAME, hisi_pcie);
    if (ret) {
        dev_err(pp->dev, "devm_request_irq%u failed %d", virq, ret);
        return ret;
    }

    virq = irq_of_parse_and_map(node, 1);
    if (virq <= 0) {
        dev_err(pp->dev, "pcie: unable to get irq from dt1, virq=%d!", virq);
        return -EFAULT;
    }

    dev_info(pp->dev, "request_irq%u for msi", virq);
    pp->msi_irq = virq;

    return 0;
}

int hisi_pcie_probe(struct platform_device *pdev)
{
    struct hisi_pcie *hisi_pcie;
    struct hip_pcie_port *pp;
    const struct of_device_id *match;
    struct device_driver *driver;
    int ret;

    dev_err(&pdev->dev, "hisi_pcie_probe\n");

    hisi_pcie = devm_kzalloc(&pdev->dev, sizeof(*hisi_pcie), GFP_KERNEL);
    if (!hisi_pcie) {
        return -ENOMEM;
    }
    pp = &hisi_pcie->pp;
    pp->dev = &pdev->dev;
    driver = (pdev->dev).driver;

    match = of_match_device(driver->of_match_table, &pdev->dev);
    if (!match) {
        dev_err(&pdev->dev, "of match device failed\n");
        return -EINVAL;
    }
    hisi_pcie->soc_ops = (struct pcie_soc_ops *)match->data;

    ret = init_pcie_ops(pdev, pp);
    if (ret) {
        return ret;
    }

    ret = init_pcie_misc(pdev, hisi_pcie, pp);
    if (ret) {
        return ret;
    }

    ret = init_pcie_dm(pdev, hisi_pcie, pp);
    if (ret) {
        return ret;
    }

    ret = init_pcie_phy(pdev, hisi_pcie, pp);
    if (ret) {
        return ret;
    }

    check_soc_type(hisi_pcie, pp);

    ret = init_pci_irq(pdev, hisi_pcie, pp);
    if (ret) {
        return ret;
    }

    ret = hisi_add_hip_pcie_port(pp, pdev);
    if (ret) {
        return ret;
    }

    platform_set_drvdata(pdev, hisi_pcie);

    hisi_pcie_serr_enable(to_hisi_pcie(pp), ENABLE);
    hisi_pcie_reg_ro(to_hisi_pcie(pp), ENABLE);

    return 0;
}

int hisi_pcie_remove(struct platform_device *platdev)
{
    struct hisi_pcie *hisi_pcie;
    struct hip_pcie_port *pp;

    hisi_pcie = platform_get_drvdata(platdev);
    if (hisi_pcie) {
        pp = &hisi_pcie->pp;
        hip_pci_free_msi_data(pp);
    }

    platform_set_drvdata(platdev, NULL);

    return 0;
}
