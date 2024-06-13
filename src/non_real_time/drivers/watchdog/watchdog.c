// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * watchdog module sdk interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "watchdog.h"

#include "watchdog_usr_api.h"
#include "wdg_csr_addr_define.h"
#include "wdg_c_union_define.h"
#include "sysctrl_csr_addr_define.h"
#include "sysctrl.h"
#include "../timer/timer.h"


cdev_st g_watchdog_dev;
const s8 *g_watchdog_dev_name = DEV_WATCHDOG_NAME;

wdg_priv_info_s g_wdg_priv_info[WDG_MAX_NUM];

static void watchdog_lock(u32 wdg_id, u32 lock)
{
    /* Disabling WDG Register Write Access */
    if (lock) {
        iowrite32(0x0, g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_LOCK_OFFSET_ADDR);
    } else {
        /* Allows WDG register write access. */
        iowrite32(0x1acce551, g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_LOCK_OFFSET_ADDR);
    }

    return;
}

static void watchdog_clr_timer_isr(u32 wdg_id)
{
    LOG(LOG_INFO, "wdg%u rcv int", wdg_id);

    if (!g_wdg_priv_info[wdg_id].feed_by_kernel || g_wdg_priv_info[wdg_id].stop_feed) {
        return;
    }

    LOG(LOG_INFO, "wdg%u feed_by_kernel", wdg_id);

    watchdog_lock(wdg_id, 0);

    /* Clear the interrupt and start counting again. */
    iowrite32(0xffffffff, g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_INTCLR_OFFSET_ADDR);

    watchdog_lock(wdg_id, 1);

    return;
}

static s32 watchdog_enable(u32 wdg_id, u32 enable)
{
    s32 ret;
    u32 wdg_load;
    u32 dlay_rst_time;
    u32 bits;
    u32 mask;
    u_wdg_control wdg_ctl;
    struct hisi_timer *timer = NULL;

    if (wdg_id >= WDG_MAX_NUM) {
        LOG(LOG_ERROR, "watchdog enable wdg_id(0x%x) is inval.\n", wdg_id);
        return -EINVAL;
    }

    timer = g_wdg_priv_info[wdg_id].feed_timer;
    if (timer == NULL) {
        LOG(LOG_ERROR, "watchdog(0x%x) timer is inval.\n", wdg_id);
        return -EINVAL;
    }

    ret = down_interruptible(&g_wdg_priv_info[wdg_id].cmd_access_sem);
    if (ret) {
        LOG(LOG_ERROR, "wdg(0x%x) set enable, down_interruptible err(%d)!", wdg_id, ret);
        return ret;
    }

    watchdog_lock(wdg_id, 0);

    if (enable) {
        /* Outputs heartbeat signals to reset the non-secure domain of the BMC. The reset is not masked. */
        bits = (0xCU << (wdg_id * 4)); // mul 4
        mask = (0xFU << (wdg_id * 4)); // mul 4
        (void)sysctrl_reg_read_write(SYSCTRL_CSR_WD_CTRL_REG_OFFSET_ADDR, mask, bits);

        /*
         * Set the reset delay after the watchdog generates the reset signal,
         * rst_dly_reg_val = seconds * (ref_frequency / 1000)
         */
        dlay_rst_time = WDG_DELAY_RESET_TIME * (SYS_REF_CLOCK_FREQ / 1000);
        (void)sysctrl_reg_write(SYSCTRL_CSR_WD_DLY_CFG_OFFSET_ADDR, dlay_rst_time);

        /* Configure the default timeout period of the watchdog, wdg_feed_reg_val = seconds * (ref_frequency / 1000) */
        wdg_load = WDG_TIMEOUT * (SYS_REF_CLOCK_FREQ / 1000);
        iowrite32(wdg_load, g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_LOAD_OFFSET_ADDR);
    }

    /* Configuring the WDG Function */
    wdg_ctl.val = ioread32(g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_CONTROL_OFFSET_ADDR);
    wdg_ctl.bits.inten = enable;
    wdg_ctl.bits.resen = enable;
    iowrite32(wdg_ctl.val, g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_CONTROL_OFFSET_ADDR);

    watchdog_lock(wdg_id, 1);

    g_wdg_priv_info[wdg_id].wait_counter = 0;

    /* Enable the timer. */
    if (enable) {
        (void)timer->ops->start(timer);
    } else {
        (void)timer->ops->stop(timer);
    }

    up(&g_wdg_priv_info[wdg_id].cmd_access_sem);

    return 0;
}

static s32 watchdog_feed(u32 wdg_id, u32 feed_by_kernel)
{
    s32 ret;
    struct hisi_timer *timer = NULL;

    if (wdg_id >= WDG_MAX_NUM) {
        LOG(LOG_ERROR, "watchdog set feed wdg_id(0x%x) is inval.\n", wdg_id);
        return -EINVAL;
    }

    timer = g_wdg_priv_info[wdg_id].feed_timer;
    if (timer == NULL) {
        LOG(LOG_ERROR, "watchdog(0x%x) timer is inval.\n", wdg_id);
        return -EINVAL;
    }

    ret = down_interruptible(&g_wdg_priv_info[wdg_id].cmd_access_sem);
    if (ret) {
        LOG(LOG_ERROR, "wdg(0x%x) set feed, down_interruptible err(%d)!", wdg_id, ret);
        return -EINVAL;
    }

    if (feed_by_kernel != g_wdg_priv_info[wdg_id].feed_by_kernel) {
        g_wdg_priv_info[wdg_id].feed_by_kernel = feed_by_kernel;
        g_wdg_priv_info[wdg_id].wait_counter = 0;
    }

    up(&g_wdg_priv_info[wdg_id].cmd_access_sem);

    return 0;
}

static s32 watchdog_clr(u32 wdg_id)
{
    s32 ret;

    if (wdg_id >= WDG_MAX_NUM) {
        LOG(LOG_ERROR, "watchdogset enable wdg_id(0x%x) is inval.\n", wdg_id);
        return -EINVAL;
    }

    if (g_wdg_priv_info[wdg_id].stop_feed) {
        return 0;
    }

    ret = down_interruptible(&g_wdg_priv_info[wdg_id].cmd_access_sem);
    if (ret) {
        LOG(LOG_ERROR, "wdg(0x%x) set enable, down_interruptible err(%d)!", wdg_id, ret);
        return ret;
    }

    watchdog_lock(wdg_id, 0);

    /* Clear historical interrupts and start counting again. */
    iowrite32(0xffffffff, g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_INTCLR_OFFSET_ADDR);

    watchdog_lock(wdg_id, 1);

    /* Clear timer timeout */
    g_wdg_priv_info[wdg_id].wait_counter = 0;

    up(&g_wdg_priv_info[wdg_id].cmd_access_sem);

    return 0;
}

static s32 watchdog_stop(u32 wdg_id)
{
    s32 ret;
    u32 wdg_load;
    u_wdg_control wdg_ctl;
    struct hisi_timer *timer = NULL;

    if (wdg_id >= WDG_MAX_NUM) {
        LOG(LOG_ERROR, "watchdog_stop wdg_id(0x%x) is inval.\n", wdg_id);
        return -EINVAL;
    }

    timer = g_wdg_priv_info[wdg_id].feed_timer;
    if (timer == NULL) {
        LOG(LOG_ERROR, "watchdog(0x%x) timer is inval.\n", wdg_id);
        return -EINVAL;
    }

    ret = down_interruptible(&g_wdg_priv_info[wdg_id].cmd_access_sem);
    if (ret) {
        LOG(LOG_ERROR, "watchdog(%u) stop down_interruptible err(%d)!", wdg_id, ret);
        return ret;
    }

    /* Disable the watchdog feeding timer. */
    (void)timer->ops->stop(timer);

    g_wdg_priv_info[wdg_id].stop_feed = 0x1;
    g_wdg_priv_info[wdg_id].wait_counter = WDG_APP_WAIT_COUNTER;

    watchdog_lock(wdg_id, 0);

    /* Disable the watchdog. */
    wdg_ctl.val = ioread32(g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_CONTROL_OFFSET_ADDR);
    wdg_ctl.bits.inten = 0x0;
    wdg_ctl.bits.resen = 0x0;
    iowrite32(wdg_ctl.val, g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_CONTROL_OFFSET_ADDR);

    /*
     * Configure the default timeout time of the watchdog. It should be set to 0, indicating that the watchdog
     * is reset immediately. To reserve time for the secure M3 to process interrupts, set this parameter to 2s.
     */
    /* wdg_feed_reg_val = seconds * (ref_frequency / 1000) */
    wdg_load = WDG_DELAY_RESET_TIME * (SYS_REF_CLOCK_FREQ / 1000);
    iowrite32(wdg_load, g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_LOAD_OFFSET_ADDR);

    /* Enable the watchdog. */
    wdg_ctl.val = ioread32(g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_CONTROL_OFFSET_ADDR);
    wdg_ctl.bits.inten = 0x1;
    wdg_ctl.bits.resen = 0x1;
    iowrite32(wdg_ctl.val, g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_CONTROL_OFFSET_ADDR);

    watchdog_lock(wdg_id, 1);

    up(&g_wdg_priv_info[wdg_id].cmd_access_sem);

    return 0;
}

s32 watchdog_enable_set(unsigned long arg)
{
    s32 ret;
    u32 wdg_id;
    wdg_enable_s wdt_enable_info;
    u_wdg_control wdg_ctl;
    struct hisi_timer *timer = NULL;

    ret = copy_from_user(&wdt_enable_info, (void *)(uintptr_t)arg, sizeof(wdt_enable_info));
    if (ret) {
        LOG(LOG_ERROR, "watchdog enable copy_from_user failed.\n");
        return -EFAULT;
    }

    wdg_id = wdt_enable_info.wdg_id;
    if (wdg_id >= WDG_MAX_NUM) {
        LOG(LOG_ERROR, "watchdogset enable wdg_id(0x%x) is inval.\n", wdg_id);
        return -EINVAL;
    }

    timer = g_wdg_priv_info[wdg_id].feed_timer;
    if (timer == NULL) {
        LOG(LOG_ERROR, "wdg(0x%x) timer is inval.\n", wdg_id);
        return -EINVAL;
    }

    ret = down_interruptible(&g_wdg_priv_info[wdg_id].cmd_access_sem);
    if (ret) {
        LOG(LOG_ERROR, "wdg(0x%x) enable, down_interruptible err(%d)!", wdg_id, ret);
        return ret;
    }

    watchdog_lock(wdg_id, 0);

    /* Configuring the WDG Function */
    wdg_ctl.val = ioread32(g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_CONTROL_OFFSET_ADDR);
    wdg_ctl.bits.inten = ((wdt_enable_info.enable != 0) ? 0x1 : 0);
    iowrite32(wdg_ctl.val, g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_CONTROL_OFFSET_ADDR);

    /* Clear historical interrupts and start counting again. */
    iowrite32(0xffffffff, g_wdg_priv_info[wdg_id].map_addr + WDG_CSR_INTCLR_OFFSET_ADDR);

    watchdog_lock(wdg_id, 1);

    /* Enable the timer. */
    if (wdt_enable_info.enable) {
        (void)timer->ops->start(timer);
    } else {
        (void)timer->ops->stop(timer);
    }

    up(&g_wdg_priv_info[wdg_id].cmd_access_sem);

    return 0;
}

s32 watchdog_timeout_wait(unsigned long arg)
{
    s32 ret;
    u32 wdg_id;
    struct hisi_timer *timer = NULL;

    if (copy_from_user(&wdg_id, (void *)(uintptr_t)arg, sizeof(u32))) {
        LOG(LOG_ERROR, "dfx copy_from_user failed!");
        return -EFAULT;
    }

    if (wdg_id >= WDG_MAX_NUM) {
        LOG(LOG_ERROR, "wdg_id(%u) is invalid", wdg_id);
        return -EINVAL;
    }

    timer = g_wdg_priv_info[wdg_id].feed_timer;
    if (timer == NULL) {
        LOG(LOG_ERROR, "watchdog(%u) timer is NULL", wdg_id);
        return -EINVAL;
    }

    ret = wait_event_interruptible(g_wdg_priv_info[wdg_id].wait_queue,
        (g_wdg_priv_info[wdg_id].wait_counter >= WDG_APP_WAIT_COUNTER));
    if (ret == -ERESTARTSYS) {
        LOG(LOG_ERROR, "ERESTARTSYS happen, watchdig(%u) wait fail(%d)!", wdg_id, ret);
        ret = -EINTR;
    }

    return ret;
}

static s32 watchdog_timeout(unsigned long arg)
{
    s32 ret;
    u32 wdg_id;
    struct hisi_timer *timer = NULL;

    if (copy_from_user(&wdg_id, (void *)(uintptr_t)arg, sizeof(u32))) {
        LOG(LOG_ERROR, "dfx copy_from_user failed!");
        return -EFAULT;
    }

    if (wdg_id >= WDG_MAX_NUM) {
        LOG(LOG_ERROR, "watchdog_timeout wdg_id(0x%x) is inval.\n", wdg_id);
        return -EINVAL;
    }

    timer = g_wdg_priv_info[wdg_id].feed_timer;
    if (timer == NULL) {
        LOG(LOG_ERROR, "watchdog(0x%x) timer is inval(NULL).\n", wdg_id);
        return -EINVAL;
    }

    ret = down_interruptible(&g_wdg_priv_info[wdg_id].cmd_access_sem);
    if (ret) {
        LOG(LOG_ERROR, "watchdog(%u) stop down_interruptible err(%d)!", wdg_id, ret);
        return ret;
    }

    g_wdg_priv_info[wdg_id].stop_feed = 0x1;
    g_wdg_priv_info[wdg_id].wait_counter = WDG_APP_WAIT_COUNTER;
    wake_up_interruptible(&g_wdg_priv_info[wdg_id].wait_queue);

    up(&g_wdg_priv_info[wdg_id].cmd_access_sem);

    return 0;
}

static void watchdog_timer_isr(struct hisi_timer *hitimer)
{
    watchdog_clr_timer_isr(WDG_NO_A55_RESET);

    if (!g_wdg_priv_info[WDG_NO_A55_RESET].feed_by_kernel) {
        g_wdg_priv_info[WDG_NO_A55_RESET].wait_counter++;
        wake_up_interruptible(&g_wdg_priv_info[WDG_NO_A55_RESET].wait_queue);
    }

    return;
}

static void watchdog_timer_uninit(u32 wdg_id)
{
    struct hisi_timer *timer = g_wdg_priv_info[wdg_id].feed_timer;

    if (timer) {
        /* Disabling the timer */
        (void)timer->ops->stop(timer);

        /* Disabling timer interrupts */
        (void)timer->ops->disable_irq(timer);

        /* Release the timer. */
        hitimer_release(HISITIMER_NO_FEED_A55_WDG, "watchdog_timer");

        g_wdg_priv_info[wdg_id].feed_timer = NULL;
    }

    return;
}

static s32 watchdog_timer_init(u32 wdg_id)
{
    u32 val_l;
    u32 val_h;
    u64 regval;
    struct hisi_timer *timer = NULL;

    timer = hitimer_request(HISITIMER_NO_FEED_A55_WDG, "watchdog_timer", NULL);
    if (timer == NULL) {
        LOG(LOG_ERROR, "Failed to request hitimer\n");
        return -EFAULT;
    }

    /* Configuring the Watchdog Feeding Time, wdg_feed_reg_val = seconds * (ref_frequency / 1000) */
    regval = WDG_FEED_TIME * (SYS_APB_CLOCK_FREQ / 1000);
    val_h = (u32)(regval >> 32); /* get high 32 bits, right shift 32 */
    val_l = (u32)regval;
    (void)timer->ops->set_load_value(timer, val_l, val_h);

    /* Periodic timing */
    (void)timer->ops->set_timer_ctrl(timer, TIMER_PERIODIC, TIMER_NO_PREDIV, TIMER_CNT_64BIT);

    /* Registering the Interrupt Handling Function */
    (void)timer->ops->register_irq_handler(timer, watchdog_timer_isr);

    /* Disable debug printing. */
    (void)timer->ops->debug_on(timer, 0);

    /* Enable timer interrupt */
    (void)timer->ops->enable_irq(timer);

    /* Enable the timer. */
    (void)timer->ops->start(timer);

    init_waitqueue_head(&g_wdg_priv_info[wdg_id].wait_queue);

    g_wdg_priv_info[wdg_id].feed_timer = timer;

    return 0;
}

static s32 watchdog_ioremap(struct platform_device *pdev)
{
    s32 ret = 0;
    u32 i;
    struct device_node *node = pdev->dev.of_node;

    for (i = 0; i < WDG_MAX_NUM; i++) {
        g_wdg_priv_info[i].map_addr = of_iomap(node, i);
        if (!g_wdg_priv_info[i].map_addr) {
            LOG(LOG_ERROR, "watchdog%u ioremap failed.\n", i);
            ret = -EFAULT;
            goto ioremap_err;
        }
    }

    return 0;

ioremap_err:
    for (i = 0; i < WDG_MAX_NUM; i++) {
        if (g_wdg_priv_info[i].map_addr) {
            iounmap(g_wdg_priv_info[i].map_addr);
            g_wdg_priv_info[i].map_addr = NULL;
        }
    }

    return ret;
}

static s32 watchdog_iounmap(void)
{
    u32 i;

    for (i = 0; i < WDG_MAX_NUM; i++) {
        if (g_wdg_priv_info[i].map_addr) {
            iounmap(g_wdg_priv_info[i].map_addr);
            g_wdg_priv_info[i].map_addr = NULL;
        }
    }

    return 0;
}

static s32 watchdog_ioctl_base_ctrl(u32 cmd, unsigned long arg)
{
    s32 ret;
    u32 wdg_id;

    if (copy_from_user(&wdg_id, (u32 *)(uintptr_t)arg, sizeof(u32))) {
        LOG(LOG_ERROR, "watchdog cmd(0x%x) copy_from_user failed.\n", cmd);
        return -EFAULT;
    }

    switch (cmd) {
        case WDG_CMD_FEED_BY_USER:
            ret = watchdog_feed(wdg_id, 0);
            break;

        case WDG_CMD_FEED_BY_KERNEL:
            ret = watchdog_feed(wdg_id, 1);
            break;

        case WDG_CMD_CLR:
            ret = watchdog_clr(wdg_id);
            break;

        case WDG_CMD_STOP:
            ret = watchdog_stop(wdg_id);
            break;

        default:
            LOG(LOG_ERROR, "watchdog cmd(0x%x) isn't support.\n", cmd);
            ret = -EINVAL;
            break;
    }

    return ret;
}

static s32 watchdog_ioctl_advance_ctrl(u32 cmd, unsigned long arg)
{
    s32 ret;

    switch (cmd) {
        case WDG_CMD_ENABLE:
            ret = watchdog_enable_set(arg);
            break;

        case WDG_CMD_WAIT:
            ret = watchdog_timeout_wait(arg);
            break;

        case WDG_CMD_TIMEOUT:
            ret = watchdog_timeout(arg);
            break;

        default:
            LOG(LOG_ERROR, "watchdog cmd(0x%x) isn't support.\n", cmd);
            ret = -EINVAL;
            break;
    }

    return ret;
}

long watchdog_ioctl(struct file *filp, u32 cmd, unsigned long arg)
{
    long ret;

    /* Command word validity check */
    ret = comm_ioc_parm_check(cmd, (void __user *)arg, WDG_IOC_MAGIC, WDG_CMD_MAX_NR);
    if (ret) {
        LOG(LOG_ERROR, "watchdog cmd(0x%x) dir err(%d)", cmd, ret);
        return -EFAULT;
    }

    if (cmd == WDG_CMD_FEED_BY_USER || cmd == WDG_CMD_FEED_BY_KERNEL || cmd == WDG_CMD_CLR || cmd == WDG_CMD_STOP) {
        ret = watchdog_ioctl_base_ctrl(cmd, arg);
    } else if (cmd == WDG_CMD_ENABLE || cmd == WDG_CMD_WAIT || cmd == WDG_CMD_TIMEOUT) {
        ret = watchdog_ioctl_advance_ctrl(cmd, arg);
    } else {
        LOG(LOG_ERROR, "watchdog cmd(0x%x) isn't support.\n", cmd);
        ret = -EINVAL;
    }

    return ret;
}

const struct file_operations g_watchdog_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = watchdog_ioctl,
};

static s32 watchdog_probe(struct platform_device *pdev)
{
    s32 ret;
    u32 i;

    ret = comm_init_dev(&g_watchdog_dev, &g_watchdog_fops, g_watchdog_dev_name);
    if (ret) {
        LOG(LOG_ERROR, "comm_init_dev watchdog err(0x%x)!\n", ret);
        return -EFAULT;
    }

    ret = watchdog_ioremap(pdev);
    if (ret) {
        LOG(LOG_ERROR, "watchdog ioremap err(0x%x)!\n", ret);
        ret = -EFAULT;
        goto iomap_err;
    }

    for (i = 0; i < WDG_MAX_NUM; i++) {
        /* Initialization command semaphore */
        init_mutex(&g_wdg_priv_info[i].cmd_access_sem);

        /* Default kernel watchdog. */
        g_wdg_priv_info[i].feed_by_kernel = 0x0;
        g_wdg_priv_info[i].stop_feed = 0x0;
        g_wdg_priv_info[i].feed_timer = NULL;
    }

    /* Initialize the watchdog timer. */
    ret = watchdog_timer_init(WDG_NO_A55_RESET);
    if (ret) {
        LOG(LOG_ERROR, "watchdog timer init err(0x%x)!\n", ret);
        ret = -EFAULT;
        goto timer_init_err;
    }

    /* Enable the watchdog. */
    ret = watchdog_enable(WDG_NO_A55_RESET, 1);
    if (ret) {
        LOG(LOG_ERROR, "watchdog enable err(0x%x)!\n", ret);
        ret = -EFAULT;
        goto enable_err;
    }

    LOG(LOG_PRINT, "watchdog init ok!");

    return 0;

enable_err:
    watchdog_timer_uninit(WDG_NO_A55_RESET);

timer_init_err:
    (void)watchdog_iounmap();

iomap_err:
    comm_cleanup_dev(&g_watchdog_dev);

    return ret;
}

static s32 watchdog_remove(struct platform_device *pdev)
{
    s32 ret;

    /* To enable the watchdog. */
    ret = watchdog_enable(WDG_NO_A55_RESET, 0);
    if (ret) {
        LOG(LOG_ERROR, "watchdog enable failed(%d)!", ret);
    }

    /* Delete the watchdog timer. */
    watchdog_timer_uninit(WDG_NO_A55_RESET);

    ret = watchdog_iounmap();
    if (ret) {
        LOG(LOG_ERROR, "watchdog iounmap failed(%d)!", ret);
    }

    comm_cleanup_dev(&g_watchdog_dev);

    return 0;
}

static const struct of_device_id g_watchdog_match[] = {
    { .compatible = "hisilicon, hi1711-watchdog" }, {}
};

static struct platform_driver g_watchdog_driver = {
    .driver = {
        .name = DEV_WATCHDOG_NAME,
        .of_match_table = g_watchdog_match,
    },

    .probe = watchdog_probe,
    .remove = watchdog_remove,
};
module_platform_driver(g_watchdog_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("watchdog driver");