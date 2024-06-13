// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Synopsys Designware PCIe host controller driver
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _PCIE_FRAMEWORK_H
#define _PCIE_FRAMEWORK_H
#include "pcie_hip02_reg.h"
/*
 * Maximum number of MSI IRQs can be 256 per controller. But keep
 * it 32 as of now. Probably we will never need more than 32. If needed,
 * then increment it in multiple of 32.
 */
#define MAX_MSI_IRQS 32
#define MAX_MSI_IRQS_PER_CTRL 32
#define MAX_MSI_CTRLS (MAX_MSI_IRQS / MAX_MSI_IRQS_PER_CTRL)
#define MSI_REG_CTRL_BLOCK_SIZE 12

#define VIEW_PORT_COUNT  2
#define RESOURCE_CFG     0

#define LANES_COUNT_1      1
#define LANES_COUNT_2      2
#define LANES_COUNT_4      4
#define LANES_COUNT_8      8

#define LOW_BYTE_4        4
#define LOW_BYTE_2        2
#define LOW_BYTE_1        1
#define LONG_BIT_32        32
#define LONG_BIT_16        16
#define STEP_SIZE_12       12
#define HALF_SIZE_2        2

struct pcie_atu_info {
    u64 cpu_addr;
    u64 pci_addr;
    u32 size;
};

struct pcie_cfg_unit {
    int where;
    int size;
};

struct hip_pcie_port {
    struct device *dev;
    u8 root_bus_nr;
    u64 cfg0_base;
    u32 cfg0_size;
    u64 cfg1_base;
    u32 cfg1_size;
    void __iomem *va_cfg1_base;
    void __iomem *va_cfg0_base;
    void __iomem *dbi_base;
    struct resource *cfg;
    struct resource *io;
    struct resource *mem;
    struct resource *busn;
    resource_size_t io_base;
    u64 mem_base;
    u32 io_size;
    u32 mem_size;
    phys_addr_t io_bus_addr;
    phys_addr_t mem_bus_addr;
    u32 lanes;
    u32 num_viewport;
    struct pcie_host_ops *ops;
    int irq;
    int msi_irq;
    u32 num_vectors;
    struct irq_domain *irq_domain;
    struct irq_domain *msi_domain;
    unsigned long msi_data;
    DECLARE_BITMAP(msi_irq_in_use, MAX_MSI_IRQS);
    raw_spinlock_t lock;
    u32 irq_status[MAX_MSI_CTRLS];
    struct page *msi_page;
};

struct pcie_host_ops {
    void (*readl_rc)(struct hip_pcie_port *pp,
                     void __iomem *dbi_base, u32 *val);
    void (*writel_rc)(struct hip_pcie_port *pp,
                      u32 val, void __iomem *dbi_base);
    int (*rd_own_conf)(struct hip_pcie_port *pp, int where, int size, u32 *val);
    int (*wr_own_conf)(struct hip_pcie_port *pp, int where, int size, u32 val);
    int (*rd_other_conf)(struct hip_pcie_port *pp, struct pci_bus *bus,
                         unsigned int devfn, struct pcie_cfg_unit *cfg_unit, u32 *val);
    int (*wr_other_conf)(struct hip_pcie_port *pp, struct pci_bus *bus,
                         unsigned int devfn, struct pcie_cfg_unit *cfg_unit, u32 val);
    int (*link_up)(struct hip_pcie_port *pp);
    void (*host_init)(struct hip_pcie_port *pp);
    void (*msi_set_irq)(struct hip_pcie_port *pp, int irq);
    void (*msi_clear_irq)(struct hip_pcie_port *pp, int irq);
    phys_addr_t (*get_msi_addr)(struct hip_pcie_port *pp);
    u32 (*get_msi_data)(struct hip_pcie_port *pp, int pos);
    void (*scan_bus)(struct hip_pcie_port *pp);
    int (*msi_host_init)(struct hip_pcie_port *pp, struct msi_controller *chip);
};

int hip_pcie_cfg_read(const void __iomem *addr, int size, u32 *val);
int hip_pcie_cfg_write(void __iomem *addr, int size, u32 val);
irqreturn_t hip_handle_msi_irq(struct hip_pcie_port *pp);
void hip_pcie_msi_init(struct hip_pcie_port *pp);
int hip_pcie_link_up(struct hip_pcie_port *pp);
void hip_pcie_setup_rc(struct hip_pcie_port *pp);
int hip_pcie_host_init(struct hip_pcie_port *pp);
void hip_pcie_setup_rc_lane_num(struct hip_pcie_port *pp);
void hip_pcie_setup_rc_base(struct hip_pcie_port *pp);
void hip_pci_free_msi_data(struct hip_pcie_port *pp);

#endif /* _PCIE_DESIGNWARE_H */
