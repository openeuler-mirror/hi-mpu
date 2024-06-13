// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * i2c dirver, which can be used with the Linux i2c general-purpose input/output framework.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <asm/memory.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/reset.h>

#define HI309X_I2C_DEFAULT_FREQUENCY    100000
#define HI309X_I2C_FAST_FREQUENCY       400000
#define HI309X_I2C_SS_SCL_HCNT_CYCLE    0x188
#define HI309X_I2C_SS_SCL_LCNT_CYCLE    0x1D5
#define HI309X_I2C_FS_SCL_HCNT_CYCLE    0x34
#define HI309X_I2C_FS_SCL_LCNT_CYCLE    0x81
#define HI309X_I2C_TIMEOUT              0x400
#define HI309X_I2C_XFER_TIMEOUT         (-10)

#define HI309X_ICR_CONTROL              0x0000
#define HI309X_ICR_TAR                  0x0004
#define HI309X_ICR_DATA_CMD             0x0010
#define HI309X_ICR_SS_SCL_HCNT          0x0014
#define HI309X_ICR_SS_SCL_LCNT          0x0018
#define HI309X_ICR_FS_SCL_HCNT          0x001C
#define HI309X_ICR_FS_SCL_LCNT          0x0020
#define HI309X_ICR_INTR_MASK            0x0030
#define HI309X_ICR_CLR_INTR             0x0040
#define HI309X_ICR_ENABLE               0x006C
#define HI309X_ICR_STATUS               0x0070
#define HI309X_ICR_TXFLR                0x0074
#define HI309X_ICR_RXFLR                0x0078
#define HI309X_ICR_COMP_PARAM_1         0x00F4

#ifndef BIT
#define BIT(x)                         (1UL << (x))
#endif

#define HI309X_IC_STATUS_ACTIVITY       BIT(0)
#define HI309X_IC_ENA_ENABLE            BIT(0)
#define HI309X_IC_CON_MASTER            BIT(0)
#define HI309X_IC_CON_SPEED_DEFAULT     BIT(1)
#define HI309X_IC_CON_SPEED_FAST        BIT(2)
#define HI309X_IC_STATUS_TFE            BIT(2)
#define HI309X_IC_STATUS_RFNE           BIT(3)
#define HI309X_IC_CON_RESTART_EN        BIT(5)
#define HI309X_IC_CON_SLAVE_DISABLE     BIT(6)
#define HI309X_IC_DATA_CMD_WRITE        BIT(8)

struct hi309x_i2c_dev {
    struct device *dev;
    struct i2c_adapter adap;
    resource_size_t phybase;
    void __iomem *base;
    struct clk *clk;
    int irq;

    unsigned int freq;
    struct i2c_msg *msg;
    unsigned int msg_num;
    unsigned int msg_idx;
    unsigned int msg_buf_ptr;
    struct completion msg_complete;

    spinlock_t lock;
    int status;
};

static char* g_hi309x_i2c_name[] = {
    "hi309x_i2c0",
    "hi309x_i2c1",
    "hi309x_i2c2",
    "hi309x_i2c3",
    "hi309x_i2c4",
    "hi309x_i2c5",
    "hi309x_i2c6",
    "hi309x_i2c6",
    "hi309x_i2c8",
    "hi309x_i2c9",
    "hi309x_i2c10",
    "hi309x_i2c11",
    "hi309x_i2c12",
    "hi309x_i2c13",
    "hi309x_i2c14",
    "hi309x_i2c15",
};

static int g_hi309x_i2c_phybase[] = {
    0x08707000,
    0x08708000,
    0x08709000,
    0x0870a000,
    0x0870b000,
    0x0870c000,
    0x0870d000,
    0x0870e000,
    0x08750000,
    0x08751000,
    0x08752000,
    0x08753000,
    0x08754000,
    0x08755000,
    0x08756000,
    0x08757000,
};

static void hi309x_i2c_enable(struct hi309x_i2c_dev *i2c)
{
    writel(HI309X_IC_ENA_ENABLE, i2c->base + HI309X_ICR_ENABLE);
}

static void hi309x_i2c_disable(struct hi309x_i2c_dev *i2c)
{
    writel(0x0, i2c->base + HI309X_ICR_ENABLE);
}

static void hi309x_i2c_disable_int(struct hi309x_i2c_dev *i2c)
{
    writel(0x0, i2c->base + HI309X_ICR_INTR_MASK);
    readl(i2c->base + HI309X_ICR_CLR_INTR);
}

static int hi309x_i2c_wait_idle(struct hi309x_i2c_dev *i2c)
{
    unsigned int time_cnt;
    unsigned int val;

    time_cnt = 0;

    do {
        val = readl(i2c->base + HI309X_ICR_STATUS);
        if (val == 0) {
            break;
        }
        udelay(50); /* delay 50 us */
    } while (time_cnt++ > HI309X_I2C_TIMEOUT);

    if (time_cnt > HI309X_I2C_TIMEOUT) {
        return HI309X_I2C_XFER_TIMEOUT;
    }

    hi309x_i2c_disable(i2c);

    return 0;
}

static void hi309x_i2c_config_speed(struct hi309x_i2c_dev *i2c)
{
    unsigned int control;

    control = HI309X_IC_CON_MASTER | HI309X_IC_CON_SLAVE_DISABLE | HI309X_IC_CON_RESTART_EN;

    if (i2c->freq <= HI309X_I2C_DEFAULT_FREQUENCY) {
        control |= HI309X_IC_CON_SPEED_DEFAULT;
        writel(HI309X_I2C_SS_SCL_HCNT_CYCLE, i2c->base + HI309X_ICR_SS_SCL_HCNT);
        writel(HI309X_I2C_SS_SCL_LCNT_CYCLE, i2c->base + HI309X_ICR_SS_SCL_LCNT);
    } else {
        control |= HI309X_IC_CON_SPEED_FAST;
        writel(HI309X_I2C_FS_SCL_HCNT_CYCLE, i2c->base + HI309X_ICR_FS_SCL_HCNT);
        writel(HI309X_I2C_FS_SCL_LCNT_CYCLE, i2c->base + HI309X_ICR_FS_SCL_LCNT);
    }

    writel(control, i2c->base + HI309X_ICR_CONTROL);
}

static void hi309x_i2c_set_addr(struct hi309x_i2c_dev *i2c)
{
    unsigned int addr;
    struct i2c_msg *msg;

    msg = i2c->msg;
    addr = msg->addr;

    writel(addr, i2c->base + HI309X_ICR_TAR);
}

static int hi309x_i2c_check_xfer_finish(struct hi309x_i2c_dev *i2c, unsigned int time_cnt, int flag)
{
    unsigned int status;
    unsigned int time_out;

    time_out = time_cnt;

    while (!(readl(i2c->base + HI309X_ICR_STATUS) & flag) && time_out > 0) {
        status = readl(i2c->base + HI309X_ICR_STATUS);
        if (flag == HI309X_IC_STATUS_TFE && ((status >> 2) & 0x1)) { /* Shift right by 2 bits */
            break;
        } else if (flag == HI309X_IC_STATUS_RFNE && !((status >> 3) & 0x1)) { /* Shift right by 3 bits */
            break;
        }

        time_out--;
        udelay(50); /* delay 50 us */
    }

    if (!time_out) {
        return HI309X_I2C_XFER_TIMEOUT;
    }

    return 0;
}

static int hi309x_i2c_read_sync(struct hi309x_i2c_dev *i2c)
{
    unsigned int val;
    unsigned int rx_depth;
    unsigned int rx_cnt;
    unsigned int rlimit;
    unsigned int cnt;
    unsigned int time_cnt;
    struct i2c_msg *msg;

    i2c->msg_buf_ptr = 0;
    msg = i2c->msg;
    val = readl(i2c->base + HI309X_ICR_COMP_PARAM_1);
    rx_depth = ((val >> 8) & 0xff) + 1;  /* bit8~bit15   rx_fifo_depth */
    rx_cnt = readl(i2c->base + HI309X_ICR_RXFLR);
    rlimit = ((rx_depth - rx_cnt) <= rx_depth) ? (rx_depth - rx_cnt) : 1; // 1 is mini xfer num
    cnt = 0;
    time_cnt = HI309X_I2C_TIMEOUT;

    while ((cnt < msg->len) && (rlimit > 0)) {
        writel(HI309X_IC_DATA_CMD_WRITE, i2c->base + HI309X_ICR_DATA_CMD);
        cnt++;
        rlimit--;
    }

    while (i2c->msg_buf_ptr < msg->len) {
        if (readl(i2c->base + HI309X_ICR_RXFLR)) {
            val = (unsigned char)readl(i2c->base + HI309X_ICR_DATA_CMD);
            msg->buf[i2c->msg_buf_ptr++] = val;
            if (cnt < msg->len) {
                writel(HI309X_IC_DATA_CMD_WRITE, i2c->base + HI309X_ICR_DATA_CMD);
                cnt++;
            }
        }

        if (time_cnt <= 0) {
            break;
        }

        if (time_cnt > 0) {
            time_cnt--;
            udelay(50); /* delay 50 us */
        }
    }

    if (hi309x_i2c_check_xfer_finish(i2c, time_cnt, HI309X_IC_STATUS_RFNE)) {
        return HI309X_I2C_XFER_TIMEOUT;
    }

    return 0;
}

static int hi309x_i2c_write_sync(struct hi309x_i2c_dev *i2c)
{
    unsigned char val;
    unsigned int time_cnt;
    struct i2c_msg *msg;

    i2c->msg_buf_ptr = 0;
    msg = i2c->msg;
    time_cnt = HI309X_I2C_TIMEOUT;

    while (i2c->msg_buf_ptr < msg->len) {
        if ((readl(i2c->base + HI309X_ICR_TXFLR) == 0) &&
            (((readl(i2c->base + HI309X_ICR_STATUS) >> 2) & 0x1) == 1)) { /* Shift right by 2 bits valid */
            val = msg->buf[i2c->msg_buf_ptr++];
            writel(val, i2c->base + HI309X_ICR_DATA_CMD);
        }

        if (time_cnt <= 0) {
            break;
        }

        if (time_cnt > 0) {
            time_cnt--;
            udelay(50); /* delay 50 us */
        }
    }

    if (hi309x_i2c_check_xfer_finish(i2c, time_cnt, HI309X_IC_STATUS_TFE)) {
        return HI309X_I2C_XFER_TIMEOUT;
    }

    return 0;
}

static int hi309x_i2c_polling_xfer_one_msg(struct hi309x_i2c_dev *i2c)
{
    int status;
    struct i2c_msg *msg;

    msg = i2c->msg;
    status = hi309x_i2c_wait_idle(i2c);
    if (status) {
        return status;
    }

    hi309x_i2c_disable_int(i2c);
    hi309x_i2c_config_speed(i2c);
    hi309x_i2c_set_addr(i2c);
    hi309x_i2c_enable(i2c);

    if (msg->flags & I2C_M_RD) {
        status = hi309x_i2c_read_sync(i2c);
    } else {
        status = hi309x_i2c_write_sync(i2c);
    }

    if (status) {
        return status;
    }

    status = hi309x_i2c_wait_idle(i2c);
    return status;
}

/*
 * Master transfer function
 */
static int hi309x_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
{
    struct hi309x_i2c_dev *i2c = i2c_get_adapdata(adap);
    int status = -EINVAL;
    unsigned long flags;

    if (msgs == NULL || (num <= 0)) {
        dev_err(i2c->dev, "msgs == NULL || num <= 0, Invalid argument!\n");
        return -EINVAL;
    }

    spin_lock_irqsave(&i2c->lock, flags);

    i2c->msg = msgs;
    i2c->msg_num = num;
    i2c->msg_idx = 0;

    while (i2c->msg_idx < i2c->msg_num) {
        status = hi309x_i2c_polling_xfer_one_msg(i2c);
        if (status) {
            spin_unlock_irqrestore(&i2c->lock, flags);
            return status;
        }
        i2c->msg++;
        i2c->msg_idx++;
    }

    if (!status || i2c->msg_idx > 0) {
        status = i2c->msg_idx;
    }

    spin_unlock_irqrestore(&i2c->lock, flags);
    return status;
}

static u32 hi309x_i2c_func(struct i2c_adapter *adap)
{
    return I2C_FUNC_I2C | I2C_FUNC_10BIT_ADDR |
            I2C_FUNC_PROTOCOL_MANGLING |
            I2C_FUNC_SMBUS_WORD_DATA |
            I2C_FUNC_SMBUS_BYTE_DATA |
            I2C_FUNC_SMBUS_BYTE |
            I2C_FUNC_SMBUS_I2C_BLOCK;
}

static const struct i2c_algorithm hi309x_i2c_algo = {
    .master_xfer = hi309x_i2c_xfer,
    .functionality = hi309x_i2c_func,
};

static int hi309x_i2c_init_adap(struct i2c_adapter* const adap, struct hi309x_i2c_dev* const i2c,
    struct platform_device* const pdev)
{
    int status;
    char *name;

    for (int i = 0; i < sizeof(g_hi309x_i2c_phybase) / sizeof(g_hi309x_i2c_phybase[0]); i++) {
        if (g_hi309x_i2c_phybase[i] == i2c->phybase) {
            name = g_hi309x_i2c_name[i];
            break;
        }
    }

    i2c_set_adapdata(adap, i2c);
    adap->owner = THIS_MODULE;
    strlcpy(adap->name, name, sizeof(adap->name));
    adap->dev.parent = &pdev->dev;
    adap->dev.of_node = pdev->dev.of_node;
    adap->algo = &hi309x_i2c_algo;

    /* Add the i2c adapter */
    status = i2c_add_adapter(adap);
    if (status) {
        dev_err(i2c->dev, "failed to add bus to i2c core\n");
    }

    return status;
}

static int hi309x_i2c_probe(struct platform_device *pdev)
{
    int status;
    struct hi309x_i2c_dev *i2c;
    struct i2c_adapter *adap = NULL;
    struct resource *res = NULL;

    i2c = devm_kzalloc(&pdev->dev, sizeof(*i2c), GFP_KERNEL);
    if (i2c == NULL)
        return -ENOMEM;

    platform_set_drvdata(pdev, i2c);
    i2c->dev = &pdev->dev;
    spin_lock_init(&i2c->lock);
    init_completion(&i2c->msg_complete);

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (res == NULL) {
        dev_err(i2c->dev, "Invalid mem resource./n");
        return -ENODEV;
    }

    i2c->phybase = res->start;
    i2c->base = devm_ioremap_resource(&pdev->dev, res);
    if (IS_ERR(i2c->base)) {
        dev_err(i2c->dev, "cannot ioremap resource\n");
        return -ENOMEM;
    }

    if (of_property_read_u32(pdev->dev.of_node, "clock-frequency", &i2c->freq)) {
        dev_warn(i2c->dev, "setting default clock-frequency@%dHz\n", HI309X_I2C_DEFAULT_FREQUENCY);
        i2c->freq = HI309X_I2C_DEFAULT_FREQUENCY;
    }

    adap = &i2c->adap;
    status = hi309x_i2c_init_adap(adap, i2c, pdev);

    return status;
}

static int hi309x_i2c_remove(struct platform_device *pdev)
{
    struct hi309x_i2c_dev *i2c = platform_get_drvdata(pdev);

    clk_disable_unprepare(i2c->clk);
    i2c_del_adapter(&i2c->adap);

    return 0;
}

static const struct of_device_id g_hi309x_i2c_match[] = {
    {.compatible = "hisilicon,hi309x-i2c"},
    {},
};
MODULE_DEVICE_TABLE(of, g_hi309x_i2c_match);

static struct platform_driver g_hi309x_i2c_driver = {
    .driver = {
        .name = "hi309x_i2c",
        .of_match_table = g_hi309x_i2c_match,
    },
    .probe = hi309x_i2c_probe,
    .remove = hi309x_i2c_remove,
};

module_platform_driver(g_hi309x_i2c_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("i2c driver");