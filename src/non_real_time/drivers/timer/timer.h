// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * timer driver data structure and API header files
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include "comm_knl_api.h"
#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/types.h>
#include <linux/of.h>
#include <linux/spinlock_types.h>
#endif

/* Timer working mode */
typedef enum {
    TIMER_FREE_RUN, /* Default */
    TIMER_PERIODIC,
    TIMER_ONESHOT,
    TIMER_MODE_MAX
} hisi_timer_mode;

/* Pre divisor factor */
typedef enum {
    TIMER_NO_PREDIV, /* Default */
    TIMER_16_PREDIV,
    TIMER_256_PREDIV,
    TIMER_RSVD_PREDIV,
    TIMER_PREDIV_MAX
} hisi_timer_prediv;

/* Timer counter size */
typedef enum {
    TIMER_CNT_32BIT,
    TIMER_CNT_64BIT, /* Default */
    TIMER_CNT_SZ_MAX
} hisi_timer_cnt_sz;

#define HISI_TIMER_INIT_MAGIC              (0x5A5AA5A5)
#define HISI_TIMER_MAX_NUM                 (6)
#define HISI_TIMER_CLKSEL_OSCILLATOR_DIV64 (0)
#define HISI_TIMER_CLKSEL_OSCILLATOR       (1)
#define HISI_TIMER_CLKSEL_BUS_FREQ         (2) /* Default */
#define HISI_TIMER_IRQ_NAME_LEN            (24)

#ifdef __KERNEL__
struct hisi_timer;
/* Hisi timer irq handler callback for user(In interrupt context!!!!) */
typedef void (*hitimer_irq_handler_t)(struct hisi_timer *hitimer);

/* Note:
 * all callbacks except (un)register_irq_handler are lock free
 * register_irq_handler and unregister_irq_handler are using irq_lock
 * with spin_lock_irqsave() and spin_unlock_irqrestore()
 */
typedef struct {
    /* User optional operations */
    void (*set_priv_data)(struct hisi_timer *hitimer, void *priv_data);
    void *(*get_priv_data)(struct hisi_timer *hitimer);
    /* Base operations */
    s32(*get_bgload_value)(struct hisi_timer *hitimer,
                           u32 *bgload_l, u32 *bgload_h); /* For periodic mode usage */
    s32(*set_bgload_value)(struct hisi_timer *hitimer,
                           u32 bgload_l, u32 bgload_h);
    s32(*get_load_value)(struct hisi_timer *hitimer,
                         u32 *load_l, u32 *load_h);
    s32(*set_load_value)(struct hisi_timer *hitimer,
                         u32 load_l, u32 load_h);
    s32 (*get_cnt_value)(struct hisi_timer *hitimer, u32 *val_l, u32 *val_h);
    s32(*get_timer_ctrl)(struct hisi_timer *hitimer, hisi_timer_mode *mode,
                         hisi_timer_prediv *prediv, hisi_timer_cnt_sz *cnt_sz);
    s32(*set_timer_ctrl)(struct hisi_timer *hitimer, hisi_timer_mode mode,
                         hisi_timer_prediv prediv, hisi_timer_cnt_sz cnt_sz);
    s32 (*register_irq_handler)(struct hisi_timer *hitimer, hitimer_irq_handler_t handler);
    s32 (*unregister_irq_handler)(struct hisi_timer *hitimer);
    s32 (*enable_irq)(struct hisi_timer *hitimer);
    s32 (*disable_irq)(struct hisi_timer *hitimer);
    s32 (*get_enable)(struct hisi_timer *hitimer, s32 *en);
    s32 (*start)(struct hisi_timer *hitimer);
    s32 (*stop)(struct hisi_timer *hitimer);
    /* Raw register access: for debug interface */
    s32 (*read_reg)(struct hisi_timer *hitimer, u32 reg_off, u32 *val);
    s32 (*write_reg)(struct hisi_timer *hitimer, u32 reg_off, u32 val);
    s32 (*debug_on)(struct hisi_timer *hitimer, s32 dbg_flag);
} hisi_timer_operations;

struct hisi_timer {
    /* Base property */
    u32 id;
    s32 init_magic;          /* HISI_TIMER_INIT_MAGIC: initialized */
    u32 v_irq;               /* virtual interrupt ID */
    u32 clock_sel;           /* 0:use main oscillator freq/64
                              * 1: use main scillator freq
                              * 2: use bus clock(apb bus clock)
                              */
    phys_addr_t phys_base;   /* physical register base address */
    unsigned long phys_size; /* physical register base address size */
    u32 *virt_base;          /* vitual register base address, ioremap() */
    char irq_name[HISI_TIMER_IRQ_NAME_LEN];       /* hitmr_irq%d */
    /* Private properties: do not touch */
    u32 held;     /* Indicates if timer is held by user, one timer can only be held by one user at one time */
    s32 dbg_flag; /* Debug flag: reserved for debugging */
    char held_module[MODULE_NAME_LEN + 4]; /* timer dev name is MODULE_NAME_LEN + 4 */
    spinlock_t lock; /* Fore hitimer_reqeust/release/query */
    spinlock_t irq_lock;
    /* Public properties and methods */
    void *priv_data;
    hisi_timer_operations *ops;
    hitimer_irq_handler_t irq_handler;
};

/* User interfaces */
struct hisi_timer *hitimer_request(u32 timer_id, const char *module_name, void *priv_data);
void hitimer_release(u32 timer_id, const char *module_name);
s32 hitimer_query(u32 timer_id, s32 *is_held, char *held_module);
#endif /* __KERNEL_H__ */

#define TIMER_OPS_RW_REG       (1) /* Read/Write timer regs */
#define TIMER_OPS_INT_EN       (2) /* Enable or disable interrupt */
#define TIMER_OPS_ENABLE       (3) /* Enalbe or disable timer(start,stop) */
#define TIMER_OPS_CTRL         (4) /* Set timer mode,prediv and counter size */
#define TIMER_OPS_LONGTIME     (5) /* Long time test for timer */
#define TIMER_OPS_GET_LOAD_L   (6) /* Get timer load_l */
#define TIMER_OPS_GET_LOAD_H   (7) /* Get timer load_h */
#define TIMER_OPS_SET_LOAD_L   (8) /* Set timer load_l */
#define TIMER_OPS_SET_LOAD_H   (9) /* Set timer load_h */
#define TIMER_OPS_GET_VALUE_L  (10)
#define TIMER_OPS_GET_VALUE_H  (11)
#define TIMER_OPS_GET_BGLOAD_L (12) /* Get timer bg_load_l */
#define TIMER_OPS_GET_BGLOAD_H (13) /* Get timer bg_load_h */
#define TIMER_OPS_SET_BGLOAD_L (14) /* Set timer bg_load_l */
#define TIMER_OPS_SET_BGLOAD_H (15) /* Set timer bg_load_h */

struct timer_reg_rw_arg {
    s32 rw; /* 0 - read, 1 - write */
    u32 reg_offset;
    u32 reg_val;
};

struct timer_ctrl_arg {
    hisi_timer_mode mode;
    hisi_timer_prediv prediv;
    hisi_timer_cnt_sz sz;
};

struct hitimer_ioctl {
    s32 timer_id;
    s32 ops;

    union {
        struct timer_reg_rw_arg rw_arg;
        s32 int_en; /* 0 - disable int, 1 - enable int */
        s32 enable; /* 0 - disable timer, 1 - enable timer */
        struct timer_ctrl_arg ctrl_arg;
        s32 longtime_en; /* 0 - disable longtime test, 1 - enable longtime test */
        u32 timer_l;
        u32 timer_h;
    } args;
};

#endif /* __TIMER_H__ */
