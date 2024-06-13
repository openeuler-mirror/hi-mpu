// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * public module lp_subsys external interface header
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __USR_DATA_H__
#define __USR_DATA_H__

enum {
    PCIE1_FLAG = 1,                 /* 0x84800004: whether to enable the PCIe1 black box function. */
    USB_TX_PREEMPH,                 /* 0x84800008: usb TX pre-emphasis */
    USB_TX_AMPLITUDE,               /* 0x8480000c: usb TX swing */
    USB_RX_EQ,                      /* 0x84800010: usb RX equalization */
    SGMII0_TX_PREEMPH,              /* 0x84800014: GMAC2 corresponding Serdes TX pre-emphasis. */
    SGMII0_TX_AMPLITUDE,            /* 0x84800018: GMAC2 corresponding Serdes TX amplitude. */
    SGMII0_RX_EQ,                   /* 0x8480001c: GMAC2 corresponding Serdes RX equalization. */
    SGMII1_TX_PREEMPH,              /* 0x84800020: GMAC3 corresponding Serdes TX pre-emphasis. */
    SGMII1_TX_AMPLITUDE,            /* 0x84800024: GMAC3 corresponding Serdes TX amplitude. */
    SGMII1_RX_EQ,                   /* 0x84800028: GMAC3 corresponding Serdes RX equalization. */
    DAC0_GC,
    DAC1_GC,
    DAC2_GC,
    PCIE0_TX_PREEMPH,
    PCIE0_TX_PREEMPH3P5,
    PCIE0_TX_PREEMPH6,
    PCIE0_TX_AMPLITUDEFULL,
    PCIE0_TX_AMPLITUDELOW,
    PCIE0_RX_EQ,
    PCIE1_TX_PREEMPH,
    PCIE1_TX_PREEMPH3P5,
    PCIE1_TX_PREEMPH6,
    PCIE1_TX_AMPLITUDEFULL,
    PCIE1_TX_AMPLITUDELOW,
    PCIE1_RX_EQ,
    PCIE1BAR0,
    PCIE1BAR1,
    PCIE1BAR2,
    PCIE1BAR3,
    PCIE1BAR4,
    PCIE1BAR5,
    USB_2PHY_CTRL4,
    PCIE1_BAR_ADDR_MODE,
    BMC_UART_PORT,
    USR_DATA_PARM_MAX
};

extern s32 usr_data_init(void);
extern s32 usr_data_exit(void);
extern s32 usr_data_read(u32 data_type, u32 *val);

#endif /* __USR_DATA_H__ */