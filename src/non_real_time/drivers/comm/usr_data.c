// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * public module usr_data external interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "comm_knl_api.h"
#include "usr_data.h"

#define USR_DATA_PARM_BASE 0x84800000
#define USR_DATA_PARM_SIZE 0x1000

struct usr_data_mgr {
    atomic_t init_flag;
    spinlock_t lock;
    unsigned long virt_base;
};

struct usr_data_mgr g_usr_data_mgr = { 0 };

static u32 usr_data_checksum(u8 *pdata, u32 len)
{
    u32 sum = 0;
    u32 i;

    /* calculate checksum by summing every 4 bytes. */
    for (i = 0; i < (len / sizeof(u32)); i++) {
        sum += (*((u32 *)pdata));
        pdata += sizeof(u32);
    }

    return sum;
}

/**
 * @brief: user parameter area resource initialization function.
 * @param: void
 * @retval: 0 is success, other is failure.
 */
s32 usr_data_init(void)
{
    u32 parm_num;
    u32 checksum;
    u32 data_len;

    if (atomic_read(&g_usr_data_mgr.init_flag)) {
        return 0;
    }

    g_usr_data_mgr.virt_base = (unsigned long)ioremap(USR_DATA_PARM_BASE, USR_DATA_PARM_SIZE);
    if (!g_usr_data_mgr.virt_base) {
        return -ENOMEM;
    }

    spin_lock_init(&g_usr_data_mgr.lock);

    /*
     * parameter data verification: even if the verification fails,
     * return 0 to prevent system startup failure due to driver loading failure.
     * If feature driver fails to get data, use the feature's default values directly.
     * The structure of the parameter area data is as follows:
     *  u32 parm_num,
     *  u32 data1,
     *  u32 data2,
     *  ......
     *  u32 data_n,
     *  u32 checksum
     */
    parm_num = readl((void *)(uintptr_t)(g_usr_data_mgr.virt_base));
    if ((parm_num == 0) || (parm_num >= (USR_DATA_PARM_SIZE / sizeof(u32)))) {
        LOG(LOG_ERROR, "usr data num is inval:0x%x!", parm_num);
        goto exit;
    }

    data_len = (parm_num + 1) * sizeof(u32);
    checksum = readl((void *)(uintptr_t)(g_usr_data_mgr.virt_base + data_len));
    if (usr_data_checksum((u8 *)(uintptr_t)(g_usr_data_mgr.virt_base), data_len) != checksum) {
        LOG(LOG_ERROR, "usr data checksum(0x%x) is err!", checksum);
        goto exit;
    }

    /* init_flag only indicates whether usr_data is available, not whether the driver module is loaded successfully. */
    atomic_set(&g_usr_data_mgr.init_flag, 1);

exit:
    return 0;
}

/**
 * @brief: user parameter area resource cleanup function.
 * @param: void
 * @retval: 0 is success, other is failure.
 */
s32 usr_data_exit(void)
{
    /*  Do not unmap based on init_flag, errors may occur during probe but loading will not fail,
     *  in this case init_flag is 0.
     */
    atomic_set(&g_usr_data_mgr.init_flag, 0);

    if (g_usr_data_mgr.virt_base != 0) {
        iounmap((void *)(uintptr_t)g_usr_data_mgr.virt_base);
    }

    return 0;
}

/**
 * @brief: get user parameter area data.
 * @param: data_type: parameter type.
 *         val: the get value.
 * @retval: 0 is success, other is failure.
 */
s32 usr_data_read(u32 data_type, u32 *val)
{
    u32 offset;

    if ((val == NULL) || (data_type >= USR_DATA_PARM_MAX)) {
        return -EINVAL;
    }

    if (atomic_read(&g_usr_data_mgr.init_flag) == 0 || (g_usr_data_mgr.virt_base == 0)) {
        return -ENODEV;
    }

    offset = data_type * sizeof(u32);
    *val = readl((void *)(uintptr_t)(g_usr_data_mgr.virt_base + offset));

    return 0;
}
EXPORT_SYMBOL(usr_data_read);
