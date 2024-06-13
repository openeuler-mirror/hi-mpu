// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * kernel general log description
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/rtc.h>
#include <linux/securec.h>
#include <linux/version.h>

#include "log_api.h"
#include "kbox.h"
#include "log.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#define FILE_OPS proc_ops
#define OPS_O proc_open
#define OPS_R proc_read
#define OPS_W proc_write
#else
#define FILE_OPS file_operations
#define OPS_O open
#define OPS_R read
#define OPS_W write
#endif

/* Global control variable of the log level */
int g_sdk_log_level = LOG_PRINT;
/* Node for which the log level is changed */
static struct proc_dir_entry *g_sdk_log_dir = NULL;
static const struct proc_dir_entry *g_sdk_log_level_file = NULL;

void log_kproc(int level, const char *func_name, int line, const char *format, ...)
{
    va_list vptr;

    if (!func_name || !format) {
        return;
    }

    if (g_sdk_log_level < level) {
        return;
    }

    va_start(vptr, format);

    /* The value 1 indicates that the printk mode is used. */
    kbox_log_arglist(-1, func_name, line, format, vptr);

    if (level == LOG_EMERG) {
        kbox_log_arglist(KBOX_EMERG, func_name, line, format, vptr);
    }

    va_end(vptr);
}
EXPORT_SYMBOL(log_kproc);

static int sdk_log_level_show(struct seq_file *m, void *v)
{
    seq_printf(m, "%u\n", (unsigned int)g_sdk_log_level);

    return 0;
}

static int sdk_log_level_open(struct inode *inodes, struct file *files)
{
    return single_open(files, sdk_log_level_show, NULL);
}

static ssize_t sdk_log_level_write(struct file *filp, const char __user *buff, size_t len, loff_t *data)
{
    char level = LOG_ERROR;

    if (len > 0) {
        if (get_user(level, buff)) {
            return -EFAULT;
        }

        if ((unsigned int)level <= 4) {
            g_sdk_log_level = (unsigned int)level - 0;
        } else {
            return -EINVAL;
        }
    }

    return (int)len;
}

static const struct FILE_OPS g_sdk_log_level_file_ops = {
    .OPS_O = sdk_log_level_open,
    .OPS_R = seq_read,
    .OPS_W = sdk_log_level_write,
};

static int __init sdk_log_init(void)
{
    int ret = 0;

    g_sdk_log_dir = proc_mkdir(SDK_LOG_NAME, NULL);
    if (!g_sdk_log_dir) {
        LOG(LOG_ERROR, "can not create /proc/%s!\n", SDK_LOG_NAME);
        ret = -ENOMEM;
        goto err_exit;
    }

    g_sdk_log_level_file = proc_create(SDK_LOG_LEVEL_NAME, 0644, g_sdk_log_dir, &g_sdk_log_level_file_ops);
    if (!g_sdk_log_level_file) {
        LOG(LOG_ERROR, "can not create /proc/%s/%s!\n", SDK_LOG_NAME, SDK_LOG_LEVEL_NAME);
        ret = -ENOMEM;
        goto err_exit;
    }

    kbox_init();
    return 0;

err_exit:
    if (g_sdk_log_level_file) {
        remove_proc_entry(SDK_LOG_LEVEL_NAME, g_sdk_log_dir);
    }

    if (g_sdk_log_dir) {
        remove_proc_entry(SDK_LOG_NAME, NULL);
    }

    return ret;
}

static void __exit sdk_log_exit(void)
{
    if (g_sdk_log_level_file) {
        remove_proc_entry(SDK_LOG_LEVEL_NAME, g_sdk_log_dir);
    }

    if (g_sdk_log_dir) {
        remove_proc_entry(SDK_LOG_NAME, NULL);
    }

    return;
}

module_init(sdk_log_init);
module_exit(sdk_log_exit);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("log dirver");