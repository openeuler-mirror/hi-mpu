// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * timer driver core API header file
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include "timer_regs.h"
#include "comm_knl_api.h"
#include "timer_core.h"

#define timer_reg_addr(hitimer, offset) (void *)(((unsigned long)((hitimer)->virt_base)) + (offset))

static inline s32 hitimer_ready(struct hisi_timer *hitimer)
{
    return (hitimer->init_magic == HISI_TIMER_INIT_MAGIC);
}

void hitimer_set_priv_data(struct hisi_timer *hitimer, void *priv_data)
{
    if (hitimer) {
        hitimer->priv_data = priv_data;
    }
}

void *hitimer_get_priv_data(struct hisi_timer *hitimer)
{
    return hitimer ? hitimer->priv_data : NULL;
}

s32 hitimer_get_bgload_value(struct hisi_timer *hitimer,
                             u32 *bgload_l, u32 *bgload_h)
{
    u32 reg_val;

    if (hitimer == NULL || bgload_l == NULL || bgload_h == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    reg_val = readl(timer_reg_addr(hitimer, TIMER_BGLOAD_H_REG));
    *bgload_h = reg_val;
    reg_val = readl(timer_reg_addr(hitimer, TIMER_BGLOAD_L_REG));
    *bgload_l = reg_val;

    return 0;
}

/*
 * It is used for periodic counting in periodic counting mode.
 * Note that the load value cannot be set to 0 and the minimum value is 1. (For details, see nManager.)
 */
s32 hitimer_set_bgload_value(struct hisi_timer *hitimer,
                             u32 bgload_l, u32 bgload_h)
{
    if (hitimer == NULL || bgload_l == 0 || bgload_h == 0) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    writel(bgload_h, timer_reg_addr(hitimer, TIMER_BGLOAD_H_REG));
    writel(bgload_l, timer_reg_addr(hitimer, TIMER_BGLOAD_L_REG));

    return 0;
}

s32 hitimer_get_load_value(struct hisi_timer *hitimer, u32 *load_l, u32 *load_h)
{
    u32 reg_val;

    if (hitimer == NULL || load_l == NULL || load_h == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    reg_val = readl(timer_reg_addr(hitimer, TIMER_LOAD_H_REG));
    *load_h = reg_val;
    reg_val = readl(timer_reg_addr(hitimer, TIMER_LOAD_L_REG));
    *load_l = reg_val;

    return 0;
}

s32 hitimer_set_load_value(struct hisi_timer *hitimer, u32 load_l, u32 load_h)
{
    if (hitimer == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    writel(load_h, timer_reg_addr(hitimer, TIMER_LOAD_H_REG));
    writel(load_l, timer_reg_addr(hitimer, TIMER_LOAD_L_REG));

    return 0;
}

s32 hitimer_get_cnt_value(struct hisi_timer *hitimer, u32 *cnt_val_l, u32 *cnt_val_h)
{
    if (hitimer == NULL || cnt_val_l == NULL || cnt_val_h == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    *cnt_val_l = readl(timer_reg_addr(hitimer, TIMER_VALUE_L_REG));
    *cnt_val_h = readl(timer_reg_addr(hitimer, TIMER_VALUE_H_REG));

    return 0;
}

s32 hitimer_get_timer_ctrl(struct hisi_timer *hitimer, hisi_timer_mode *mode,
                           hisi_timer_prediv *prediv, hisi_timer_cnt_sz *cnt_sz)
{
    u32 reg_val;

    if (hitimer == NULL || mode == NULL
        || prediv == NULL || cnt_sz == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    reg_val = readl(timer_reg_addr(hitimer, TIMER_CTRL_REG));
    if (reg_val & TIMER_CTRL_ONESHOT) {
        *mode = TIMER_ONESHOT;
    } else {
        *mode = (reg_val & TIMER_CTRL_MODE) ? TIMER_PERIODIC : TIMER_FREE_RUN;
    }

    *prediv = ((reg_val & TIMER_CTRL_PREDIV) >> TIMER_CTRL_PREDIV_SHIFT);
    *cnt_sz = (reg_val & TIMER_CTRL_SZ) ? TIMER_CNT_64BIT : TIMER_CNT_32BIT;

    return 0;
}

static u32 hitimer_get_mode_mask(u32 mode)
{
    if (mode == TIMER_FREE_RUN) {
        return 0;
    } else if (mode == TIMER_PERIODIC) {
        return TIMER_CTRL_MODE;
    } else if (mode == TIMER_ONESHOT) {
        return TIMER_CTRL_ONESHOT;
    } else {
        return TIMER_CTRL_INVAL;
    }
}

s32 hitimer_set_timer_ctrl(struct hisi_timer *hitimer, hisi_timer_mode mode,
                           hisi_timer_prediv prediv, hisi_timer_cnt_sz cnt_sz)
{
    u32 mode_mask;
    u32 reg_val;

    if (hitimer == NULL || mode >= TIMER_MODE_MAX
        || prediv >= TIMER_PREDIV_MAX || cnt_sz >= TIMER_CNT_SZ_MAX) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    reg_val = readl(timer_reg_addr(hitimer, TIMER_CTRL_REG));
    reg_val &= ~(TIMER_CTRL_MODE | TIMER_CTRL_PREDIV | TIMER_CTRL_SZ | TIMER_CTRL_ONESHOT);

    mode_mask = hitimer_get_mode_mask(mode);
    if (mode_mask == TIMER_CTRL_INVAL) {
        return -EINVAL;
    }

    reg_val |= mode_mask;
    reg_val |= ((((u32)(prediv)) & 0x3U) << TIMER_CTRL_PREDIV_SHIFT);
    reg_val |= (cnt_sz == TIMER_CNT_32BIT) ? 0 : TIMER_CTRL_SZ;
    writel(reg_val, timer_reg_addr(hitimer, TIMER_CTRL_REG));

    return 0;
}

s32 hitimer_register_irq_handler(struct hisi_timer *hitimer, hitimer_irq_handler_t handler)
{
    unsigned long irqflag = 0;

    if (hitimer == NULL || handler == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    disable_irq(hitimer->v_irq);
    /*
     * Even if we called disable_irq()
     * It does not mean we can safely reassign irq_handler
     * (Another CPU may already entered irq_handler and processing)
     * We must obtain irq_lock to ensure
     */
    spin_lock_irqsave(&hitimer->irq_lock, irqflag);
    hitimer->irq_handler = handler;
    spin_unlock_irqrestore(&hitimer->irq_lock, irqflag);
    enable_irq(hitimer->v_irq);

    return 0;
}

s32 hitimer_unregister_irq_handler(struct hisi_timer *hitimer)
{
    unsigned long irqflag = 0;
    if (hitimer == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    disable_irq(hitimer->v_irq);
    /*
     * Even if we called disable_irq()
     * It does not mean we can safely reassign irq_handler
     * (Another CPU may already entered irq_handler and processing)
     * We must obtain irq_lock to ensure
     */
    spin_lock_irqsave(&hitimer->irq_lock, irqflag);
    hitimer->irq_handler = NULL;
    spin_unlock_irqrestore(&hitimer->irq_lock, irqflag);
    enable_irq(hitimer->v_irq);

    return 0;
}

s32 hitimer_enable_irq(struct hisi_timer *hitimer)
{
    u32 reg_val;

    if (hitimer == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    reg_val = readl(timer_reg_addr(hitimer, TIMER_CTRL_REG));
    reg_val |= TIMER_CTRL_INTEN;
    writel(reg_val, timer_reg_addr(hitimer, TIMER_CTRL_REG));

    return 0;
}

s32 hitimer_disable_irq(struct hisi_timer *hitimer)
{
    u32 reg_val;

    if (hitimer == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    reg_val = readl(timer_reg_addr(hitimer, TIMER_CTRL_REG));
    reg_val &= ~TIMER_CTRL_INTEN;
    writel(reg_val, timer_reg_addr(hitimer, TIMER_CTRL_REG));

    return 0;
}

s32 hitimer_get_enable(struct hisi_timer *hitimer, s32 *en)
{
    u32 reg_val;

    if (hitimer == NULL || en == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    reg_val = readl(timer_reg_addr(hitimer, TIMER_CTRL_REG));
    *en = (reg_val & TIMER_CTRL_EN) ? 1 : 0;

    return 0;
}

s32 hitimer_start(struct hisi_timer *hitimer)
{
    u32 reg_val;

    if (hitimer == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    reg_val = readl(timer_reg_addr(hitimer, TIMER_CTRL_REG));
    reg_val |= TIMER_CTRL_EN;
    writel(reg_val, timer_reg_addr(hitimer, TIMER_CTRL_REG));

    return 0;
}

s32 hitimer_stop(struct hisi_timer *hitimer)
{
    u32 reg_val;

    if (hitimer == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    reg_val = readl(timer_reg_addr(hitimer, TIMER_CTRL_REG));
    reg_val &= ~TIMER_CTRL_EN;
    writel(reg_val, timer_reg_addr(hitimer, TIMER_CTRL_REG));

    return 0;
}

s32 hitimer_read_reg(struct hisi_timer *hitimer, u32 reg_off, u32 *val)
{
    if (hitimer == NULL || val == NULL || reg_off > TIMER_REG_OFF_MAX) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    *val = readl(timer_reg_addr(hitimer, reg_off));

    return 0;
}

s32 hitimer_write_reg(struct hisi_timer *hitimer, u32 reg_off, u32 val)
{
    if (hitimer == NULL || reg_off > TIMER_REG_OFF_MAX) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    writel(val, timer_reg_addr(hitimer, reg_off));

    return 0;
}

s32 hitimer_debug_on(struct hisi_timer *hitimer, s32 dbg_flag)
{
    if (hitimer == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    hitimer->dbg_flag = dbg_flag;

    return 0;
}

/* Core management */
static irqreturn_t hitimer_default_interrupt_handler(s32 irq, void *dev_id)
{
    unsigned long irqflags = 0;
    struct hisi_timer *hitimer = (struct hisi_timer *)dev_id;
    u32 val_h = 0xDEADBEEF;
    u32 val_l = 0xDEADBEEF;

    if (hitimer == NULL) {
        return IRQ_HANDLED;
    }

    if (hitimer->dbg_flag) {
        hitimer->ops->get_cnt_value(hitimer, &val_l, &val_h);
        printk("Hisi Timer%u IRQ: Counter High(0x%x) Low(0x%x)\n", hitimer->id, val_h, val_l);
        printk("                  Int status: 0x%08x Int Mask status: 0x%08x\n",
               readl(timer_reg_addr(hitimer, TIMER_RIS_REG)),
               readl(timer_reg_addr(hitimer, TIMER_MIS_REG)));
        printk("Hisi Timer%u IRQ: clear intterupt\n", hitimer->id);
    }

    /*
     * Clear int status: according to register description
     * write any value to clear timer interrupt
     */
    writel(1, timer_reg_addr(hitimer, TIMER_INTCLR_REG));

    spin_lock_irqsave(&hitimer->irq_lock, irqflags);
    if (hitimer->irq_handler) {
        hitimer->irq_handler(hitimer);
    }
    spin_unlock_irqrestore(&hitimer->irq_lock, irqflags);

    if (hitimer->dbg_flag) {
        printk("Int status: 0x%08x Int Mask status: 0x%08x\n",
               readl(timer_reg_addr(hitimer, TIMER_RIS_REG)),
               readl(timer_reg_addr(hitimer, TIMER_MIS_REG)));
    }

    return IRQ_HANDLED;
}

static hisi_timer_operations g_hitimer_ops = {
    .set_priv_data = hitimer_set_priv_data,
    .get_priv_data = hitimer_get_priv_data,
    .get_bgload_value = hitimer_get_bgload_value,
    .set_bgload_value = hitimer_set_bgload_value,
    .get_load_value = hitimer_get_load_value,
    .set_load_value = hitimer_set_load_value,
    .get_cnt_value = hitimer_get_cnt_value,
    .get_timer_ctrl = hitimer_get_timer_ctrl,
    .set_timer_ctrl = hitimer_set_timer_ctrl,
    .register_irq_handler = hitimer_register_irq_handler,
    .unregister_irq_handler = hitimer_unregister_irq_handler,
    .enable_irq = hitimer_enable_irq,
    .disable_irq = hitimer_disable_irq,
    .get_enable = hitimer_get_enable,
    .start = hitimer_start,
    .stop = hitimer_stop,
    .read_reg = hitimer_read_reg,
    .write_reg = hitimer_write_reg,
    .debug_on = hitimer_debug_on,
};

void hitimer_init(struct hisi_timer *hitimer, u32 timer_id)
{
    (void)memset_s(hitimer, sizeof(struct hisi_timer), 0, sizeof(struct hisi_timer));
    hitimer->id = timer_id;
    (void)snprintf_s(hitimer->irq_name, HISI_TIMER_IRQ_NAME_LEN, HISI_TIMER_IRQ_NAME_LEN - 1, "hitmr_irq%u", timer_id);
    hitimer->init_magic = HISI_TIMER_INIT_MAGIC;
    hitimer->clock_sel = HISI_TIMER_CLKSEL_BUS_FREQ;
    hitimer->held = 0;
    hitimer->dbg_flag = 1;
    spin_lock_init(&hitimer->lock);
    spin_lock_init(&hitimer->irq_lock);
    hitimer->ops = &g_hitimer_ops;
}

void hitimer_deinit(struct hisi_timer *hitimer)
{
    (void)memset_s(hitimer, sizeof(struct hisi_timer), 0, sizeof(struct hisi_timer));
}

s32 hitimer_setup_device(struct hisi_timer *hitimer, s32 v_irq, phys_addr_t phys_base, unsigned long phys_size,
                         u32 clk_sel)
{
    s32 ret;

    if (hitimer == NULL || phys_base == 0) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    hitimer->v_irq = v_irq;
    hitimer->phys_base = phys_base;
    hitimer->virt_base = ioremap(phys_base, phys_size);
    if (!hitimer->virt_base) {
        return -ENOMEM;
    }

    ret = hitimer_disable_irq(hitimer);
    if (ret) {
        goto out_unmap;
    }

    ret = request_irq(hitimer->v_irq, hitimer_default_interrupt_handler,
                      0, hitimer->irq_name, (void *)hitimer);
    if (ret) {
        goto out_unmap;
    }

    return 0;
out_unmap:
    iounmap(hitimer->virt_base);
    return ret;
}

s32 hitimer_close_device(struct hisi_timer *hitimer)
{
    s32 ret;

    if (hitimer == NULL) {
        return -EINVAL;
    }

    if (!hitimer_ready(hitimer)) {
        /* Timer is not initialized, may happen if timer is not defined in DTS, just return OK */
        return 0;
    }

    if (hitimer->held) {
        return -EBUSY;
    }

    ret = hitimer_stop(hitimer);
    if (ret) {
        return ret;
    }

    ret = hitimer_disable_irq(hitimer);
    if (ret) {
        return ret;
    }

    disable_irq(hitimer->v_irq);
    free_irq(hitimer->v_irq, hitimer);
    iounmap(hitimer->virt_base);
    hitimer->init_magic = 0;

    return 0;
}

s32 hitimer_reference(struct hisi_timer *hitimer, const char *module_name)
{
    s32 ret = 0;

    if (hitimer == NULL ||
        module_name == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    spin_lock(&hitimer->lock);
    if (hitimer->held) {
        ret = -EBUSY;
        goto out_busy;
    }

    hitimer->held = 1;
    (void)strncpy_s(hitimer->held_module, sizeof(hitimer->held_module), module_name, MODULE_NAME_LEN);
out_busy:
    spin_unlock(&hitimer->lock);
    return ret;
}

s32 hitimer_unreference(struct hisi_timer *hitimer, const char *module_name)
{
    s32 ret = 0;

    if (hitimer == NULL || module_name == NULL) {
        return -EINVAL;
    }

    if (unlikely(!hitimer_ready(hitimer))) {
        return -ENODEV;
    }

    spin_lock(&hitimer->lock);
    if (hitimer->held) {
        /* Only the requested module itself can unreference the timer */
        if (strncmp(hitimer->held_module, module_name, MODULE_NAME_LEN) != 0) {
            ret = -EPERM;
            goto out_noperm;
        }

        hitimer->held = 0;
        (void)memset_s(hitimer->held_module, sizeof(hitimer->held_module), 0, sizeof(hitimer->held_module));
    }

out_noperm:
    spin_unlock(&hitimer->lock);
    return ret;
}

s32 hitimer_query_reference(struct hisi_timer *hitimer, u32 *is_held, char *held_module, size_t dest_max)
{
    if (hitimer == NULL || is_held == NULL || held_module == NULL) {
        return -EINVAL;
    }

    if (strlen(hitimer->held_module) >= dest_max) {
        return -EINVAL;
    }

    *is_held = 0;

    if (unlikely(!hitimer_ready(hitimer))) {
        /* If timer is not defined in DTS, this may happen, just return OK */
        return 0;
    }

    spin_lock(&hitimer->lock);
    if (hitimer->held) {
        *is_held = 1;
        (void)strncpy_s(held_module, dest_max, hitimer->held_module, strlen(hitimer->held_module));
    }
    spin_unlock(&hitimer->lock);

    return 0;
}
