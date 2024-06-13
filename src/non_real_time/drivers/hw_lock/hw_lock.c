// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * hardware lock module interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/rtc.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/module.h>

#include "log_api.h"
#include "hwlock_com_api.h"
#include "sysctrl_csr_addr_define.h"
#include "sysctrl.h"
#include "hw_lock.h"

struct hw_lock *g_hwlock = NULL;

s32 hw_query_lock_status(u32 lock_id)
{
    u32 reg = 0;
    s32 ret;

    if (lock_id >= HW_LOCK_NUM) {
        LOG(LOG_ERROR, "hw_query_lock_status lock_id(0x%x) must be 0~15!", lock_id);
        return -EINVAL;
    }

    ret = sysctrl_reg_read(SYSCTRL_CSR_LOCK_CORE_ST_OFFSET_ADDR, &reg);
    if (ret) {
        LOG(LOG_ERROR, "Inquire hw_lock's(0x%x) status failed(%d)!", lock_id, ret);
        return -1;
    }

    return ((reg >> (HWLOCK_BIT_NUM * lock_id)) & STATUS_MASK);
}
EXPORT_SYMBOL(hw_query_lock_status);

static s32 hw_get_hard_lock(u32 lock_id)
{
    s32 ret;
    u32 lock_mask = (LOCK_REQ << lock_id);
    u32 lock_req = (LOCK_REQ << lock_id);
    s32 status;

    status = hw_query_lock_status(lock_id);
    /*
     * If the hardware lock is locked by the M3,
     * the system returns a message indicating that the lock fails to be obtained.
     */
    if (status == M3_LOCKED) {
        LOG(LOG_ERROR, "hw_lock(0x%x) is locked by M3", lock_id);
        return -1;
    }

    /* If the hardware lock is locked by the A55,
     * the hardware lock is used directly without initiating a request again.
     */
    if (status == A55_LOCKED) {
        return 0;
    }

    /* 1: Initiate a hardware lock request. 0: The hardware lock request is not initiated. */
    ret = sysctrl_reg_read_write(SYSCTRL_CSR_A55_LOCK_REQ_OFFSET_ADDR, lock_mask, lock_req);
    if (ret) {
        LOG(LOG_ERROR, "start a hw_lock(0x%x) request failed(%d)!", lock_id, ret);
        return ret;
    }

    /* Querying the Hardware Lock Usage Status */
    status = hw_query_lock_status(lock_id);
    if (status != A55_LOCKED) {
        LOG(LOG_ERROR, "hw_lock(0x%x) status check failed(%d)!", lock_id, status);
        /* Cancel the hardware lock request. */
        if (g_hwlock->lock_count[lock_id] == 0) {
            sysctrl_reg_read_write(SYSCTRL_CSR_A55_LOC_DREQ_OFFSET_ADDR, lock_mask, lock_req);
        }

        return -1;
    }

    return 0;
}

s32 hw_acquire_hard_lock(u32 lock_id)
{
    s32 ret;

    if (lock_id >= HW_LOCK_NUM) {
        LOG(LOG_ERROR, "hw_acquire_hard_lock lock_id(%u) must be 0~15!", lock_id);
        return -EINVAL;
    }

    ret = down_interruptible(&(g_hwlock->hw_sem[lock_id]));
    if (ret) {
        LOG(LOG_ERROR, " g_hwlock(%u) get down_interruptible %d.", lock_id, ret);
        return ret;
    }

    ret = hw_get_hard_lock(lock_id);
    if (ret) {
        LOG(LOG_ERROR, "hardware lock(%u) is't locked by A55(%d)", lock_id, ret);
        up(&(g_hwlock->hw_sem[lock_id]));
        return -ENOLCK;
    }

    (g_hwlock->lock_count[lock_id])++;
    up(&(g_hwlock->hw_sem[lock_id]));

    return 0;
}
EXPORT_SYMBOL(hw_acquire_hard_lock);

void hw_release_hard_lock(u32 lock_id)
{
    s32 ret;
    u32 lock_mask;
    u32 realease_req;

    if (lock_id >= HW_LOCK_NUM) {
        LOG(LOG_ERROR, "hw_release_hard_lock lock_id(0x%x) must be 0~15!", lock_id);
        return;
    }

    ret = down_interruptible(&(g_hwlock->hw_sem[lock_id]));
    if (ret) {
        LOG(LOG_ERROR, " g_hwlock(0x%x) release down_interruptible %d!", lock_id, ret);
        return;
    }

    if (!g_hwlock->lock_count[lock_id]) {
        LOG(LOG_ERROR, "No one is using the lock(0x%x), no need to release it!", lock_id);
        goto exit;
    }

    lock_mask = (RELEASE_REQ << lock_id);
    realease_req = (RELEASE_REQ << lock_id);

    g_hwlock->lock_count[lock_id]--;
    if (!g_hwlock->lock_count[lock_id]) {
        sysctrl_reg_read_write(SYSCTRL_CSR_A55_LOC_DREQ_OFFSET_ADDR, lock_mask, realease_req);
    }

exit:
    up(&(g_hwlock->hw_sem[lock_id]));

    return;
}
EXPORT_SYMBOL(hw_release_hard_lock);

s32 hw_query_exclusive_lock_status(u32 lock_id)
{
    s32 ret;
    u32 reg;
    u32 lock_offset;

    if (lock_id >= EX_LOCK_NUM) {
        LOG(LOG_ERROR, "hw_query_exclusive_lock_status lock_id(0x%x) must be 0~7!", lock_id);
        return -EINVAL;
    }

    ret = sysctrl_reg_read(SYSCTRL_CSR_RW_EXCLUSIVE_OFFSET_ADDR, &reg);
    if (ret) {
        LOG(LOG_ERROR, "query exclusive lock(0x%x) failed %d.", lock_id, ret);
        return ret;
    }

    lock_offset = lock_id * EXLOCK_BIT_NUM;

    return ((reg >> lock_offset) & EXCLUSIVE_LOCK_MASK);
}
EXPORT_SYMBOL(hw_query_exclusive_lock_status);

s32 hw_acquire_exclusive_lock(u32 lock_id, u32 value)
{
    s32 ret;
    u32 lock_offset;
    u32 lock_value = 0xFFFFFFFF;

    if (lock_id >= EX_LOCK_NUM) {
        LOG(LOG_ERROR, "hw_acquire_exclusive_lock lock_id(0x%x) must be 0~7!", lock_id);
        return -EINVAL;
    }

    if ((value == EXCLUSIVE_LOCK_IDLE) || (value >= 0xF)) {
        LOG(LOG_ERROR, "hw_acquire_exclusive_lock value(%u) must not be 0 or greater than 0xF!", value);
        return -EINVAL;
    }

    lock_offset = lock_id * EXLOCK_BIT_NUM;
    ret = down_interruptible(&(g_hwlock->ex_sem[lock_id]));
    if (ret) {
        LOG(LOG_ERROR, " exclusive(0x%x) acquire down_interruptible %d.", lock_id, ret);
        return ret;
    }

    ret = hw_query_exclusive_lock_status(lock_id);
    if (ret != EXCLUSIVE_LOCK_IDLE) {
        goto ERR_EXIT;
    }

    lock_value &= ~(EXCLUSIVE_LOCK_MASK << lock_offset);
    lock_value |= ((EXCLUSIVE_LOCK_MASK & value) << lock_offset);
    (void)sysctrl_reg_write(SYSCTRL_CSR_RW_EXCLUSIVE_OFFSET_ADDR, lock_value);

    ret = hw_query_exclusive_lock_status(lock_id);
    if (ret != value) {
        LOG(LOG_ERROR, "acquire exclusive lock(0x%x) failed %d.", lock_id, ret);
        goto ERR_EXIT;
    }

    up(&(g_hwlock->ex_sem[lock_id]));

    return 0;

ERR_EXIT:
    up(&(g_hwlock->ex_sem[lock_id]));
    return ret;
}
EXPORT_SYMBOL(hw_acquire_exclusive_lock);

void hw_release_exclusive_lock(u32 lock_id)
{
    u32 lock_offset;
    u32 lock_value = 0xFFFFFFFF;

    if (lock_id >= EX_LOCK_NUM) {
        LOG(LOG_ERROR, "hw_release_exclusive_lock lock_id(0x%x) must be 0~7!", lock_id);
        return;
    }

    lock_offset = lock_id * EXLOCK_BIT_NUM;
    lock_value &= ~(EXCLUSIVE_LOCK_MASK << lock_offset);

    sysctrl_reg_write(SYSCTRL_CSR_RW_EXCLUSIVE_OFFSET_ADDR, lock_value);
    return;
}
EXPORT_SYMBOL(hw_release_exclusive_lock);

s32 hw_lock_open(struct inode *inode, struct file *file)
{
    return 0;
}

s32 hw_lock_release(struct inode *inode, struct file *file)
{
    return 0;
}

long hw_lock_ioctl(struct file *file, u32 cmd, unsigned long arg)
{
    return 0;
}

const struct file_operations g_hwlock_fops = {
    .owner = THIS_MODULE,
    .open = hw_lock_open,
    .release = hw_lock_release,
    .unlocked_ioctl = hw_lock_ioctl
};

static s32 hw_lock_probe(struct platform_device *pdev)
{
    s32 ret;
    s32 i;

    g_hwlock = (struct hw_lock *)kzalloc(sizeof(struct hw_lock), GFP_KERNEL);
    if (g_hwlock == NULL) {
        LOG(LOG_ERROR, "[%s]: failed to allocate hw_lock structure", __FUNCTION__);
        return -ENOMEM;
    }

    ret = comm_init_dev(&g_hwlock->hwlock_dev, &g_hwlock_fops, DEV_HWLOCK_NAME);
    if (ret) {
        LOG(LOG_ERROR, "hwlock_dev comm_init_dev failed!");
        kfree(g_hwlock);
        g_hwlock = NULL;
        return ret;
    }

    for (i = 0; i < HW_LOCK_NUM; i++) {
        init_mutex(&(g_hwlock->hw_sem[i]));
    }
    for (i = 0; i < EX_LOCK_NUM; i++) {
        init_mutex(&(g_hwlock->ex_sem[i]));
    }

    /* The hardware lock on the A55 side is released when the driver is loaded. */
    sysctrl_reg_write(SYSCTRL_CSR_A55_LOC_DREQ_OFFSET_ADDR, RELEASE_ALL_HWLOCK);

    return 0;
}

static s32 hw_lock_remove(struct platform_device *pdev)
{
    if (g_hwlock) {
        comm_cleanup_dev(&g_hwlock->hwlock_dev);

        kfree(g_hwlock);
        g_hwlock = NULL;
    }

    return 0;
}

static const struct of_device_id g_hw_lock_match[] = {
    { .compatible = "hisilicon,hi1711-hwlock" }, {}
};

static struct platform_driver g_hw_lock_driver = {
    .driver = {
        .name = DEV_HWLOCK_NAME,
        .of_match_table = g_hw_lock_match,
    },

    .probe = hw_lock_probe,
    .remove = hw_lock_remove,
};
module_platform_driver(g_hw_lock_driver);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("hardware lock device driver");