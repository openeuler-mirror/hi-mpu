// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * spi module sdk interface
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "comm_knl_api.h"
#include "hispi_usr_api.h"
#include "hispi.h"

s32 spi_cmd_std_init(u32 cmd, unsigned long arg)
{
    s32 ret;
    dev_spi_ioctl_std_init std_init = { 0 };

    if (copy_from_user(&std_init, (void __user *)arg, sizeof(dev_spi_ioctl_std_init))) {
        LOG(LOG_ERROR, "copy_from_user spi_cmd_std_init fail");
        return -EFAULT;
    }
    ret = kernspi_init(&std_init);

    return ret;
}

s32 spi_cmd_std_write(u32 cmd, unsigned long arg)
{
    s32 ret;
    u8 data_buf[MAX_CMD_LEN] = "";
    dev_spi_ioctl_std_write std_write = { 0 };
    dev_spi_id spi_id;

    if (copy_from_user(&std_write, (void __user *)arg, sizeof(dev_spi_ioctl_std_write))) {
        LOG(LOG_ERROR, "copy_from_user std_write fail");
        return -EFAULT;
    }

    if ((std_write.cmd_len > MAX_CMD_LEN) || (!std_write.cmd_len)) {
        LOG(LOG_ERROR, "cmd len 0x%x err", std_write.cmd_len);
        return -EINVAL;
    }

    /* After the exception is handled, avoid unnecessary copy. */
    if (copy_from_user(data_buf, std_write.cmd, std_write.cmd_len)) {
        LOG(LOG_ERROR, "spi_cmd_std_write copy_from_user data_buf fail");
        return -EFAULT;
    }

    spi_id.ctrl_id = std_write.ctrl_id;
    spi_id.cs_id = std_write.cs_id;
    ret = kernspi_write_data(&spi_id, data_buf, std_write.cmd_len);

    return ret;
}

s32 spi_cmd_std_read(u32 cmd, unsigned long arg)
{
    s32 ret;
    u8 data_buf[MAX_CMD_LEN] = "";
    u8 *pdata_buf = &data_buf[0];
    u8 cmd_buf[READ_CMD_BUF_LEN] = "";
    dev_spi_ioctl_std_read std_read = { 0 };
    dev_spi_id spi_id;

    if (copy_from_user(&std_read, (void __user *)arg, sizeof(dev_spi_ioctl_std_read))) {
        LOG(LOG_ERROR, "copy_from_user std_read fail");
        return -EFAULT;
    }

    if ((std_read.cmd_len > READ_CMD_BUF_LEN) || (!std_read.cmd_len)) {
        LOG(LOG_ERROR, "cmd len 0x%x err", std_read.cmd_len);
        return -EINVAL;
    }

    if (copy_from_user(cmd_buf, std_read.cmd, std_read.cmd_len)) {
        LOG(LOG_ERROR, "copy_from_user cmd_tmp fail");
        return -EFAULT;
    }

    spi_id.ctrl_id = std_read.ctrl_id;
    spi_id.cs_id = std_read.cs_id;
    ret = kernspi_read_data(&spi_id, &cmd_buf[0], std_read.cmd_len, pdata_buf, std_read.data_len);
    if (ret < 0) {
        LOG(LOG_ERROR, "kernspi_read_data bus 0x%x, cs 0x%x err", std_read.ctrl_id, std_read.cs_id);
        return ret;
    }

    if (copy_to_user(std_read.p_data, (void *)(&data_buf[0]), std_read.data_len)) {
        LOG(LOG_ERROR, "copy_to_user err");
        ret = -EFAULT;
    }

    return ret;
}

s32 spi_cmd_init(u32 cmd, unsigned long arg)
{
    dev_spi_ioctl_std_init std_init;
    spi_ioctl_init spi_init;

    if (copy_from_user(&spi_init, (void __user *)arg, sizeof(spi_ioctl_init))) {
        LOG(LOG_ERROR, "copy_from_user ioctl_init fail");
        return -EFAULT;
    }

    std_init.ctrl_id = 0;
    std_init.cs_id = spi_init.cs_id;
    std_init.pro_type = spi_init.pro_type;
    std_init.baud = spi_init.baud;
    std_init.frame_len = spi_init.frame_len;
    std_init.clk_edge = spi_init.clk_edge;
    std_init.clk_pol = spi_init.clk_pol;

    return kernspi_init(&std_init);
}

s32 spi_cmd_write(u32 cmd, unsigned long arg)
{
    spi_ioctl_write ioctl_write;
    dev_spi_id spi_id;
    u8 data_buf[MAX_CMD_LEN] = "";

    if (copy_from_user(&ioctl_write, (void __user *)arg, sizeof(spi_ioctl_write))) {
        LOG(LOG_ERROR, "copy_from_user ioctl_write fail");
        return -EFAULT;
    }

    if ((ioctl_write.cmd_len > MAX_CMD_LEN) || (!ioctl_write.cmd_len)) {
        LOG(LOG_ERROR, "cmd len 0x%x err", ioctl_write.cmd_len);
        return -EINVAL;
    }

    if (copy_from_user(data_buf, ioctl_write.cmd, ioctl_write.cmd_len)) {
        LOG(LOG_ERROR, "copy_from_user data_buf fail");
        return -EFAULT;
    }

    spi_id.ctrl_id = 0;
    spi_id.cs_id = ioctl_write.cs_id;

    return kernspi_write_data(&spi_id, data_buf, ioctl_write.cmd_len);
}

s32 spi_cmd_read(u32 cmd, unsigned long arg)
{
    s32 ret;
    spi_ioctl_read ioctl_read;
    u8 cmd_buf[READ_CMD_BUF_LEN] = "";
    u8 data_buf[MAX_CMD_LEN] = "";
    dev_spi_id spi_id;

    if (copy_from_user(&ioctl_read, (void __user *)arg, sizeof(spi_ioctl_read))) {
        LOG(LOG_ERROR, "copy_from_user ioctl_read fail");
        return -EFAULT;
    }

    if ((ioctl_read.cmd_len > READ_CMD_BUF_LEN) || (!ioctl_read.cmd_len)) {
        LOG(LOG_ERROR, "cmd len 0x%x err", ioctl_read.cmd_len);
        return -EINVAL;
    }

    if (copy_from_user(cmd_buf, ioctl_read.cmd, ioctl_read.cmd_len)) {
        LOG(LOG_ERROR, "copy_from_user cmd_buf fail");
        return -EFAULT;
    }

    spi_id.ctrl_id = 0;
    spi_id.cs_id = ioctl_read.cs_id;
    ret = kernspi_read_data(&spi_id, cmd_buf, ioctl_read.cmd_len, data_buf, ioctl_read.data_len);
    if (ret < 0) {
        LOG(LOG_ERROR, "spi read data fail(%d)", ret);
        return ret;
    }

    ret = copy_to_user(ioctl_read.pdata, (void *)(&data_buf[0]), ioctl_read.data_len);
    if (ret) {
        LOG(LOG_ERROR, "copy_to_user data_buf fail(%d)", ret);
        ret = -EFAULT;
    }

    return ret;
}

s32 spi_process(u32 cmd, unsigned long arg)
{
    s32 ret;

    switch (cmd) {
        case SPI_CMD_INIT:
            ret = spi_cmd_init(cmd, arg);
            break;
        case SPI_CMD_WRITE:
            ret = spi_cmd_write(cmd, arg);
            break;
        case SPI_CMD_READ:
            ret = spi_cmd_read(cmd, arg);
            break;
        case SPI_CMD_STD_INIT:
            ret = spi_cmd_std_init(cmd, arg);
            break;
        case SPI_CMD_STD_WRITE:
            ret = spi_cmd_std_write(cmd, arg);
            break;
        case SPI_CMD_STD_READ:
            ret = spi_cmd_std_read(cmd, arg);
            break;
        default:
            ret = -ENOTTY;
            break;
    }

    return ret;
}
