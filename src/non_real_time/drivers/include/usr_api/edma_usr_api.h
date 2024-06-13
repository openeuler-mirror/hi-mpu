// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * edma user api
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _EDMA_USR_API_H_
#define _EDMA_USR_API_H_

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN
#endif

#include <linux/types.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/time.h>
#include <linux/errno.h>
#include <linux/version.h>
#include "bdinfo.h"
#include "sys_info_usr_api.h"


enum {
    /* 0 -127 msg */
    TYPE_LOGIC_PARTITION = 0,
    TYPE_UPGRADE = 1,
    TYPE_CDEV = 2,
    TYPE_VETH = 0x40,

    TYPE_MAX = 128,

    /* Special treatment */
    TYPE_KBOX = 129,
    TYPE_EDMA_DRIVER = 130,
    TYPE_UNKNOWN = 0xff,
};

enum {
    DLOG_ERROR = 0,
    DLOG_DEBUG = 1,
};

enum {
    DEV_CLOSE = 0,
    DEV_OPEN = 1,
    DEV_OPEN_STATUS_REQ = 0xf0,
    DEV_OPEN_STATUS_ANS
};

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0)
#define CONST_SWITCH const
#else
#define CONST_SWITCH
#endif

/*  1MB   */
#define PAGE_ORDER  8

#define H2BBUF_SIZE     ((1<<PAGE_ORDER)<<12)
#define B2HBUF_SIZE     ((1<<PAGE_ORDER)<<12)

#define EDMA_IOC_MAGIC 'e'

#define EDMA_IOC_MAXNR 7

#define BMC_SYSTEM          0
#define SINGLE_CHIP_SYSTEM  1
#define OLD_BMC_SYSTEM      0xff
#define EDMA_CMD_INIT           _IOWR(EDMA_IOC_MAGIC, 0, int)
#define EDMA_CMD_CLOSE          _IOWR(EDMA_IOC_MAGIC, 1, int)
#define EDMA_REGISTER_TYPE      _IOW(EDMA_IOC_MAGIC, 3, unsigned long)
#define EDMA_UNREGISTER_TYPE    _IOW(EDMA_IOC_MAGIC, 4, unsigned long)
#define EDMA_DMA_START          _IOW(EDMA_IOC_MAGIC, 5, unsigned long)
#define EDMA_DMA_TRANSFER       _IOW(EDMA_IOC_MAGIC, 6, unsigned long)
#define EDMA_DMA_STOP           _IOW(EDMA_IOC_MAGIC, 7, unsigned long)

#define DRIVER_VERSION "3.28.03"
#pragma pack(1)

/*  Data structure for exchanging information between apps and kernels */
typedef struct  _edma_buffer_info {
    unsigned int timeout;
    char *buffer;
    unsigned int len;
    unsigned int h2b_copy;
    unsigned int b2h_copy;
    unsigned long h2b_phyaddr;
    unsigned long b2h_phyaddr;
    int reserved;
} edma_buffer_info_s, *p_edma_buffer_info;

/* Data structure for exchanging information between the BMC and host, which is stored in the shared memory. */
typedef struct  _notify_msg {
    volatile unsigned int    host_registered;
    volatile unsigned int    host_heartbeat;
    volatile unsigned int    bmc_registered;
    volatile unsigned int    bmc_heartbeat;
    volatile unsigned int    int_flag;            /*  Interrupt flag, bit 0 H2B, bit 1 B2H */

    volatile unsigned int    reservrd5;
    unsigned int    h2b_addr;
    unsigned int    h2b_size;   /* Size of the receive DMA buffer on the H2B BMC side */
    unsigned int    h2b_rsize;
    unsigned int    b2h_addr;
    unsigned int    b2h_size;   /* B2H DMA data length on the BMC side */
    unsigned int    b2h_rsize;
} notify_msg, *pnotify_msg;

#pragma pack()

static inline void get_sys_seconds(__kernel_long_t *t)
{
    struct timespec64 uptime = ktime_to_timespec64(ktime_get_boottime());
    *t = uptime.tv_sec;
}

#define SECONDS_PER_DAY (24*3600)
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_MINUTE 60

/* Kernel interface */
int edma_intf_register_int_notifier(struct notifier_block *nb);
void edma_intf_unregister_intnotifier(struct notifier_block *nb);
#define EDMA_OS_DEV_NUM   0
#define EDMA_BIOS_DEV_NUM 1
int edma_intf_register_type(int scope_id, unsigned int type, unsigned int sub_type,
                            unsigned int support_int, void **handle);
int edma_intf_unregister_type(int scope_id, void **handle);

int edma_intf_int_to_host(int scope_id, void *handle);
int edma_intf_is_link_ok(int scope_id);
int edma_intf_get_remote_status(int scope_id, void *handle);
void edma_intf_set_open_status(int scope_id, void *handle, unsigned char s);

int  edma_cdev_recv_msg(int scope_id, void* handle, char __user* data, size_t count);
int  edma_cdev_add_msg(int scope_id, void* handle, const char __user* msg, size_t msg_len);

int edma_cdev_check_recv(void *handle);
void *edma_cdev_get_wait_queue(void *handle);
int edma_param_set_debug(const char *buf, CONST_SWITCH struct kernel_param *kp);

int edma_get_front_count(unsigned long arg);
int  edma_get_rear_count(unsigned long arg);
int edma_set_front_count(unsigned long arg);
int edma_set_rear_count(unsigned long arg);
int edma_write_partition(unsigned long arg);
int edma_read_partition(unsigned long arg);
void edma_activate_tasklet(void);
int edmaOperatePara(unsigned long arg);

struct device *edma_get_pdev(void);
#endif
