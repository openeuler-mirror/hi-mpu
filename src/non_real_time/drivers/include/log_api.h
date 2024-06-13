// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * kbox external interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __LOG_API_H__
#define __LOG_API_H__

enum log_level {
    LOG_EMERG = 0,
    LOG_ERROR,
    LOG_WARN,
    LOG_PRINT,
    LOG_INFO
};

enum kbox_type {
    KBOX_COUNTER,
    KBOX_EMERG,
};


void kbox_msg(enum kbox_type type, const char *format, ...);

/*
 * Log recording interface
 */
void log_kproc(int level, const char *funcName, int line, const char *format, ...);

/*
 * KBOX log recording interface
 */
void kbox_log(enum kbox_type type, const char *funcname, int line, const char *format, ...);

#ifndef LOG
#define LOG(level, format, arg...) \
    do {log_kproc(level, __FUNCTION__, __LINE__, format, ##arg);} while (0)
#endif

#ifndef KBOX_MSG
#define KBOX_MSG(type, format, arg...) \
    do {kbox_msg(type, format, ##arg);} while (0)
#endif

#ifndef KBOX_LOG
#define KBOX_LOG(type, format, arg...) \
    do {kbox_log(type, __FUNCTION__, __LINE__, format, ##arg);} while (0)
#endif

#endif /* __LOG_API_H__ */

