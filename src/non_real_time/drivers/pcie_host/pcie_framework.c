// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Synopsys Designware Hip02 PCIe host controller driver framework
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/irq.h>
#include <linux/irqdomain.h>
#include <linux/pci.h>
#include <linux/pci_regs.h>
#include <linux/msi.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/version.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#include <linux/kallsyms.h>
#pragma GCC diagnostic pop

#include "pcie_framework.h"

int (*ppci_remap_iospace)(const struct resource *res, phys_addr_t phys_addr);
void (*ppci_msi_unmask_irq)(struct irq_data *data);
void (*ppci_msi_mask_irq)(struct irq_data *data);
int (*pof_pci_get_host_bridge_resources)(struct device_node *dev,
                                         unsigned char busno, unsigned char bus_max,
                                         struct list_head *resources, resource_size_t *io_base);

static struct pci_ops g_hip_pcie_ops;

void set_pci_msi_enable(struct hip_pcie_port *pp)
{
    unsigned int val;
    u64 msi_target = (u64)pp->msi_data;

    writel(lower_32_bits(msi_target), pp->dbi_base + HIP02_PCIE_MSI_ADDR_LO);
    val = readl(pp->dbi_base + HIP02_PCIE_MSI_ADDR_LO);
    if (val != lower_32_bits(msi_target)) {
        dev_err(pp->dev, "lower address set failed %x\n", val);
    }

    writel(upper_32_bits(msi_target), pp->dbi_base + HIP02_PCIE_MSI_ADDR_HI);
    val = readl(pp->dbi_base + HIP02_PCIE_MSI_ADDR_HI);
    if (val != upper_32_bits(msi_target)) {
        dev_err(pp->dev, "upper address set failed %x\n", val);
    }

    writel(0xffffffff, pp->dbi_base + HIP02_PCIE_MSI_INTR0_ENABLE);
    val = readl(pp->dbi_base + HIP02_PCIE_MSI_INTR0_ENABLE);
    if (val != 0xffffffff) {
        dev_err(pp->dev, "MSI_ENABLE set failed %x\n", val);
    }

    writel(0x0, pp->dbi_base + HIP02_PCIE_MSI_INTR0_MASK);
    val = readl(pp->dbi_base + HIP02_PCIE_MSI_INTR0_MASK);
    if (val) {
        dev_err(pp->dev, "MSI_MASK set failed %x\n", val);
    }
}

int hip_pcie_cfg_read(const void __iomem *addr, int size, u32 *val)
{
    if ((uintptr_t)addr & (u32)(size - 1)) {
        *val = 0;
        return PCIBIOS_BAD_REGISTER_NUMBER;
    }

    if (size == LOW_BYTE_4) {
        *val = readl(addr);
    } else if (size == LOW_BYTE_2) {
        *val = readw(addr);
    } else if (size == LOW_BYTE_1) {
        *val = readb(addr);
    } else {
        *val = 0;
        return PCIBIOS_BAD_REGISTER_NUMBER;
    }

    return PCIBIOS_SUCCESSFUL;
}

int hip_pcie_cfg_write(void __iomem *addr, int size, u32 val)
{
    if ((uintptr_t)addr & (u32)(size - 1)) {
        return PCIBIOS_BAD_REGISTER_NUMBER;
    }

    if (size == LOW_BYTE_4) {
        writel(val, addr);
    } else if (size == LOW_BYTE_2) {
        writew(val, addr);
    } else if (size == LOW_BYTE_1) {
        writeb(val, addr);
    } else {
        return PCIBIOS_BAD_REGISTER_NUMBER;
    }

    return PCIBIOS_SUCCESSFUL;
}

static inline void hip_pcie_readl_rc(struct hip_pcie_port *pp, u32 reg, u32 *val)
{
    if (pp->ops->readl_rc) {
        pp->ops->readl_rc(pp, pp->dbi_base + reg, val);
    } else {
        *val = readl(pp->dbi_base + reg);
    }
}

static inline void hip_pcie_writel_rc(struct hip_pcie_port *pp, u32 val, u32 reg)
{
    if (pp->ops->writel_rc) {
        pp->ops->writel_rc(pp, val, pp->dbi_base + reg);
    } else {
        writel(val, pp->dbi_base + reg);
    }
}

static int hip_pcie_rd_own_conf(struct hip_pcie_port *pp, int where, int size, u32 *val)
{
    int ret;

    if (pp->ops->rd_own_conf) {
        ret = pp->ops->rd_own_conf(pp, where, size, val);
    } else {
        ret = hip_pcie_cfg_read(pp->dbi_base + where, size, val);
    }

    return ret;
}

static int hip_pcie_wr_own_conf(struct hip_pcie_port *pp, int where, int size,
                                u32 val)
{
    int ret;

    if (pp->ops->wr_own_conf) {
        ret = pp->ops->wr_own_conf(pp, where, size, val);
    } else {
        ret = hip_pcie_cfg_write(pp->dbi_base + where, size, val);
    }

    return ret;
}

static void hip_pcie_prog_outbound_atu(struct hip_pcie_port *pp, int index, int type, struct pcie_atu_info *pcie_atu)
{
    hip_pcie_writel_rc(pp, HIP02_PCIE_ATU_REGION_OUTBOUND | (u32)index, HIP02_PCIE_ATU_VIEWPORT);
    hip_pcie_writel_rc(pp, lower_32_bits(pcie_atu->cpu_addr), HIP02_PCIE_ATU_LOWER_BASE);
    hip_pcie_writel_rc(pp, upper_32_bits(pcie_atu->cpu_addr), HIP02_PCIE_ATU_UPPER_BASE);
    hip_pcie_writel_rc(pp, lower_32_bits(pcie_atu->cpu_addr + pcie_atu->size - 1), HIP02_PCIE_ATU_LIMIT);
    hip_pcie_writel_rc(pp, lower_32_bits(pcie_atu->pci_addr), HIP02_PCIE_ATU_LOWER_TARGET);
    hip_pcie_writel_rc(pp, upper_32_bits(pcie_atu->pci_addr), HIP02_PCIE_ATU_UPPER_TARGET);
    hip_pcie_writel_rc(pp, type, HIP02_PCIE_ATU_CR1);
    hip_pcie_writel_rc(pp, HIP02_PCIE_ATU_ENABLE, HIP02_PCIE_ATU_CR2);
}

int hip_pcie_link_up(struct hip_pcie_port *pp)
{
    if (pp->ops->link_up) {
        return pp->ops->link_up(pp);
    }
    return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
int get_fun_interface(struct hip_pcie_port *pp, struct list_head *pres)
{
    struct device_node *np = pp->dev->of_node;
    int ret;

    pof_pci_get_host_bridge_resources = (void *)kallsyms_lookup_name("of_pci_get_host_bridge_resources");
    if (!pof_pci_get_host_bridge_resources) {
        pr_err("cannot find <of_pci_get_host_bridge_resources> pointer\n");
        return -ENOSYS;
    }

    ret = pof_pci_get_host_bridge_resources(np, 0, 0xff, pres, &pp->io_base);
    if (ret) {
        return ret;
    }

    ppci_remap_iospace = (void *)kallsyms_lookup_name("pci_remap_iospace");
    if (!ppci_remap_iospace) {
        pr_err("cannot find <pci_remap_iospace> pointer\n");
        return -ENOSYS;
    }

    return 0;
}
#endif

void get_range_from_dts(struct hip_pcie_port *pcie_port, struct list_head *pres)
{
    struct resource_entry *win;
    struct resource_entry *tmp;

    resource_list_for_each_entry_safe(win, tmp, pres) {
        switch (resource_type(win->res)) {
            case IORESOURCE_MEM:
                pcie_port->mem = win->res;
                pcie_port->mem->name = "MEM";
                pcie_port->mem_size = resource_size(pcie_port->mem);
                pcie_port->mem_bus_addr = pcie_port->mem->start - win->offset;
                break;
            case IORESOURCE_BUS:
                pcie_port->busn = win->res;
                break;
            default:
                break;
        }
    }
}

int init_iomap_addr(struct hip_pcie_port *pp)
{
    if (!pp->dbi_base) {
        pp->dbi_base = devm_ioremap(pp->dev, pp->cfg->start, resource_size(pp->cfg));
        if (!pp->dbi_base) {
            dev_err(pp->dev, "error with ioremap\n");
            return -ENOMEM;
        }
    }

    pp->mem_base = pp->mem->start;

    if (!pp->va_cfg0_base) {
        pp->va_cfg0_base = devm_ioremap(pp->dev, pp->cfg0_base, pp->cfg0_size);
        if (!pp->va_cfg0_base) {
            dev_err(pp->dev, "error with ioremap in function\n");
            return -ENOMEM;
        }
    }

    if (!pp->va_cfg1_base) {
        pp->va_cfg1_base = devm_ioremap(pp->dev, pp->cfg1_base, pp->cfg1_size);
        if (!pp->va_cfg1_base) {
            dev_err(pp->dev, "error with ioremap\n");
            return -ENOMEM;
        }
    }

    return 0;
}

void init_lanes_viewport(struct hip_pcie_port *pp)
{
    int ret;
    struct device_node *np = pp->dev->of_node;

    ret = of_property_read_u32(np, "num-lanes", &pp->lanes);
    if (ret) {
        pp->lanes = 0;
    }

    ret = of_property_read_u32(np, "num-viewport", &pp->num_viewport);
    if (ret) {
        pp->num_viewport = VIEW_PORT_COUNT;
    }
}

int scan_pci_bus(struct hip_pcie_port *pport, struct list_head *pres)
{
    struct pci_bus *pbus, *pchild;
    u32 val;

    (void)hip_pcie_rd_own_conf(pport, HIP02_PCIE_LINK_WIDTH_SPEED_CONTROL, LOW_BYTE_4, &val);
    val |= HIP02_PORT_LOGIC_SPEED_CHANGE;
    (void)hip_pcie_wr_own_conf(pport, HIP02_PCIE_LINK_WIDTH_SPEED_CONTROL, LOW_BYTE_4, val);

    pport->root_bus_nr = pport->busn->start;
    pbus = pci_scan_root_bus(pport->dev, pport->root_bus_nr, &g_hip_pcie_ops, pport, pres);
    if (!pbus) {
        return -ENOMEM;
    }

    if (pport->ops->scan_bus) {
        pport->ops->scan_bus(pport);
    }

#ifdef CONFIG_ARM
    /* Incorrectly describe IRQs for old dtbs */
    pci_fixup_irqs(pci_common_swizzle, of_irq_parse_and_map_pci);
#endif

    if (!pci_has_flag(PCI_PROBE_ONLY)) {
        pci_bus_size_bridges(pbus);
        pci_bus_assign_resources(pbus);

        list_for_each_entry(pchild, &pbus->children, node) {
            pcie_bus_configure_settings(pchild);
        }
    }

    pci_bus_add_devices(pbus);

    return 0;
}

static void hip_msi_ack_irq(struct irq_data *d)
{
    irq_chip_ack_parent(d);
}

static void hip_msi_mask_irq(struct irq_data *d)
{
    pci_msi_mask_irq(d);
    irq_chip_mask_parent(d);
}

static void hip_msi_unmask_irq(struct irq_data *d)
{
    pci_msi_unmask_irq(d);
    irq_chip_unmask_parent(d);
}

static struct irq_chip g_hip_pcie_msi_irq_chip = {
    .name = "PCI-MSI",
    .irq_ack = hip_msi_ack_irq,
    .irq_mask = hip_msi_mask_irq,
    .irq_unmask = hip_msi_unmask_irq,
};

static struct msi_domain_info g_hip_pcie_msi_domain_info = {
    .flags = (MSI_FLAG_USE_DEF_DOM_OPS | MSI_FLAG_USE_DEF_CHIP_OPS | MSI_FLAG_PCI_MSIX | MSI_FLAG_MULTI_PCI_MSI),
    .chip = &g_hip_pcie_msi_irq_chip,
};

irqreturn_t hip_handle_msi_irq(struct hip_pcie_port *pp)
{
    int i, pos, irq;
    u32 val = 0;
    u32 num_ctrls;

    num_ctrls = pp->num_vectors / MAX_MSI_IRQS_PER_CTRL;

    for (i = 0; i < num_ctrls; i++) {
        (void)hip_pcie_rd_own_conf(pp, HIP02_PCIE_MSI_INTR0_STATUS + (i * MSI_REG_CTRL_BLOCK_SIZE), LOW_BYTE_4, &val);
        if (!val) {
            continue;
        }

        pos = 0;
        while ((pos = find_next_bit((unsigned long *) &val, MAX_MSI_IRQS_PER_CTRL, pos)) != MAX_MSI_IRQS_PER_CTRL) {
            irq = irq_find_mapping(pp->irq_domain, (i * MAX_MSI_IRQS_PER_CTRL) + pos);
            generic_handle_irq(irq);
            pos++;
        }
    }

    return IRQ_HANDLED;
}

/* Chained MSI interrupt service routine */
static void hip_chained_msi_isr(struct irq_desc *desc)
{
    struct irq_chip *chip = irq_desc_get_chip(desc);
    struct hip_pcie_port *pp;
    int ret;

    chained_irq_enter(chip, desc);

    pp = irq_desc_get_handler_data(desc);

    ret = hip_handle_msi_irq(pp);
    if (!ret) {
        dev_err(pp->dev, "Failed to handle MSI irq\n");
    }

    chained_irq_exit(chip, desc);
}

static void hip_pci_setup_msi_msg(struct irq_data *data, struct msi_msg *msg)
{
    struct hip_pcie_port *pport = irq_data_get_irq_chip_data(data);
    u64 msi_addr;

    if (pport->ops->get_msi_addr) {
        msi_addr = pport->ops->get_msi_addr(pport);
    } else {
        msi_addr = (u64)pport->msi_data;
    }

    msg->address_lo = lower_32_bits(msi_addr);
    msg->address_hi = upper_32_bits(msi_addr);

    if (pport->ops->get_msi_data) {
        msg->data = pport->ops->get_msi_data(pport, data->hwirq);
    } else {
        msg->data = data->hwirq;
    }

    dev_dbg(pport->dev, "msi#%d address_hi %#x address_lo %#x\n", (int)data->hwirq, msg->address_hi, msg->address_lo);
}

static int hip_pci_msi_set_affinity(struct irq_data *irq_data, const struct cpumask *mask, bool force)
{
    return -EINVAL;
}

static void hip_pci_bottom_mask(struct irq_data *data)
{
    struct hip_pcie_port *pp = irq_data_get_irq_chip_data(data);
    unsigned int bit, ctrl;
    unsigned long flags;

    raw_spin_lock_irqsave(&pp->lock, flags);

    if (pp->ops->msi_clear_irq) {
        pp->ops->msi_clear_irq(pp, data->hwirq);
    } else {
        ctrl = data->hwirq / MAX_MSI_IRQS_PER_CTRL;
        bit = data->hwirq % MAX_MSI_IRQS_PER_CTRL;
        pp->irq_status[ctrl] &= ~(1 << bit);
        (void)hip_pcie_wr_own_conf(pp, HIP02_PCIE_MSI_INTR0_MASK, LOW_BYTE_4, ~pp->irq_status[ctrl]);
    }

    raw_spin_unlock_irqrestore(&pp->lock, flags);
}

static void hip_pci_bottom_unmask(struct irq_data *data)
{
    struct hip_pcie_port *pp = irq_data_get_irq_chip_data(data);
    unsigned int bit, ctrl;
    unsigned long flags;

    raw_spin_lock_irqsave(&pp->lock, flags);

    if (pp->ops->msi_set_irq) {
        pp->ops->msi_set_irq(pp, data->hwirq);
    } else {
        ctrl = data->hwirq / MAX_MSI_IRQS_PER_CTRL;
        bit = data->hwirq % MAX_MSI_IRQS_PER_CTRL;

        pp->irq_status[ctrl] |= 1 << bit;
        (void)hip_pcie_wr_own_conf(pp, HIP02_PCIE_MSI_INTR0_MASK, LOW_BYTE_4, ~pp->irq_status[ctrl]);
    }

    raw_spin_unlock_irqrestore(&pp->lock, flags);
}

static void hip_pci_bottom_ack(struct irq_data *d)
{
    struct hip_pcie_port *pp  = irq_data_get_irq_chip_data(d);
    unsigned int res, bit, ctrl;
    unsigned long flags;

    ctrl = d->hwirq / MAX_MSI_IRQS_PER_CTRL;
    res = ctrl * MSI_REG_CTRL_BLOCK_SIZE;
    bit = d->hwirq % MAX_MSI_IRQS_PER_CTRL;

    raw_spin_lock_irqsave(&pp->lock, flags);

    (void)hip_pcie_wr_own_conf(pp, HIP02_PCIE_MSI_INTR0_STATUS + res, LOW_BYTE_4, 1 << bit);

    raw_spin_unlock_irqrestore(&pp->lock, flags);
}

static struct irq_chip g_hip_pci_msi_bottom_irq_chip = {
    .name = "HISI_PCI-MSI",
    .irq_ack = hip_pci_bottom_ack,
    .irq_compose_msi_msg = hip_pci_setup_msi_msg,
    .irq_set_affinity = hip_pci_msi_set_affinity,
    .irq_mask = hip_pci_bottom_mask,
    .irq_unmask = hip_pci_bottom_unmask,
};

static int hip_pcie_irq_domain_alloc(struct irq_domain *domain, unsigned int virq, unsigned int nr_irqs,
                                     void *args)
{
    struct hip_pcie_port *pp = domain->host_data;
    unsigned long flags;
    u32 i;
    int bit;

    raw_spin_lock_irqsave(&pp->lock, flags);

    bit = bitmap_find_free_region(pp->msi_irq_in_use, pp->num_vectors, order_base_2(nr_irqs));

    raw_spin_unlock_irqrestore(&pp->lock, flags);

    if (bit < 0) {
        return -ENOSPC;
    }
    dev_err(pp->dev, "bit allocated %d\n", bit);
    dev_err(pp->dev, "nr_irqs allocated %u\n", nr_irqs);
    for (i = 0; i < nr_irqs; i++) {
        irq_domain_set_info(domain, virq + i, bit + i, &g_hip_pci_msi_bottom_irq_chip,
                            pp, handle_edge_irq, NULL, NULL);
    }

    return 0;
}

static void hip_pcie_irq_domain_free(struct irq_domain *domain, unsigned int virq, unsigned int nr_irqs)
{
    struct irq_data *data;
    struct hip_pcie_port *pp;
    unsigned long flags;

    data = irq_domain_get_irq_data(domain, virq);
    if (!data) {
        return;
    }

    pp = irq_data_get_irq_chip_data(data);
    if (!pp) {
        return;
    }

    raw_spin_lock_irqsave(&pp->lock, flags);

    bitmap_release_region(pp->msi_irq_in_use, data->hwirq, order_base_2(nr_irqs));

    raw_spin_unlock_irqrestore(&pp->lock, flags);
}

static const struct irq_domain_ops g_hip_pcie_msi_domain_ops = {
    .alloc = hip_pcie_irq_domain_alloc,
    .free = hip_pcie_irq_domain_free,
};

int hip_pcie_allocate_domains(struct hip_pcie_port *pp)
{
    struct fwnode_handle *fwnode = of_node_to_fwnode(pp->dev->of_node);

    pp->irq_domain = irq_domain_create_linear(fwnode, MAX_MSI_IRQS, &g_hip_pcie_msi_domain_ops, pp);
    if (!pp->irq_domain) {
        dev_err(pp->dev, "Failed to create IRQ domain\n");
        return -ENOMEM;
    }

    pp->msi_domain = pci_msi_create_irq_domain(fwnode, &g_hip_pcie_msi_domain_info, pp->irq_domain);
    if (!pp->msi_domain) {
        dev_err(pp->dev, "Failed to create MSI domain\n");
        irq_domain_remove(pp->irq_domain);
        return -ENOMEM;
    }

    return 0;
}

void hip_pcie_free_msi(struct hip_pcie_port *pp)
{
    irq_set_chained_handler(pp->msi_irq, NULL);
    irq_set_handler_data(pp->msi_irq, NULL);

    irq_domain_remove(pp->msi_domain);
    irq_domain_remove(pp->irq_domain);
}

void hip_pcie_msi_init(struct hip_pcie_port *pp)
{
    struct device *dev = pp->dev;

    pp->msi_page = alloc_page(GFP_KERNEL);
    pp->msi_data = dma_map_page(dev, pp->msi_page, 0, PAGE_SIZE, DMA_FROM_DEVICE);
    if (dma_mapping_error(dev, pp->msi_data)) {
        dev_err(dev, "Failed to map MSI data\n");
        __free_page(pp->msi_page);
        pp->msi_page = NULL;
        return;
    }
}

void hip_pci_free_msi_data(struct hip_pcie_port *pp)
{
    if (pp) {
        if (pp->msi_page) {
            dma_unmap_page(pp->dev, pp->msi_data, PAGE_SIZE, DMA_FROM_DEVICE);
            __free_page(pp->msi_page);
            pp->msi_page = NULL;
        }
    }
}

int init_msi_irq(struct hip_pcie_port *pp)
{
    int ret = 0;

    if (IS_ENABLED(CONFIG_PCI_MSI) && pci_msi_enabled()) {
        dev_err(pp->dev, "can hip pcie msi init\n");
        hip_pcie_msi_init(pp);
        pp->num_vectors = MAX_MSI_IRQS;
        if (!pp->ops->msi_host_init) {
            ret = hip_pcie_allocate_domains(pp);
            if (ret) {
                hip_pci_free_msi_data(pp);
                return ret;
            }
            if (pp->msi_irq) {
                irq_set_chained_handler_and_data(pp->msi_irq, hip_chained_msi_isr, pp);
            }
        }
    } else {
        dev_err(pp->dev, "can't hip pcie msi init\n");
    }

    return ret;
}

int hip_pcie_host_init(struct hip_pcie_port *pp)
{
    int ret;
    struct platform_device *pdev = to_platform_device(pp->dev);
    struct resource *cfg_res;
    struct pcie_atu_info pcie_atu;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
    struct pci_host_bridge *bridge;
#else
    LIST_HEAD(res);
#endif

    cfg_res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "config");
    if (cfg_res) {
        pp->cfg0_size = resource_size(cfg_res) / HALF_SIZE_2;
        pp->cfg1_size = resource_size(cfg_res) / HALF_SIZE_2;
        pp->cfg0_base = cfg_res->start;
        pp->cfg1_base = cfg_res->start + pp->cfg0_size;
    } else {
        dev_err(pp->dev, "missing *config* reg space\n");
        return -1;
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
    bridge = devm_pci_alloc_host_bridge(pp->dev, 0);
    if (!bridge) {
        return -ENOMEM;
    }
    /* Get the I/O and memory ranges from DT */
    get_range_from_dts(pp, &bridge->windows);
#else
    ret = get_fun_interface(pp, &res);
    if (ret) {
        return ret;
    }
    /* Get the I/O and memory ranges from DT */
    get_range_from_dts(pp, &res);
#endif

    ret = init_iomap_addr(pp);
    if (ret) {
        return ret;
    }

    init_lanes_viewport(pp);

    ret = init_msi_irq(pp);
    if (ret) {
        return ret;
    }

    if (pp->ops->host_init) {
        pp->ops->host_init(pp);
        set_pci_msi_enable(pp);
    }

    if (!pp->ops->rd_other_conf) {
        pcie_atu.cpu_addr = pp->mem_base;
        pcie_atu.pci_addr = pp->mem_bus_addr;
        pcie_atu.size = pp->mem_size;
        hip_pcie_prog_outbound_atu(pp, HIP02_PCIE_ATU_REGION_INDEX1, HIP02_PCIE_ATU_TYPE_MEM, &pcie_atu);
    }

    (void)hip_pcie_wr_own_conf(pp, PCI_BASE_ADDRESS_0, LOW_BYTE_4, 0);

    /* program correct class for RC */
    (void)hip_pcie_wr_own_conf(pp, PCI_CLASS_DEVICE, LOW_BYTE_2, PCI_CLASS_BRIDGE_PCI);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
    ret = scan_pci_bus(pp, &bridge->windows);
#else
    ret = scan_pci_bus(pp, &res);
#endif
    if (ret) {
        hip_pci_free_msi_data(pp);
        return ret;
    }

    return 0;
}

static void hip_pcie_rw_other_conf_prepare(struct hip_pcie_port *pp, struct pci_bus *bus,
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

    hip_pcie_prog_outbound_atu(pp, HIP02_PCIE_ATU_REGION_INDEX1, type, pcie_atu);
}

static void hip_pcie_rw_other_conf_end(struct hip_pcie_port *pp, struct pcie_atu_info *pcie_atu)
{
    if (pp->num_viewport <= VIEW_PORT_COUNT) {
        pcie_atu->cpu_addr = pp->io_base;
        pcie_atu->pci_addr = pp->io_bus_addr;
        pcie_atu->size = pp->io_size;
        hip_pcie_prog_outbound_atu(pp, HIP02_PCIE_ATU_REGION_INDEX1, HIP02_PCIE_ATU_TYPE_IO, pcie_atu);
    }
}

static int hip_pcie_rd_other_conf(struct hip_pcie_port *pp, struct pci_bus *bus,
                                  u32 devfn, struct pcie_cfg_unit *cfg_unit, u32 *val)
{
    int ret;
    struct pcie_atu_info pcie_atu;
    void __iomem *va_cfg_base;

    hip_pcie_rw_other_conf_prepare(pp, bus, devfn, &pcie_atu, &va_cfg_base);
    ret = hip_pcie_cfg_read(va_cfg_base + cfg_unit->where, cfg_unit->size, val);
    hip_pcie_rw_other_conf_end(pp, &pcie_atu);

    return ret;
}

static int hip_pcie_wr_other_conf(struct hip_pcie_port *pp, struct pci_bus *bus,
                                  u32 devfn, struct pcie_cfg_unit *cfg_unit, u32 val)
{
    int ret;
    struct pcie_atu_info pcie_atu;
    void __iomem *va_cfg_base;

    hip_pcie_rw_other_conf_prepare(pp, bus, devfn, &pcie_atu, &va_cfg_base);
    ret = hip_pcie_cfg_write(va_cfg_base + cfg_unit->where, cfg_unit->size, val);
    hip_pcie_rw_other_conf_end(pp, &pcie_atu);

    return ret;
}

static int hip_pcie_valid_config(struct hip_pcie_port *pcie_port,
                                 struct pci_bus *pcie_bus, int device)
{
    if (pcie_bus->number != pcie_port->root_bus_nr) {
        if (!hip_pcie_link_up(pcie_port)) {
            return 0;
        }
    }

    if ((pcie_bus->number == pcie_port->root_bus_nr) && (device > 0)) {
        return 0;
    }
    if (pcie_bus->primary == pcie_port->root_bus_nr && device > 0) {
        return 0;
    }

    return 1;
}

static int hip_pcie_rd_conf(struct pci_bus *bus, u32 devfn, int where, int size, u32 *val)
{
    struct hip_pcie_port *pp = bus->sysdata;
    struct pcie_cfg_unit cfg_unit;
    int ret;

    if (hip_pcie_valid_config(pp, bus, PCI_SLOT(devfn)) == 0) {
        *val = 0xffffffff;
        return PCIBIOS_DEVICE_NOT_FOUND;
    }

    if (bus->number != pp->root_bus_nr) {
        cfg_unit.where = where;
        cfg_unit.size = size;
        if (pp->ops->rd_other_conf) {
            ret = pp->ops->rd_other_conf(pp, bus, devfn, &cfg_unit, val);
        } else {
            ret = hip_pcie_rd_other_conf(pp, bus, devfn, &cfg_unit, val);
        }
    } else {
        ret = hip_pcie_rd_own_conf(pp, where, size, val);
    }

    return ret;
}

static int hip_pcie_wr_conf(struct pci_bus *bus, u32 devfn, int where, int size, u32 val)
{
    struct hip_pcie_port *pp = bus->sysdata;
    struct pcie_cfg_unit cfg_unit;
    int ret;

    if (hip_pcie_valid_config(pp, bus, PCI_SLOT(devfn)) == 0) {
        return PCIBIOS_DEVICE_NOT_FOUND;
    }

    if (bus->number != pp->root_bus_nr) {
        cfg_unit.where = where;
        cfg_unit.size = size;
        if (pp->ops->wr_other_conf) {
            ret = pp->ops->wr_other_conf(pp, bus, devfn, &cfg_unit, val);
        } else {
            ret = hip_pcie_wr_other_conf(pp, bus, devfn, &cfg_unit, val);
        }
    } else {
        ret = hip_pcie_wr_own_conf(pp, where, size, val);
    }

    return ret;
}

static struct pci_ops g_hip_pcie_ops = {
    .read = hip_pcie_rd_conf,
    .write = hip_pcie_wr_conf,
};

void hip_pcie_setup_rc_base(struct hip_pcie_port *pp)
{
    u32 val;
    u32 membase;
    u32 memlimit;
    struct pcie_atu_info pcie_atu;

    /* setup RC BARs */
    hip_pcie_writel_rc(pp, 0x00000004, PCI_BASE_ADDRESS_0);
    hip_pcie_writel_rc(pp, 0x00000000, PCI_BASE_ADDRESS_1);

    /* setup interrupt pins */
    hip_pcie_readl_rc(pp, PCI_INTERRUPT_LINE, &val);
    val &= 0xffff00ff;
    val |= 0x00000100;
    hip_pcie_writel_rc(pp, val, PCI_INTERRUPT_LINE);

    /* setup memory base, memory limit */
    membase = ((u32)pp->mem_base & 0xfff00000) >> LONG_BIT_16;
    memlimit = (pp->mem_size + (u32)pp->mem_base) & 0xfff00000;
    val = memlimit | membase;
    hip_pcie_writel_rc(pp, val, PCI_MEMORY_BASE);

    /* setup command register */
    hip_pcie_readl_rc(pp, PCI_COMMAND, &val);
    val &= 0xffff0000;
    val |= PCI_COMMAND_IO | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER | PCI_COMMAND_SERR;
    hip_pcie_writel_rc(pp, val, PCI_COMMAND);

    if (!pp->ops->rd_other_conf) {
        pcie_atu.cpu_addr = pp->mem_base;
        pcie_atu.pci_addr = pp->mem_bus_addr;
        pcie_atu.size = pp->mem_size;
        hip_pcie_prog_outbound_atu(pp, HIP02_PCIE_ATU_REGION_INDEX0, HIP02_PCIE_ATU_TYPE_MEM, &pcie_atu);
        if (pp->num_viewport > VIEW_PORT_COUNT) {
            pcie_atu.cpu_addr = pp->io_base;
            pcie_atu.pci_addr = pp->io_bus_addr;
            pcie_atu.size = pp->io_size;
            hip_pcie_prog_outbound_atu(pp, HIP02_PCIE_ATU_REGION_INDEX2, HIP02_PCIE_ATU_TYPE_IO, &pcie_atu);
        }
    }
    (void)hip_pcie_wr_own_conf(pp, PCI_BASE_ADDRESS_0, LOW_BYTE_4, 0);

    /* program correct class for RC */
    (void)hip_pcie_wr_own_conf(pp, PCI_CLASS_DEVICE, LOW_BYTE_2, PCI_CLASS_BRIDGE_PCI);
}

void hip_pcie_setup_rc(struct hip_pcie_port *pp)
{
    u32 val;

    hip_pcie_setup_rc_base(pp);

    /*
     * Instructs the LTSSM to initiate a speed change to Gen2 or Gen3
     * after the link is initialized at Gen1.
     */
    (void)hip_pcie_rd_own_conf(pp, HIP02_PCIE_LINK_WIDTH_SPEED_CONTROL, LOW_BYTE_4, &val);
    val |= HIP02_PORT_LOGIC_SPEED_CHANGE;
    (void)hip_pcie_wr_own_conf(pp, HIP02_PCIE_LINK_WIDTH_SPEED_CONTROL, LOW_BYTE_4, val);
}

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("PCIe host controller driver");