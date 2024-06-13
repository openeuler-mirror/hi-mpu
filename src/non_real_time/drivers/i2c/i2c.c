// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * i2c dirver.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "i2c_dw.h"
#include "i2c_core.h"

#define I2C_ALGORITHM_DEFAULT                                         \
    {                                                                 \
        .algo_name = "default",                                       \
        .algo_type = I2C_ALGO_TYPE_NONE,                              \
    }

__weak struct i2c_algorithm g_i2c_dw_master_algo = I2C_ALGORITHM_DEFAULT;
__weak struct i2c_algorithm g_i2c_dw_slave_algo = I2C_ALGORITHM_DEFAULT;
static struct i2c_algorithm *g_i2c_algo_list[] = {
    &g_i2c_dw_master_algo,
    &g_i2c_dw_slave_algo,
    NULL,
};
__weak struct i2c_slave g_slave_eeprom;
__weak struct i2c_slave g_slave_smbus;
static struct i2c_slave *g_i2c_slave_list[] = {
    &g_slave_eeprom,
    &g_slave_smbus,
    NULL,
};
__weak struct i2c_adapter *g_i2c_platdrv_adap_list[] = {
    NULL,
};
static const struct i2c_driver g_i2c_drv = {
    .algo_list = g_i2c_algo_list,
    .slave_list = g_i2c_slave_list,
    .adapter_list = g_i2c_platdrv_adap_list,
};

__weak int i2c_platdrv_random(void)
{
    return jiffies;
}

__weak void *i2c_platdrv_mem_calloc(unsigned int mtype, int len)
{
    return NULL;
}

__weak void i2c_platdrv_mem_free(unsigned int mtype, const void *ptr)
{
    return;
}

__weak unsigned long i2c_platdrv_clk_hz(void)
{
    return 100000000; /* 100MHz, 100000000 */
}

__weak void i2c_platdrv_udelay(unsigned int delay)
{
    udelay(delay);
}

unsigned int i2c_random(void)
{
    return i2c_platdrv_random();
}

void *i2c_mcalloc(unsigned int mtype, int len)
{
    if (len <= 0) {
        return NULL;
    }

    return i2c_platdrv_mem_calloc(mtype, len);
}

void i2c_mfree(unsigned int mtype, const void *ptr)
{
    if (ptr) {
        i2c_platdrv_mem_free(mtype, ptr);
    }
}

unsigned long i2c_clk_hz(void)
{
    return i2c_platdrv_clk_hz();
}

void i2c_udelay(unsigned int delay)
{
    if (delay) {
        i2c_platdrv_udelay(delay);
    }
}

struct i2c_algorithm *i2c_algo_lookup_byname(const char *algo_name)
{
    struct i2c_algorithm *algo = NULL;
    int i = 0;

    if (!algo_name) {
        return NULL;
    }

    while ((algo = g_i2c_drv.algo_list[i++]) != NULL) {
        if (algo->algo_name && !strcmp(algo->algo_name, algo_name)) {
            return algo;
        }
    }

    return NULL;
}

struct i2c_slave *i2c_slave_lookup_byname(const char *slave_name)
{
    struct i2c_slave *slave = NULL;
    int i = 0;

    if (!slave_name) {
        return NULL;
    }

    while ((slave = g_i2c_drv.slave_list[i++]) != NULL) {
        if (slave->name && !strcmp(slave->name, slave_name)) {
            return slave;
        }
    }

    return NULL;
}

int i2c_adapter_isr_register(struct i2c_adapter *adap, irqreturn_t (*isr_routine)(int, void *))
{
    if (!adap->ops->platform_isr_register) {
        return -EOPNOTSUPP;
    } else {
        return adap->ops->platform_isr_register(adap, isr_routine);
    }
}

int i2c_adapter_isr_unregister(struct i2c_adapter *adap)
{
    if (!adap->ops->platform_isr_unregister) {
        return -EOPNOTSUPP;
    } else {
        adap->ops->platform_isr_unregister(adap);
        return 0;
    }
}

int i2c_adapter_acquire_devinfo(struct i2c_adapter *adap, struct i2c_devinfo *dev_info)
{
    if (!adap->ops->platform_acquire_devinfo) {
        return -EOPNOTSUPP;
    } else {
        return adap->ops->platform_acquire_devinfo(adap, dev_info);
    }
}

int i2c_adapter_platform_init(struct i2c_adapter *adap)
{
    if (!adap->ops->platform_init) {
        return 0;
    } else {
        return adap->ops->platform_init(adap);
    }
}

int i2c_adapter_platform_destroy(struct i2c_adapter *adap)
{
    if (!adap->ops->platform_destroy) {
        return 0;
    } else {
        return adap->ops->platform_destroy(adap);
    }
}

int i2c_adapter_platform_recovery(struct i2c_adapter *adap)
{
    if (!adap->ops->platform_recovery) {
        return 0;
    } else {
        return adap->ops->platform_recovery(adap);
    }
}

int i2c_adapter_platform_reset(struct i2c_adapter *adap)
{
    if (!adap->ops->platform_reset) {
        return 0;
    } else {
        return adap->ops->platform_reset(adap);
    }
}

int i2c_adapter_platform_clk_operate(struct i2c_adapter *adap, unsigned int code)
{
    if (!adap->ops->platform_clk_operate) {
        return -EOPNOTSUPP;
    } else {
        return adap->ops->platform_clk_operate(adap, code);
    }
}

dma_addr_t i2c_adapter_platform_dma_map(struct i2c_adapter *adap, void *addr, size_t size, enum i2c_dma_type type)
{
    if (!adap->ops->platform_dma_map) {
        return 0;
    } else {
        return adap->ops->platform_dma_map(adap, addr, size, type);
    }
}

void i2c_adapter_platform_dma_unmap(struct i2c_adapter *adap, dma_addr_t addr, size_t size, enum i2c_dma_type type)
{
    if (!adap->ops->platform_dma_unmap) {
        return;
    } else {
        adap->ops->platform_dma_unmap(adap, addr, size, type);
    }
}

int i2c_adapter_platform_dma_config(struct i2c_adapter *adap, dma_addr_t addr, size_t size, enum i2c_dma_type type)
{
    if (!adap->ops->platform_dma_config) {
        return -EOPNOTSUPP;
    } else {
        return adap->ops->platform_dma_config(adap, addr, size, type);
    }
}

int i2c_adapter_platform_dma_wait_complete(struct i2c_adapter *adap, int id)
{
    if (!adap->ops->platform_dma_wait_complete) {
        return -EOPNOTSUPP;
    } else {
        return adap->ops->platform_dma_wait_complete(adap, id);
    }
}

struct i2c_adapter *i2c_adapter_lookup(unsigned int drv_id)
{
    struct i2c_adapter *adap = NULL;
    int i = 0;

    while ((adap = g_i2c_drv.adapter_list[i++]) != NULL) {
        if (adap->drv_id == drv_id) {
            return adap;
        }
    }

    return NULL;
}

void i2c_adapter_traverse(int lock_adap, int (*cb)(struct i2c_adapter *))
{
    struct i2c_adapter *adap = NULL;
    int i = 0;

    while ((adap = g_i2c_drv.adapter_list[i++]) != NULL) {
        if (lock_adap) {
            down(&adap->sem);
        }

        if (cb) {
            (void)(*cb)(adap);
        }

        if (lock_adap) {
            up(&adap->sem);
        }
    }
}

static void i2c_adapter_init_loglevel(unsigned int log_level_size, unsigned char *log_level)
{
    union i2c_log_ctrl lctrl;
    unsigned int i;

    if (!log_level_size || !log_level) {
        return;
    }

    lctrl.bits.toggle = 0;
    lctrl.bits.level = LOG_ERROR;

    for (i = 0; i < log_level_size; i++) {
        log_level[i] = lctrl.val;
    }
}

void i2c_adapter_set_loglevel(struct i2c_adapter *adap, unsigned short address, unsigned char level)
{
    const unsigned char log_max_toggle = 20;
    union i2c_log_ctrl lctrl;
    unsigned int i;

    if (!adap->di.log_level_size || !adap->di.log_level) {
        return;
    }

    if (level != LOG_ERROR && level != LOG_INFO) {
        return;
    }

    i = (address & I2C_10BIT_ADDR_MASK) % adap->di.log_level_size;
    lctrl.val = adap->di.log_level[i];

    if (lctrl.bits.level == level) {
        return;
    }

    if (level == LOG_ERROR && lctrl.bits.toggle >= log_max_toggle) {
        return;
    }

    lctrl.bits.toggle++;
    if (lctrl.bits.toggle < log_max_toggle) {
        lctrl.bits.level = LOG_ERROR;
    } else {
        lctrl.bits.level = LOG_INFO;
    }
    adap->di.log_level[i] = lctrl.val;
}

unsigned char i2c_adapter_acquire_loglevel(struct i2c_adapter *adap, unsigned short address)
{
    union i2c_log_ctrl lctrl;
    unsigned int i;

    if (!adap->di.log_level_size || !adap->di.log_level) {
        return (unsigned char)LOG_ERROR;
    }

    i = (address & I2C_10BIT_ADDR_MASK) % adap->di.log_level_size;
    lctrl.val = adap->di.log_level[i];
    return (unsigned char)lctrl.bits.level;
}

void i2c_adapter_set_msgptr(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    unsigned long flags = 0;

    spin_lock_irqsave(&adap->lock, flags);
    adap->msg = msg;
    adap->msg_flags = msg ? msg->flags : 0;
    spin_unlock_irqrestore(&adap->lock, flags);
}

static int i2c_adapter_set_default(struct i2c_adapter *adap)
{
    adap->algo = NULL;
    adap->slave_address = 0;
    adap->slave_priv = NULL;
    adap->slave_data = NULL;
    adap->slave_dlen = 0;
    adap->slave = NULL;
    adap->control = 0;
    adap->deploy_flags = 0;
    adap->tx_fifo_depth = 0;
    adap->rx_fifo_depth = 0;

    i2c_adapter_set_msgptr(adap, NULL);

    init_completion(&adap->cmd_complete);

    if (memset_s(&adap->di, sizeof(adap->di), 0, sizeof(adap->di))) {
        return -I2C_EMEMSET;
    } else {
        return 0;
    }
}

static int i2c_adapter_deploy_handle(struct i2c_adapter *adap, struct i2c_algorithm *algo,
                                     struct i2c_kdepl *deploy, struct i2c_devinfo *di)
{
    int ret;

    if (!algo || !algo->initialize) {
        return -EOPNOTSUPP;
    }

    if (di->irqn <= 0 || !di->regbase) {
        return -I2C_EDIINVAL;
    }

    adap->di = *di;
    adap->algo = algo;
    adap->deploy_flags = deploy->flags;

    ret = algo->initialize(adap, deploy);
    if (ret) {
        i2c_adapter_set_default(adap);
    }

    return ret;
}

int i2c_adapter_reset(struct i2c_adapter *adap)
{
    if (!adap->algo) {
        return -EPERM;
    }

    i2c_adapter_platform_reset(adap);
    i2c_adapter_platform_recovery(adap);
    return 0;
}

int i2c_adapter_clk_operate(struct i2c_adapter *adap, unsigned int code)
{
    if (!adap->algo || adap->algo->algo_type != I2C_ALGO_TYPE_MASTER) {
        return -EPERM;
    }

    return i2c_adapter_platform_clk_operate(adap, code);
}

int i2c_adapter_revocation(struct i2c_adapter *adap)
{
    int ret;

    if (!adap->algo) {
        return 0;
    }

    if (!adap->algo->destroy) {
        return -EOPNOTSUPP;
    }

    ret = adap->algo->destroy(adap);
    if (ret) {
        return ret;
    }

    i2c_adapter_set_default(adap);
    return 0;
}

static int i2c_adapter_deploy__(struct i2c_adapter *adap, const char *algo_name, struct i2c_kdepl *deploy)
{
    struct i2c_algorithm *algo = NULL;
    struct i2c_devinfo di = { 0 };
    int ret;

    if (!(deploy->flags & I2C_KDEPLOY_SM_MASK)) {
        return -EINVAL;
    }

    if (!(deploy->flags & I2C_KDEPLOY_SPEED_MODE_MASK)) {
        return -EINVAL;
    }

    algo = i2c_algo_lookup_byname(algo_name);
    if (!algo) {
        return -I2C_ENOALGO;
    }

    ret = i2c_adapter_acquire_devinfo(adap, &di);
    if (ret) {
        return ret;
    }

    i2c_adapter_init_loglevel(di.log_level_size, di.log_level);

    return i2c_adapter_deploy_handle(adap, algo, deploy, &di);
}

int i2c_adapter_deploy(struct i2c_adapter *adap, const char *algo_name, struct i2c_kdepl *deploy)
{
    if (!adap->algo) {
        return i2c_adapter_deploy__(adap, algo_name, deploy);
    }

    if (!(deploy->flags & I2C_KDEPLOY_FORCE)) {
        return -I2C_EFORCE;
    }

    if (i2c_adapter_revocation(adap)) {
        return -I2C_EREVOCATION;
    } else {
        return i2c_adapter_deploy__(adap, algo_name, deploy);
    }
}

int i2c_adapter_transfer(struct i2c_adapter *adap, struct i2c_kmsg *msg)
{
    int ret;

    if (!adap->algo || adap->algo->algo_type != I2C_ALGO_TYPE_MASTER) {
        return -EPERM;
    }

    i2c_adapter_set_msgptr(adap, msg);

    if (msg->flags & I2C_KMSG_SMBUS) {
        ret = i2c_smbus_xfer(adap, msg);
    } else {
        ret = i2c_xfer(adap, msg);
    }

    i2c_adapter_set_msgptr(adap, NULL);

    return ret;
}

int i2c_adapter_acquire_mode(struct i2c_adapter *adap, unsigned int *mode)
{
    if (!mode) {
        return -EINVAL;
    }

    *mode = adap->algo ? adap->algo->algo_type : I2C_ALGO_TYPE_NONE;
    return 0;
}

static int i2c_adapter_slave_write_cache_eeprom(struct i2c_adapter *adap, const void *wb,
                                                unsigned int wlen, unsigned long offset)
{
    unsigned long flags = 0;

    spin_lock_irqsave(&adap->lock, flags);
    (void)memcpy_s(adap->slave_data + offset, wlen, wb, wlen);
    spin_unlock_irqrestore(&adap->lock, flags);

    return 0;
}

static int i2c_adapter_slave_write_cache_smbus(struct i2c_adapter *adap, const void *wb, unsigned int wlen)
{
    struct i2c_smbus_control *smbus = NULL;
    unsigned long flags = 0;
    unsigned char val = 0;
    unsigned int cnt = 0;
    unsigned int i = 0;

    smbus = adap->slave_priv;
    if (smbus->ret_response) {
        return -I2C_EMSGEXISTED;
    }

    cnt = min_t(unsigned int, IC_S_TX_FIFO_DEPTH_SMBUS - i2c_readl(adap, ICR_TXFLR), wlen);
    (void)memcpy_s(smbus->buf_response, wlen, wb, wlen);
    smbus->res_data_len = wlen;
    smbus->res_data_index = 0;

    spin_lock_irqsave(&smbus->lock, flags);
    smbus->ret_response = true;
    if (timer_pending(&smbus->clk_strech_timer)) {
        spin_unlock_irqrestore(&smbus->lock, flags);
        del_timer_sync(&smbus->clk_strech_timer);
        for (i = 0; i < cnt; i++) {
            val = smbus->buf_response[smbus->res_data_index++];
            i2c_writel(adap, val, ICR_DATA_CMD);
        }
    } else {
        spin_unlock_irqrestore(&smbus->lock, flags);
    }

    return 0;
}

int i2c_adapter_slave_write_cache(struct i2c_adapter *adap, i2c_slave_cache_rdwr *rdwr)
{
    void *slave_data = NULL;
    int ret = 0;

    if (!adap->algo || adap->algo->algo_type != I2C_ALGO_TYPE_SLAVE) {
        return -EPERM;
    }

    if (!adap->slave_data || !adap->slave_dlen) {
        return -I2C_ENOSLAVEMEM;
    }

    if (!rdwr->buff || !rdwr->size) {
        return -EINVAL;
    }

    if (adap->slave_dlen < rdwr->size || adap->slave_dlen - rdwr->size < rdwr->offset) {
        return -I2C_ESLAVESCOPE;
    }

    slave_data = kzalloc(rdwr->size, GFP_KERNEL);
    if (!slave_data) {
        return -ENOMEM;
    }

    if (copy_from_user(slave_data, rdwr->buff, rdwr->size)) {
        kfree(slave_data);
        return -I2C_ECPYFROMUSR;
    }

    if (!strcmp(adap->slave->name, "smbus")) {
        ret = i2c_adapter_slave_write_cache_smbus(adap, slave_data, rdwr->size);
    } else if (!strcmp(adap->slave->name, "eeprom")) {
        ret = i2c_adapter_slave_write_cache_eeprom(adap, slave_data, rdwr->size, rdwr->offset);
    } else {
        ret = -I2C_ESLAVENAME;
    }

    kfree(slave_data);
    return ret;
}

static int i2c_adapter_slave_read_cache_smbus(struct i2c_adapter *adap, void *rb,
                                              unsigned int *size, unsigned int timeout)
{
    struct i2c_smbus_control *smbus = adap->slave_priv;
    struct i2c_queue_node *node = NULL;
    long wait_ret = 0;

    if (i2c_queue_empty(&smbus->req_queue_inused)) {
        wait_ret = wait_event_interruptible_timeout(smbus->wait, i2c_queue_count(&smbus->req_queue_inused) > 0,
                                                    msecs_to_jiffies(timeout));
        if (wait_ret < 0) {
            i2c_print("[I2C-%02u-RT] Read smbus slave error, ret: %ld", adap->drv_id, wait_ret);
            return wait_ret;
        } else if (wait_ret == 0) {
            return -ETIMEDOUT;
        }
    }

    node = i2c_deque(&smbus->req_queue_inused);
    (void)memcpy_s(rb, node->buf_len, node->buf, node->buf_len);
    i2c_enque(&smbus->req_queue_unused, node);
    *size = node->buf_len;

    return 0;
}

static int i2c_adapter_slave_read_cache_eeprom(struct i2c_adapter *adap, void *rb,
                                               unsigned long rlen, unsigned long offset)
{
    unsigned long flags = 0;

    spin_lock_irqsave(&adap->lock, flags);
    (void)memcpy_s(rb, rlen, adap->slave_data + offset, rlen);
    spin_unlock_irqrestore(&adap->lock, flags);

    return 0;
}

int i2c_adapter_slave_read_cache(struct i2c_adapter *adap, i2c_slave_cache_rdwr *rdwr)
{
    void *slave_data = NULL;
    int ret = 0;

    if (!adap->algo || adap->algo->algo_type != I2C_ALGO_TYPE_SLAVE) {
        return -EPERM;
    }

    if (!adap->slave_data || !adap->slave_dlen) {
        return -I2C_ENOSLAVEMEM;
    }

    if (!rdwr->buff || !rdwr->size) {
        return -EINVAL;
    }

    if (adap->slave_dlen < rdwr->size || adap->slave_dlen - rdwr->size < rdwr->offset) {
        return -I2C_ESLAVESCOPE;
    }

    slave_data = kzalloc(adap->slave_dlen, GFP_KERNEL);
    if (!slave_data) {
        return -ENOMEM;
    }

    rdwr->timeout = max_t(unsigned int, rdwr->timeout, I2C_SLAVE_RD_WAITTIME_MIN);

    if (!strcmp(adap->slave->name, "smbus")) {
        ret = i2c_adapter_slave_read_cache_smbus(adap, slave_data, &rdwr->size, rdwr->timeout);
    } else if (!strcmp(adap->slave->name, "eeprom")) {
        ret = i2c_adapter_slave_read_cache_eeprom(adap, slave_data, rdwr->size, rdwr->offset);
    } else {
        kfree(slave_data);
        return -I2C_ESLAVENAME;
    }

    if (copy_to_user(rdwr->buff, slave_data, rdwr->size)) {
        ret = I2C_ECPYFROMUSR;
    }

    kfree(slave_data);
    return ret;
}

int i2c_adapter_slave_set_err_code(struct i2c_adapter *adap, struct i2c_slave_err_code *err_code)
{
    struct i2c_smbus_control *smbus = NULL;
    void *slave_data = NULL;
    unsigned long flags = 0;

    if (!adap->algo || adap->algo->algo_type != I2C_ALGO_TYPE_SLAVE) {
        return -EPERM;
    }

    if (!adap->slave_priv) {
        return -I2C_ENOSLAVEMEM;
    }

    if (strcmp(adap->slave->name, "smbus")) {
        return -I2C_ENOSLAVETYPE;
    }

    smbus = adap->slave_priv;
    if (!err_code->enable) {
        spin_lock_irqsave(&smbus->lock, flags);
        smbus->ret_errcode = false;
        spin_unlock_irqrestore(&smbus->lock, flags);
        return 0;
    }

    if (!err_code->buff || !err_code->size) {
        return -EINVAL;
    }

    if (err_code->size > I2C_SLAVE_ERR_CODE_LEN_MAX) {
        return -EINVAL;
    }

    slave_data = kzalloc(err_code->size, GFP_KERNEL);
    if (!slave_data) {
        return -ENOMEM;
    }

    if (copy_from_user(slave_data, err_code->buff, err_code->size)) {
        kfree(slave_data);
        i2c_print("[I2C-%02u-RT] set errcode error", smbus->drv_id);
        return -I2C_ECPYFROMUSR;
    }

    spin_lock_irqsave(&smbus->lock, flags);
    (void)memcpy_s(smbus->err_code, err_code->size, slave_data, err_code->size);
    smbus->err_code_len = (unsigned char)(err_code->size);
    smbus->ret_errcode = true;
    spin_unlock_irqrestore(&smbus->lock, flags);

    kfree(slave_data);
    return 0;
}

int i2c_adapter_slave_event(struct i2c_adapter *adap, enum i2c_slave_event event)
{
    if (!adap->slave) {
        return -EOPNOTSUPP;
    }

    return adap->slave->slave_cb(adap, event);
}

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("i2c driver");