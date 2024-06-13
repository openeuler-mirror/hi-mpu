// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sfc controler ops
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "sfc_osadp.h"
#include "sfc_addr_define.h"
#include "sfc_cfi.h"
#include "sfc0_core.h"

static void sfc_core_dma_resp_err_process(const hisfc_core_host *c_host)
{
    if (hisfc_read((c_host), SFC_INT_RAW_STATUS_OFFSET_ADDR) & HISFC_INT_STA_DMA_RESP_ERR) {
        hisfc_write((c_host), SFC_INT_CLEAR_OFFSET_ADDR, HISFC_INT_STA_DMA_RESP_ERR);
        sfc_log(LOG_EMERG_F, SFC_CORE_C, "sfc%u dma resp err", (c_host)->bus);
    }

    return;
}

void sfc_core_dma_wait_cpu_finish(const hisfc_core_host *c_host)
{
    u32 timeout = SFC_CPU_FINISH_MAX_CNT;

    while (((hisfc_read((c_host), SFC_BUS_DMA_CTRL_OFFSET_ADDR) & HISFC350_BUS_DMA_CTRL_START)) && timeout) {
        --timeout;
        cond_resched();
    }

    sfc_core_dma_resp_err_process(c_host);
    if (!timeout) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "dma wait cpu finish timeout");
    }

    return;
}

static void sfc_core_pp_fail_process(const hisfc_core_host *c_host)
{
    if (hisfc_read((c_host), SFC_INT_RAW_STATUS_OFFSET_ADDR) & HISFC_INT_STA_PP_FAIL) {
        hisfc_write((c_host), SFC_INT_CLEAR_OFFSET_ADDR, HISFC_INT_STA_PP_FAIL);
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc%u pp failed", (c_host)->bus);
    }

    return;
}

void sfc_core_wait_cpu_finish(const hisfc_core_host *c_host)
{
    u32 timeout = SFC_CPU_FINISH_MAX_CNT;

    while (((hisfc_read((c_host), SFC_CMD_CONFIG_OFFSET_ADDR) & HISFC351_CMD_CONFIG_START)) && timeout) {
        --timeout;
    }
    sfc_core_pp_fail_process(c_host);
    if (!timeout) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "cmd wait cpu finish timeout");
    }

    return;
}

s32 sfc_core_release_spi_lock(hisfc_spi *spi)
{
    hisfc_core_host *host = NULL;

    if (spi == NULL) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_release_spi_lock spi is NULL");
        return -EINVAL;
    }

    host = (hisfc_core_host *)spi->host;

    /* Set bit0 and bit20 to 0. */
    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR, HISFC351_CMD_CONFIG_LOCK_FLASH(SPI_OPR_NOLOCK));

    return 0;
}

s32 sfc_core_read_reg_by_cmd(hisfc_spi *spi, u32 cmd, u8 iftype, u32 *value)
{
    hisfc_core_host *host = NULL;

    if (!spi || !value) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "spi or value is NULL.");
        return -EINVAL;
    }

    host = (hisfc_core_host *)spi->host;
    if (!host) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "spi->host is NULL.");
        return -EINVAL;
    }

    hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, cmd);

    /* Only one u8 data can be read. */
    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                HISFC351_CMD_CONFIG_MEM_IF_TYPE(iftype) |
                HISFC351_CMD_CONFIG_DATA_EN |
                HISFC351_CMD_CONFIG_RW_READ |
                HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect) |
                HISFC351_CMD_CONFIG_START);

    sfc_core_wait_cpu_finish(host);

    *value = hisfc_read(host, SFC_CMD_DATABUF_0_OFFSET_ADDR);

    return 0;
}

s32 sfc_core_write_enable_and_judge(hisfc_spi *spi, u8 iftype)
{
    u32 status = 0;
    s32 ret;

    ret = spi->driver->write_enable(spi);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "write_enable failed");
        return ret;
    }

    /* Check whether the write function is enabled successfully. */
    ret = sfc_core_read_reg_by_cmd(spi, SPI_CMD_RDSR, iftype, &status);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "read spi flash status register failed(%d)", ret);
        return ret;
    }

    if (!(SPI_CMD_SR_WEL & status)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "Write Enable fialed");
        return -EBUSY;
    }

    return 0;
}

s32 sfc_core_write_reg_by_cmd(hisfc_spi *spi, u8 cmd, u8 iftype, u8 len, u32 value)
{
    s32 ret;
    u32 cnt = 0x400;
    hisfc_core_host *host = NULL;

    if ((spi == NULL) || (spi->host == NULL)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_write_reg_by_cmd spi or spi->host is NULL");
        return -EINVAL;
    }
    host = (hisfc_core_host *)spi->host;

    if (len > REG_WR_CMD_LEN) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "only support u32, len = %u > 4", len);
        return -EINVAL;
    }
    ret = sfc_core_write_enable_and_judge(spi, iftype);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_write_enable_and_judge failed(%d)!", ret);
        return ret;
    }

    hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, cmd);
    /* The write line operation mode is used by default. */
    hisfc_write(host, SFC_CMD_DATABUF_0_OFFSET_ADDR, value);
    /* Configure the CMD configuration register. */
    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                HISFC351_CMD_CONFIG_MEM_IF_TYPE(iftype) |
                HISFC351_CMD_CONFIG_DATA_CNT(len) |
                HISFC351_CMD_CONFIG_DATA_EN |
                HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect) |
                HISFC351_CMD_CONFIG_START);
    while (cnt--) {
        ;
    }

    sfc_core_wait_cpu_finish(host);

    return 0;
}

s32 sfc_core_read_any_reg(hisfc_spi *spi, u32 reg_addr, u8 iftype, u8 *value, u8 is_lock)
{
    hisfc_core_host *host = NULL;

    if (spi == NULL) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_read_any_reg spi is NULL");
        return -EINVAL;
    }

    host = (hisfc_core_host *)spi->host;
    if ((!value) || (!host)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "input err value is NULL.");
        return -EINVAL;
    }

    hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, SPI_CMD_RDAR);
    hisfc_write(host, SFC_CMD_ADDR_OFFSET_ADDR, reg_addr);
    /* Only one u8 data can be read. */
    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                HISFC351_CMD_CONFIG_LOCK_FLASH(is_lock)
                | HISFC351_CMD_CONFIG_MEM_IF_TYPE(iftype)
                | HISFC351_CMD_CONFIG_DUMMY_CNT(1)
                | HISFC351_CMD_CONFIG_DATA_EN
                | HISFC351_CMD_CONFIG_ADDR_EN
                | HISFC351_CMD_CONFIG_RW_READ
                | HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect)
                | HISFC351_CMD_CONFIG_START);

    sfc_core_wait_cpu_finish(host);

    *value = (u8)hisfc_read(host, SFC_CMD_DATABUF_0_OFFSET_ADDR);

    return 0;
}

s32 sfc_core_write_any_reg(hisfc_spi *spi, u8 iftype, u8 len, u32 value, u32 addr)
{
    u32 cmd_send_cnt;
    s32 ret;
    hisfc_core_host *host = NULL;

    if (spi == NULL) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_write_any_reg spi is NULL");
        return -EINVAL;
    }

    ret = spi->driver->write_enable(spi);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "write_enable failed");
        return ret;
    }

    /* replace delay function */
    cmd_send_cnt = CMD_SEND_WAIT_CNT;
    while (cmd_send_cnt--) {
        ;
    }

    host = (hisfc_core_host *)spi->host;
    hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, SPI_CMD_WRAR);
    hisfc_write(host, SFC_CMD_ADDR_OFFSET_ADDR, addr);
    hisfc_write(host, SFC_CMD_DATABUF_0_OFFSET_ADDR, value);
    /* Configure the CMD configuration register. */
    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                HISFC351_CMD_CONFIG_MEM_IF_TYPE(iftype) |
                HISFC351_CMD_CONFIG_DATA_CNT(len) |
                HISFC351_CMD_CONFIG_DATA_EN |
                HISFC351_CMD_CONFIG_ADDR_EN |
                HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect) |
                HISFC351_CMD_CONFIG_START);
    cmd_send_cnt = CMD_SEND_WAIT_CNT;
    while (cmd_send_cnt--) {
        ;
    }

    sfc_core_wait_cpu_finish(host);

    return 0;
}

s32 sfc_core_modify_any_reg(hisfc_spi *spi, u32 reg_addr, u8 mask, u8 val, u8 is_lock)
{
    s32 ret;
    u8 reg_val = 0;

    ret = sfc_core_read_any_reg(spi, reg_addr, 0, &reg_val, is_lock);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "read reg val failed(%d)", ret);
        return ret;
    }

    reg_val &= (~mask);
    reg_val |= (mask & val);

    ret = sfc_core_write_any_reg(spi, 0, 1, (u32)reg_val, reg_addr);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "set reg addr %u val %u failed", reg_addr, reg_val);
        return ret;
    }

    return ret;
}

#if sfc_section("spansion flash interface")
s32 sfc_core_fsxx_check_hybrid_sector_pos(hisfc_core_host *host, hisfc_spi *spi,
                                          unsigned long addr_area, u8 is_lock, u8 *at_top)
{
    s32 ret;
    u8 cr1v = 0;

    ret = sfc_core_read_any_reg(spi, spansion_cr1v_reg_addr(addr_area), spi->read->iftype, &cr1v, is_lock);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "read cr1v failed(%d)", ret);
        return ret;
    }
    *at_top = (cr1v & SECTOR_POS_MASK);

    return 0;
}

s32 sfc_core_fsxx_check_sector_arch(hisfc_core_host *host, hisfc_spi *spi,
                                    unsigned long addr_area, u8 is_lock, u8 *is_uniform_sec)
{
    s32 ret;
    u8 cr3v = 0;

    ret = sfc_core_read_any_reg(spi, spansion_cr3v_reg_addr(addr_area), spi->read->iftype, &cr3v, is_lock);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "read cr3v failed(%d)", ret);
        return ret;
    }
    *is_uniform_sec = (cr3v & SECTOR_ARCH_MASK);

    return 0;
}

static s32 sfc_core_fs01gs_init_para_check(hisfc_core_host *host, u8 sec_arch, u8 sec_pos)
{
    if (host == NULL) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "para host is NULL");
        return -EFAULT;
    }

    if ((sec_arch != UNIFORM_SECTOR_ARCH) && (sec_arch != HYBRID_SECTOR_ARCH)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "para host or arch type %u err", sec_arch);
        return -EINVAL;
    }

    if ((sec_pos != SECTOR_4KB_AT_TOP) && (sec_pos != SECTOR_4KB_AT_BOTTOM)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "para sec pos %u err", sec_pos);
        return -EINVAL;
    }

    return 0;
}

/*
 * The main Flash array is divided into erase units called physical sectors of 256 KBytes.
 * The FS01GS contains two FS512S devices stacked in a DDP. Both the lower and upper address FS512S devices
 * must be configured to define the overall sector map of the entire 1 Gbit (128 Mbyte) space selected by the CS#
 * for the DDP. The lower address FS512S may be configured for bottom parameter sectors or uniform sectors.
 * The upper address FS512S may be configured for top parameters or uniform sectors. When the lower address FS512S
 * is configured for bottom parameter sectors the upper address FS512S must be configured as uniform sectors. When
 * the upper address FS512S is configured for top parameter sectors the lower address FS512S must be configured
 * as uniform sectors. In this way, the overall address space of the DDP may have bottom, or top parameter sectors,
 * or uniform sectors. No other configurations are supported
 */
s32 sfc_core_fs01gs_init_flash_mem_by_addr(hisfc_spi *spi, unsigned long addr_area, u8 is_lock, u8 sec_arch, u8 sec_pos)
{
    s32 ret;
    u8 cr3v = 0;
    u8 cr1v = 0;
    hisfc_core_host *host;

    host = (hisfc_core_host *)spi->host;
    ret = sfc_core_fs01gs_init_para_check(host, sec_arch, sec_pos);
    if (ret) {
        return -EINVAL;
    }

    ret = sfc_core_read_any_reg(spi, spansion_cr3v_reg_addr(addr_area), spi->read->iftype, &cr3v, is_lock);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "read cr3v failed(%d)", ret);
        return ret;
    }

    if (sec_arch == (cr3v & SECTOR_ARCH_MASK)) {
        if ((cr3v & SECTOR_ARCH_MASK) == UNIFORM_SECTOR_ARCH) {
            return 0;
        }
    } else {
        /* set arch */
        cr3v &= ~SECTOR_ARCH_MASK;
        cr3v |= (sec_arch & SECTOR_ARCH_MASK);
        ret = sfc_core_write_any_reg(spi, 0, 1, (u32)cr3v, spansion_cr3v_reg_addr(addr_area));
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "set addr 0x%lx arch %u failed", addr_area, sec_arch);
            return ret;
        }
    }

    /* set sec pos */
    ret = sfc_core_read_any_reg(spi, spansion_cr1v_reg_addr(addr_area), spi->read->iftype, &cr1v, is_lock);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "read cr1v failed(%d)", ret);
        return ret;
    }

    if (sec_pos == (cr1v & SECTOR_POS_MASK)) {
        return 0;
    }

    /* set sec pos */
    cr1v &= ~SECTOR_POS_MASK;
    cr1v |= (sec_pos & SECTOR_POS_MASK);
    ret = sfc_core_write_any_reg(spi, 0, 1, (u32)cr1v, spansion_cr1v_reg_addr(addr_area));
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "set addr 0x%lx sec pos %u failed", addr_area, sec_pos);
        return ret;
    }

    return ret;
}

/*
 * Initializes the flash memory of the S70fs01gs series. The current code supports only 256 KB and 4 KB address 0.
 */
s32 sfc_core_fs01gs_init_flash_mem(hisfc_spi *spi, u8 is_lock)
{
    s32 ret;

    if (spi == NULL) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "fs01gs_init_flash_mem spi is NULL");
        return -EINVAL;
    }

    ret = sfc_core_fs01gs_init_flash_mem_by_addr(spi, SPANSION_S70FS01GS_FIRST_HALF_ADDR,
                                                 is_lock, HYBRID_SECTOR_ARCH, SECTOR_4KB_AT_BOTTOM);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "init first half failed(%d)", ret);
        return ret;
    }
    ret = sfc_core_fs01gs_init_flash_mem_by_addr(spi, SPANSION_S70FS01GS_SECOND_HALF_ADDR,
                                                 is_lock, UNIFORM_SECTOR_ARCH, SECTOR_4KB_AT_BOTTOM);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "init second half failed(%d)", ret);
        return ret;
    }

    return ret;
}

/*
 * Initializes the flash memory of the fs512s series. The current code supports only 256 KB and 4 KB address 0.
 */
s32 sfc_core_fs512s_init_flash_mem(hisfc_spi *spi, u8 is_lock)
{
    s32 ret;

    if (spi == NULL) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "fs512s_init_flash_mem spi is NULL");
        return -EINVAL;
    }

    ret = sfc_core_fs01gs_init_flash_mem_by_addr(spi, SPANSION_S70FS01GS_FIRST_HALF_ADDR,
                                                 is_lock, UNIFORM_SECTOR_ARCH, SECTOR_4KB_AT_BOTTOM);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "init fs512s uniform sector failed(%d)", ret);
        return ret;
    }

    return ret;
}

s32 sfc_core_4kb_subsector_erase(hisfc_core_host *host, hisfc_spi *spi, u32 addr, u32 len, u8 is_lock)
{
    s32 ret;

    if (spi->chipsize < (addr + len)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "Input para addr:%u and len:%u is beyond!", addr, len);
        return -EINVAL;
    }
    /* _4K align for input addr and len */
    if (((addr & (_4K - 1)) != 0) || ((len & (_4K - 1)) != 0)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "Do not _4k align for input addr or len !");
        return -EINVAL;
    }

    while (len) {
        ret = sfc_core_reg_erase_one_block(host, spi, addr, is_lock, SECTOR_TYPE_4KB);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc erase _4k failed(%d)!", ret);
            return ret;
        }
        addr += _4K;
        len -= _4K;
    }

    return 0;
}

s32 sfc_core_spansionfsxx_bottom_4k_erase(hisfc_core_host *host, hisfc_spi *spi,
                                          u32 addr, u32 len, u8 is_lock)
{
    s32 ret;
    u8 is_uniform_sec = 0;

    ret = sfc_core_fsxx_check_sector_arch(host, spi, addr, is_lock, &is_uniform_sec);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_fsxx_check_sector_arch failed(%d)", ret);
        return ret;
    }
    sfc_log(LOG_INFO_F, SFC_CORE_C, "is_uniform_sec = %u", is_uniform_sec);
    if (!is_uniform_sec) {
        ret = sfc_core_4kb_subsector_erase(host, spi, 0, (SECTOR_4KB_CNT * _4K), is_lock);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_4kb_subsector_erase failed(%d)", ret);
            return ret;
        }
    }

    return 0;
}

s32 sfc_core_spansionfsxx_erase(hisfc_spi *spi, unsigned long long addr, unsigned long long len, u8 is_lock)
{
    s32 ret;
    hisfc_core_host *host = NULL;
    u8 at_top = 0;

    if ((spi == NULL) || (spi->driver == NULL)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_spansionfsxx_erase spi or spi->driver is NULL");
        return -EINVAL;
    }
    host = (hisfc_core_host *)spi->host;

    /* Check the location of the sub-sector according to the default configuration. */
    if (((addr == 0) && (len >= spi->erasesize)) || ((u32)(spi->chipsize) - spi->erasesize == (u32)addr)) {
        ret = sfc_core_fsxx_check_hybrid_sector_pos(host, spi, 0, is_lock, &at_top);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_fsxx_check_hybrid_sector_pos failed(%d)", ret);
            return ret;
        }
        if (SECTOR_4KB_AT_TOP == at_top) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "warning: cr1v is modified");
        }
    }

    if ((addr == 0x0) && (len >= spi->erasesize)) {
        ret = sfc_core_spansionfsxx_bottom_4k_erase(host, spi, (u32)addr, (u32)len, is_lock);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "spi_flash_spansionfsxx_top_4k_erase failed(%d)", ret);
            return ret;
        }
    }

    ret = sfc_core_reg_erase(spi, addr, len);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_reg_erase sector_erase failed(%d)", ret);
        return ret;
    }

    return 0;
}

#endif

s32 sfc_core_check_addr_len(hisfc_spi *spi, loff_t addr, size_t len, u8 is_erase)
{
    if (spi == NULL) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc0_flash_check_addr_len spi is NULL");
        return -EINVAL;
    }

    if (((u64)addr > spi->chipsize) || ((u64)len > spi->chipsize) || ((u64)(addr + len) > spi->chipsize)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "write or read data out of range. addr 0x%llx, len %u", addr, len);
        return -EINVAL;
    }

    if (!len) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "write length is 0. addr = 0x%llx", addr);
        return SFC_RW_LEN_ZERO;
    }

    /* Check Address Alignment */
    if (is_erase) {
        if ((u32)addr & (spi->erasesize - 1)) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "erase start address is not alignment");
            return -EINVAL;
        }

        if ((u32)len & (spi->erasesize - 1)) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "erase length is not alignment");
            return -EINVAL;
        }
    }

    return 0;
}

static s8 *sfc_core_reg_read_buf(const hisfc_spi *spi, u32 spi_start_addr, u32 size, u8 *buffer, u8 is_lock)
{
    u32 numread;
    s32 index = 0;
    s8 tmp[MIN_BUF_LEN];
    u8 *ptr = buffer;
    hisfc_core_host *host = NULL;

    if (size > HISFC350_REG_BUF_SIZE) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "reg read out of reg range.");
        return (s8 *)buffer;
    }

    host = (hisfc_core_host *)spi->host;
    hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, spi->read->cmd);
    hisfc_write(host, SFC_CMD_ADDR_OFFSET_ADDR, (spi_start_addr & HISFC350_CMD_ADDR_MASK));
    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                HISFC351_CMD_CONFIG_LOCK_FLASH(is_lock)
                | HISFC351_CMD_CONFIG_MEM_IF_TYPE(spi->read->iftype)
                | HISFC351_CMD_CONFIG_DATA_CNT(size)
                | HISFC351_CMD_CONFIG_RW_READ
                | HISFC351_CMD_CONFIG_DATA_EN
                | HISFC351_CMD_CONFIG_DUMMY_CNT(spi->read->dummy)
                | HISFC351_CMD_CONFIG_ADDR_EN
                | HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect)
                | HISFC351_CMD_CONFIG_START);
    sfc_core_wait_cpu_finish(host);

    /* Read of data greater than or equal to four bytes */
    numread = (size & ~0x03);
    while (numread) {
        *(u32 *)ptr = hisfc_read(host, SFC_CMD_DATABUF_0_OFFSET_ADDR + index);
        ptr += BUF_READ_OFF;
        index += BUF_READ_OFF;
        numread -= BUF_READ_OFF;
    }
    /* Reads of less than four bytes of data */
    numread = (size & 0x03);
    if (numread) {
        *(u32 *)tmp = hisfc_read(host, SFC_CMD_DATABUF_0_OFFSET_ADDR + index);
        if (memcpy_s(ptr, sizeof(tmp), tmp, numread)) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "read memcpy failed!");
        }
    }

    return (s8 *)buffer;
}

s32 sfc_core_s70fs01gs_reg_read(hisfc_spi *spi, loff_t from, size_t len, size_t *retlen, u8 *buf)
{
    s32 ret;
    size_t temp_retlen_first = 0;
    size_t temp_retlen_second = 0;

    /* S70fs01gs Reads Across 64 MB Boundary */
    if (((u32)from < S70FS01GS_BOUND_ADDR) && ((u32)(from + len) > S70FS01GS_BOUND_ADDR)) {
        ret = sfc_core_reg_read(spi, from, S70FS01GS_BOUND_ADDR - from, &temp_retlen_first, buf);
        if (ret) {
            *retlen = temp_retlen_first;
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc reg read first failed(%d), retlen1=%u", ret, (u32)temp_retlen_first);

            return ret;
        }

        ret = sfc_core_reg_read(spi, S70FS01GS_BOUND_ADDR, from + len - S70FS01GS_BOUND_ADDR,
                                &temp_retlen_second, buf + S70FS01GS_BOUND_ADDR - from);
        *retlen = temp_retlen_first + temp_retlen_second;
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_reg_read second failed ret %d, retlen2=%u, retlen=%u",
                    ret, (u32)temp_retlen_second, (u32)*retlen);
        }

        return ret;
    }

    ret = sfc_core_reg_read(spi, from, len, retlen, buf);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_reg_read full failed ret %d, retlen=%u", ret, (u32)*retlen);
        return ret;
    }

    return ret;
}

void sfc_core_dma_rw_config(const hisfc_core_host *c_host, dma_rw_addr rm_addr)
{
    hisfc_write(c_host, SFC_BUS_DMA_MEM_SADDR_OFFSET_ADDR, rm_addr.dma_phy_addr & 0xffffffffU);
    hisfc_write(c_host, SFC_BUS_DMA_FLASH_SADDR_OFFSET_ADDR, rm_addr.spi_flash_addr);
    hisfc_write(c_host, SFC_BUS_DMA_LEN_OFFSET_ADDR, HISFC350_BUS_DMA_LEN_DATA_CNT(rm_addr.len));
    hisfc_write(c_host, SFC_BUS_DMA_AHB_CTRL_OFFSET_ADDR,
                HISFC350_BUS_DMA_AHB_CTRL_INCR4_EN
                | HISFC350_BUS_DMA_AHB_CTRL_INCR8_EN
                | HISFC350_BUS_DMA_AHB_CTRL_INCR16_EN);
    hisfc_write(c_host, SFC_BUS_DMA_CTRL_OFFSET_ADDR,
                HISFC350_BUS_DMA_CTRL_RW(rm_addr.is_read)
                | HISFC350_BUS_DMA_CTRL_CS(rm_addr.cs)
                | HISFC350_BUS_DMA_CTRL_START);

    return;
}

s32 sfc_core_reg_read(hisfc_spi *spi, loff_t from, size_t len, size_t *retlen, u8 *buf)
{
    s32 num;
    s32 ret;
    u8 *ptr = buf;
    hisfc_core_host *host = NULL;

    if ((spi == NULL) || (spi->driver == NULL)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_reg_read spi or spi->driver is NULL");
        return -EINVAL;
    }

    *retlen = 0;

    ret = sfc_core_check_addr_len(spi, from, len, 0);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_reg_read sfc0_flash_check_addr_len failed %d", ret);
        if (ret == SFC_RW_LEN_ZERO) {
            return 0;
        }

        return ret;
    }

    ret = spi->driver->wait_ready(spi, SPI_OPR_LOCK);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "spi wait ready failed");
        goto fail;
    }

    host = (hisfc_core_host *)spi->host;
    host->set_system_clock(host, spi->read, SFC_TRUE);

    while (len > 0) {
        num = (s32)(((u64)(from + len) >= spi->chipsize) ? (spi->chipsize - (u64)from) : len);
        while (num >= HISFC350_REG_BUF_SIZE) {
            (void)sfc_core_reg_read_buf(spi, (u32)from, HISFC350_REG_BUF_SIZE, ptr, SPI_OPR_LOCK);
            ptr += HISFC350_REG_BUF_SIZE;
            from += HISFC350_REG_BUF_SIZE;
            len -= HISFC350_REG_BUF_SIZE;
            num -= HISFC350_REG_BUF_SIZE;
        }

        if (num) {
            (void)sfc_core_reg_read_buf(spi, (u32)from, (u32)num, ptr, SPI_OPR_LOCK);
            from += num;
            ptr += num;
            len -= (u32)num;
        }
    }

    ret = 0;
    *retlen = (size_t)(ptr - buf);
fail:
    (void)spi->driver->spi_opr_finish_call(spi);

    return ret;
}

s32 sfc_core_reg_read_for_all(hisfc_spi *spi, loff_t from, size_t len, size_t *retlen, u8 *buf)
{
    s32 ret;

    if (spi->driver == NULL) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "spi->driver is NULL");
        return -EINVAL;
    }

    if (spi->driver->read_process != NULL) {
        ret = spi->driver->read_process(spi, from, len, retlen, buf);
    } else {
        ret = sfc_core_reg_read(spi, from, len, retlen, buf);
    }

    return ret;
}

/*
 * Waiting for the completion of SFC erase, including the chip and flash memory
 */
s32 sfc_core_wait_wip_finish(const hisfc_core_host *host, hisfc_spi *spi, u8 is_lock)
{
    s32 ret;

    sfc_core_wait_cpu_finish(host);

    udelay(1);
    ret = spi->driver->wait_ready(spi, is_lock);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc0_wait_wip_finish sfc wait reay failed");
        return ret;
    }

    return ret;
}

void sfc_core_memcpy_u8_to_u32(u32 *data_des, u8 *data, u32 len)
{
    u8 *pdata = NULL;
    u32 *pdata_des = NULL;
    u32 i;
    u32 tmp;
    u32 loop_num;
    u32 remain;

    if ((data == NULL) || (data_des == NULL)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "input err, data or data_des is NULL.");
        return;
    }

    if (len == 0) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "input err, len = %u.", len);
        return;
    }

    pdata = data;
    pdata_des = data_des;
    /* Calculate the number of cycles shift 2 */
    loop_num = (u32)(len >> 2);
    remain = len & 0x3;

    for (i = 0; i < loop_num; i++) {
        tmp = *(u32 *)pdata;
        pdata += BUF_READ_OFF;
        *pdata_des = tmp;
        pdata_des++;
    }

    if (remain != 0) {
        if (memcpy_s((u8 *)&tmp, remain, pdata, (unsigned long)remain)) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "memcpy_s remain failed");
        }
        *pdata_des = tmp;
    }

    return;
}

static s32 sfc_core_reg_write_buf(hisfc_spi *spi, loff_t addr, u8 *data, u32 len, u8 is_lock)
{
    s32 ret;
    hisfc_core_host *host = NULL;

    if (data == NULL) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "input err, data is NULL.");
        return -EINVAL;
    }

    if ((len > HISFC350_REG_BUF_SIZE) || (len == 0)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "input err, len = %u, addr = 0x%llx.", len, addr);
        return -EINVAL;
    }

    ret = spi->driver->write_enable(spi);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "Write Enable failed(%d)!", ret);
        return ret;
    }

    /* Configure data based on the length of the write data. */
    host = (hisfc_core_host *)spi->host;
    sfc_core_memcpy_u8_to_u32((u32 *)(host->regbase + SFC_CMD_DATABUF_0_OFFSET_ADDR), data, len);

    /* Configuration address register */
    hisfc_write(host, SFC_CMD_ADDR_OFFSET_ADDR, ((u32)addr));
    /* Configure the instruction register and write the instruction. */
    hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, spi->write->cmd);
    /* Configure the CMD configuration register. */
    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                HISFC351_CMD_CONFIG_LOCK_FLASH(is_lock)
                | HISFC351_CMD_CONFIG_MEM_IF_TYPE(spi->write->iftype)
                | HISFC351_CMD_CONFIG_DATA_CNT(len)
                | HISFC351_CMD_CONFIG_DATA_EN
                | HISFC351_CMD_CONFIG_DUMMY_CNT(spi->write->dummy)
                | HISFC351_CMD_CONFIG_ADDR_EN
                | HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect)
                | HISFC351_CMD_CONFIG_START);

    ret = sfc_core_wait_wip_finish(host, spi, is_lock);

    return ret;
}

static s32 sfc_core_reg_write_data(hisfc_spi *spi, loff_t to, size_t len, size_t *retlen, const u8 *data)
{
    s32 ret;
    u8 *data_temp = NULL;
    loff_t addr_temp;
    u32 page_bottom;
    u32 beyond_remain;

    data_temp = (u8 *)data;
    addr_temp = to;
    /* Distance from bottom of page */
    page_bottom = SPI_FLASH_PAGE_SIZE - (to % SPI_FLASH_PAGE_SIZE);
    /* Do not write the remainder in a spread. After that, the remainder can be divisible by 64. */
    beyond_remain = page_bottom % HISFC350_REG_BUF_SIZE;

    if (len >= beyond_remain) {
        if (beyond_remain != 0) { /* Guaranteed not to spread */
            ret = sfc_core_reg_write_buf(spi, addr_temp, data_temp, beyond_remain, SPI_OPR_LOCK);
            if (ret != 0) {
                sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc reg write buf failed(%d)!", ret);
                return ret;
            }
            addr_temp += beyond_remain;
            data_temp += beyond_remain;
            len -= beyond_remain;
        }
    }

    while (len >= HISFC350_REG_BUF_SIZE) {
        ret = sfc_core_reg_write_buf(spi, addr_temp, data_temp, HISFC350_REG_BUF_SIZE, SPI_OPR_LOCK);
        if (ret != 0) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc reg write buf failed(%d)!", ret);
            return ret;
        }
        addr_temp += HISFC350_REG_BUF_SIZE;
        data_temp += HISFC350_REG_BUF_SIZE;
        len -= HISFC350_REG_BUF_SIZE;
    }

    if (len) {
        ret = sfc_core_reg_write_buf(spi, addr_temp, data_temp, len, SPI_OPR_LOCK);
        if (ret != 0) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc reg write buf failed(%d)!", ret);
            return ret;
        }
        data_temp += len;
    }

    *retlen = (size_t)(data_temp - data);

    return 0;
}

s32 sfc_core_reg_write(hisfc_spi *spi, loff_t to, size_t len, size_t *retlen, const u8 *data)
{
    s32 ret;

    if (spi == NULL) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_reg_write spi is NULL");
        return -EINVAL;
    }

    *retlen = 0;
    ret = sfc_core_check_addr_len(spi, to, len, 0);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_reg_write sfc_core_check_addr_len failed %d", ret);
        if (ret == SFC_RW_LEN_ZERO) {
            return 0;
        }

        return ret;
    }

    ret = spi->driver->wait_ready(spi, SPI_OPR_LOCK);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc wait reay failed");
        goto LOCK_OUT;
    }

    ret = sfc_core_reg_write_data(spi, to, len, retlen, data);
    if (ret != 0) {
        goto LOCK_OUT;
    }

LOCK_OUT:
    (void)spi->driver->spi_opr_finish_call(spi);

    return ret;
}

s32 sfc_core_reg_erase_one_block(hisfc_core_host *host, hisfc_spi *spi,
                                 u32 offset, u8 is_lock, u8 sector_type)
{
    u8 erase_cmd = spi->erase->cmd;
    if (spi->driver->wait_ready(spi, is_lock)) {
        return -EBUSY;
    }

    (void)spi->driver->write_enable(spi);
    host->set_system_clock(host, spi->erase, SFC_TRUE);

    if (SECTOR_TYPE_4KB == sector_type) {
        erase_cmd = SPI_CMD_SE_4K;
    }

    hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, erase_cmd);
    hisfc_write(host, SFC_CMD_ADDR_OFFSET_ADDR, (offset & HISFC350_CMD_ADDR_MASK));
    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                HISFC351_CMD_CONFIG_LOCK_FLASH(is_lock)
                | HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect)
                | HISFC351_CMD_CONFIG_MEM_IF_TYPE(spi->erase->iftype)
                | HISFC351_CMD_CONFIG_DUMMY_CNT(0)
                | HISFC351_CMD_CONFIG_ADDR_EN
                | HISFC351_CMD_CONFIG_START);
    sfc_core_wait_cpu_finish(host);

    return 0;
}

s32 sfc_core_reg_erase(hisfc_spi *spi, unsigned long long addr, unsigned long long len)
{
    s32 ret;
    hisfc_core_host *host = NULL;
    unsigned long long temp_addr = addr;
    unsigned long long temp_len = len;

    if (spi == NULL) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_reg_read spi is NULL");
        return -EINVAL;
    }

    host = (hisfc_core_host *)spi->host;

    ret = sfc_core_check_addr_len(spi, temp_addr, temp_len, 1);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_reg_read sfc0_flash_check_addr_len failed %d", ret);
        if (ret == SFC_RW_LEN_ZERO) {
            return 0;
        }

        return ret;
    }

    while (temp_len) {
        ret = sfc_core_reg_erase_one_block(host, spi, (u32)temp_addr, SPI_OPR_LOCK, SECTOR_TYPE_DEFAULT);
        if (ret != 0) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "erase offset is 0x%llx, length is 0x%llx, ret %d", temp_addr, len, ret);
            goto FAIL;
        }

        temp_addr += spi->erase->size;
        temp_len -= spi->erase->size;
    }
    /* Ensure that the last erase operation is complete. Otherwise, data read errors may occur
     * if the flash memory does not complete the operation after the last erase command is issued. */
    ret = sfc_core_wait_wip_finish(host, spi, SPI_OPR_LOCK);

FAIL:
    (void)spi->driver->spi_opr_finish_call(spi);

    return ret;
}

s32 sfc_core_bus_addr_align_detect(loff_t addr)
{
    if ((u32)addr & BUS_ADDR_ALIGN_MASK) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "bus rw addr is not alignment.");
        return -EINVAL;
    }

    return 0;
}

u8 *sfc_core_read_ids(hisfc_core_host *host, u32 cs, u8 *buffer)
{
    s32 regindex = 0;
    u32 numread = 8; /* 8: The flash ID contains a maximum of eight bytes. */
    u32 *ptr = (u32 *)buffer;

    if (host->set_host_addr_mode) {
        host->set_host_addr_mode(host, SFC_4BYTE_DISABLE);
    }

    hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, SPI_CMD_RDID);
    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                HISFC351_CMD_CONFIG_LOCK_FLASH(SPI_OPR_NOLOCK)
                | HISFC351_CMD_CONFIG_SEL_CS(cs)
                | HISFC351_CMD_CONFIG_RW_READ
                | HISFC351_CMD_CONFIG_DATA_EN
                | HISFC351_CMD_CONFIG_DATA_CNT(numread)
                | HISFC351_CMD_CONFIG_START);
    sfc_core_wait_cpu_finish(host);

    while (numread) {
        *ptr = hisfc_read(host, SFC_CMD_DATABUF_0_OFFSET_ADDR + regindex);
        ptr += 1;
        regindex += BUF_READ_OFF;
        numread -= BUF_READ_OFF;
    }

    memcpy_s(&host->flash_id[cs][0], SFC_MAX_ID_LEN, buffer, SFC_MAX_ID_LEN);
    return buffer;
}

s32 sfc_core_iftype_check(u32 iftype, u32 is_write)
{
    u32 i;
    const s32 iftype_read[] = {
        SPI_IF_READ_STD,
        SPI_IF_READ_FAST,
        SPI_IF_READ_DUAL,
        SPI_IF_READ_DUAL_ADDR,
        SPI_IF_READ_QUAD,
        SPI_IF_READ_QUAD_ADDR,
        0,
    };

    const s32 iftype_write[] = {
        SPI_IF_WRITE_STD,
        SPI_IF_WRITE_DUAL,
        SPI_IF_WRITE_DUAL_ADDR,
        SPI_IF_WRITE_QUAD,
        SPI_IF_WRITE_QUAD_ADDR,
        0,
    };

    if (is_write) {
        for (i = 0; iftype_write[i]; i++) {
            if (iftype == iftype_write[i]) {
                return 0;
            }
        }
    } else {
        for (i = 0; iftype_read[i]; i++) {
            if (iftype == iftype_read[i]) {
                return 0;
            }
        }
    }

    sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_iftype_check failed");

    return -EINVAL;
}

void sfc_core_update_iftype_and_clock(hisfc_spi *spi, u32 is_write)
{
    s32 ix;
    s32 iftype_read[] = {
        SPI_IF_READ_STD, HISFC350_IFCYCLE_STD, SPI_IF_READ_FAST, HISFC350_IFCYCLE_STD,
        SPI_IF_READ_DUAL, HISFC350_IFCYCLE_DUAL, SPI_IF_READ_DUAL_ADDR, HISFC350_IFCYCLE_DUAL_ADDR,
        SPI_IF_READ_QUAD, HISFC350_IFCYCLE_QUAD, SPI_IF_READ_QUAD_ADDR, HISFC350_IFCYCLE_QUAD_ADDR,
        0, 0,
    };
    s32 iftype_write[] = {
        SPI_IF_WRITE_STD, HISFC350_IFCYCLE_STD, SPI_IF_WRITE_DUAL, HISFC350_IFCYCLE_DUAL,
        SPI_IF_WRITE_DUAL_ADDR, HISFC350_IFCYCLE_DUAL_ADDR, SPI_IF_WRITE_QUAD, HISFC350_IFCYCLE_QUAD,
        SPI_IF_WRITE_QUAD_ADDR, HISFC350_IFCYCLE_QUAD_ADDR, 0, 0,
    };

    if (is_write) {
        for (ix = 0; iftype_write[ix]; ix += 2) { // offset 2
            if (spi->write->iftype == iftype_write[ix]) {
                spi->write->iftype = (u8)iftype_write[ix + 1];
                break;
            }
        }
        hisfc351_get_best_clock(&spi->write->clock);
    } else {
        for (ix = 0; iftype_read[ix]; ix += 2) { // offset 2
            if (spi->read->iftype == iftype_read[ix]) {
                spi->read->iftype = (u8)iftype_read[ix + 1];
                break;
            }
        }
        hisfc351_get_best_clock(&spi->read->clock);
    }

    return;
}
#define SFC_QUAD_TEST
#ifdef SFC_QUAD_TEST
static s32 sfc_core_macronix_quad_enable(hisfc_spi *spi, s32 enable)
{
    s32 ret;
    u32 val = 0;

    ret = sfc_core_read_reg_by_cmd(spi, SPI_CMD_RDSR, 0, &val);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_read_reg_by_cmd error %d", ret);
        return ret;
    }

    sfc_log(LOG_ERROR_F, SFC_CORE_C, "sta %u", val);
    (void)spi->driver->write_enable(spi);
    val = (enable) ? (val | SR_QUAD_EN_MX) : (val & (~SR_QUAD_EN_MX));
    ret = sfc_core_write_reg_by_cmd(spi, SPI_CMD_MT25_WRSR, 0, 1, val);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_write_reg_by_cmd failed");
        return ret;
    }

    ret = spi->driver->wait_ready(spi, 0);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "Macronix wait_ready failed");
        return ret;
    }
    val = 0;
    ret = sfc_core_read_reg_by_cmd(spi, SPI_CMD_RDSR, 0, &val);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "Macronix Quad bit not set ret %d", ret);
        return -EINVAL;
    }

    if (enable) {
        if (!(val & SR_QUAD_EN_MX)) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "Macronix Quad bit not set val %u", val);
            return -EINVAL;
        }
    } else {
        if (val & SR_QUAD_EN_MX) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "Macronix Quad bit not clr val %u", val);
            return -EINVAL;
        }
    }

    return 0;
}

static s32 sfc_core_micron_quad_enable(hisfc_spi *spi, s32 enable)
{
    s32 ret;
    u32 val;

    ret = sfc_core_read_reg_by_cmd(spi, SPINOR_OP_RD_EVCR, 0, &val);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "error %d reading EVCR", ret);
        return ret;
    }

    (void)spi->driver->write_enable(spi);
    /* set EVCR, enable quad I/O */
    val = (enable) ? (val & ~EVCR_QUAD_EN_MICRON) : (val | EVCR_QUAD_EN_MICRON);
    ret = sfc_core_write_reg_by_cmd(spi, SPINOR_OP_WD_EVCR, 0, 1, val);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "error while writing EVCR register");
        return ret;
    }

    ret = spi->driver->wait_ready(spi, 0);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "micronix wait_ready failed");
        return ret;
    }

    val = 0;
    /* read EVCR and check it */
    ret = sfc_core_read_reg_by_cmd(spi, SPINOR_OP_RD_EVCR, 0, &val);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "error %d reading EVCR", ret);
        return ret;
    }

    if (enable) {
        if (val & EVCR_QUAD_EN_MICRON) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "Micron EVCR Quad bit not clear");
            return -EINVAL;
        }
    } else {
        if (!(val & EVCR_QUAD_EN_MICRON)) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "Micron EVCR Quad bit not set");
            return -EINVAL;
        }
    }

    return 0;
}

static s32 sfc_core_set_quad_mode(hisfc_spi *spi, s32 enable, struct spi_info *spiinfo)
{
    s32 ret = 0;

    switch (jedec_mfr(spiinfo)) {
        case CFI_MFR_MACRONIX:
            ret = sfc_core_macronix_quad_enable(spi, enable);
            if (ret) {
                sfc_log(LOG_ERROR_F, SFC_CORE_C, "Macronix quad-read not enabled %d", ret);
                return -EINVAL;
            }
            break;
        case CFI_MFR_MICRON:
            ret = sfc_core_micron_quad_enable(spi, enable);
            if (ret) {
                sfc_log(LOG_ERROR_F, SFC_CORE_C, "Micron quad-read not enabled");
                return -EINVAL;
            }
            break;
        default:
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "Spansion or other quad-read not enabled");
            return -EINVAL;
    }

    return ret;
}
#endif

s32 sfc_core_spi_iftype_update(hisfc_core_host *host, u32 chipselect, u32 is_write, u32 iftype)
{
    u8 ids[SFC_MAX_ID_LEN] = {0};
    u8 *pids = &ids[0];
    struct spi_info *spiinfo = NULL;
    hisfc_spi *spi = NULL;
    struct spi_operation temp[1] = { 0 };
    s32 ret;
    u8 sup_quad_write;
    u8 sup_quad_read;
    s32 en_quad;

    spi = &(host->spi[chipselect]);

    (void)sfc_core_read_ids(host, chipselect, pids);
    spiinfo = sfc_spi_serach_ids(g_hisfc_spi_info_table, pids);
    if (!spiinfo) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "Spi(cs%u): find unrecognized spi flash.", chipselect);
        return -EINVAL;
    }

    sfc_spi_search_rw(spiinfo, temp, iftype, HISFC_SUPPORT_MAX_DUMMY,
                      (is_write ? (SFC_SERACH_WRITE) : (SFC_SERACH_READ)));
    if (!(temp[0].iftype & iftype)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "Spi(cs%u): iftype %u not support", chipselect, iftype);
        return -EINVAL;
    }

    if (is_write) {
        (void)memcpy_s(spi->write, sizeof(struct spi_operation), temp, sizeof(struct spi_operation));
    } else {
        (void)memcpy_s(spi->read, sizeof(struct spi_operation), temp, sizeof(struct spi_operation));
    }
    sfc_log(LOG_ERROR_F, SFC_CORE_C, "Spi(cs%u): switch rw %u in iftype %u, out iftype %u",
        chipselect, is_write, iftype, temp[0].iftype);
    sfc_core_update_iftype_and_clock(spi, is_write);
#ifdef SFC_QUAD_TEST
    sup_quad_write = is_write && ((temp[0].iftype & SPI_IF_WRITE_QUAD) || (temp[0].iftype & SPI_IF_WRITE_QUAD_ADDR));
    sup_quad_read = !is_write && ((temp[0].iftype & SPI_IF_READ_QUAD) || (temp[0].iftype & SPI_IF_READ_QUAD_ADDR));
    sfc_log(LOG_ERROR_F, SFC_CORE_C, "cs%u:is_write%u, iftype%u quad disable", chipselect, is_write, temp[0].iftype);
    en_quad = (sup_quad_write || sup_quad_read) ? 1 : 0;
    ret = sfc_core_set_quad_mode(spi, en_quad, spiinfo);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc_core_micron_quad_enable err");
        return ret;
    }
#endif
    sfc_log(LOG_ERROR_F, SFC_CORE_C, "read iftype:%u,cmd %u, write:iftype %u,cmd %u",
            spi->read->iftype, spi->read->cmd, spi->write->iftype, spi->write->cmd);
    sfc_log(LOG_ERROR_F, SFC_CORE_C, "dummy %u: dummy write %u", spi->read[0].dummy,
            spi->write[0].dummy);

    return ret;
}

static void sfc_core_map_iftype_and_clock(hisfc_spi *spi)
{
    sfc_core_update_iftype_and_clock(spi, 1);
    sfc_core_update_iftype_and_clock(spi, 0);

    hisfc351_get_best_clock(&spi->erase->clock);
    spi->erase->iftype = HISFC350_IFCYCLE_STD;

    return;
}

u32 sfc_core_map_chipsize(unsigned long long chipsize)
{
    u32 shift = 0;
    unsigned long long size = chipsize;

    size >>= (19 - 3); // 19: 512kb; 3: bytes->bit, is equal to div 512kbits
    while (size) {
        size >>= 1;
        shift++;
    }

    return shift;
}

static s32 sfc_core_controller_spi_init(const hisfc_spi *spi, s32 offset, u32 bus)
{
    u32 regval;
    hisfc_core_host *host = (hisfc_core_host *)spi->host;

    regval = hisfc_read(host, SFC_BUS_FLASH_SIZE_OFFSET_ADDR);
    /* cs bit field indication 3 */
    regval &= ~((u32)HISFC350_BUS_FLASH_SIZE_CS0_MASK << ((u32)spi->chipselect << 3));
    /* cs bit field indication 3 */
    regval |= (sfc_core_map_chipsize(spi->chipsize) << ((u32)spi->chipselect << 3));
    hisfc_write(host, SFC_BUS_FLASH_SIZE_OFFSET_ADDR, regval);
    /* cs bit field indication 2 */
    hisfc_write(host, (SFC_BUS_BASE_ADDR_CS0_OFFSET_ADDR + ((u32)spi->chipselect << 2)),
                (SFC0_CS0_MEM_ADDR + bus * (SFC1_CS0_MEM_ADDR - SFC0_CS0_MEM_ADDR) + offset));

    return 0;
}

#ifdef CONFIG_HISFC351_SHOW_CYCLE_TIMING
static s8 *sfc_core_get_ifcycle_str(s32 ifcycle)
{
    static s8 *ifcycle_str[] = {
        "single",
        "dual",
        "dual-addr",
        "dual-cmd",
        "reserve",
        "quad",
        "quad-addr",
        "quad-cmd",
    };

    return ifcycle_str[(ifcycle & 0x07)];
}
#endif

static void sfc_core_show_spi(const hisfc_spi *spi)
{
    sfc_log(LOG_PRINT_F, SFC_CORE_C, "Spi(cs %u): ", spi->chipselect);
    sfc_log(LOG_PRINT_F, SFC_CORE_C, "Name:\"%s\"", spi->name);

#ifdef CONFIG_HISFC351_SHOW_CYCLE_TIMING
    if (spi->addrcycle == SFC_4BYTE_ADDR) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "4 addrcycle ");
    }

    sfc_log(LOG_ERROR_F, SFC_CORE_C, "read:%s,%02X,%s ",
            sfc_core_get_ifcycle_str(spi->read->iftype),
            spi->read->cmd,
            hisfc351_get_clock_str(spi->read->clock));
    sfc_log(LOG_ERROR_F, SFC_CORE_C, "write:%s,%02X,%s ",
            sfc_core_get_ifcycle_str(spi->write->iftype),
            spi->write->cmd,
            hisfc351_get_clock_str(spi->write->clock));
    sfc_log(LOG_ERROR_F, SFC_CORE_C, "erase:%s,%02X,%s",
            sfc_core_get_ifcycle_str(spi->erase->iftype),
            spi->erase->cmd,
            hisfc351_get_clock_str(spi->erase->clock));
#endif /* CONFIG_HISFC351_SHOW_CYCLE_TIMING */

    return;
}

void sfc_core_timing_cfg(const hisfc_core_host *c_host)
{
    hisfc_write(c_host, SFC_TIMING_OFFSET_ADDR,
                HISFC350_TIMING_TCSS(0x6)
                | HISFC350_TIMING_TCSH(0x6)
                | HISFC350_TIMING_TSHSL(0xf));

    return;
}

void sfc_core_suspend_shutdown_com_process(hisfc_core_host *c_host, u32 is_suspend)
{
    u32 ix;
    hisfc_spi *spi = NULL;

    if (!c_host) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "shutdown sus drv_host is bull is_suspend %u", is_suspend);
        return;
    }

    if (sfc_core_bus_is_used(c_host->bus, c_host->cfg_use_cs)) {
        for (ix = 0; ix < SFC_CS_CNT; ix++) {
            if (sfc_core_cs_is_used(c_host->bus, ix, c_host->cfg_use_cs) == 0) {
                return;
            }
            spi = &c_host->spi[ix];
            if (spi == NULL) {
                sfc_log(LOG_ERROR_F, SFC_CORE_C, "shutdown spi is NULL");
                continue;
            }
            if (spi->driver == NULL) {
                sfc_log(LOG_ERROR_F, SFC_CORE_C, "shutdown spi->driver is NULL,may be not match");
                continue;
            }
            (void)spi->driver->wait_ready(spi, SPI_OPR_NOLOCK);
            (void)spi->driver->entry_4addr(spi, SFC_FALSE);
            if (is_suspend) {
                c_host->set_system_clock(c_host, spi->read, SFC_FALSE);
            }
        }
    }

    return;
}

s32 sfc_core_resume(hisfc_core_host *c_host)
{
    s32 ix;
    hisfc_spi *spi = NULL;
    unsigned long regval;

    if (c_host == NULL) {
        return -EFAULT;
    }

    spi = c_host->spi;
    hisfc351_set_system_clock(c_host, NULL, SFC_TRUE);
    sfc_core_timing_cfg(c_host);

    for (ix = 0; ix < SFC_CS_CNT; spi++, ix++) {
        if (spi == NULL) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "resume spi is NULL");
            break;
        }
        if (!sfc_core_cs_is_used(c_host->bus, ix, c_host->cfg_use_cs)) {
            continue;
        }
        if (spi->driver == NULL) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "resume spi->driver is NULL,may be not match");
            continue;
        }

        regval = hisfc_read(c_host, SFC_BUS_FLASH_SIZE_OFFSET_ADDR);
        /* bit off 3 */
        regval &= ~((u32)HISFC350_BUS_FLASH_SIZE_CS0_MASK << ((u32)spi->chipselect << 3));
        /* bit off 3 */
        regval |= (sfc_core_map_chipsize(spi->chipsize) << ((u32)spi->chipselect << 3));
        hisfc_write(c_host, SFC_BUS_FLASH_SIZE_OFFSET_ADDR, regval);
        /* bit off 2 */
        hisfc_write(c_host, (SFC_BUS_BASE_ADDR_CS0_OFFSET_ADDR + ((u32)spi->chipselect << 2)),
                    (SFC1_CS0_MEM_ADDR + (unsigned long)((s8 *)spi->iobase - (s8 *)(&c_host->iobase))));
        if (spi->addrcycle == SFC_4BYTE_ADDR) {
            (void)spi->driver->entry_4addr(spi, SFC_TRUE);
        }
    }

    return 0;
}

void sfc_core_set_host_addr_mode(hisfc_core_host *host_s, s32 enable)
{
    u32 regval;

    regval = hisfc_read(host_s, SFC_GLOBAL_CONFIG_OFFSET_ADDR);
    if (enable) {
        regval |= HISFC350_GLOBAL_CONFIG_ADDR_MODE_4B;
    } else {
        regval &= ~HISFC350_GLOBAL_CONFIG_ADDR_MODE_4B;
    }

    hisfc_write(host_s, SFC_GLOBAL_CONFIG_OFFSET_ADDR, regval);
}

s32 sfc_core_com_para_check(sfc_com_info *com_para)
{
    s32 ret = 0;
    if ((com_para->addr_cycle != SFC_3BYTE_ADDR) && (com_para->addr_cycle != SFC_4BYTE_ADDR)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc com para err addr mode = %u", com_para->addr_cycle);
        return -EINVAL;
    }

    if ((com_para->bus >= SFC_CTRL_CNT) || (com_para->cs >= SFC_CS_CNT)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc com para err bus = %u, cs = %u", com_para->bus, com_para->cs);
        ret = -EINVAL;
    }

    return ret;
}

u32 sfc_core_cs_is_used(u32 bus, u32 cs, u32 dts_cfg)
{
    u32 use_mask;

    if ((bus >= SFC_CTRL_CNT) || (cs >= SFC_CS_CNT)) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "sfc com para err bus = %u, cs = %u", bus, cs);
        return 0;
    }

    use_mask = (1UL << cs);
    if ((dts_cfg & use_mask) == use_mask) {
        return 1;
    }

    return 0;
}

u32 sfc_core_bus_is_used(u32 bus, u32 dts_cfg)
{
    return (u32)(sfc_core_cs_is_used(bus, 0, dts_cfg) || sfc_core_cs_is_used(bus, 1, dts_cfg));
}

void sfc_core_addr_mode_set(hisfc_spi *spi)
{
    if ((spi == NULL) || (spi->driver == NULL)) {
        return;
    }

    if (spi->addrcycle == SFC_4BYTE_ADDR) {
        (void)spi->driver->entry_4addr(spi, SFC_TRUE);
    } else {
        (void)spi->driver->entry_4addr(spi, SFC_FALSE);
    }

    return;
}

void sfc_core_bus_write_enable(const hisfc_core_host *host, u32 enable)
{
    u32 regval;

    regval = hisfc_read(host, SFC_BUS_CONFIG1_OFFSET_ADDR);
    if (enable) {
        regval |= HISFC350_BUS_CONFIG1_WRITE_EN;
    } else {
        regval &= (~HISFC350_BUS_CONFIG1_WRITE_EN);
    }
    hisfc_write(host, SFC_BUS_CONFIG1_OFFSET_ADDR, regval);

    return;
}

u32 sfc_core_is_wpen(const hisfc_core_host *host)
{
    u32 regval;

    regval = hisfc_read(host, SFC_GLOBAL_CONFIG_OFFSET_ADDR);

    return (regval & SFC_WP_EN);
}

s32 sfc_core_spec_cfg_in_probe(hisfc_spi *spi)
{
    s32 ret = 0;

    /* The parameter has been guaranteed by the upper layer. */
    if (spi->driver->init_flash_mem) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "Spi(cs%u) init flash mem array", spi->chipselect);
        ret = spi->driver->init_flash_mem(spi, SPI_OPR_NOLOCK);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "Spi(cs%u): spi_flash_init_flash_mem failed.", spi->chipselect);
            return ret;
        }
    }

    /* Default configuration for spansion reset. */
    if (spi->driver->spec_cfg) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "Spi(cs%u) spec_cfg", spi->chipselect);
        ret = spi->driver->spec_cfg(spi, SPI_OPR_NOLOCK);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "Spi(cs%u): spec_cfg failed.", spi->chipselect);
            return ret;
        }
    }

    return ret;
}

static void sfc_core_base_spi_info_copy(hisfc_spi *spi_dest, struct spi_info *spi_src, u32 cur_cs)
{
    spi_dest->name = spi_src->name;
    spi_dest->chipselect = cur_cs;
    spi_dest->mtd_device_regis = 0;
    spi_dest->chipsize = spi_src->chipsize;
    spi_dest->erasesize = spi_src->erasesize;
    spi_dest->addrcycle = spi_src->addrcycle;
    spi_dest->original_addrcycle = spi_src->addrcycle;
    spi_dest->driver = spi_src->driver;

    return;
}

static s32 sfc_core_entry_4addr(hisfc_spi *spi)
{
    s32 ret;

    if ((spi == NULL) || (spi->driver == NULL)) {
        return -EINVAL;
    }

    if (spi->addrcycle == SFC_4BYTE_ADDR) {
        ret = spi->driver->entry_4addr(spi, SFC_TRUE);
    } else {
        ret = spi->driver->entry_4addr(spi, SFC_FALSE);
    }

    return ret;
}

static void sfc_core_bus_read_prepare(hisfc_spi *spi)
{
    s32 ret;

    if (!spi || !spi->driver) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "input err, spi or spi driver is NULL");
        return;
    }

    if (sfc_core_entry_4addr(spi)) {
        sfc_log(LOG_WARN_F, SFC_CORE_C, "sfc_core_entry_4addr failed");
    }
    ret = spi->driver->wait_ready(spi, SPI_OPR_NOLOCK);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_CORE_C, "SPIFlash is WIP!");
        return;
    }

    (void)spi->driver->bus_prepare(spi, SFC_SERACH_READ);

    return;
}

s32 sfc_core_spi_probe(hisfc_core_host *host)
{
    s32 ret;
    u32 total;
    u8 ids[SFC_MAX_ID_LEN];
    u8 *pids = NULL;
    struct spi_info *spiinfo = NULL;
    hisfc_spi *spi[SFC_CS_CNT];
    u32 cs;

    pids = &ids[0];
    host->num_chip = 0;

    for (cs = 0; cs < SFC_CS_CNT; cs++) {
        spi[cs] = &(host->spi[cs]);
        if (!sfc_core_cs_is_used(host->bus, cs, host->cfg_use_cs)) {
            continue;
        }
        (void)sfc_core_read_ids(host, cs, pids);

        /* can't find spi flash device */
        sfc_log(LOG_PRINT_F, SFC_CORE_C, "Spi(cs%u) ID: 0x%08x 0x%08x", cs,
                BYTES_TO_U32(ids[3], ids[2], ids[1], ids[0]),  /* id index 0 1 2 3 */
                BYTES_TO_U32(ids[7], ids[6], ids[5], ids[4])); /* id index 4 5 6 7 */
        /* id index 0 1 2 */
        if (!(ids[0] | ids[1] | ids[2]) || ((ids[0] & ids[1] & ids[2]) == 0xFF)) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "Spi(cs%u): RDID not right!", cs);
            continue;
        }

        spiinfo = sfc_spi_serach_ids(g_hisfc_spi_info_table, pids);
        if (!spiinfo) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "Spi(cs%u): find unrecognized spi flash.", cs);
            continue;
        }

        sfc_core_base_spi_info_copy(spi[cs], spiinfo, cs);
        spi[cs]->host = host;
        sfc_spi_search_rw(spiinfo, spi[cs]->read, HISFC351_SUPPORT_READ, HISFC_SUPPORT_MAX_DUMMY, SFC_SERACH_READ);
        sfc_spi_search_rw(spiinfo, spi[cs]->write, HISFC351_SUPPORT_WRITE, HISFC_SUPPORT_MAX_DUMMY, SFC_SERACH_WRITE);
        sfc_spi_get_erase(spiinfo, spi[cs]->erase);
        sfc_core_map_iftype_and_clock(spi[cs]);

        total = (cs == 1) ? SFC_CS_SIZE : 0;
        spi[cs]->iobase = (s8 *)host->iobase + total;
        (void)sfc_core_controller_spi_init(spi[cs], (s32)total, host->bus);
        sfc_core_bus_read_prepare(spi[cs]);
        ret = sfc_core_spec_cfg_in_probe(spi[cs]);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_CORE_C, "Spi(cs%u): sfc_core_spec_cfg_in_probe failed.", cs);
            continue;
        }

        sfc_core_show_spi(spi[cs]);
        host->num_chip++;
    }

    return host->num_chip;
}
