// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * kbox external interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __KBOX_H__
#define __KBOX_H__

#include "log_api.h"


void kbox_msg_arglist(enum kbox_type type, const char *format, va_list arglist);
void kbox_log_arglist(enum kbox_type type, const char *funcname, int line,
                      const char *format, va_list arglist);
void kbox_init(void);

#endif
