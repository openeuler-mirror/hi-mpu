// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Creation of I2C device, Implemention of I2C ioctl.
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <asm/memory.h>
#include "i2c.h"
#include "i2c_api.h"

struct i2c_dev_cmd_handler {
    const unsigned int cmd;
    int (*routine)(void *);
    const unsigned int cmd_size;
};

#define I2C_DEV_MEMDUP_MAX 0x20000000

#define i2c_dev_cmd(command, func, arg_type)              \
    {                                                     \
        .cmd = I2C_CMD_##command,                         \
        .routine = (int (*)(void *))i2c_dev_cmd_##func,   \
        .cmd_size = sizeof(arg_type)                      \
    }

static void i2c_dev_msg_mfree(struct i2c_kmsg *msg)
{
    if (msg->wbuf) {
        kfree(msg->wbuf);
    }

    if (msg->rbuf) {
        kfree(msg->rbuf);
    }

    kfree(msg);
}

static struct i2c_kmsg *i2c_dev_msg_memdump_user(const void __user *wbuf, unsigned int wlen,
                                                 const void __user *rbuf, unsigned int rlen)
{
    struct i2c_kmsg *kmsg = NULL;
    char *wb = NULL;
    char *rb = NULL;

    if (wlen > I2C_DEV_MEMDUP_MAX || rlen > I2C_DEV_MEMDUP_MAX) {
        return NULL;
    }

    kmsg = kzalloc(sizeof(struct i2c_kmsg), GFP_KERNEL);
    if (!kmsg) {
        return NULL;
    }

    if (wbuf && wlen) {
        wb = kzalloc(wlen + I2C_RSERVED_EXTERN_LEN, GFP_KERNEL);
        if (!wb || copy_from_user(wb, wbuf, wlen)) {
            if (wb) {
                kfree(wb);
            }

            kfree(kmsg);
            return NULL;
        } else {
            kmsg->wbuf = wb;
            kmsg->wlen = wlen;
        }
    }

    if (rbuf && rlen) {
        rb = kzalloc(rlen + I2C_RSERVED_EXTERN_LEN, GFP_KERNEL);
        if (!rb) {
            if (wb) {
                kfree(wb);
            }
            kfree(kmsg);
            return NULL;
        } else {
            kmsg->rbuf = rb;
            kmsg->rlen = rlen;
        }
    }

    return kmsg;
}

static int i2c_dev_cmd_init(void *arg)
{
    dev_i2c_ioctl_init_s *init = arg;
    struct i2c_kdepl deploy = { 0 };
    const char *algo_name = NULL;
    unsigned int val;

    deploy.flags |= I2C_KDEPLOY_FORCE;

    algo_name = init->is_slave ? "dw-slave" : "dw-master";

    val = init->is_slave ? I2C_KDEPLOY_SLAVE : I2C_KDEPLOY_MASTER;
    deploy.flags |= val;

    if (init->speed == I2C_CLK_100K) {
        deploy.flags |= I2C_KDEPLOY_SPEED_MODE_STD;
    } else if (init->speed == I2C_CLK_400K) {
        deploy.flags |= I2C_KDEPLOY_SPEED_MODE_FAST;
    } else {
        return -EINVAL;
    }

    if (init->is_slave) {
        deploy.flags |= I2C_KDEPLOY_SLV_TYPE_EEPROM;
        deploy.slave_address = init->address & I2C_7BIT_ADDR_MASK;
        deploy.slave_dlen = (unsigned long)I2C_DEFAULT_SLAVE_LEN;
    }

    deploy.sda_hold = ((init->sda_hold_ratio >> 8) & 0xFFFFFFU); // >> 8
    deploy.sda_hold_ratio = (init->sda_hold_ratio & 0xFFU);

    return i2c_deploy(init->drv_id, algo_name, &deploy);
}

static int i2c_dev_cmd_write__(dev_i2c_ioctl_write_s *w, u16 addr, u8 *wbuf, u32 wlen, u16 msg_flags)
{
    struct i2c_kmsg *msg = NULL;
    int ret;

    msg = i2c_dev_msg_memdump_user(wbuf, wlen, NULL, 0);
    if (!msg) {
        return -ENOMEM;
    }

    msg->addr = addr;
    msg->flags = msg_flags;
    msg->timeout = msecs_to_jiffies(w->time_out_cnt);
    ret = i2c_transaction(w->drv_id, msg);
    i2c_dev_msg_mfree(msg);

    return ret;
}

static int i2c_dev_cmd_read__(dev_i2c_ioctl_read_s *r, u16 addr, u8 *wbuf, u32 wlen, u16 msg_flags)
{
    struct i2c_kmsg *msg = NULL;
    int ret;

    msg = i2c_dev_msg_memdump_user(wbuf, wlen, r->p_read_buf, r->length);
    if (!msg) {
        return -ENOMEM;
    }

    msg->addr = addr;
    msg->flags = (msg_flags | I2C_KMSG_RD);
    msg->timeout = msecs_to_jiffies(r->time_out_cnt);
    msg->retries = r->re_read_cnt;
    msg->retries = (msg->retries == I2C_RE_READ_DEFAULT) ? I2C_MSG_RETRIES : msg->retries;

    ret = i2c_transaction(r->drv_id, msg);
    if (!ret) {
        if (!msg->rlen || msg->rlen > r->length) {
            ret = -I2C_EMSGRCVLEN;
        } else if (copy_to_user(r->p_read_buf, msg->rbuf, msg->rlen)) {
            ret = -I2C_ECPYTOUSR;
        } else {
            r->length = msg->rlen;
        }
    }

    i2c_dev_msg_mfree(msg);

    return ret;
}

static int i2c_dev_cmd_write(void *arg)
{
    dev_i2c_ioctl_write_s *w = arg;
    unsigned char val;
    u16 addr;
    u8 *wbuf = NULL;
    u32 wlen;

    if (!w->p_tx_buffer || !w->length) {
        return -EINVAL;
    }

    if (copy_from_user(&val, w->p_tx_buffer, 1)) {
        return -I2C_ECPYFROMUSR;
    }

    wbuf = &w->p_tx_buffer[1];
    wlen = w->length - 1;
    addr = (val >> 1) & I2C_7BIT_ADDR_MASK;

    return i2c_dev_cmd_write__(w, addr, wbuf, wlen, 0);
}

static int i2c_dev_cmd_read(void *arg)
{
    dev_i2c_ioctl_read_s *r = arg;
    unsigned char val;
    unsigned short addr;
    u8 *wbuf = NULL;
    u32 wlen;

    if (!r->tx_buffer || !r->tx_len) {
        return -EINVAL;
    }

    if (copy_from_user(&val, r->tx_buffer, 1)) {
        return -I2C_ECPYFROMUSR;
    }

    addr = (val >> 1) & I2C_7BIT_ADDR_MASK;
    wbuf = &r->tx_buffer[1];
    wlen = r->tx_len - 1;

    return i2c_dev_cmd_read__(r, addr, wbuf, wlen, 0);
}

static int i2c_dev_cmd_reset(const void *arg)
{
    unsigned int drv_id = *((unsigned int *)arg);

    return i2c_reset(drv_id);
}

static int i2c_dev_cmd_slave_cache_write(void *arg)
{
    return i2c_slave_write_cache(arg);
}

static int i2c_dev_cmd_slave_cache_read(void *arg)
{
    return i2c_slave_read_cache(arg);
}

static int i2c_dev_cmd_acquire_mode(void *arg)
{
    i2c_ioctl_mode_check_s *c = arg;

    return i2c_acquire_mode(c->drv_id, &c->mode);
}

static int i2c_dev_cmd_revocation(const void *arg)
{
    unsigned int drv_id = *((unsigned int *)arg);

    return i2c_revocation(drv_id);
}

static int i2c_dev_cmd_deploy(void *arg)
{
    struct i2c_depl *depl = arg;
    struct i2c_kdepl deploy;
    const char *algo_name = NULL;

    deploy.sda_hold = ((depl->sda_hold >> 8) & 0xFFFFFFU); // >> 8
    deploy.sda_hold_ratio = (depl->sda_hold & 0xFFU);
    deploy.flags = depl->flags;
    deploy.scl_high = depl->scl_high;
    deploy.scl_low = depl->scl_low;
    deploy.scl_low_timeout = depl->scl_low_timeout;
    deploy.slave_address = depl->slave_address;
    deploy.slave_dlen = (unsigned long)depl->slave_dlen;
    deploy.clk_strech_timeout = depl->clk_strech_timeout;

    if ((deploy.clk_strech_timeout < I2C_SLAVE_CLK_STRECH_TIME_MIN) ||
        (deploy.clk_strech_timeout > I2C_SLAVE_CLK_STRECH_TIME_MAX)) {
        deploy.clk_strech_timeout = I2C_SLAVE_CLK_STRECH_TIME_MAX;
        if ((depl->flags & I2C_DEPLOY_SLAVE) && (depl->flags & I2C_DEPLOY_SMBUS)) {
            i2c_print("[I2C-%02u-RT] clk_strech_timeout, input value:%u set value:%u",
                      depl->drv_id, depl->clk_strech_timeout, deploy.clk_strech_timeout);
        }
    }

    algo_name = (depl->flags & I2C_DEPLOY_SLAVE) ? "dw-slave" : "dw-master";

    return i2c_deploy(depl->drv_id, algo_name, &deploy);
}

static int i2c_dev_cmd_msg_xfer(void *arg)
{
    struct i2c_msg *msg = arg;
    struct i2c_kmsg *kmsg = NULL;
    int ret;

    kmsg = i2c_dev_msg_memdump_user(msg->wbuf, msg->wlen,
                                    msg->rbuf, msg->rlen);
    if (!kmsg) {
        return -ENOMEM;
    }

    kmsg->flags = msg->flags;
    kmsg->addr = msg->addr;
    kmsg->protocol = msg->smbus_protocol;
    kmsg->timeout = msecs_to_jiffies(msg->timeout);
    kmsg->retries = msg->retries;

    ret = i2c_transaction(msg->drv_id, kmsg);
    if (ret || !(kmsg->flags & I2C_KMSG_RD)) {
        goto xfer_end;
    }

    if (!kmsg->rlen || kmsg->rlen > msg->rlen) {
        ret = -I2C_EMSGRCVLEN;
    } else if (copy_to_user(msg->rbuf, kmsg->rbuf, kmsg->rlen)) {
        ret = -I2C_ECPYTOUSR;
    } else {
        msg->rlen = kmsg->rlen;
    }

xfer_end:
    i2c_dev_msg_mfree(kmsg);
    return ret;
}

static int i2c_dev_cmd_slave_err_code(void *arg)
{
    return i2c_slave_set_err_code(arg);
}

static long i2c_dev_ioctl_process(unsigned int cmd, unsigned long arg)
{
    static const struct i2c_dev_cmd_handler cmd_handler[] = {
        i2c_dev_cmd(INIT, init, dev_i2c_ioctl_init_s),
        i2c_dev_cmd(WRITE, write, dev_i2c_ioctl_write_s),
        i2c_dev_cmd(READ, read, dev_i2c_ioctl_read_s),
        i2c_dev_cmd(RESET, reset, unsigned int),
        i2c_dev_cmd(SLAVE_CACHE_WR, slave_cache_write, i2c_slave_cache_rdwr),
        i2c_dev_cmd(SLAVE_CACHE_RD, slave_cache_read, i2c_slave_cache_rdwr),
        i2c_dev_cmd(MODE_CHECK, acquire_mode, i2c_ioctl_mode_check_s),
        i2c_dev_cmd(REVOCATION, revocation, unsigned int),
        i2c_dev_cmd(DEPLOY, deploy, struct i2c_depl),
        i2c_dev_cmd(MSG_XFER, msg_xfer, struct i2c_msg),
        i2c_dev_cmd(SET_SLAVE_ERR_CODE, slave_err_code, struct i2c_slave_err_code),
    };
    const struct i2c_dev_cmd_handler *handler = NULL;
    char cmd_buffer[I2C_MAX_CMD_LEN];
    unsigned int i;
    int ret;

    for (i = 0; i < ARRAY_SIZE(cmd_handler); i++) {
        if (cmd_handler[i].cmd == cmd) {
            handler = &cmd_handler[i];
            break;
        }
    }

    if (!handler) {
        return -ENOSYS;
    }

    if (handler->cmd_size > sizeof(cmd_buffer)) {
        return -I2C_EBADCMDSIZE;
    }

    if (copy_from_user(cmd_buffer, (void *)arg, handler->cmd_size)) {
        return -I2C_ECPYFROMUSR;
    }

    ret = handler->routine((void *)cmd_buffer);
    if (ret) {
        return ret;
    }

    if (_IOC_DIR(cmd) & _IOC_READ) {
        if (copy_to_user((void *)arg, cmd_buffer, handler->cmd_size)) {
            return -I2C_ECPYTOUSR;
        }
    }

    return 0;
}

static long i2c_dev_ioctl(struct file *pfile, unsigned int cmd, unsigned long arg)
{
    long ret;

    if (!arg) {
        return -EINVAL;
    }

    ret = comm_ioc_parm_check(cmd, (void __user *)arg, I2C_CMD_MAGIC, I2C_CMD_MAXNR);
    if (ret) {
        return ret;
    }

    return i2c_dev_ioctl_process(cmd, arg);
}

static cdev_st g_i2c_dev;

static const struct file_operations g_i2c_dev_file_ops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = i2c_dev_ioctl,
};

static const struct of_device_id g_i2c_dev_ids[] = {
    { .compatible = "hisilicon,hi1711-i2c" },
    {}
};

static int i2c_dev_probe(struct platform_device *pdev)
{
    i2c_init(pdev);

    return comm_init_dev(&g_i2c_dev, &g_i2c_dev_file_ops, "i2c");
}

static int i2c_dev_remove(struct platform_device *pdev)
{
    i2c_revocation_all();
    comm_cleanup_dev(&g_i2c_dev);
    i2c_destroy(pdev);

    return 0;
}

static struct platform_driver g_i2c_dev_platform_driver = {
    .probe = i2c_dev_probe,
    .remove = i2c_dev_remove,
    .driver = {
        .name = "i2c",
        .of_match_table = g_i2c_dev_ids,
    },
};

module_platform_driver(g_i2c_dev_platform_driver);
MODULE_LICENSE("GPL v2");
