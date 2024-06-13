/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */
#include <common.h>
#include <flash.h>
#include <log.h>
#include <watchdog.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <dm/devres.h>
#include <linux/bitops.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/log2.h>
#include <linux/math64.h>
#include <linux/sizes.h>
#include <linux/bitfield.h>
#include <linux/delay.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/spi-nor.h>
#include <mtd/cfi_flash.h>
#include <spi-mem.h>
#include <spi.h>
#include <asm/io.h>
#include <hi309x_common.h>
#include "sf_internal.h"

static void spi_flash_addr(struct spi_flash *flash, u32 addr, u8 *cmd)
{
    /* cmd[0] is actual command */
    if (flash->spi->spec_para.use_4b_flag) {
        cmd[1] = addr >> 24;
        cmd[2] = addr >> 16;
        cmd[3] = addr >> 8;
        cmd[4] = addr >> 0;
        return;
    }
    cmd[1] = addr >> 16;
    cmd[2] = addr >> 8;
    cmd[3] = addr >> 0;
}

/* start for spansion or cypress */
static int spi_flash_read_any_reg(struct spi_flash *flash, unsigned int reg_addr, u8 *val)
{
    int ret = 0;
    u8 cmdsz = 0;
    /* max dummy byte 7 */
    u8 cmd[SPI_FLASH_CMD_LEN + 8] = {0};

    cmd[0] = flash->spi->spec_para.cmd_rdar;
    if (flash->spi->spec_para.use_4b_flag) {
        cmdsz = SPI_FLASH_4B_CMD_LEN + flash->spi->spec_para.rdar_dummy_byte;
    } else {
        cmdsz = SPI_FLASH_CMD_LEN + flash->spi->spec_para.rdar_dummy_byte;
    }
    spi_flash_addr(flash, reg_addr, cmd);

    ret = spi_flash_read_common(flash, cmd, cmdsz, val, 1);
    if (ret < 0) {
        printf("SF: spi_flash_read_any_reg  cmdlen 0x%x failed\n", cmdsz);
        return ret;
    }

    return 0;
}

int spi_flash_write_any_reg(struct spi_flash *flash, u32 reg_addr, u32 len, u32 value)
{
    int ret = 0;
    u8 cmd[SPI_FLASH_4B_CMD_LEN];
    u8 cmdsz = 0;

    if (flash->spi->spec_para.use_4b_flag) {
        cmdsz = SPI_FLASH_4B_CMD_LEN;
    } else {
        cmdsz = SPI_FLASH_CMD_LEN;
    }

    cmd[0] = flash->spi->spec_para.cmd_wrar;
    spi_flash_addr(flash, reg_addr, cmd);

    ret = spi_flash_write_common(flash, cmd, cmdsz, &value, len);
    if (ret < 0) {
        debug("SF: write failed %d\n", ret);
        return ret;
    }

    return 0;
}

/*****************************************************************************
 函 数 名  : spi_flash_fs01gs_init_flash_mem_by_addr
 功能描述  : 初始化spansion s70fs01gsfalsh的mem array
 输入参数  : hisfc_core_host *host
             struct hisfc_spi *spi
             unsigned long addr_area
             u8 is_lock
             u8 sec_arch 需满足mem arr的要求
             u8 sec_pos 需满足mem arr的要求
 输出参数  : 无
 返 回 值  : int
 调用函数  :
 被调函数  :
 说    明  : The main Flash array is divided into erase units called physical
             sectors of 256 KBytes. The FS01GS contains two FS512S devices
             stacked in a DDP. Both the lower and upper address FS512S devices
             must be configured to define the overall sector map of the
             entire 1 Gbit (128 Mbyte) space selected by the CS# for the DDP.
             The lower address FS512S may be configured for bottom parameter
             sectors or uniform sectors. The upper address FS512S may be
             configured for top parameters or uniform sectors. When the lower
             address FS512S is configured for bottom parameter sectors the
             upper address FS512S must be configured as uniform sectors. When
             the upper address FS512S is configured for top parameter sectors
             the lower address FS512S must be configured as uniform sectors.
             In this way, the overall address space of the DDP may have bottom,
             or top parameter sectors, or uniform sectors.
             No other configurations are supported
 修改历史      :
  1.日    期   : 2018年9月15日
    修改内容   : 新生成函数

*****************************************************************************/
int spi_flash_fs01gs_init_flash_mem_by_addr(struct spi_flash *flash, unsigned long addr_area, u8 sec_arch, u8 sec_pos)
{
    int ret = 0;
    u8 cr3v = 0;
    u8 cr1v = 0;

    if ((sec_arch != UNIFORM_SECTOR_ARCH) && (sec_arch != HYBRID_SECTOR_ARCH)) {
        printf("sf para arch type 0x%x err", sec_arch);
        return -EINVAL;
    }

    if ((sec_pos != SECTOR_4KB_AT_TOP) && (sec_pos != SECTOR_4KB_AT_BOTTOM)) {
        printf("sf para sec pos 0x%x err", sec_pos);
        return -EINVAL;
    }
    ret = spi_flash_read_any_reg(flash, SPANSION_CR3V_REG_ADDR(addr_area), &cr3v);
    if (ret) {
        printf("sf read cr3v fail(%d)\n", ret);
        return ret;
    }

    if ((cr3v & SECTOR_ARCH_MASK) == sec_arch) {
        /* 在架构设置ok的情形下  判断是否是uniform架构  是则不用继续配置的sec pos */
        if ((cr3v & SECTOR_ARCH_MASK) == UNIFORM_SECTOR_ARCH) {
            return 0;
        }
    } else {
        /* set arch */
        cr3v &= ~SECTOR_ARCH_MASK;
        cr3v |= (sec_arch & SECTOR_ARCH_MASK);
        ret = spi_flash_write_any_reg(flash, SPANSION_CR3V_REG_ADDR(addr_area), 1, (u32)cr3v);
        if (ret) {
            printf("sf set addr 0x%lx arch 0x%x failed\n", addr_area, sec_arch);
            return ret;
        }
    }

    /* set sec pos */
    ret = spi_flash_read_any_reg(flash, SPANSION_CR1V_REG_ADDR(addr_area), &cr1v);
    if (ret) {
        printf("sf read cr1v fail(%d)\n", ret);
        return ret;
    }

    if ((cr1v & SECTOR_POS_MASK) == sec_pos) {
        return 0;
    }

    /* set sec pos */
    cr1v &= ~SECTOR_POS_MASK;
    cr1v |= (sec_pos & SECTOR_POS_MASK);
    ret = spi_flash_write_any_reg(flash, SPANSION_CR1V_REG_ADDR(addr_area), 1, (u32)cr1v);
    if (ret) {
        printf("sf set addr 0x%lx sec pos 0x%x failed\n", addr_area, sec_pos);
        return ret;
    }

    return ret;
}

/* 修改寄存器对应bit的值将mask为1的bit置为val对应bit的值 */
int spi_flash_modify_any_reg(struct spi_flash *flash, uint reg_addr, u8 mask, u8 val)
{
    int ret = 0;
    u8 reg_val = 0;

    ret = spi_flash_read_any_reg(flash, reg_addr, &reg_val);
    if (ret) {
        printf("sf spi_flash_read_any_reg 0x%x fail(%d)\n", reg_addr, ret);
        return ret;
    }

    reg_val &= (~mask);
    reg_val |= (mask & val);

    ret = spi_flash_write_any_reg(flash, reg_addr, 1, (u32)reg_val);
    if (ret) {
        printf("sf set reg addr 0x%x val 0x%x failed\n", reg_addr, reg_val);
        return ret;
    }

    return ret;
}

/* bottom 4kB其余全部256KB */
int spi_flash_fs01gs_init_flash_mem(struct spi_flash *flash)
{
    int ret = 0;

    ret = spi_flash_fs01gs_init_flash_mem_by_addr(
        flash, SPANSION_S70FS01GS_FIRST_HALF_ADDR, HYBRID_SECTOR_ARCH, SECTOR_4KB_AT_BOTTOM);
    if (ret) {
        printf("sf init first half fail(%d)\n", ret);
        return ret;
    }
    ret = spi_flash_fs01gs_init_flash_mem_by_addr(
        flash, SPANSION_S70FS01GS_SECOND_HALF_ADDR, UNIFORM_SECTOR_ARCH, SECTOR_4KB_AT_BOTTOM);
    if (ret) {
        printf("sf init second half fail(%d)\n", ret);
        return ret;
    }

    return ret;
}

/* end for spansion or cypress */

/*****************************************************************************
 函 数 名  : exec_cmd_resume_bank0
 功能描述  : uboot的sf命令使用切bank的操作，uboot启动读取切换到非bank0，
             当热复位，总线读取会读取到对应bank的数据，在sf命令的总入口
             执行完后将bank切换到bank0
 输入参数  : struct spi_flash *flash
 输出参数  : 无
 返 回 值  : void
 调用函数  : sf do_spi_flash
 被调函数  :

 修改历史      : 1711 add非框架原生内容
  1.日    期   : 2018年8月30日
    修改内容   : 新生成函数

*****************************************************************************/
int write_bar(struct spi_nor *nor, u32 offset);
int read_fsr(struct spi_nor *nor);
int read_sr(struct spi_nor *nor);
void exec_cmd_resume_bank0(struct spi_flash *flash)
{
    int ret = 0;

    /* 返回值代表当前的bank */
    ret = write_bar(flash, 0);
    if (ret < 0) {
        printf("exec_cmd_resume_bank0 failed\n");
        return;
    } else if (ret > 0) {
        printf("exec_cmd_resume_bank0 failed, cur bank 0x%x\n", ret);
        return;
    }

    return;
}

static int spi_flash_sr_ready(struct spi_flash *flash)
{
    u8 sr;
    int ret;

    ret = read_sr(flash);
    if (ret < 0) {
        return ret;
    }

    return !(sr & STATUS_WIP);
}

static int spi_flash_fsr_ready(struct spi_flash *flash)
{
    u8 fsr;
    int ret;

    ret = read_fsr(flash);  // yuliang
    if (ret < 0) {
        return ret;
    }

    return fsr & STATUS_PEC;
}
static int spi_flash_ready(struct spi_flash *flash)
{
    int sr, fsr;

    sr = spi_flash_sr_ready(flash);
    if (sr < 0) {
        return sr;
    }

    fsr = 1;
    if (flash->flags & SNOR_F_USE_FSR) {
        fsr = spi_flash_fsr_ready(flash);
        if (fsr < 0) {
            return fsr;
        }
    }

    return sr && fsr;
}
static int spi_flash_wait_till_ready(struct spi_flash *flash, unsigned long timeout)
{
    unsigned long timebase;
    int ret;

    timebase = get_timer(0);

    while (get_timer(timebase) < timeout) {
        ret = spi_flash_ready(flash);
        if (ret < 0) {
            return ret;
        }
        if (ret) {
            return 0;
        }
    }

    printf("SF: Timeout!\n");

    return -ETIMEDOUT;
}

int spi_flash_cmd_write(struct spi_slave *spi, const u8 *cmd, size_t cmd_len, const void *data, size_t data_len);
int spi_flash_write_common(struct spi_flash *flash, const u8 *cmd, size_t cmd_len, const void *buf, size_t buf_len)
{
    struct spi_slave *spi = flash->spi;
    unsigned long timeout = SPI_FLASH_PROG_TIMEOUT;
    int ret;

    if (buf == NULL) {
        timeout = SPI_FLASH_PAGE_ERASE_TIMEOUT;
    }

    ret = spi_claim_bus(spi);
    if (ret) {
        debug("SF: unable to claim SPI bus\n");
        return ret;
    }

    ret = spi_flash_cmd_write_enable(flash);
    if (ret < 0) {
        debug("SF: enabling write failed\n");
        return ret;
    }

    ret = spi_flash_cmd_write(spi, cmd, cmd_len, buf, buf_len);
    if (ret < 0) {
        debug("SF: write cmd failed\n");
        return ret;
    }

    ret = spi_flash_wait_till_ready(flash, timeout);
    if (ret < 0) {
        debug("SF: write %s timed out\n", timeout == SPI_FLASH_PROG_TIMEOUT ? "program" : "page erase");
        return ret;
    }

    spi_release_bus(spi);

    return ret;
}

int spi_flash_cmd_read(struct spi_slave *spi, const u8 *cmd, size_t cmd_len, void *data, size_t data_len);
int spi_flash_read_common(struct spi_flash *flash, const u8 *cmd, size_t cmd_len, void *data, size_t data_len)
{
    struct spi_slave *spi = flash->spi;
    int ret;

    ret = spi_claim_bus(spi);
    if (ret) {
        debug("SF: unable to claim SPI bus\n");
        return ret;
    }

    ret = spi_flash_cmd_read(spi, cmd, cmd_len, data, data_len);
    if (ret < 0) {
        debug("SF: read cmd failed\n");
        return ret;
    }

    spi_release_bus(spi);

    return ret;
}

/* flash memory init 及地址模式切换 */

static void hisfc_set_host_addr_mode(struct spi_flash *flash, u8 enable_4b)
{
    unsigned int regval;
    unsigned int bus = 0;

    bus = flash->spi->bus;

    regval = hisfc0_read(bus, HISFC350_GLOBAL_CONFIG);
    if (enable_4b) {
        regval |= HISFC350_GLOBAL_CONFIG_ADDR_MODE_4B;
    } else {
        regval &= ~HISFC350_GLOBAL_CONFIG_ADDR_MODE_4B;
    }

    hisfc0_write(bus, HISFC350_GLOBAL_CONFIG, regval);

    return;
}

int spi_flash_s70fs01gs_enter4b(struct spi_flash *flash, u8 enable_4b)
{
    int ret = 0;

    if (enable_4b) {
        ret = spi_flash_cmd_write_enable(flash);
        if (ret) {
            printf("SF: spi_flash_cmd_write_enable failed, 0x%x", enable_4b);
            return ret;
        }
        ret = spi_flash_cmd_enter_4b(flash);
    } else {
        if ((FS01GS_FLASH_TYPE == flash->spi->spec_para.flash_type) ||
            (FS512S_FLASH_TYPE == flash->spi->spec_para.flash_type)) {
            /* 0xe9 not support */
            ret = spi_flash_modify_any_reg(flash, SPANSION_CR2V_REG_ADDR(SPANSION_S70FS01GS_FIRST_HALF_ADDR), 0x80, 0);
            if (ret) {
                printf("SF:spi_flash_modify_any_reg failed, 0x%x", enable_4b);
                return ret;
            }
        }
        if (FS01GS_FLASH_TYPE == flash->spi->spec_para.flash_type) {
            ret = spi_flash_modify_any_reg(flash, SPANSION_CR2V_REG_ADDR(SPANSION_S70FS01GS_SECOND_HALF_ADDR), 0x80, 0);
        }
    }

    if (ret) {
        printf("SF: spi_flash_cmd 4b switch failed, 0x%x", enable_4b);
        return ret;
    }

    hisfc_set_host_addr_mode(flash, enable_4b);

    return 0;
}

/* 等价于writebar */
int spi_flash_cmd_prep(struct spi_flash *flash)
{
    int ret = 0;

    if (flash->spi->spec_para.use_4b_flag) {
        ret = spi_flash_s70fs01gs_enter4b(flash, 1);
        if (ret) {
            printf("SF: spi_flash_s70fs01gs_enter4b enable failed");
            return ret;
        }
    }

    return 0;
}
/* 等价于resume bar */
int spi_flash_cmd_release(struct spi_flash *flash)
{
    int ret = 0;

    if (flash->spi->spec_para.use_4b_flag) {
        ret = spi_flash_s70fs01gs_enter4b(flash, 0);
        if (ret) {
            printf("SF: spi_flash_s70fs01gs_enter4b disable failed");
            return ret;
        }
    }

    return 0;
}
