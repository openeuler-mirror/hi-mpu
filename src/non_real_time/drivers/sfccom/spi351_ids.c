// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * flash info parse
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include "sfc_osadp.h"
#include "sfc0_core.h"
#include "sfc_cfi.h"
#include "spi351_ids.h"

s8 *sfc351_ultohstr(unsigned long long size)
{
    s32 ix;
    s32 ret;
    static s8 buffer[BUFFER_MAX_LEN] = { 0 };
    s8 *fmt[] = { "%u", "%uK", "%uM", "%uG", "%uT", "%uT" };

    // up to 5 shifts
    for (ix = 0; (ix < 5) && !(size & 0x3FF) && size; ix++) {
        size = (size >> 10); // >> 10
    }
    ret = snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, fmt[ix], size);
    if (ret < 0) {
        sfc_log(LOG_PRINT_F, SFC_IDS_C, "sfc351_ultohstr sprintf failed");
    }

    return buffer;
}

static s32 sfc_spec_id_cmp(const u8 ids_tbl[SFC_MAX_ID_LEN], const u8 ids_media[SFC_MAX_ID_LEN], u32 id_tbl_len)
{
    if ((id_tbl_len == 1) && (ids_tbl[CFI_MFR_ADDR] == 0xff)) {
        return 0;
    }

    if ((ids_media[CFI_MFR_ADDR] == CFI_MFR_SPANSION) && (id_tbl_len == SPANSION_SPEC_ID_LEN)) {
        if ((ids_media[CFI_MFR_ADDR] == ids_tbl[CFI_MFR_ADDR]) &&
            (ids_media[CFI_PSA_ADDR_SPANSION] == ids_tbl[SPANSION_SPEC_ID_LEN - 1])) {
            return 0;
        }
    }

    return 1;
}

struct spi_info *sfc_spi_serach_ids(struct spi_info *info_table, const u8 ids[SFC_MAX_ID_LEN])
{
    struct spi_info *info = info_table;
    struct spi_info *fit_info = NULL;

    for (; info->name; info++) {
        if (memcmp(info->id, ids, info->id_len)) {
            /* Special processing for matching the default ID */
            if (sfc_spec_id_cmp(info->id, ids, info->id_len)) {
                continue;
            }
        }

        if (fit_info == NULL || fit_info->id_len < info->id_len) {
            fit_info = info;
        }
    }

    return fit_info;
}

void sfc_spi_search_rw(struct spi_info *spiinfo, struct spi_operation *spiop_rw, u32 iftype,
                       u32 max_dummy, s32 is_read)
{
    int ix = 0;
    struct spi_operation **spiop;
    struct spi_operation **fitspiop;

    for (fitspiop = spiop = (is_read ? spiinfo->read : spiinfo->write);
         (*spiop) && ix < MAX_SPI_OP; spiop++, ix++) {
        sfc_log(LOG_INFO_F, SFC_IDS_C, "dump[%d] isread(%d) iftype:%u", ix, is_read, (*spiop)->iftype);

        if (((*spiop)->iftype & iftype) && ((*spiop)->dummy <= max_dummy)
            && (*fitspiop)->iftype < (*spiop)->iftype) {
            fitspiop = spiop;
        }
    }
    sfc_log(LOG_PRINT_F, SFC_IDS_C, "dump[%d] isread(%d) iftype:%u", ix, is_read, (*fitspiop)->iftype);
    (void)memcpy_s(spiop_rw, sizeof(struct spi_operation), (*fitspiop), sizeof(struct spi_operation));

    return;
}

void sfc_spi_get_erase(const struct spi_info *spiinfo, struct spi_operation *spiop_erase)
{
    int ix;
    spiop_erase->size = 0;

    for (ix = 0; ix < MAX_SPI_ERASE_OP; ix++) {
        if (spiinfo->erase[ix] == NULL) {
            break;
        }
        if (spiinfo->erasesize == spiinfo->erase[ix]->size) {
            (void)memcpy_s(spiop_erase, sizeof(struct spi_operation), spiinfo->erase[ix], sizeof(struct spi_operation));
            break;
        }
    }
    if (!spiop_erase->size) {
        sfc_log(LOG_ERROR_F, SFC_IDS_C, "spi erasesize error!");
    }

    return;
}
