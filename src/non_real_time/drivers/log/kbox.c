// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * kbox external interface
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
#include <linux/kbox/kbox.h>
#include <linux/securec.h>

#include "log_api.h"

#define KBOX_MSGHDR_LEN     96  /* Maximum length of the KBOX log header */
#define KBOX_MSGBDY_LEN     152 /* Maximum length of the KBOX log body */
#define KBOX_MSGGAP_LEN     8   /* Reserved bytes for KBOX log information */
#define KBOX_TASKNAME_LEN   32  /* Maximum length of the task name */
#define KBOX_TIMESTAMP_LEN  32  /* Maximum length of the timestamp */

#define kbox_array_size(arr)    (sizeof(arr) / sizeof((arr)[0]))
#define kbox_type_custom(type)  ((unsigned int)(type) < kbox_array_size(g_kbox_region))

static struct kbox_region g_kbox_region[] = {
    [KBOX_COUNTER] = {
        .name = "dying",
        .size = 0x20000, /* 128k */
        .mod = THIS_MODULE,
        .rfd = -1,
    },
    [KBOX_EMERG] = {
        .name = "emergency",
        .size = 0x10000, /* 64k */
        .mod = THIS_MODULE,
        .rfd = -1,
    },
};

static void kbox_msg_write(enum kbox_type type, const char *msg, int msg_len)
{
    static const char *drop_prempt = "\n********droped********\n";
    struct kbox_region *kbox = NULL;

    if (!kbox_type_custom(type)) {
        printk("%s", msg);
        return;
    }

    kbox = &g_kbox_region[type];
    if (kbox->rfd < 0) {
        return;
    }

    if (msg_len > 0) {
        kbox_write(kbox->rfd, msg, (unsigned int)msg_len);
    } else if (msg_len < 0) {
        kbox_write(kbox->rfd, drop_prempt, (unsigned int)strlen(drop_prempt));
    } else {
        /* do nothing */
    }
}

void kbox_msg_arglist(enum kbox_type type, const char *format, va_list arglist)
{
    char msg[KBOX_MSGBDY_LEN] = {0};
    int ret;

    ret = vsnprintf_s(msg, sizeof(msg), sizeof(msg) - 1, format, arglist);
    ret = min_t(int, sizeof(msg), ret);
    kbox_msg_write(type, msg, ret);
}

void kbox_msg(enum kbox_type type, const char *format, ...)
{
    va_list vptr;

    if (!format) {
        return;
    }

    va_start(vptr, format);
    kbox_msg_arglist(type, format, vptr);
    va_end(vptr);
}
EXPORT_SYMBOL(kbox_msg);

static int kbox_log_date(char *date, const unsigned int data_len)
{
    struct rtc_time tm_local = { 0 };

    rtc_time64_to_tm(kbox_ktime_get_real_seconds(), &tm_local);
    return snprintf_s(date, data_len, data_len - 1, "%4d-%02d-%02d %02d:%02d:%02d",
                      tm_local.tm_year + 1900, // add 1900
                      tm_local.tm_mon + 1, tm_local.tm_mday,
                      tm_local.tm_hour, tm_local.tm_min,
                      tm_local.tm_sec);
}

static int kbox_log_header(char *buffer, unsigned int length, const char *funcname, int line)
{
    char timestamp[KBOX_TIMESTAMP_LEN] = {0};
    char taskname[KBOX_TASKNAME_LEN] = {0};
    static unsigned int seq_num = 0;
    int ret;

    seq_num++;
    kbox_log_date(timestamp, sizeof(timestamp));

    if (in_interrupt()) {
        ret = strncpy_s(taskname, sizeof(taskname),  "ISR", sizeof(taskname) - 1);
        if (ret) {
            return -EFAULT;
        }
    } else {
        ret = snprintf_s(taskname, sizeof(taskname), sizeof(taskname) - 1, "%d", current->pid);
        if (ret < 0) {
            return -EFAULT;
        }
    }

    return snprintf_s(buffer, length, length - 1, "%u,%s,%s,%s,%d", seq_num,
        timestamp, taskname, funcname, line);
}

void kbox_log_arglist(enum kbox_type type, const char *funcname, int line,
                      const char *format, va_list arglist)
{
    char msg[KBOX_MSGHDR_LEN + KBOX_MSGBDY_LEN + KBOX_MSGGAP_LEN] = {0};
    char hdr[KBOX_MSGHDR_LEN] = {0};
    char body[KBOX_MSGBDY_LEN] = {0};
    int ret;

    ret = kbox_log_header(hdr, sizeof(hdr), funcname, line);
    if (ret < 0) {
        printk("********kbox droped********\n");
        return;
    }

    ret = vsnprintf_s(body, sizeof(body), sizeof(body) - 1, format, arglist);
    if (ret < 0) {
        printk("********kbox droped********\n");
        return;
    }

    ret = snprintf_s(msg, sizeof(msg), sizeof(msg) - 1, "%s,%s\n", hdr, body);
    ret = min_t(int, sizeof(msg), ret);
    kbox_msg_write(type, msg, ret);
}

void kbox_log(enum kbox_type type, const char *funcname, int line,
              const char *format, ...)
{
    va_list vptr;

    if (!funcname || !format) {
        return;
    }

    va_start(vptr, format);
    kbox_log_arglist(type, funcname, line, format, vptr);
    va_end(vptr);
}
EXPORT_SYMBOL(kbox_log);

void kbox_init(void)
{
    struct kbox_region *kbox = NULL;
    struct kbox_region kb;
    unsigned int i;
    int rfd;

    for (i = 0; i < kbox_array_size(g_kbox_region); i++) {
        kbox = &g_kbox_region[i];
        kb = *kbox;
        rfd = kbox_register_region(kb);
        kbox->rfd = rfd;
    }
}

