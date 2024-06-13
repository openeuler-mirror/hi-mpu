// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Definitions for i2c interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __COM_I2C_H__
#define __COM_I2C_H__

#include "i2c_platform.h"
#include "comm_knl_api.h"
#include "i2c_api.h"

#define I2C_SLAVE_ERR_CODE_LEN_MAX       4
#define IC_MSG_MAX_NUM                   10U

#define I2C_GENMASK(h, l) \
    ((((~0UL) - (1UL << (l))) + 1) & ((~0UL) >> ((BITS_PER_LONG - 1) - (h))))

struct i2c_adapter;
struct i2c_kmsg;
struct i2c_kdepl;

union i2c_log_ctrl {
    struct {
        unsigned char toggle : 5;
        unsigned char level : 3;
    } __attribute__((packed)) bits;

    unsigned char val;
} __attribute__((packed));

enum i2c_dma_type {
    I2C_DMA_TO_DEVICE,
    I2C_DMA_FROM_DEVICE,
};

enum {
    I2C_MTYPE_DMA_WRITE = 0,
    I2C_MTYPE_DMA_READ,
    I2C_MTYPE_SLAVE_DATA,
};

enum i2c_slave_event {
    I2C_SLAVE_READ_REQUESTED,  /* Notify i2c slave that new read transfer is starting */
    I2C_SLAVE_WRITE_REQUESTED, /* Notify i2c slave that new write transfer is starting */
    I2C_SLAVE_READ_PROCESSED,  /* master wants to read from us */
    I2C_SLAVE_WRITE_RECEIVED,  /* master wants to write to us */
    I2C_SLAVE_STOP,            /* master sent stop */
    I2C_SLAVE_ABORT,           /* slave detect abort */
};

typedef int (*i2c_slave_cb_t)(struct i2c_adapter *adap, enum i2c_slave_event event);
typedef void (*i2c_slave_init_t)(struct i2c_adapter *adap);

struct i2c_slave {
    const char *name;
    const unsigned int control_memlen;
    const i2c_slave_cb_t slave_cb;
    const i2c_slave_init_t slave_init;
};

enum {
    I2C_ALGO_TYPE_NONE = 0,
    I2C_ALGO_TYPE_MASTER,
    I2C_ALGO_TYPE_SLAVE,
};

struct i2c_queue_node {
    unsigned char *buf;
    unsigned int buf_len;
    struct list_head entry;
};

struct i2c_queue {
    spinlock_t lock;
    unsigned char list_count;
    struct list_head head;
};

struct i2c_smbus_control {
    spinlock_t lock;
    volatile bool ret_response;
    volatile bool ret_errcode;
    unsigned char err_code_len;
    unsigned char err_code[I2C_SLAVE_ERR_CODE_LEN_MAX];  /* error code buffer */
    unsigned int drv_id;
    unsigned int req_data_index;
    unsigned int res_data_index;
    unsigned int res_data_len;
    unsigned int buf_len;                           /* buf_request_len和buf_response_len */
    unsigned char *buf_request;                     /* request buffer */
    unsigned char *buf_response;                    /* response buffer */
    unsigned short clk_strech_timeout;              /* Maximum extension time of the slave clock */
    struct i2c_queue req_queue_inused;              /* request cache queue inused */
    struct i2c_queue req_queue_unused;              /* request cache queue unused */
    struct i2c_queue_node i2c_msg_buf[IC_MSG_MAX_NUM];
    struct timer_list clk_strech_timer;
    struct timer_list stop_detect_timer;
    wait_queue_head_t wait;
};

static inline void i2c_queue_init(struct i2c_queue *q)
{
    q->list_count = 0;
    spin_lock_init(&q->lock);
    INIT_LIST_HEAD(&q->head);
}

static inline int i2c_queue_empty(struct i2c_queue *q)
{
    int ret;
    unsigned long flags = 0;

    spin_lock_irqsave(&q->lock, flags);
    ret = list_empty(&q->head);
    spin_unlock_irqrestore(&q->lock, flags);

    return ret;
}

static inline struct i2c_queue_node *i2c_queue_head(struct i2c_queue *q)
{
    if (i2c_queue_empty(q)) {
        return NULL;
    }

    return list_first_entry(&q->head, struct i2c_queue_node, entry);
}

static inline unsigned int i2c_queue_count(struct i2c_queue *q)
{
    unsigned int count;
    unsigned long flags = 0;

    spin_lock_irqsave(&q->lock, flags);
    count = q->list_count;
    spin_unlock_irqrestore(&q->lock, flags);

    return count;
}

static inline void i2c_enque(struct i2c_queue *q, struct i2c_queue_node *node)
{
    unsigned long flags = 0;

    spin_lock_irqsave(&q->lock, flags);
    q->list_count++;
    list_add_tail(&node->entry, &q->head);
    spin_unlock_irqrestore(&q->lock, flags);
}

static inline struct i2c_queue_node *i2c_deque_handler(struct i2c_queue *q)
{
    struct i2c_queue_node *n = NULL;

    q->list_count--;
    n = list_first_entry(&q->head, struct i2c_queue_node, entry);
    list_del(&n->entry);

    return n;
}


static inline struct i2c_queue_node *i2c_deque(struct i2c_queue *q)
{
    struct i2c_queue_node *n = NULL;
    unsigned long flags = 0;

    if (i2c_queue_empty(q)) {
        return NULL;
    }

    spin_lock_irqsave(&q->lock, flags);
    n = i2c_deque_handler(q);
    spin_unlock_irqrestore(&q->lock, flags);

    return n;
}

struct i2c_algorithm {
    const char *algo_name;
    const unsigned int algo_type;
    int (*i2c_master_xfer)(struct i2c_adapter *adap, struct i2c_kmsg *msg);
    int (*initialize)(struct i2c_adapter *adap, struct i2c_kdepl *deploy);
    int (*destroy)(struct i2c_adapter *adap);
    int (*idle)(struct i2c_adapter *adap);
};

struct i2c_devinfo {
    unsigned int log_level_size;
    unsigned char *log_level;
    int irqn;
    void __iomem *regbase;
    unsigned long phy_base;
};

struct i2c_adapter_platform_operations {
    int (*platform_isr_register)(struct i2c_adapter *adap, irqreturn_t (*isr_routine)(int, void *));
    void (*platform_isr_unregister)(struct i2c_adapter *adap);
    int (*platform_acquire_devinfo)(struct i2c_adapter *adap, struct i2c_devinfo *di);
    int (*platform_init)(struct i2c_adapter *adap);
    int (*platform_destroy)(struct i2c_adapter *adap);
    int (*platform_recovery)(struct i2c_adapter *adap);
    int (*platform_reset)(struct i2c_adapter *adap);
    int (*platform_clk_operate)(struct i2c_adapter *adap, unsigned int code);
    dma_addr_t (*platform_dma_map)(struct i2c_adapter *adap, void *addr, size_t size, enum i2c_dma_type type);
    void (*platform_dma_unmap)(struct i2c_adapter *adap, dma_addr_t addr, size_t size, enum i2c_dma_type type);
    int (*platform_dma_config)(struct i2c_adapter *adap, dma_addr_t addr, size_t size, enum i2c_dma_type type);
    int (*platform_dma_wait_complete)(struct i2c_adapter *adap, int channel);
};

struct i2c_adapter_quirks {
    unsigned short max_write_len;
    unsigned short max_read_len;
};

enum {
    I2C_CMD_ERR_TX_ABRT = 0x01,
    I2C_CMD_ERR_SCL_STUCK_LOW = 0x02,
    I2C_CMD_ERR_NO_MSG_PTR = 0x04,
    I2C_CMD_ERR_NO_MSG_WBUF = 0x08,
    I2C_CMD_ERR_NO_MSG_RBUF = 0x10,
    I2C_CMD_ERR_NO_ACTIVITY_INT = 0x20,
    I2C_CMD_ERR_NOT_ENABLE = 0x40,
    I2C_CMD_ERR_DMA = 0x80,
    I2C_CMD_ERR_DMA_UNSUPPORTED = 0x100,
    I2C_CMD_ERR_TX_UNFINISHED = 0x200,
    I2C_CMD_ERR_RX_UNFINISHED1 = 0x400,
    I2C_CMD_ERR_RX_UNFINISHED2 = 0x800,
    I2C_CMD_ERR_CONTROLLER_BUSY = 0x1000,
    I2C_CMD_ERR_RX_UNDER = 0x2000,
    I2C_CMD_ERR_RX_OVER = 0x4000,
    I2C_CMD_ERR_TX_OVER = 0x8000,
    I2C_CMD_ERR_TIMEOUT = 0x10000,
};

enum {
    I2C_MSG_ERR_RX_EXCEED = 0x1,
    I2C_MSG_ERR_SMBUS_RECVLEN_INVALID = 0x2,
    I2C_MSG_ERR_NO_RD = 0x4,
};

struct i2c_adapter {
    const unsigned int drv_id;
    const char * const name;

    spinlock_t lock;
    struct semaphore sem;
    struct completion cmd_complete;

    struct i2c_devinfo di;

    unsigned int deploy_flags;

    unsigned int control;
    unsigned int hcnt;
    unsigned int lcnt;
    unsigned int tx_fifo_depth;
    unsigned int rx_fifo_depth;

    dma_addr_t dma_cmd_address;

    unsigned int irq_status;
    unsigned int irq_raw_status;
    unsigned int irq_mask;

    unsigned int may_hangup;
    unsigned int abort_source;
    unsigned int msg_error;
    unsigned int cmd_error;
    unsigned int status;
    int restart_sent;
    struct i2c_kmsg *msg;
    unsigned short msg_flags;

    unsigned int slave_status;
    unsigned short slave_address;
    unsigned short clk_strech_timeout;
    void *slave_priv;
    void *slave_data;
    unsigned long slave_dlen;
    const struct i2c_slave *slave;

    void * const platform_private;

    const struct i2c_algorithm *algo;
    const struct i2c_adapter_quirks * const quirks;
    const struct i2c_adapter_platform_operations * const ops;
};

struct i2c_driver {
    struct i2c_slave **slave_list;
    struct i2c_algorithm **algo_list;
    struct i2c_adapter **adapter_list;
};

#define i2c_adapter_initiallzer(prefix, adap_name, id) {                            \
        .drv_id = (id),                                                             \
        .name = (adap_name),                                                        \
        .platform_private = &(g_##prefix##_plat[id]),                               \
        .quirks = &(g_##prefix##_quirks),                                           \
        .ops = &(g_##prefix##_ops),                                                 \
        .lock = __SPIN_LOCK_INITIALIZER(g_##prefix##_adap[id].lock),                \
        .sem = __SEMAPHORE_INITIALIZER(g_##prefix##_adap[id].sem, 1),               \
        .cmd_complete = COMPLETION_INITIALIZER(g_##prefix##_adap[id].cmd_complete), \
    }

static inline void *i2c_register(struct i2c_adapter *adap, unsigned int off)
{
    return (void *)adap->di.regbase + off;
}

static inline unsigned int i2c_readl(struct i2c_adapter *adap, unsigned int off)
{
    return readl(i2c_register(adap, off));
}

static inline void i2c_writel(struct i2c_adapter *adap, unsigned int val, unsigned int off)
{
    writel(val, i2c_register(adap, off));
}

extern void *i2c_mcalloc(unsigned int mtype, int len);
extern void i2c_mfree(unsigned int mtype, const void *ptr);
extern unsigned long i2c_clk_hz(void);
extern void i2c_udelay(unsigned int delay);
extern struct i2c_algorithm *i2c_algo_lookup_byname(const char *algo_name);
extern struct i2c_slave *i2c_slave_lookup_byname(const char *slave_name);

extern int i2c_adapter_slave_event(struct i2c_adapter *adap, enum i2c_slave_event event);
extern int i2c_adapter_isr_register(struct i2c_adapter *adap, irqreturn_t(*isr_routine)(int, void *));
extern int i2c_adapter_isr_unregister(struct i2c_adapter *adap);
extern int i2c_adapter_acquire_devinfo(struct i2c_adapter *adap, struct i2c_devinfo *dev_info);
extern int i2c_adapter_platform_init(struct i2c_adapter *adap);
extern int i2c_adapter_platform_destroy(struct i2c_adapter *adap);
extern int i2c_adapter_platform_recovery(struct i2c_adapter *adap);
extern int i2c_adapter_platform_reset(struct i2c_adapter *adap);
extern int i2c_adapter_platform_clk_operate(struct i2c_adapter *adap, unsigned int code);
extern dma_addr_t i2c_adapter_platform_dma_map(struct i2c_adapter *adap,
                                               void *addr, size_t size, enum i2c_dma_type type);
extern void i2c_adapter_platform_dma_unmap(struct i2c_adapter *adap, dma_addr_t addr, size_t size,
                                           enum i2c_dma_type type);
extern int i2c_adapter_platform_dma_config(struct i2c_adapter *adap, dma_addr_t addr, size_t size,
                                           enum i2c_dma_type type);
extern int i2c_adapter_platform_dma_wait_complete(struct i2c_adapter *adap, int id);
extern struct i2c_adapter *i2c_adapter_lookup(unsigned int drv_id);
extern void i2c_adapter_traverse(int lock_adap, int(*cb)(struct i2c_adapter *));
extern void i2c_adapter_set_loglevel(struct i2c_adapter *adap, unsigned short address, unsigned char level);
extern unsigned char i2c_adapter_acquire_loglevel(struct i2c_adapter *adap, unsigned short address);
extern void i2c_adapter_set_msgptr(struct i2c_adapter *adap, struct i2c_kmsg *msg);
extern int i2c_adapter_reset(struct i2c_adapter *adap);
extern int i2c_adapter_clk_operate(struct i2c_adapter *adap, unsigned int code);
extern int i2c_adapter_revocation(struct i2c_adapter *adap);
extern int i2c_adapter_deploy(struct i2c_adapter *adap, const char *algo_name, struct i2c_kdepl *deploy);
extern int i2c_adapter_transfer(struct i2c_adapter *adap, struct i2c_kmsg *msg);
extern int i2c_adapter_acquire_mode(struct i2c_adapter *adap, unsigned int *mode);
extern int i2c_adapter_slave_write_cache(struct i2c_adapter *adap, i2c_slave_cache_rdwr *rdwr);
extern int i2c_adapter_slave_read_cache(struct i2c_adapter *adap, i2c_slave_cache_rdwr *rdwr);
extern int i2c_adapter_slave_set_err_code(struct i2c_adapter *adap, struct i2c_slave_err_code *err_code);

#endif /* __COM_I2C_H__ */