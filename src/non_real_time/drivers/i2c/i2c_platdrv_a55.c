// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Platform driver based on Linux(hi1711).
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <asm/memory.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/jiffies.h>
#include <linux/random.h>
#include "comm_knl_api.h"
#include "lp_subsys_csr_addr_define.h"
#include "lp_subsys_c_union_define.h"
#include "lp_subsys.h"
#include "i2c.h"
#include "i2c_api.h"

struct i2c_platdrv_platform {
    struct i2c_devinfo di;
    struct platform_device *pdev;
    irqreturn_t (*irq_routine)(int, void *);
};

static DEFINE_SEMAPHORE(i2c_platdrv_sem);

unsigned int i2c_platdrv_random(void)
{
    unsigned int val;

    get_random_bytes(&val, sizeof(val));
    return val;
}

void *i2c_platdrv_mem_calloc(unsigned int mtype, int len)
{
    void *ptr = NULL;

    if (len <= 0) {
        return NULL;
    }

    ptr = kzalloc(len, GFP_KERNEL);
    return ptr;
}

void i2c_platdrv_mem_free(unsigned int mtype, const void *ptr)
{
    if (ptr) {
        kfree(ptr);
    }
}

unsigned long i2c_platdrv_clk_hz(void)
{
    return SYS_APB_CLOCK_FREQ;
}

void i2c_platdrv_udelay(unsigned int delay)
{
    unsigned long msecs;
    long timeout;

    if (delay < 1000) { /* 1000us */
        udelay(delay);
        return;
    }

    msecs = (unsigned long)(delay / 1000); /* us to ms by div 1000 */
    timeout = msecs_to_jiffies(msecs);
    timeout = timeout ? : 1;

    while (timeout > 0) {
        timeout = schedule_timeout(timeout);
    }
}

static void i2c_platdrv_hangup_handle(struct i2c_adapter *adap, u32 count)
{
    u_lpsc_i2c_ctrl0 lpsc_ctrl0;
    u32 oe_mask;
    u32 drv_id;
    u32 i;
    u32 reg_sel_num;
    u32 mask_offset;

    drv_id = adap->drv_id;
    oe_mask = 0x12;

    /* Register sequence number corresponding to each channel. Each register corresponds to two I2C channels.
     * The offset between registers is 0x4. */
    reg_sel_num = LP_SUBSYS_CSR_LPSC_I2C_CTRL0_OFFSET_ADDR + 0x4 * (drv_id / 2);
    /* The mask left shift offsets for odd and even channels are 6 and 0.
     * %2 identifies whether the channel is odd or even. */
    mask_offset = ((drv_id % 2) * 6);

    lpsc_ctrl0.val = 0;
    lpsc_ctrl0.bits.i2c0_clk_mux_sel = 1;
    lpsc_ctrl0.bits.i2c0_dat_mux_sel = 1;
    lpsc_ctrl0.val = (lpsc_ctrl0.val << mask_offset);

    /* When hanging, use the system controller signal to drive SCL with active-high logic. */
    lp_subsys_reg_read_write(reg_sel_num, lpsc_ctrl0.val, lpsc_ctrl0.val);

    lpsc_ctrl0.val = 0;
    lpsc_ctrl0.bits.i2c0_clk_oe_cfg = 1;
    lpsc_ctrl0.bits.i2c0_dat_oe_cfg = 0;
    oe_mask = (oe_mask << mask_offset);
    lpsc_ctrl0.val = (lpsc_ctrl0.val << mask_offset);

    lp_subsys_reg_read_write(reg_sel_num, oe_mask, lpsc_ctrl0.val);

    lpsc_ctrl0.val = 0;
    lpsc_ctrl0.bits.i2c0_scl_cfg = 1;
    lpsc_ctrl0.val = (lpsc_ctrl0.val << mask_offset);

    for (i = 0; i < (count * 2 + 1); i++) { // mul 2
        lp_subsys_reg_set_bits(reg_sel_num, lpsc_ctrl0.val, (i + 1) % 2); // %2 indicates that the output is high or low
        udelay(5);
    }

    /* Simulate a stop signal by toggling both the driver clock and data lines. */
    udelay(500);
    lp_subsys_reg_set_bits(reg_sel_num, lpsc_ctrl0.val, 0);
    lp_subsys_reg_set_bits(reg_sel_num, (1ul << (5 + mask_offset)), 1); // add 5
    lp_subsys_reg_set_bits(reg_sel_num, (1ul << (5 + mask_offset)), 0); // add 5
    udelay(500);
    lp_subsys_reg_set_bits(reg_sel_num, lpsc_ctrl0.val, 1);
    udelay(500);
    lp_subsys_reg_set_bits(reg_sel_num, (1ul << (5 + mask_offset)), 1); // add 5

    lpsc_ctrl0.val = 0;
    lpsc_ctrl0.bits.i2c0_clk_mux_sel = 1;
    lpsc_ctrl0.bits.i2c0_dat_mux_sel = 1;
    lpsc_ctrl0.val = (lpsc_ctrl0.val << mask_offset);
    lp_subsys_reg_read_write(reg_sel_num, lpsc_ctrl0.val, 0);
}

static int i2c_platdrv_clk_enable(struct i2c_adapter *adap)
{
    u32 clk_mask = (0x1UL << (adap->drv_id + 10));
    u32 cnt = 0;
    u32 max_cnt = 100;
    u32 status;

    while (cnt++ < max_cnt) {
        lp_subsys_reg_read(LP_SUBSYS_CSR_LPSC_CLKSTA_CTRL0_OFFSET_ADDR, &status);
        if (status & clk_mask) {
            break;
        }
        lp_subsys_reg_write(LP_SUBSYS_CSR_LPSC_CLKEN_CTRL0_OFFSET_ADDR, clk_mask);
        udelay(100); /* 100us */
    }

    return (cnt >= max_cnt) ? -ETIME : 0;
}

static int i2c_platdrv_clk_disable(struct i2c_adapter *adap)
{
    u32 clk_mask = (0x1UL << (adap->drv_id + 10));
    u32 cnt = 0;
    u32 max_cnt = 100;
    u32 status;

    if (adap->algo && adap->algo->idle) {
        if (!adap->algo->idle(adap)) {
            return -EBUSY;
        }
    }

    while (cnt++ < max_cnt) {
        lp_subsys_reg_read(LP_SUBSYS_CSR_LPSC_CLKSTA_CTRL0_OFFSET_ADDR, &status);
        if (!(status & clk_mask)) {
            break;
        }
        lp_subsys_reg_write(LP_SUBSYS_CSR_LPSC_CLKDIS_CTRL0_OFFSET_ADDR, clk_mask);
        udelay(100); /* 100us */
    }

    return (cnt >= max_cnt) ? -ETIME : 0;
}

static int i2c_platdrv_isr_register(struct i2c_adapter *adap, irqreturn_t (*irq_routine)(int, void *))
{
    struct i2c_platdrv_platform *plat = NULL;
    int ret;

    plat = (struct i2c_platdrv_platform *)adap->platform_private;

    if (!plat || plat->irq_routine) {
        return -I2C_EIRQ;
    }

    if (adap->di.irqn <= 0 || !irq_routine) {
        return -I2C_EIRQ;
    }

    ret = request_irq(adap->di.irqn, irq_routine, 0, adap->name, adap);
    if (ret) {
        return ret;
    }

    plat->irq_routine = irq_routine;

    return 0;
}

static void i2c_platdrv_isr_unregister(struct i2c_adapter *adap)
{
    struct i2c_platdrv_platform *plat;

    plat = (struct i2c_platdrv_platform *)adap->platform_private;

    if (!plat || !plat->irq_routine || adap->di.irqn <= 0) {
        return;
    }

    plat->irq_routine = NULL;
    free_irq(adap->di.irqn, adap);
}

static int i2c_platdrv_acquire_devinfo(struct i2c_adapter *adap, struct i2c_devinfo *di)
{
    struct i2c_platdrv_platform *platform = NULL;
    const struct i2c_devinfo *devinfo = NULL;

    platform = adap->platform_private;
    if (!platform) {
        return -I2C_EDEFAULT;
    }

    devinfo = &platform->di;

    if (devinfo->irqn <= 0 || !devinfo->regbase) {
        return -I2C_EDIINVAL;
    }

    *di = *devinfo;

    return 0;
}

static int i2c_platdrv_adapter_init(struct i2c_adapter *adap)
{
    u_lpsc_smbus_i2c_mux lpsc_mux;
    u32 mux_reg_offset;

    if (!(adap->deploy_flags & I2C_KDEPLOY_SMBUS)) {
        lpsc_mux.val = (0x0U << adap->drv_id);
    } else {
        lpsc_mux.val = (0x1U << adap->drv_id);
    }

    mux_reg_offset = LP_SUBSYS_CSR_LPSC_SMBUS_I2C_MUX_OFFSET_ADDR;

    return lp_subsys_reg_read_write(mux_reg_offset, 0x1U << adap->drv_id, lpsc_mux.val);
}

static int i2c_platdrv_adapter_destroy(struct i2c_adapter *adap)
{
    u_lpsc_smbus_i2c_mux lpsc_mux;
    u32 mux_reg_offset;

    mux_reg_offset = LP_SUBSYS_CSR_LPSC_SMBUS_I2C_MUX_OFFSET_ADDR;
    lpsc_mux.val = (0x0U << adap->drv_id);

    return lp_subsys_reg_read_write(mux_reg_offset, 0x1U << adap->drv_id, lpsc_mux.val);
}

static int i2c_platdrv_adapter_recovery(struct i2c_adapter *adap)
{
    down(&i2c_platdrv_sem);
    i2c_platdrv_hangup_handle(adap, 9); // 9 clock decoupling buses
    up(&i2c_platdrv_sem);

    return 0;
}

static int i2c_platdrv_adapter_reset(struct i2c_adapter *adap)
{
    u32 offset;
    u32 mask;

    offset = LP_SUBSYS_CSR_LPSC_SRST_REQ0_OFFSET_ADDR;
    /* Bit 9 to bit 24 indicate the soft reset request bits of I2C0 to I2C15 respectively. */
    mask = 0x1U << (adap->drv_id + 9);

    return m_reset_handle(offset, mask);
}

static int i2c_platdrv_adapter_clk_operate(struct i2c_adapter *adap, unsigned int code)
{
    if (code == 0) {
        return i2c_platdrv_clk_disable(adap);
    } else if (code == 1) {
        return i2c_platdrv_clk_enable(adap);
    } else {
        return -EINVAL;
    }
}

static dma_addr_t i2c_platdrv_adapter_dma_map(struct i2c_adapter *adap, void *addr, size_t size,
                                              enum i2c_dma_type type)
{
    struct i2c_platdrv_platform *platform = adap->platform_private;
    struct device *dev = NULL;
    dma_addr_t phy_addr;
    int dma_type;

    if (!platform->pdev) {
        return 0;
    }

    dev = &platform->pdev->dev;
    dma_type = (type == I2C_DMA_TO_DEVICE) ? DMA_TO_DEVICE : DMA_FROM_DEVICE;
    phy_addr = dma_map_single(dev, addr, size, dma_type);

    return (phy_addr && dma_mapping_error(dev, phy_addr)) ? 0 : phy_addr;
}

static void i2c_platdrv_adapter_dma_unmap(struct i2c_adapter *adap, dma_addr_t addr, size_t size,
                                          enum i2c_dma_type type)
{
    struct i2c_platdrv_platform *platform = adap->platform_private;
    struct device *dev = NULL;
    int dma_type;

    if (!platform->pdev) {
        return;
    }

    dev = &platform->pdev->dev;
    dma_type = (type == I2C_DMA_TO_DEVICE) ? DMA_TO_DEVICE : DMA_FROM_DEVICE;

    if (addr && !dma_mapping_error(dev, addr)) {
        dma_unmap_single(dev, addr, size, dma_type);
    }
}

static int i2c_platdrv_adapter_dma_config(struct i2c_adapter *adap, dma_addr_t addr, size_t size,
                                          enum i2c_dma_type type)
{
    return -I2C_EDMACONFIG;
}

static int i2c_platdrv_adapter_dma_wait_complete(struct i2c_adapter *adap, int channel)
{
    return -I2C_EDMAWAIT;
}

static int i2c_platdrv_irq_count(struct device_node *dev)
{
    struct of_phandle_args irq;
    int nr = 0;

    while (!of_irq_parse_one(dev, nr, &irq)) {
        nr++;
    }

    return nr;
}

static int i2c_platdrv_get_irqn(struct device_node *node, int id, int *irqn)
{
    int cnt = i2c_platdrv_irq_count(node);
    if (id >= cnt) {
        return -I2C_EDEFAULT;
    }

    *irqn = irq_of_parse_and_map(node, id);

    return (*irqn) ? 0 : -ENODEV;
}

static int i2c_platdrv_generate_devinfo(struct platform_device *pdev, struct i2c_devinfo *devinfo, int id)
{
    void __iomem *map = NULL;
    unsigned char *log_level = NULL;
    int irqn;
    int ret;

    map = of_iomap(pdev->dev.of_node, id);
    if (!map) {
        goto failed;
    }

    log_level = kzalloc(I2C_MAX_DEV_COUNT, GFP_KERNEL);
    if (!log_level) {
        goto failed;
    }

    ret = i2c_platdrv_get_irqn(pdev->dev.of_node, id, &irqn);
    if (ret) {
        goto failed;
    }

    devinfo->irqn = irqn;
    devinfo->regbase = map;
    devinfo->log_level = log_level;
    devinfo->log_level_size = I2C_MAX_DEV_COUNT;
    return 0;

failed:
    if (map) {
        iounmap((void __iomem *)map);
    }

    if (log_level) {
        kfree(log_level);
    }

    return -I2C_EDEFAULT;
}

static void i2c_platdrv_destroy_devinfo(struct platform_device *pdev, struct i2c_devinfo *devinfo)
{
    if (devinfo->regbase) {
        iounmap(devinfo->regbase);
    }

    if (devinfo->log_level) {
        kfree(devinfo->log_level);
    }

    devinfo->irqn = 0;
    devinfo->log_level = NULL;
    devinfo->regbase = NULL;
    devinfo->log_level_size = 0;
}

static struct i2c_platdrv_platform g_i2c_platdrv_plat[] = {
    {   .di = {
        .phy_base = 0x8707000,
        }
    },
    {   .di = {
        .phy_base = 0x8708000,
        }
    },
    {   .di = {
        .phy_base = 0x8709000,
        }
    },
    {   .di = {
        .phy_base = 0x870a000,
        }
    },
    {   .di = {
        .phy_base = 0x870b000,
        }
    },
    {   .di = {
        .phy_base = 0x870c000,
        }
    },
    {   .di = {
        .phy_base = 0x870d000,
        }
    },
    {   .di = {
        .phy_base = 0x870e000,
        }
    },
    {   .di = {
        .phy_base = 0x8750000,
        }
    },
    {   .di = {
        .phy_base = 0x8751000,
        }
    },
    {   .di = {
        .phy_base = 0x8752000,
        }
    },
    {   .di = {
        .phy_base = 0x8753000,
        }
    },
    {   .di = {
        .phy_base = 0x8754000,
        }
    },
    {   .di = {
        .phy_base = 0x8755000,
        }
    },
    {   .di = {
        .phy_base = 0x8756000,
        }
    },
    {   .di = {
        .phy_base = 0x8757000,
        }
    },
};

static const struct i2c_adapter_quirks g_i2c_platdrv_quirks = {
    .max_write_len = 2048, /* max write lenth 2048 */
    .max_read_len = 2048,  /* max read lenth 2048 */
};

static const struct i2c_adapter_platform_operations g_i2c_platdrv_ops = {
    .platform_isr_register = i2c_platdrv_isr_register,
    .platform_isr_unregister = i2c_platdrv_isr_unregister,
    .platform_acquire_devinfo = i2c_platdrv_acquire_devinfo,
    .platform_init = i2c_platdrv_adapter_init,
    .platform_destroy = i2c_platdrv_adapter_destroy,
    .platform_recovery = i2c_platdrv_adapter_recovery,
    .platform_reset = i2c_platdrv_adapter_reset,
    .platform_clk_operate = i2c_platdrv_adapter_clk_operate,
    .platform_dma_map = i2c_platdrv_adapter_dma_map,
    .platform_dma_unmap = i2c_platdrv_adapter_dma_unmap,
    .platform_dma_config = i2c_platdrv_adapter_dma_config,
    .platform_dma_wait_complete = i2c_platdrv_adapter_dma_wait_complete,
};

struct i2c_adapter g_i2c_platdrv_adap[] = {
    i2c_adapter_initiallzer(i2c_platdrv, "i2c0", 0),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c1", 1),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c2", 2),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c3", 3),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c4", 4),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c5", 5),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c6", 6),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c7", 7),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c8", 8),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c9", 9),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c10", 10),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c11", 11),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c12", 12),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c13", 13),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c14", 14),
    i2c_adapter_initiallzer(i2c_platdrv, "i2c15", 15),
};

struct i2c_adapter *g_i2c_platdrv_adap_list[] = {
    &g_i2c_platdrv_adap[0],
    &g_i2c_platdrv_adap[1],
    &g_i2c_platdrv_adap[2],
    &g_i2c_platdrv_adap[3],
    &g_i2c_platdrv_adap[4],
    &g_i2c_platdrv_adap[5],
    &g_i2c_platdrv_adap[6],
    &g_i2c_platdrv_adap[7],
    &g_i2c_platdrv_adap[8],
    &g_i2c_platdrv_adap[9],
    &g_i2c_platdrv_adap[10],
    &g_i2c_platdrv_adap[11],
    &g_i2c_platdrv_adap[12],
    &g_i2c_platdrv_adap[13],
    &g_i2c_platdrv_adap[14],
    &g_i2c_platdrv_adap[15],
    NULL,
};

void i2c_platdrv_init(void *arg)
{
    struct platform_device *pdev = arg;
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(g_i2c_platdrv_plat); i++) {
        g_i2c_platdrv_plat[i].pdev = pdev;
        i2c_platdrv_generate_devinfo(pdev, &g_i2c_platdrv_plat[i].di, i);
    }
}

void i2c_platdrv_destroy(void *arg)
{
    struct platform_device *pdev = arg;
    unsigned int i;

    for (i = 0; i < ARRAY_SIZE(g_i2c_platdrv_plat); i++) {
        i2c_platdrv_destroy_devinfo(pdev, &g_i2c_platdrv_plat[i].di);
    }
}