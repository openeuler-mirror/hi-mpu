// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __USB_DRV_H__
#define __USB_DRV_H__

extern int usb_id2name(unsigned int id, char *name, unsigned int max_len);
extern int usb_name2id(const char *name, unsigned int *id);
extern void usb_cr_write(unsigned int id, unsigned int addr, unsigned int data);
extern unsigned int usb_cr_read(unsigned int id, unsigned int addr);
extern void usb_creg_write(unsigned int id, unsigned int val, unsigned int off);
extern unsigned int usb_creg_read(unsigned int id, unsigned int off);
extern void usb_mreg_write(unsigned int id, unsigned int val, unsigned int off);
extern void usb_reset(unsigned int id);
extern void usb_unreset(unsigned int id);
extern void usb_dfx_regs_collect(void);

#endif /* __USB_DRV_H__ */