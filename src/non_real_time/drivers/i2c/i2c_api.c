// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Implemention of i2c API
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "i2c.h"
#include "i2c_api.h"

__weak void i2c_platdrv_init(void *arg)
{
    return;
}
__weak void i2c_platdrv_destroy(void *arg)
{
    return;
}

void i2c_init(void *arg)
{
    i2c_platdrv_init(arg);
}

void i2c_destroy(void *arg)
{
    i2c_platdrv_destroy(arg);
}

int i2c_reset(unsigned int drv_id)
{
    struct i2c_adapter *adap = NULL;
    int ret;

    adap = i2c_adapter_lookup(drv_id);
    if (!adap) {
        return -I2C_ENOADAP;
    }

    down(&adap->sem);
    ret = i2c_adapter_reset(adap);
    up(&adap->sem);

    return ret;
}
EXPORT_SYMBOL(i2c_reset);

int i2c_clk_operate(unsigned int drv_id, unsigned int code)
{
    struct i2c_adapter *adap = NULL;
    int ret;

    adap = i2c_adapter_lookup(drv_id);
    if (!adap) {
        return -I2C_ENOADAP;
    }

    down(&adap->sem);
    ret = i2c_adapter_clk_operate(adap, code);
    up(&adap->sem);

    return ret;
}
EXPORT_SYMBOL(i2c_clk_operate);

int i2c_revocation(unsigned int drv_id)
{
    struct i2c_adapter *adap = NULL;
    int ret;

    adap = i2c_adapter_lookup(drv_id);
    if (!adap) {
        return -I2C_ENOADAP;
    }

    down(&adap->sem);
    ret = i2c_adapter_revocation(adap);
    up(&adap->sem);

    return ret;
}
EXPORT_SYMBOL(i2c_revocation);

void i2c_revocation_all(void)
{
    i2c_adapter_traverse(1, i2c_adapter_revocation);
}
EXPORT_SYMBOL(i2c_revocation_all);

int i2c_deploy(unsigned int drv_id, const char *algo_name, struct i2c_kdepl *deploy)
{
    struct i2c_adapter *adap = NULL;
    int ret;

    if (!algo_name || !deploy) {
        return -EINVAL;
    }

    adap = i2c_adapter_lookup(drv_id);
    if (!adap) {
        return -I2C_ENOADAP;
    }

    down(&adap->sem);
    ret = i2c_adapter_deploy(adap, algo_name, deploy);
    up(&adap->sem);

    return ret;
}
EXPORT_SYMBOL(i2c_deploy);

int i2c_transaction(unsigned int drv_id, struct i2c_kmsg *msg)
{
    struct i2c_adapter *adap = NULL;
    unsigned short max_address;
    int ret;

    if (!msg) {
        return -EINVAL;
    }

    max_address = (msg->flags & I2C_KMSG_TEN) ? I2C_10BIT_MAX_ADDR : I2C_7BIT_MAX_ADDR;
    if (msg->addr > max_address) {
        return -I2C_EADDRESS;
    }

    adap = i2c_adapter_lookup(drv_id);
    if (!adap) {
        return -I2C_ENOADAP;
    }

    down(&adap->sem);
    ret = i2c_adapter_transfer(adap, msg);
    i2c_adapter_set_loglevel(adap, msg->addr, ret ? LOG_INFO : LOG_ERROR);
    up(&adap->sem);

    return ret;
}
EXPORT_SYMBOL(i2c_transaction);

int i2c_acquire_mode(unsigned int drv_id, unsigned int *mode)
{
    struct i2c_adapter *adap = NULL;
    int ret;

    adap = i2c_adapter_lookup(drv_id);
    if (!adap) {
        return -I2C_ENOADAP;
    }

    down(&adap->sem);
    ret = i2c_adapter_acquire_mode(adap, mode);
    up(&adap->sem);

    return ret;
}
EXPORT_SYMBOL(i2c_acquire_mode);

int i2c_slave_write_cache(i2c_slave_cache_rdwr *rdwr)
{
    struct i2c_adapter *adap = NULL;
    int ret;

    adap = i2c_adapter_lookup(rdwr->drv_id);
    if (!adap) {
        return -I2C_ENOADAP;
    }

    down(&adap->sem);
    ret = i2c_adapter_slave_write_cache(adap, rdwr);
    up(&adap->sem);

    return ret;
}

int i2c_slave_read_cache(i2c_slave_cache_rdwr *rdwr)
{
    struct i2c_adapter *adap = NULL;
    int ret;

    adap = i2c_adapter_lookup(rdwr->drv_id);
    if (!adap) {
        return -I2C_ENOADAP;
    }

    down(&adap->sem);
    ret = i2c_adapter_slave_read_cache(adap, rdwr);
    up(&adap->sem);

    return ret;
}

int i2c_slave_set_err_code(struct i2c_slave_err_code *err_code)
{
    struct i2c_adapter *adap = NULL;
    int ret;

    adap = i2c_adapter_lookup(err_code->drv_id);
    if (!adap) {
        return -I2C_ENOADAP;
    }

    down(&adap->sem);
    ret = i2c_adapter_slave_set_err_code(adap, err_code);
    up(&adap->sem);

    return ret;
}
