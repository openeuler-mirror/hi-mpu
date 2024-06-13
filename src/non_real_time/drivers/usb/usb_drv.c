// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * usb driver
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include "comm_knl_api.h"
#include "usr_data.h"
#include "hp_subsys.h"
#include "usb_drv.h"

#define USB_BUFSIZE                  32

#define PERI_USB3_GSBUSCFG0 0xc100

#define PERI_USB3_GCTL      0xc110
#define USB_DSBBLCLKGTNG    BIT(0)

#define MISC_CTRL_CTRL3     0xc0c
#define USB_RAM0_INIT_START BIT(1)
#define USB_RAM_ECC_EN      BIT(16)
#define USB_RAM_ECC_CLR     BIT(17)

#define MISC_CTRL_STATUS8   0xc50
#define USB_RAM0_INIT_DONE  BIT(27)
#define USB_RAM_NUM         4

#define HPSUB_SRST_ST2      0x4
#define HPSUB_SRST_REQ2     0xc
#define HPSUB_SRST_DIS2     0x14
#define USB_3_CMB_PHY_SRST  BIT(13)
#define USB_SRST(n)         BIT((n) * 3 + 3)
#define USB_PHY_SRST(n)     BIT((n) * 3 + 2)
#define USB_APB_RST_SRST(n) BIT((n) * 3 + 1)

#define LANEN_DIG_TX_OVRD_DRV_LO 0x1002
#define LANEN_DIG_RX_OVRD_INHI   0x1006

#define RX_EQ_EN                BIT(6)
#define RX_EQ_EN_OVRD           BIT(7)
#define RX_EQ           (BIT(8) | BIT(9) | BIT(10))
#define RX_EQ_OVRD              BIT(11)
#define RX_EQ_SHIFT             8
#define TX_OVERRIDE_ENABLE      BIT(14)
#define TX_PREEMPH_SHIFT        7

#define USB_SERDES_DEFAULT_RX_EQ            0x3
#define USB_SERDES_MAX_RX_EQ                0x7
#define USB_SERDES_DEFAULT_TX_PREEMPH       0x15
#define USB_SERDES_MAX_TX_PREEMPH           0x7f
#define USB_SERDES_DEFAULT_TX_AMPLITUDE     0x7f
#define USB_SERDES_MAX_TX_AMPLITUDE         0x7f


#define USB2_PHY_CTRL4          0x0410
#define USB3_PHY_CTRL7          0x101c
#define USB3_PHY_STATUS2        0x104c

#define USB_CONTROLLER_0        0
#define USB_CONTROLLER_1        1
#define USB_CONTROLLER_2        2
#define USB_CONTROLLER_3        3
#define USB_CONTROLLER_MAX      4

#define USB_NAME_SIZE           32

enum {
    USB_MODE_HOST = 0,
    USB_MODE_DEVICE,
    USB_MODE_OTG,
    USB_MODE_MAX,
};

typedef struct {
    char name[USB_NAME_SIZE];
    unsigned int index;
    int usb_mode;
    int irq;
    unsigned int  ctrl_start;
    void __iomem *ctrl_reg;
    void __iomem *misc_reg;
    void __iomem *hp_sub_reg;
} usb_dev_driver_s;

#define usb_ctrol_read(dev, off)       readl((dev)->ctrl_reg + (off))
#define usb_ctrol_write(dev, val, off) writel((val), ((dev)->ctrl_reg + (off)))
#define usb_misc_read(dev, off)        readl((dev)->misc_reg + (off))
#define usb_misc_write(dev, val, off)  writel((val), ((dev)->misc_reg + (off)))
#define usb_hpsub_read(dev, off)       readl((dev)->hp_sub_reg + (off))
#define usb_hpsub_write(dev, val, off) writel((val), ((dev)->hp_sub_reg + (off)))

#define usb_phy_check_error(ud, cond, desc) \
    do { \
        unsigned int __ret_val = usb_misc_read((ud), USB3_PHY_STATUS2); \
        unsigned int __cond1 = !(cond); \
        unsigned int __cond2 = !(__ret_val & BIT(16)); \
        if (__cond1 == __cond2) { \
            LOG(LOG_INFO, "%s\n", (desc)); \
        } \
    } while (0)

static usb_dev_driver_s g_usb_drv[USB_CONTROLLER_MAX];

void usb_creg_write(unsigned int id, unsigned int val, unsigned int off)
{
    if (id >= ARRAY_SIZE(g_usb_drv)) {
        LOG(LOG_ERROR, "creg write index out.");
    } else {
        usb_ctrol_write(&g_usb_drv[id], val, off);
    }
}
EXPORT_SYMBOL(usb_creg_write);

unsigned int usb_creg_read(unsigned int id, unsigned int off)
{
    if (id >= ARRAY_SIZE(g_usb_drv)) {
        LOG(LOG_ERROR, "creg read index out.");
        return 0;
    } else {
        return usb_ctrol_read(&g_usb_drv[id], off);
    }
}
EXPORT_SYMBOL(usb_creg_read);

void usb_mreg_write(unsigned int id, unsigned int val, unsigned int off)
{
    if (id >= ARRAY_SIZE(g_usb_drv)) {
        LOG(LOG_ERROR, "creg write index out.");
    } else {
        usb_misc_write(&g_usb_drv[id], val, off);
    }
}
EXPORT_SYMBOL(usb_mreg_write);

unsigned int usb_mreg_read(unsigned int id, unsigned int off)
{
    if (id >= ARRAY_SIZE(g_usb_drv)) {
        LOG(LOG_ERROR, "creg read index out.");
        return 0;
    } else {
        return usb_misc_read(&g_usb_drv[id], off);
    }
}
EXPORT_SYMBOL(usb_mreg_read);

int usb_id2name(unsigned int id, char *name, unsigned int max_len)
{
    usb_dev_driver_s *udev = NULL;

    if (!max_len || !name || (id >= ARRAY_SIZE(g_usb_drv))) {
        return -EINVAL;
    }

    udev = &g_usb_drv[id];
    (void)memset_s(name, max_len, 0, max_len);
    (void)snprintf_s(name, max_len, max_len - 1, "%x.dwc3", udev->ctrl_start);
    return 0;
}
EXPORT_SYMBOL(usb_id2name);

int usb_name2id(const char *name, unsigned int *id)
{
    usb_dev_driver_s *udev = NULL;
    char usb_name[USB_BUFSIZE];
    unsigned int i;

    if (!name || !id) {
        return -EINVAL;
    }

    for (i = 0; i < ARRAY_SIZE(g_usb_drv); i++) {
        udev = &g_usb_drv[i];
        (void)memset_s(usb_name, sizeof(usb_name), 0, sizeof(usb_name));
        (void)snprintf_s(usb_name, sizeof(usb_name),
            sizeof(usb_name) - 1, "%x.dwc3", udev->ctrl_start);
        if (!strncmp(name, usb_name, sizeof(usb_name))) {
            *id = i;
            return 0;
        }
    }

    return -EFAULT;
}
EXPORT_SYMBOL(usb_name2id);

static void usb_irq_error_dump(const usb_dev_driver_s *udev, unsigned int sts, unsigned int ram_index)
{
    LOG(LOG_ERROR, "usb-controller-%u ram%u ecc err 0x%x.", udev->index, ram_index, sts);
}

static irqreturn_t usb_irq(int irq, void *arg)
{
    usb_dev_driver_s *udev = arg;
    unsigned int sts[USB_RAM_NUM] = { 0 };
    unsigned int val;
    unsigned int i;

    /* ECC status */
    for (i = 0; i < ARRAY_SIZE(sts); i++) {
        sts[i] = usb_misc_read(udev,
            MISC_CTRL_STATUS8 + (i * sizeof(unsigned int)));
    }

    /* Masking interrupts */
    val = usb_misc_read(udev, MISC_CTRL_CTRL3);
    val &= ~USB_RAM_ECC_EN;
    usb_misc_write(udev, val, MISC_CTRL_CTRL3);

    /* Clear the interrupt status. */
    val = usb_misc_read(udev, MISC_CTRL_CTRL3);
    val |= USB_RAM_ECC_CLR;
    usb_misc_write(udev, val, MISC_CTRL_CTRL3);

    for (i = 0; i < ARRAY_SIZE(sts); i++) {
        usb_irq_error_dump(udev, sts[i], i);
    }

    /* Clear the interrupt status. */
    val = usb_misc_read(udev, MISC_CTRL_CTRL3);
    val &= ~USB_RAM_ECC_CLR;
    usb_misc_write(udev, val, MISC_CTRL_CTRL3);

    /* Open Interrupt */
    val = usb_misc_read(udev, MISC_CTRL_CTRL3);
    val |= USB_RAM_ECC_EN;
    usb_misc_write(udev, val, MISC_CTRL_CTRL3);

    return IRQ_HANDLED;
}

static int usb_of_get_mode(struct device_node * const node, int *usb_mode)
{
    static const char * const strings[USB_MODE_MAX] = {
        [USB_MODE_HOST] = "host",
        [USB_MODE_DEVICE] = "peripheral",
        [USB_MODE_OTG] = "otg",
    };
    const char *mode_string = NULL;
    int i;
    int ret;

    ret = of_property_read_string(node, "dr_mode", &mode_string);
    if (ret < 0 || !mode_string) {
        return -1;
    }

    for (i = 0; i < USB_MODE_MAX; i++) {
        if (strings[i] && !strcmp(mode_string, strings[i])) {
            *usb_mode = i;
            return 0;
        }
    }

    return -1;
}

static int usb_of_get_index(struct device_node * const node, unsigned int *usb_index)
{
    static const char *const strings[ARRAY_SIZE(g_usb_drv)] = {
        "0", "1", "2", "3"
    };
    const char *idx_string = NULL;
    unsigned int i;
    int ret;

    ret = of_property_read_string(node, "controller_index", &idx_string);
    if (ret < 0 || !idx_string) {
        return -1;
    }

    for (i = 0; i < ARRAY_SIZE(g_usb_drv); i++) {
        if (strings[i] && !strcmp(idx_string, strings[i])) {
            *usb_index = i;
            return 0;
        }
    }

    return -1;
}

static int usb_of_map(struct platform_device * const pdev, usb_dev_driver_s *udev)
{
    struct resource *res = NULL;
    int retval = 0;

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0); /* ctrl reg described in resource table index 0 */
    if (!res) {
        LOG(LOG_ERROR, "resource error");
        return -ENOMEM;
    }
    udev->ctrl_reg = ioremap(res->start, resource_size(res));
    if (!udev->ctrl_reg) {
        LOG(LOG_ERROR, "ioremap error");
        return -ENOMEM;
    }
    udev->ctrl_start = res->start;

    res = platform_get_resource(pdev, IORESOURCE_MEM, 1); /* misc reg described in resource table index 1 */
    if (!res) {
        LOG(LOG_ERROR, "resource error");
        retval = -ENOMEM;
        goto err0;
    }
    udev->misc_reg = ioremap(res->start, resource_size(res));
    if (!udev->misc_reg) {
        LOG(LOG_ERROR, "ioremap error");
        retval = -ENOMEM;
        goto err0;
    }

    res = platform_get_resource(pdev, IORESOURCE_MEM, 2); /* hp sub reg described in resource table index 2 */
    if (!res) {
        LOG(LOG_ERROR, "resource error");
        retval = -ENOMEM;
        goto err1;
    }
    udev->hp_sub_reg = ioremap(res->start, resource_size(res));
    if (!udev->hp_sub_reg) {
        LOG(LOG_ERROR, "ioremap error");
        retval = -ENOMEM;
        goto err1;
    }

    return 0;
err1:
    iounmap(udev->misc_reg);
    udev->misc_reg = NULL;
err0:
    iounmap(udev->ctrl_reg);
    udev->ctrl_reg = NULL;
    return retval;
}

static int usb_of_irq(struct platform_device * const pdev, usb_dev_driver_s *udev)
{
    udev->irq = platform_get_irq(pdev, 0);
    if (udev->irq < 0) {
        LOG(LOG_ERROR, "irq error.");
        return -ENODEV;
    }

    return request_irq(udev->irq, usb_irq, 0, udev->name, udev);
}

static void usb_memmap_free(usb_dev_driver_s * const udev)
{
    if (udev->ctrl_reg) {
        iounmap(udev->ctrl_reg);
        udev->ctrl_reg = NULL;
    }
    if (udev->misc_reg) {
        iounmap(udev->misc_reg);
        udev->misc_reg = NULL;
    }
    if (udev->hp_sub_reg) {
        iounmap(udev->hp_sub_reg);
        udev->hp_sub_reg = NULL;
    }
}

static void usb_irq_free(usb_dev_driver_s *udev)
{
    unsigned int val;

    val = usb_misc_read(udev, MISC_CTRL_CTRL3);
    val &= ~USB_RAM_ECC_EN;
    usb_misc_write(udev, val, MISC_CTRL_CTRL3);

    if (udev->irq > 0) {
        free_irq(udev->irq, udev);
        udev->irq = 0;
    }
}

static void usb_phy_cr_write(const usb_dev_driver_s *udev, unsigned int addr, unsigned int data)
{
    unsigned int reg_val;

    reg_val = (addr << 16) | BIT(7); // << 16, bit 7
    usb_misc_write(udev, reg_val, USB3_PHY_CTRL7);
    udelay(1);
    usb_phy_check_error(udev, 0, "capture address failed");

    reg_val = usb_misc_read(udev, USB3_PHY_CTRL7);
    reg_val &= ~BIT(7); /* Bit 7: CR register address capture */
    usb_misc_write(udev, reg_val, USB3_PHY_CTRL7);
    udelay(1);
    usb_phy_check_error(udev, 1, "dis-capture address failed");

    reg_val = (data << 16) | BIT(8); // << 16, bit 8
    usb_misc_write(udev, reg_val, USB3_PHY_CTRL7);
    udelay(1);
    usb_phy_check_error(udev, 0, "capture data failed");

    reg_val = usb_misc_read(udev, USB3_PHY_CTRL7);
    reg_val &= ~BIT(8); /* Bit 8: CR register data capture */
    usb_misc_write(udev, reg_val, USB3_PHY_CTRL7);
    udelay(1);
    usb_phy_check_error(udev, 1, "dis-capture data failed");

    reg_val = usb_misc_read(udev, USB3_PHY_CTRL7);
    reg_val |= BIT(5); /* Bit 5: CR register write control */
    usb_misc_write(udev, reg_val, USB3_PHY_CTRL7);
    udelay(1);
    usb_phy_check_error(udev, 0, "write data failed");

    reg_val = usb_misc_read(udev, USB3_PHY_CTRL7);
    reg_val &= ~BIT(5);  /* Bit 5: CR register write control */
    usb_misc_write(udev, reg_val, USB3_PHY_CTRL7);
    udelay(1);
    usb_phy_check_error(udev, 1, "dis-write data failed");
}

void usb_cr_write(unsigned int id, unsigned int addr, unsigned int data)
{
    if (id >= ARRAY_SIZE(g_usb_drv)) {
        LOG(LOG_ERROR, "cr write index out.");
    } else {
        usb_phy_cr_write(&g_usb_drv[id], addr, data);
    }
}
EXPORT_SYMBOL(usb_cr_write);

static unsigned int usb_phy_cr_read(const usb_dev_driver_s *udev, unsigned int addr)
{
    unsigned int reg_val;
    unsigned int ret_val;

    reg_val = (addr << 16) | BIT(7); // << 16, bit 7
    usb_misc_write(udev, reg_val, USB3_PHY_CTRL7);
    udelay(1);
    usb_phy_check_error(udev, 0, "capture address failed");

    reg_val = usb_misc_read(udev, USB3_PHY_CTRL7);
    reg_val &= ~BIT(7); // bit 7
    usb_misc_write(udev, reg_val, USB3_PHY_CTRL7);
    udelay(1);
    usb_phy_check_error(udev, 1, "dis-capture address failed");

    reg_val = usb_misc_read(udev, USB3_PHY_CTRL7);
    reg_val |= BIT(4); // bit 4
    usb_misc_write(udev, reg_val, USB3_PHY_CTRL7);
    udelay(1);
    usb_phy_check_error(udev, 0, "read data failed");

    ret_val = usb_misc_read(udev, USB3_PHY_STATUS2) & 0xFFFFU;

    reg_val = usb_misc_read(udev, USB3_PHY_CTRL7);
    reg_val &= ~BIT(4); // bit 4
    usb_misc_write(udev, reg_val, USB3_PHY_CTRL7);
    udelay(1);
    usb_phy_check_error(udev, 1, "dis-read data failed");

    return ret_val;
}

unsigned int usb_cr_read(unsigned int id, unsigned int addr)
{
    if (id >= ARRAY_SIZE(g_usb_drv)) {
        LOG(LOG_ERROR, "cr read index out.");
        return 0;
    } else {
        return usb_phy_cr_read(&g_usb_drv[id], addr);
    }
}
EXPORT_SYMBOL(usb_cr_read);

static void usb_serdes_init(usb_dev_driver_s *udev)
{
    int ret;
    unsigned int cr_reg;
    unsigned int rx_eq = 0;
    unsigned int tx_preemph = 0;
    unsigned int tx_amplitude = 0;

    ret = usr_data_read(USB_RX_EQ, &rx_eq);
    if (ret || (rx_eq > USB_SERDES_MAX_RX_EQ)) {
        LOG(LOG_ERROR, "get rx_eq(0x%x) failed", rx_eq);
        rx_eq = USB_SERDES_DEFAULT_RX_EQ;
    }

    ret = usr_data_read(USB_TX_PREEMPH, &tx_preemph);
    if (ret || (tx_preemph > USB_SERDES_MAX_TX_PREEMPH)) {
        LOG(LOG_ERROR, "get tx_preemph(0x%x) failed", tx_preemph);
        tx_preemph = USB_SERDES_DEFAULT_TX_PREEMPH;
    }

    ret = usr_data_read(USB_TX_AMPLITUDE, &tx_amplitude);
    if (ret || (tx_amplitude > USB_SERDES_MAX_TX_AMPLITUDE)) {
        LOG(LOG_ERROR, "get tx_amplitude(0x%x) failed", tx_amplitude);
        tx_amplitude = USB_SERDES_DEFAULT_TX_AMPLITUDE;
    }

    udelay(10);

    cr_reg = ((unsigned char)tx_preemph << TX_PREEMPH_SHIFT) | tx_amplitude | TX_OVERRIDE_ENABLE;
    usb_phy_cr_write(udev, LANEN_DIG_TX_OVRD_DRV_LO, cr_reg);

    cr_reg = usb_phy_cr_read(udev, LANEN_DIG_RX_OVRD_INHI);
    cr_reg &= ~RX_EQ_EN;
    usb_phy_cr_write(udev, LANEN_DIG_RX_OVRD_INHI, cr_reg);

    cr_reg = usb_phy_cr_read(udev, LANEN_DIG_RX_OVRD_INHI);
    cr_reg |=  RX_EQ_EN_OVRD;
    usb_phy_cr_write(udev, LANEN_DIG_RX_OVRD_INHI, cr_reg);

    cr_reg = usb_phy_cr_read(udev, LANEN_DIG_RX_OVRD_INHI);
    cr_reg &= ~RX_EQ; /* clear */
    cr_reg |= ((unsigned char)rx_eq << RX_EQ_SHIFT); /* RX_EQ Configuration */
    usb_phy_cr_write(udev, LANEN_DIG_RX_OVRD_INHI, cr_reg);

    cr_reg = usb_phy_cr_read(udev, LANEN_DIG_RX_OVRD_INHI);
    cr_reg |=  RX_EQ_OVRD;
    usb_phy_cr_write(udev, LANEN_DIG_RX_OVRD_INHI, cr_reg);

    udelay(10);
}

static void usb_reset_enable(const usb_dev_driver_s *udev)
{
    unsigned int val;

    /* rst combo phy */
    if (udev->index == USB_CONTROLLER_3) {
        val = USB_3_CMB_PHY_SRST;
        usb_hpsub_write(udev, val, HPSUB_SRST_REQ2);
        if (!(usb_hpsub_read(udev, HPSUB_SRST_ST2) & val)) {
            LOG(LOG_ERROR, "reset combo phy error");
        }
    }

    /* rst controller */
    val = USB_SRST(udev->index);
    usb_hpsub_write(udev, val, HPSUB_SRST_REQ2);
    if (!(usb_hpsub_read(udev, HPSUB_SRST_ST2) & val)) {
        LOG(LOG_ERROR, "reset controller %u error", udev->index);
    }

    /* rst misc */
    val = USB_APB_RST_SRST(udev->index);
    usb_hpsub_write(udev, val, HPSUB_SRST_REQ2);
    if (!(usb_hpsub_read(udev, HPSUB_SRST_ST2) & val)) {
        LOG(LOG_ERROR, "reset misc %u error", udev->index);
    }

    /* rst phy */
    val = USB_PHY_SRST(udev->index);
    usb_hpsub_write(udev, val, HPSUB_SRST_REQ2);
    if (!(usb_hpsub_read(udev, HPSUB_SRST_ST2) & val)) {
        LOG(LOG_ERROR, "reset phy %u error", udev->index);
    }
}

void usb_reset(unsigned int id)
{
    if (id >= ARRAY_SIZE(g_usb_drv)) {
        LOG(LOG_ERROR, "reset index out.");
    } else {
        usb_reset_enable(&g_usb_drv[id]);
    }
}
EXPORT_SYMBOL(usb_reset);

static void usb_reset_disable(usb_dev_driver_s *udev)
{
    unsigned int val;

    /* unrst misc */
    val = USB_APB_RST_SRST(udev->index);
    usb_hpsub_write(udev, val, HPSUB_SRST_DIS2);
    if (usb_hpsub_read(udev, HPSUB_SRST_ST2) & val) {
        LOG(LOG_ERROR, "unreset misc %u error", udev->index);
    }

    /* unrst phy */
    val = USB_PHY_SRST(udev->index);
    usb_hpsub_write(udev, val, HPSUB_SRST_DIS2);
    if (usb_hpsub_read(udev, HPSUB_SRST_ST2) & val) {
        LOG(LOG_ERROR, "unreset phy %u error", udev->index);
    }

    /* unrst combo phy */
    if (udev->index == USB_CONTROLLER_3) {
        val = USB_3_CMB_PHY_SRST;
        usb_hpsub_write(udev, val, HPSUB_SRST_DIS2);
        if (usb_hpsub_read(udev, HPSUB_SRST_ST2) & val) {
            LOG(LOG_ERROR, "unreset combo phy error");
        }

        usb_serdes_init(udev);
    }

    /* unrst controller */
    val = USB_SRST(udev->index);
    usb_hpsub_write(udev, val, HPSUB_SRST_DIS2);
    if (usb_hpsub_read(udev, HPSUB_SRST_ST2) & val) {
        LOG(LOG_ERROR, "unreset controller %u error", udev->index);
    }
}

void usb_unreset(unsigned int id)
{
    if (id >= ARRAY_SIZE(g_usb_drv)) {
        LOG(LOG_ERROR, "unreset index out.");
    } else {
        usb_reset_disable(&g_usb_drv[id]);
    }
}
EXPORT_SYMBOL(usb_unreset);

static void usb_controller_device_otg_init(const usb_dev_driver_s *udev)
{
    unsigned int val;

    if ((udev->usb_mode != USB_MODE_DEVICE) &&
        (udev->usb_mode != USB_MODE_OTG)) {
        return;
    }

    // ram0 clock gate disable
    val = usb_ctrol_read(udev, PERI_USB3_GCTL);
    val |= USB_DSBBLCLKGTNG;
    usb_ctrol_write(udev, val, PERI_USB3_GCTL);

    // ram0 init start
    val = usb_misc_read(udev, MISC_CTRL_CTRL3);
    val |= USB_RAM0_INIT_START;
    usb_misc_write(udev, val, MISC_CTRL_CTRL3);

    msleep(20);

    if (!(usb_misc_read(udev, MISC_CTRL_STATUS8) & USB_RAM0_INIT_DONE)) {
        LOG(LOG_ERROR, "init error");
    }

    val = usb_misc_read(udev, MISC_CTRL_CTRL3);
    val &= ~USB_RAM0_INIT_START;
    usb_misc_write(udev, val, MISC_CTRL_CTRL3);
}

static void usb_controller_init(usb_dev_driver_s *udev)
{
    unsigned int val;
    int ret;

    usb_reset_enable(udev);
    udelay(20);
    usb_reset_disable(udev);

    usb_controller_device_otg_init(udev);

    val = usb_ctrol_read(udev, PERI_USB3_GSBUSCFG0);
    val &= ~((unsigned int)0xffff << 16); // << 16
    val |=  ((unsigned int)0xeeee << 16); // << 16
    usb_ctrol_write(udev, val, PERI_USB3_GSBUSCFG0);

    val = usb_misc_read(udev, MISC_CTRL_CTRL3);
    val |= USB_RAM_ECC_EN;
    usb_misc_write(udev, val, MISC_CTRL_CTRL3);

    ret = usr_data_read(USB_2PHY_CTRL4, &val);
    if (ret) {
        LOG(LOG_ERROR, "get env USB2_PHY_CTRL4(0x%x) failed, set default", val);
        val = usb_misc_read(udev, USB2_PHY_CTRL4);
        val &= ~(0x3U << 2); // << 2
        val |=  (0x3U << 2); // << 2
    }
    usb_misc_write(udev, val, USB2_PHY_CTRL4);
}

void usb_dfx_regs_collect(void)
{
    int id;
    u32 reg_val = 0;

    /* read hp offset 0x4 reg */
    (void)hp_subsys_reg_read(0x4, &reg_val);
    KBOX_MSG(KBOX_COUNTER, "HP(0x4) = 0x%x\n", reg_val);
    /* read hp offset 0x18 reg */
    (void)hp_subsys_reg_read(0x18, &reg_val);
    KBOX_MSG(KBOX_COUNTER, "HP(0x18) = 0x%x\n", reg_val);

    for (id = 0; id < USB_CONTROLLER_MAX; id++) {
        /* 0x420 port sc */
        KBOX_MSG(KBOX_COUNTER, "USB%d ctr(0x420) = 0x%x\n", id, usb_ctrol_read(&g_usb_drv[id], 0x420));
        /* 0x424 port sc */
        KBOX_MSG(KBOX_COUNTER, "USB%d ctr(0x424) = 0x%x\n", id, usb_ctrol_read(&g_usb_drv[id], 0x424));
        /* 0x24 usbsts */
        KBOX_MSG(KBOX_COUNTER, "USB%d ctr(0x24) = 0x%x\n", id, usb_ctrol_read(&g_usb_drv[id], 0x24));
        /* 0xc110 reg read */
        KBOX_MSG(KBOX_COUNTER, "USB%d ctr(0xc110) = 0x%x\n", id, usb_ctrol_read(&g_usb_drv[id], 0xc110));
        /* 0xc118 reg read */
        KBOX_MSG(KBOX_COUNTER, "USB%d ctr(0xc118) = 0x%x\n", id, usb_ctrol_read(&g_usb_drv[id], 0xc118));
        /* 0xc700 reg read */
        KBOX_MSG(KBOX_COUNTER, "USB%d ctr(0xc700) = 0x%x\n", id, usb_ctrol_read(&g_usb_drv[id], 0xc700));
        /* 0xc704 reg read */
        KBOX_MSG(KBOX_COUNTER, "USB%d ctr(0xc704) = 0x%x\n", id, usb_ctrol_read(&g_usb_drv[id], 0xc704));
        /* 0xc70c reg read */
        KBOX_MSG(KBOX_COUNTER, "USB%d ctr(0xc70c) = 0x%x\n", id, usb_ctrol_read(&g_usb_drv[id], 0xc70c));

        /* 0x400 reg read */
        KBOX_MSG(KBOX_COUNTER, "USB%d misc(0x400) = 0x%x\n", id, usb_misc_read(&g_usb_drv[id], 0x400));
        /* 0x404 reg read */
        KBOX_MSG(KBOX_COUNTER, "USB%d misc(0x404) = 0x%x\n", id, usb_misc_read(&g_usb_drv[id], 0x404));
        /* 0x434 reg read */
        KBOX_MSG(KBOX_COUNTER, "USB%d misc(0x434) = 0x%x\n", id, usb_misc_read(&g_usb_drv[id], 0x434));
        /* 0x438 reg read */
        KBOX_MSG(KBOX_COUNTER, "USB%d misc(0x438) = 0x%x\n", id, usb_misc_read(&g_usb_drv[id], 0x438));
        /* 0xc04 reg read */
        KBOX_MSG(KBOX_COUNTER, "USB%d misc(0xc04) = 0x%x\n", id, usb_misc_read(&g_usb_drv[id], 0xc04));
        /* 0x1004 reg read */
        KBOX_MSG(KBOX_COUNTER, "USB%d misc(0x1004) = 0x%x\n", id, usb_misc_read(&g_usb_drv[id], 0x1004));
    }
}
EXPORT_SYMBOL(usb_dfx_regs_collect);


static int usb_drv_probe(struct platform_device *pdev)
{
    struct device_node *node = pdev->dev.of_node;
    usb_dev_driver_s *udev = NULL;
    unsigned int usb_idx;
    int mode;
    int retval;

    retval = usb_of_get_index(node, &usb_idx);
    if (retval < 0) {
        LOG(LOG_ERROR, "probe index err.");
        return -EFAULT;
    }

    if (usb_idx >= ARRAY_SIZE(g_usb_drv)) {
        LOG(LOG_ERROR, "probe index out.");
        return -EFAULT;
    }

    retval = usb_of_get_mode(node, &mode);
    if (retval < 0) {
        LOG(LOG_ERROR, "mode err.");
        return -EFAULT;
    }

    udev = &g_usb_drv[usb_idx];
    (void)memset_s(udev, sizeof(*udev), 0, sizeof(*udev));
    udev->index = usb_idx;
    udev->usb_mode = mode;
    (void)snprintf_s(udev->name, sizeof(udev->name), sizeof(udev->name) - 1, "usb_ram%u", usb_idx);

    retval = usb_of_map(pdev, udev);
    if (retval) {
        LOG(LOG_ERROR, "ofmap err.");
        return retval;
    }

    retval = usb_of_irq(pdev, udev);
    if (retval) {
        usb_memmap_free(udev);
        LOG(LOG_ERROR, "ofirq error.");
        return retval;
    } else {
        usb_controller_init(udev);
        return 0;
    }
}

static int usb_drv_remove(struct platform_device * const pdev)
{
    struct device_node *node = pdev->dev.of_node;
    usb_dev_driver_s *udev = NULL;
    unsigned int usb_idx;
    int retval;

    retval = usb_of_get_index(node, &usb_idx);
    if (retval < 0) {
        LOG(LOG_ERROR, "remove index err.");
        return -EFAULT;
    }

    if (usb_idx >= ARRAY_SIZE(g_usb_drv)) {
        LOG(LOG_ERROR, "remove index out.");
        return -EFAULT;
    }

    udev = &g_usb_drv[usb_idx];
    usb_irq_free(udev);
    usb_memmap_free(udev);
    return 0;
}

static const struct of_device_id g_usb_of_match[] = {
    {
        .compatible = "hisilicon, hi1711-usb",
    },
    {},
};

static struct platform_driver g_usb_driver = {
    .probe = usb_drv_probe,
    .remove = usb_drv_remove,
    .driver = {
        .name = "hi1711-usb",
        .bus = &platform_bus_type,
        .of_match_table = g_usb_of_match,
    },
};
module_platform_driver(g_usb_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("usb dirver");