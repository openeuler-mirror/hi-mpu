/* Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: hi1711_ipc
 * Author: xxx
 * Create: 2022-11-16
 */

#ifndef __HISI_IPC_H__
#define __HISI_IPC_H__

#define IPC_BASE               0x0102090000
#define IPC_MBX_SOURCE_REG(m)  (IPC_BASE + ((m) << 6))                  /* 源处理器配置寄存器 */
#define IPC_MBX_DSET_REG(m)    (IPC_BASE + ((m) << 6) + 0x04)           /* 目的处理器设置寄存器 */
#define IPC_MBX_DCLEAR_REG(m)  (IPC_BASE + ((m) << 6) + 0x08)           /* 目的处理器清除寄存器 */
#define IPC_MBX_DSTATUS_REG(m) (IPC_BASE + ((m) << 6) + 0x0C)           /* 目的处理器状态寄存器 */
#define IPC_MBX_MODE_REG(m)    (IPC_BASE + ((m) << 6) + 0x10)           /* 邮箱通道模式状态机状态寄存器 */
#define IPC_MBX_IMASK_REG(m)   (IPC_BASE + ((m) << 6) + 0x14)           /* 处理器中断屏蔽寄存器 */
#define IPC_MBX_ICLR_REG(m)    (IPC_BASE + ((m) << 6) + 0x18)           /* 处理器中断状态清除寄存器 */
#define IPC_MBX_SEND_REG(m)    (IPC_BASE + ((m) << 6) + 0x1C)           /* 发送寄存器 */
#define IPC_MBX_DATA_REG(m, d) (IPC_BASE + ((m) << 6) + 0x20 + ((d)*4)) /* MBX_DATA0-MBX_DATA7 */

#define IPC_LOCK_REG      (IPC_BASE + 0xA00) /* IPC lock寄存器 */
#define IPC_ACK_BIT_SHIFT (1 << 7)           /* 应答状态，此状态下，源收到应答中断未清除完毕 */
#define IPC_UNLOCK_VALUE  0x1ACCE551

enum pm_mode {
    PM_ON = 0,
    PM_OFF,
};

void hisi_ipc_pm_on_off(unsigned int core, unsigned int cluster, enum pm_mode mode);

#endif /* __HISI_IPC_H__ */
