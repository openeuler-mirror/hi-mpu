// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * timer module sdk interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/io.h>
#include "comm_knl_api.h"
#include "timer_core.h"

struct hisi_timer_mgr {
    s32 init_flag[HISI_TIMER_MAX_NUM];
    struct hisi_timer hitimer[HISI_TIMER_MAX_NUM];
    s32 chrdev_registered;
    cdev_st timer_dev;
};

struct hisi_timer_of_cfg {
    u32 timer_id;
    s32 v_irq;
    phys_addr_t phys_base;
    unsigned long phys_size;
    u32 clk_sel;
};

static struct hisi_timer_mgr g_hitimer_mgr;

static s32 hisi_timer_mgr_init(u32 timer_id)
{
    struct hisi_timer *hitimer = NULL;

    if (timer_id >= HISI_TIMER_MAX_NUM) {
        return -EINVAL;
    }

    hitimer = &g_hitimer_mgr.hitimer[timer_id];
    g_hitimer_mgr.init_flag[timer_id] = 1;
    hitimer_init(hitimer, timer_id);

    return 0;
}

static void hisi_timer_mgr_deinit(u32 timer_id)
{
    struct hisi_timer *hitimer = NULL;

    if (timer_id >= HISI_TIMER_MAX_NUM) {
        return;
    }

    hitimer = &g_hitimer_mgr.hitimer[timer_id];
    g_hitimer_mgr.init_flag[timer_id] = 0;
    hitimer_deinit(hitimer);
}

/*
 * A timer cannot be applied for multiple times.
 */
struct hisi_timer *hitimer_request(u32 timer_id, const char *module_name, void *priv_data)
{
    s32 ret;
    struct hisi_timer *hitimer = NULL;

    if (timer_id >= HISI_TIMER_MAX_NUM || module_name == NULL) {
        LOG(LOG_ERROR, "inval parameter timer_id(%u) module_name(%p)\n", timer_id, module_name);
        return NULL;
    }

    if (g_hitimer_mgr.init_flag[timer_id] == 0) {
        LOG(LOG_ERROR, "g_hitimer_mgr not ready!\n");
        return NULL;
    }

    hitimer = &g_hitimer_mgr.hitimer[timer_id];
    ret = hitimer_reference(hitimer, module_name);
    if (ret) {
        LOG(LOG_ERROR, "inc ref for hitimer %u error(%d)\n", timer_id, ret);
        return NULL;
    }

    hitimer->ops->set_priv_data(hitimer, priv_data);

    return hitimer;
}
EXPORT_SYMBOL(hitimer_request);

/*
 * When the timer is not used, this function needs to be called. Otherwise, the timer cannot be used by other modules.
 * The timer can be released only by the requested module.
 */
void hitimer_release(u32 timer_id, const char *module_name)
{
    s32 ret;
    struct hisi_timer *hitimer = NULL;

    if (timer_id >= HISI_TIMER_MAX_NUM) {
        return;
    }

    if (g_hitimer_mgr.init_flag[timer_id] == 0) {
        /* hitimer mgr is not initialized */
        LOG(LOG_ERROR, "g_hitimer_mgr not ready!\n");
        return;
    }

    hitimer = &g_hitimer_mgr.hitimer[timer_id];
    ret = hitimer_unreference(hitimer, module_name);
    if (ret) {
        LOG(LOG_ERROR, "hitimer_unreference for timer%u err(%d)\n", timer_id, ret);
        return;
    }

    hitimer->ops->unregister_irq_handler(hitimer);
    hitimer->ops->set_priv_data(hitimer, NULL);

    return;
}
EXPORT_SYMBOL(hitimer_release);

static s32 hisi_timer_mgr_parse_of_node(struct device_node *np, struct hisi_timer_of_cfg *cfg)
{
    s32 ret;
    struct resource res;

    ret = of_property_read_u32(np, "ids", &cfg->timer_id);
    if (ret) {
        LOG(LOG_ERROR, "hisi_timer_parse_of: read timer_id err %d\n", ret);
        return ret;
    }

    ret = of_address_to_resource(np, 0, &res);
    if (ret) {
        LOG(LOG_ERROR, "hisi_timer_parse_of: read phys_base err %d\n", ret);
        return ret;
    }

    cfg->phys_base = res.start;
    cfg->phys_size = resource_size(&res);

    ret = of_property_read_u32(np, "clock-sel", &cfg->clk_sel);
    if (ret) {
        LOG(LOG_ERROR, "hisi_timer_parse_of: read clock_select err %d\n", ret);
        return ret;
    }

    cfg->v_irq = irq_of_parse_and_map(np, 0);
    if (cfg->v_irq == 0) {
        LOG(LOG_ERROR, "hisi_timer_parse_of: map hw_irq err\n");
        return -EINVAL;
    }

    return 0;
}

const struct file_operations g_timer_test_ops = {
    .owner = THIS_MODULE,
};

static s32 hisi_timer_of_probe(struct platform_device *pdev)
{
    s32 ret;
    struct hisi_timer_of_cfg cfg = {0};
    struct hisi_timer *hitimer = NULL;

    ret = hisi_timer_mgr_parse_of_node(pdev->dev.of_node, &cfg);
    if (ret) {
        LOG(LOG_ERROR, "hisi_timer_mgr_parse_of err(%d)\n", ret);
        return -ENODEV;
    }

    if (cfg.timer_id >= HISI_TIMER_MAX_NUM) {
        LOG(LOG_ERROR, "timer_id %u from DTS is wrong(%d)\n", cfg.timer_id, ret);
        return -EINVAL;
    }

    if (!g_hitimer_mgr.chrdev_registered) {
        ret = comm_init_dev(&g_hitimer_mgr.timer_dev, &g_timer_test_ops, "hitimer");
        if (ret) {
            LOG(LOG_ERROR, "Create timer test char dev failed %d\n", ret);
            return ret;
        }

        g_hitimer_mgr.chrdev_registered = 1;
    }

    hitimer = &g_hitimer_mgr.hitimer[cfg.timer_id];
    ret = hisi_timer_mgr_init(cfg.timer_id);
    if (ret) {
        LOG(LOG_ERROR, "hisi_timer_mgr_init of timer %u err %d\n", cfg.timer_id, ret);
        goto err_free_chrdev;
    }

    ret = hitimer_setup_device(hitimer, cfg.v_irq,
                               cfg.phys_base, cfg.phys_size, cfg.clk_sel);
    if (ret) {
        goto err_mgr_deinit;
    }

    return ret;

err_mgr_deinit:
    hisi_timer_mgr_deinit(cfg.timer_id);
err_free_chrdev:
    if (g_hitimer_mgr.chrdev_registered) {
        g_hitimer_mgr.chrdev_registered = 0;
        comm_cleanup_dev(&g_hitimer_mgr.timer_dev);
    }
    return ret;
}

static s32 hisi_timer_of_remove(struct platform_device *pdev)
{
    s32 i;
    s32 is_held;
    s8 held_module[MODULE_NAME_LEN];
    s8 *module_name = NULL;
    s32 timer_held_mark = 0;

    for (i = 0; i < HISI_TIMER_MAX_NUM; i++) {
        is_held = 0;
        module_name = held_module;

        (void)hitimer_query_reference(&g_hitimer_mgr.hitimer[i], &is_held, module_name, MODULE_NAME_LEN);
        if (is_held) {
            timer_held_mark |= (1UL << i);
            LOG(LOG_ERROR, "hitimer %d is referenced by module %s\n", i, module_name);
        }
    }

    if (timer_held_mark) {
        /*
         * Actually this check is not neccesary,
         * 'cause if any other module are using this module, rmmod won't succeed
         * Here we keep it so we can detect the scenario that other module removed without calling hitimer_release()
         */
        LOG(LOG_ERROR, "hisi_timer_remove: cannot remove module since other module(s) has reference(s)\n");
        return -EBUSY;
    }

    /*
     * Note: consider race condition(CPU0 is calling rmmod, CPU1 is calling hitimer_request)
     *       since CPU1 is referencing hitimer.ko, rmmod cannot succeed, so we don't care for now
     */
    for (i = 0; i < HISI_TIMER_MAX_NUM; i++) {
        (void)hitimer_close_device(&g_hitimer_mgr.hitimer[i]);
        hisi_timer_mgr_deinit(i);
    }

    /* Release chrdev */
    if (g_hitimer_mgr.chrdev_registered) {
        g_hitimer_mgr.chrdev_registered = 0;
        comm_cleanup_dev(&g_hitimer_mgr.timer_dev);
    }

    return 0;
}

static const struct of_device_id g_hisi_timer_of_match[] = {
    { .compatible = "hisilicon,hisi-timer" },
    {}
};

static struct platform_driver g_hisilicon_timer_driver = {
    .driver = {
        .name = "hisi-timer",
        .of_match_table = g_hisi_timer_of_match,
    },
    .probe = hisi_timer_of_probe,
    .remove = hisi_timer_of_remove,
};

module_platform_driver(g_hisilicon_timer_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("timer driver");