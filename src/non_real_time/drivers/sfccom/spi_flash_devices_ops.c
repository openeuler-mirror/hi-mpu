// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * flash diff opfunc
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "sfc_osadp.h"
#include "sfc_addr_define.h"
#include "sfc0_core.h"

#if sfc_section("spi general devices callback")

s32 sfc_spi_general_wait_ready(hisfc_spi *spi, u8 is_lock)
{
    unsigned long regval;

#ifndef SCM3
    unsigned long deadline = jiffies + HISFC350_MAX_READY_WAIT_JIFFIES;
#else
    u32 cnt = 4000000;
#endif
    hisfc_core_host *host = (hisfc_core_host *)spi->host;

    do {
        hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, SPI_CMD_RDSR);
        hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                    HISFC351_CMD_CONFIG_LOCK_FLASH(is_lock)
                    | HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect)
                    | HISFC351_CMD_CONFIG_DATA_EN
                    | HISFC351_CMD_CONFIG_RW_READ
                    | HISFC351_CMD_CONFIG_START);
        sfc_core_wait_cpu_finish(host);

        regval = hisfc_read(host, SFC_CMD_DATABUF_0_OFFSET_ADDR);
        if (!(regval & SPI_CMD_SR_WIP)) {
            return 0;
        }

        udelay(1);
#ifndef SCM3
    } while (!time_after_eq(jiffies, deadline));
#else
        cnt--;
    } while (cnt);
#endif

    sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "Wait spi flash ready timeout");

    return -1;
}

s32 sfc_spi_general_write_enable(hisfc_spi *spi)
{
    u32 regval;
    hisfc_core_host *host = (hisfc_core_host *)spi->host;

    hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, SPI_CMD_WREN);
    regval = HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect) | HISFC351_CMD_CONFIG_START;
    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR, regval);
    sfc_core_wait_cpu_finish(host);

    return 0;
}

/*
 * To switch the 4-byte address mode, switch the chip first, and then switch the flash memory.
 * If the flash configuration table is not supported, 0 is returned.
 */
static s32 sfc_spi_general_enex_4addr(hisfc_spi *spi, s32 enable, u32 en4b_cmd, u32 ex4b_cmd)
{
    hisfc_core_host *host = (hisfc_core_host *)spi->host;

    if (spi->addrcycle != SFC_4BYTE_ADDR) {
        host->set_host_addr_mode(host, SFC_4BYTE_DISABLE);
        return 0;
    }

    host->set_host_addr_mode(host, enable);

    if (enable) {
        hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, en4b_cmd);
    } else {
        hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, ex4b_cmd);
    }

    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect) | HISFC351_CMD_CONFIG_START);
    sfc_core_wait_cpu_finish(host);

    return 0;
}

s32 sfc_spi_general_entry_4addr(hisfc_spi *spi, s32 enable)
{
    return sfc_spi_general_enex_4addr(spi, enable, SPI_CMD_EN4B, SPI_CMD_EX4B);
}

s32 sfc_spi_issi_entry_4addr(hisfc_spi *spi, s32 enable)
{
    return sfc_spi_general_enex_4addr(spi, enable, SPI_CMD_EN4B, SPI_ISSI_CMD_EX4B);
}

s32 sfc_spi_general_bus_prepare(hisfc_spi *spi, s32 op)
{
    u32 regval = 0;
    u32 rw_cmd;
    hisfc_core_host *host = (hisfc_core_host *)spi->host;

    regval |= HISFC350_BUS_CONFIG1_WRITE_INS(spi->write->cmd);
    regval |= HISFC350_BUS_CONFIG1_WRITE_DUMMY_CNT(spi->write->dummy);
    regval |= HISFC350_BUS_CONFIG1_WRITE_IF_TYPE(spi->write->iftype);
    regval |= HISFC350_BUS_CONFIG1_READ_PREF_CNT(0);
    regval |= HISFC350_BUS_CONFIG1_READ_INS(spi->read->cmd);
    regval |= HISFC350_BUS_CONFIG1_READ_DUMMY_CNT(spi->read->dummy);
    regval |= HISFC350_BUS_CONFIG1_READ_IF_TYPE(spi->read->iftype);

    regval |= (0x3U << 30); /* mask 0x3 off 30 */
    hisfc_write(host, SFC_BUS_CONFIG1_OFFSET_ADDR, regval);

    rw_cmd = (u32)((spi->write->cmd << 8) | (spi->read->cmd)); /* cmd off 8 */
    hisfc_write(host, SFC_BUS_CONFIG3_OFFSET_ADDR, rw_cmd);

    if (op == SFC_SERACH_READ) {
        host->set_system_clock(host, spi->read, SFC_TRUE);
    } else if (op == SFC_SERACH_WRITE) {
        host->set_system_clock(host, spi->write, SFC_TRUE);
    }

    return 0;
}
#endif

#if sfc_section("S25FL256S S70FS01GS")
/* for s25fl256s s70fs01gs callback */
#define SPI_BRWR 0x17
#define SPI_EN4B 0x80
#define SPI_EX4B 0x00

s32 sfc_spi_s25fl256s_entry_4addr(hisfc_spi *spi, s32 enable)
{
    hisfc_core_host *host = (hisfc_core_host *)spi->host;

    if (spi->addrcycle != SFC_4BYTE_ADDR) {
        host->set_host_addr_mode(host, SFC_4BYTE_DISABLE);
        return 0;
    }

    if (enable) {
        hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, SPI_BRWR);
        hisfc_write(host, SFC_CMD_DATABUF_0_OFFSET_ADDR, SPI_EN4B);
    } else {
        hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, SPI_BRWR);
        hisfc_write(host, SFC_CMD_DATABUF_0_OFFSET_ADDR, SPI_EX4B);
    }

    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                HISFC351_CMD_CONFIG_LOCK_FLASH(SPI_OPR_NOLOCK)
                | HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect)
                | HISFC351_CMD_CONFIG_DATA_EN
                | HISFC351_CMD_CONFIG_START);

    sfc_core_wait_cpu_finish(host);

    host->set_host_addr_mode(host, enable);

    return 0;
}

/*
 * satrt spansion s70fs01gs, read status register using RDAR
 * The following register access commands are not supported in the FS01GS: WRR(01h), RDCR(35h), RDSR1(05h), RDSR2(07h),
 * PNVDLR (43h). The RDAR (65h) and WRAR (71h) commands must be used for reading and writing the registers so that the
 * command address directs the operation to the lower or upper FS512S device in the DDP.
 */
s32 sfc_spi_fs01gs_wait_ready_by_addr(hisfc_spi *spi, unsigned long addr, u8 is_lock)
{
    s32 ret;
    unsigned long deadline = jiffies + HISFC350_MAX_READY_WAIT_JIFFIES;
    u8 sr1v = 0;

    do {
        ret = sfc_core_read_any_reg(spi, spansion_sr1v_reg_addr(addr), spi->read->iftype, &sr1v, is_lock);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "read_any_reg spansion_sr1v_reg_addr failed");
            return ret;
        }

        if (!(sr1v & SPI_CMD_SR_WIP)) {
            return 0;
        }

        udelay(1);
    } while (!time_after_eq(jiffies, deadline));

    sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "Wait spi flash ready addr 0x%lx timeout", addr);

    return -1;
}

s32 sfc_spi_fs01gs_wait_ready(hisfc_spi *spi, u8 is_lock)
{
    s32 ret;
    ret = sfc_spi_fs01gs_wait_ready_by_addr(spi, SPANSION_S70FS01GS_FIRST_HALF_ADDR, is_lock);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "sfc_spi_fs01gs_wait_ready_by_addr first half failed");
        return ret;
    }
    ret = sfc_spi_fs01gs_wait_ready_by_addr(spi, SPANSION_S70FS01GS_SECOND_HALF_ADDR, is_lock);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "sfc_spi_fs01gs_wait_ready_by_addr second half failed");
        return ret;
    }

    return ret;
}

s32 sfc_spi_fs01gs_entry_4addr(hisfc_spi *spi, s32 enable)
{
    s32 ret;
    hisfc_core_host *host = (hisfc_core_host *)spi->host;

    if (spi->addrcycle != SFC_4BYTE_ADDR) {
        host->set_host_addr_mode(host, SFC_4BYTE_DISABLE);
        return 0;
    }

    if (enable) {
        (void)spi->driver->write_enable(spi);
        hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, SPI_CMD_EN4B);
    } else {
        /* 0xe9 not support */
        ret = sfc_core_modify_any_reg(spi, spansion_cr2v_reg_addr(SPANSION_S70FS01GS_FIRST_HALF_ADDR), 0x80,
                                      0, SPI_OPR_LOCK);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "first modify_any_reg failed(%d)", ret);
            return ret;
        }
        ret = sfc_core_modify_any_reg(spi, spansion_cr2v_reg_addr(SPANSION_S70FS01GS_SECOND_HALF_ADDR),
                                      0x80, 0, SPI_OPR_LOCK);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "second modify_any_reg failed(%d)", ret);
            return ret;
        }
    }

    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                HISFC351_CMD_CONFIG_LOCK_FLASH(SPI_OPR_LOCK)
                | HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect)
                | HISFC351_CMD_CONFIG_START);

    sfc_core_wait_cpu_finish(host);
    (void)spi->driver->spi_opr_finish_call(spi);
    host->set_host_addr_mode(host, enable);

    return 0;
}

s32 sfc_spi_fs512s_entry_4addr(hisfc_spi *spi, s32 enable)
{
    hisfc_core_host *host = (hisfc_core_host *)spi->host;
    s32 ret;

    if (spi->addrcycle != SFC_4BYTE_ADDR) {
        host->set_host_addr_mode(host, SFC_4BYTE_DISABLE);
        return 0;
    }

    if (enable) {
        (void)spi->driver->write_enable(spi);
        hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, SPI_CMD_EN4B);
    } else {
        /* 0xe9 not support */
        ret = sfc_core_modify_any_reg(spi, spansion_cr2v_reg_addr(SPANSION_FS512S_FIRST_HALF_ADDR),
                                      0x80, 0, SPI_OPR_LOCK);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "first modify_any_reg failed(%d)", ret);
            return ret;
        }
    }

    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                HISFC351_CMD_CONFIG_LOCK_FLASH(SPI_OPR_LOCK) | HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect) |
                HISFC351_CMD_CONFIG_START);
    sfc_core_wait_cpu_finish(host);
    (void)spi->driver->spi_opr_finish_call(spi);
    host->set_host_addr_mode(host, enable);

    return 0;
}

#define SPANSION_RESET_ENABLE_MASK 0x20

s32 spi_flash_fsxxs_reset_cfg(hisfc_spi *spi, u8 is_lock, u32 reg_area)
{
    s32 ret;
    u8 reg_val = 0;
    u32 cr2v_addr = spansion_cr2v_reg_addr(reg_area);
    u32 cr2nv_addr = spansion_cr2nv_reg_addr(reg_area);

    if (!(spi && spi->driver && spi->driver->modify_any_reg &&
        spi->driver->write_any_reg && spi->driver->read_any_reg)) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "spi_flash_fs512s_spec_cfg para is NULL");
        return -1;
    }

    ret = spi->driver->read_any_reg(spi, cr2nv_addr, 0, &reg_val, is_lock);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "read cr2nv val failed(%d)", ret);
        return ret;
    }
    if (reg_val & SPANSION_RESET_ENABLE_MASK) {
        return 0;
    }

    reg_val |= SPANSION_RESET_ENABLE_MASK;
    ret = spi->driver->write_any_reg(spi, 0, 1, (u32)reg_val, cr2nv_addr);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "set reg addr%u val %u failed", cr2nv_addr, reg_val);
        return ret;
    }

    ret = spi->driver->modify_any_reg(spi, cr2v_addr, SPANSION_RESET_ENABLE_MASK,
                                      SPANSION_RESET_ENABLE_MASK, SPI_OPR_NOLOCK);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "modify_any_reg cr2v failed");
    }

    return ret;
}

s32 spi_flash_fs512s_spec_cfg(hisfc_spi *spi, u8 is_lock)
{
    return spi_flash_fsxxs_reset_cfg(spi, is_lock, 0);
}

s32 spi_flash_fs01gs_spec_cfg(hisfc_spi *spi, u8 is_lock)
{
    s32 ret;

    ret = spi_flash_fsxxs_reset_cfg(spi, is_lock, SPANSION_S70FS01GS_FIRST_HALF_ADDR);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "spi_flash_fsxxs_reset_cfg first half failed");
        return ret;
    }
    ret = spi_flash_fsxxs_reset_cfg(spi, is_lock, SPANSION_S70FS01GS_SECOND_HALF_ADDR);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "spi_flash_fsxxs_reset_cfg second half failed");
        return ret;
    }

    return ret;
}

#endif

#if sfc_section("MT25QUxxBB")
/* for MT25QUxxBB id0=0x20 callback */
static s32 sfc_spi_mt25qu_block_sta_pro(hisfc_spi *spi, u32 obj_pow)
{
    s32 ret;
    u32 status_old = 0;
    u32 status_new;
    u32 bp4val;

    /* Check whether the write function is enabled successfully. */
    ret = sfc_core_read_reg_by_cmd(spi, SPI_CMD_RDSR, HISFC350_IFCYCLE_STD, &status_old);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "read spi flash status register failed(%d)!", ret);
        return ret;
    }
    sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "status_old %u", status_old);

    if (!(MT_SR_TB & status_old)) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "warning : tb is modifyed");
    }

    bp4val = obj_pow + 1;
    /* bp bits clear */
    status_new = status_old & (~SPI_FLASH_MT25QU_BP_MASK);
    /* bp2-0 */
    status_new |= ((bp4val & 0x7) << 2UL);
    /* bp3 */
    status_new |= ((bp4val & 0x8) << 6UL);

    status_new |= MT_SR_TB;
    sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "status_new %u", status_new);

    /* Only modify protection if it will not unlock other areas */
    if ((status_new & (MT_SR_BP3 | MT_SR_BP2 | MT_SR_BP1 | MT_SR_BP0))
        > (status_old & (MT_SR_BP3 | MT_SR_BP2 | MT_SR_BP1 | MT_SR_BP0))) {
        ret = sfc_core_write_reg_by_cmd(spi, SPI_CMD_MT25_WRSR, HISFC350_IFCYCLE_STD, 1, status_new);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "mt block write_reg_by_cmd failed(%d)", ret);
            return ret;
        }
    }

    return ret;
}

static s32 sfc_spi_mt25qu_block_lock(hisfc_spi *spi, mode_block *pro_sec)
{
    s32 ret;
    u32 obj_pow = 0;
    u32 pow_i;
    u32 max_sec_pow = 0xf;
    u32 temp_sec_cnt;
    u32 max_sec;

    if ((!spi) || (!pro_sec)) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "sfc_spi_mt25qu_block_lock para is NULL");
        return -EINVAL;
    }

    max_sec = spi->chipsize / _64K;
    if (pro_sec->pro_sec_cnt > max_sec) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "pro_sec_cnt too long err cnt %u", pro_sec->pro_sec_cnt);
        return -EINVAL;
    }

    for (pow_i = 0; pow_i < max_sec_pow; pow_i++) {
        /* POW2 */
        temp_sec_cnt = (1UL << (pow_i));
        if (temp_sec_cnt == pro_sec->pro_sec_cnt) {
            obj_pow = pow_i;
            break;
        }
        if (temp_sec_cnt > pro_sec->pro_sec_cnt) {
            sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "pro_sec_cnt err cnt %u", pro_sec->pro_sec_cnt);
            return -EINVAL;
        }
    }

    ret = sfc_spi_mt25qu_block_sta_pro(spi, obj_pow);
    (void)spi->driver->spi_opr_finish_call(spi);

    return ret;
}

s32 sfc_spi_mt25quxxbb_all_chip_lock(hisfc_spi *spi, protect_para *p_protect)
{
    s32 ret;
    u32 status = 0;
    hisfc_core_host *host = NULL;

    host = (hisfc_core_host *)spi->host;
    if (!host) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "host is NULL");
        return -EINVAL;
    }

    ret = spi->driver->wait_ready(spi, SPI_OPR_LOCK);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "mt25quxxbb_protect_process failed(%d)", ret);
        goto RELEASE_LOCK;
    }

    /* Check whether the write function is enabled successfully. */
    ret = sfc_core_read_reg_by_cmd(spi, SPI_CMD_RDSR, HISFC350_IFCYCLE_STD, &status);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "read spi flash status register failed(%d)!", ret);
        goto RELEASE_LOCK;
    }
    sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "status:%u", status);
    if ((status & 0x80) != 0) {
        if (sfc_core_is_wpen(host)) {
            sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "wr sr disable");
        }
    }

    if (p_protect->enable) {
        status = 0xde;
    } else {
        status = (status & (~SPI_FLASH_MT25QU_BP_MASK)) & 0xFF; /* Clear protected areas */
        status = status | BIT(1);                             /* Write enable */
        status = status & 0x7f;
    }

    ret = sfc_core_write_reg_by_cmd(spi, SPI_CMD_MT25_WRSR, HISFC350_IFCYCLE_STD, 1, status);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "mt sfc_core_write_reg_by_cmd failed.(%d)", ret);
        goto RELEASE_LOCK;
    }
RELEASE_LOCK:
    (void)spi->driver->spi_opr_finish_call(spi);

    return ret;
}

s32 sfc_spi_mt25quxxbb_protect_process(hisfc_spi *spi, protect_para *p_protect)
{
    s32 ret;

    if ((!spi) || (!p_protect) || !(spi->host)) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "spi or protect or spi host para is NULL");
        return -EINVAL;
    }

    if (p_protect->pro_op_mode == SFC_PRO_MODE_ALL) {
        /* Writing the nv bit of the Wpen protection status register for full-block protection */
        ret = sfc_spi_mt25quxxbb_all_chip_lock(spi, p_protect);
    } else if (p_protect->pro_op_mode == SFC_PRO_MODE_SEC) {
        if (!p_protect->enable) {
            sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C,
                    "this mode %u only support block protect, unlcok please use mode 0", p_protect->pro_op_mode);
            return -EINVAL;
        }
        ret = sfc_spi_mt25qu_block_lock(spi, &p_protect->pro_config.block);
    } else {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "pro_op_mode %u not support", p_protect->pro_op_mode);
        return -EINVAL;
    }

    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_FLASH_DEVICES_OPS_C, "pro_op_mode %u opteartion err ,enable %u",
                p_protect->pro_op_mode, p_protect->enable);
        return -EINVAL;
    }

    return ret;
}
#endif

#if sfc_section("N25Q256A")
/* for N25Q256A callback */
s32 sfc_spi_n25q256a_entry_4addr(hisfc_spi *spi, s32 enable)
{
    hisfc_core_host *host = (hisfc_core_host *)spi->host;

    if (spi->addrcycle != SFC_4BYTE_ADDR) {
        host->set_host_addr_mode(host, SFC_4BYTE_DISABLE);
        return 0;
    }

    if (enable) {
        (void)spi->driver->write_enable(spi);
        hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, SPI_CMD_EN4B);
    } else {
        (void)spi->driver->write_enable(spi);
        hisfc_write(host, SFC_CMD_INS_OFFSET_ADDR, SPI_CMD_EX4B);
    }

    hisfc_write(host, SFC_CMD_CONFIG_OFFSET_ADDR,
                HISFC351_CMD_CONFIG_LOCK_FLASH(SPI_OPR_LOCK) | HISFC351_CMD_CONFIG_SEL_CS(spi->chipselect) |
                HISFC351_CMD_CONFIG_START);
    sfc_core_wait_cpu_finish(host);
    (void)spi->driver->spi_opr_finish_call(spi);
    host->set_host_addr_mode(host, enable);

    return 0;
}

#endif
