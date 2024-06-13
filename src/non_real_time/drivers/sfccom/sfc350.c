// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sfc linux driver
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#include <linux/of.h>
#include "sfc_osadp.h"
#include "sfc_api.h"
#include "sfc_sys_func.h"
#include "hwlock_com_api.h"
#include "sfc350.h"

static u32 g_clk_mhz = DEFAULT_SFC_CLK_MHZ;
module_param(g_clk_mhz, uint, 0600);
MODULE_PARM_DESC(g_clk_mhz, "sfc config clock frequency(max 25mhz)");

struct partition_info {
    s32 parts_num;
    struct partition_entry entry[MAX_MTD_PARTITIONS];
    struct mtd_partition parts[MAX_MTD_PARTITIONS];
};

/* host0 is sfc0, host1 is sfc1 */
static struct hisfc_host g_host[SFC_CTRL_CNT];
static struct partition_info g_ptn_info[SFC_TOTAL_CS];
static cdev_st sfc_dev;

u32 hisfc_get_config_clk_frequency(void)
{
    if (g_clk_mhz > MAX_SFC_CLK_MHZ) {
        sfc_log(LOG_PRINT_F, SFC_PLAT_C, "clock(%u) out of rang, config default val", g_clk_mhz);
        g_clk_mhz = DEFAULT_SFC_CLK_MHZ;
    }

    return g_clk_mhz;
}

static void hisfc_drv_fill_bus_info(struct mtd_info *mtd, sfc_bus_info *info)
{
    struct hisfc_host *host = (struct hisfc_host *)mtd->priv;

    if (mtd == &(host->mtd[1])) {
        info->cs = 1;
    } else {
        info->cs = 0;
    }

    info->bus = host->core_host->bus;

    return;
}

static s32 hisfc_drv_reg_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u8 *buf)
{
    sfc_bus_info info = {0};

    *retlen = 0;
    hisfc_drv_fill_bus_info(mtd, &info);

    return hisfc_reg_read(info, from, len, retlen, buf);
}

static s32 hisfc_drv_dma_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u8 *buf)
{
    sfc_bus_info info = {0};

    *retlen = 0;
    hisfc_drv_fill_bus_info(mtd, &info);

    return hisfc_dma_write(info, to, len, retlen, buf);
}

static s32 hisfc_drv_dma_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u8 *buf)
{
    sfc_bus_info info = {0};

    *retlen = 0;
    hisfc_drv_fill_bus_info(mtd, &info);

    return hisfc_dma_read(info, from, len, retlen, buf);
}

static s32 hisfc_drv_bus_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u8 *buf)
{
    sfc_bus_info info = {0};

    *retlen = 0;
    hisfc_drv_fill_bus_info(mtd, &info);

    return hisfc_bus_write(info, to, len, retlen, buf);
}

static s32 hisfc_drv_bus_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u8 *buf)
{
    sfc_bus_info info = {0};

    *retlen = 0;
    hisfc_drv_fill_bus_info(mtd, &info);

    return hisfc_bus_read(info, from, len, retlen, buf);
}

static s32 hisfc_drv_reg_erase(struct mtd_info *mtd, struct erase_info *instr)
{
    sfc_bus_info info = {0};

    hisfc_drv_fill_bus_info(mtd, &info);

    return hisfc_reg_erase(info, instr->addr, instr->len);
}

static void hisfc_drv_normal_parse(u32 idx, u64 partsize, s32 *nr_part)
{
    static const char normal_region[SFC_MAX_FLASH_NUM][SFC_REGION_NAME_SIZE] = {
        "sfc0cs0", "sfc0cs1", "sfc1cs0", "sfc1cs1"
    };

    g_ptn_info[idx].parts_num = 1;
    g_ptn_info[idx].parts[0].size = partsize;
    g_ptn_info[idx].parts[0].offset = 0;
    g_ptn_info[idx].parts[0].mask_flags = 0;
    g_ptn_info[idx].parts[0].name = normal_region[idx];

    *nr_part = g_ptn_info[idx].parts_num;
}

static bool hisfc_is_dtb_param_valid(u64 partsize, u32 offset, u32 size)
{
    if (partsize >= offset + size) {
        return true;
    }

    return false;
}

static s32 hisfc_drv_dtb_parse(u32 idx, u64 partsize, struct device_node *node, s32 *nr_part)
{
    u32 reg[2] = {0};
    struct device_node *child = NULL;
    s32 ret;
    u8 i = 0;

    while ((child = of_get_next_child(node, child)) != NULL) {
        ret = of_property_read_string(child, "label", &g_ptn_info[idx].parts[i].name);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_PLAT_C, "sfc parse property label failed");
            return -EFAULT;
        }

        ret = of_property_read_u32_array(child, "reg", reg, ARRAY_SIZE(reg));
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_PLAT_C, "sfc parse property reg failed");
            return -EFAULT;
        }

        if (!hisfc_is_dtb_param_valid(partsize, reg[0], reg[1])) {
            sfc_log(LOG_ERROR_F, SFC_PLAT_C, "invalid sfc boot dtb param:%u %u", reg[0], reg[1]);
            return -ENOMEM;
        }

        g_ptn_info[idx].parts[i].offset = reg[0];
        g_ptn_info[idx].parts[i].size = reg[1];
        g_ptn_info[idx].parts[i].mask_flags = 0;
        i++;
    }

    g_ptn_info[idx].parts_num = i;

    /* clarify the last region size */
    if (i >= 1) {
        g_ptn_info[idx].parts[i - 1].size = partsize - g_ptn_info[idx].parts[i - 1].offset;
    }

    *nr_part = g_ptn_info[idx].parts_num;

    return 0;
}

static struct mtd_partition *hisfc_drv_parse_spi_partition(u32 bus, u32 cs, u64 partsize, s32 *nr_part)
{
    struct device_node *node = NULL;
    s32 ret;
    u32 idx;
    static const char dtb_boot_compatible_name[SFC_MAX_FLASH_NUM][SFC_DTB_MATCH_NAME_SIZE] = {
        "hisilicon,boot-partition0",
        "hisilicon,boot-partition1",
        "hisilicon,boot-partition2",
        "hisilicon,boot-partition3"
    };

    if (bus > SFC_BUSID1 || cs > SFC_CSID1) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "invalid param: %u %u", bus, cs);
        *nr_part = 0;
        return NULL;
    }

    if (bus == SFC_BUSID1) {
        idx = bus + cs + 1;
    } else {
        idx = bus + cs;
    }

    node = of_find_compatible_node(NULL, NULL, dtb_boot_compatible_name[idx]);
    if (node) {
        ret = hisfc_drv_dtb_parse(idx, partsize, node, nr_part);
        if (ret) {
            /* make sure sfc region has a default config to set */
            sfc_log(LOG_ERROR_F, SFC_PLAT_C, "hisfc drv dtb parse failed:%d, use normal parse mode.", ret);
            hisfc_drv_normal_parse(idx, partsize, nr_part);
        }
    } else {
        hisfc_drv_normal_parse(idx, partsize, nr_part);
    }

    return g_ptn_info[idx].parts;
}

static void hisfc_drv_probe_mtd_size(struct hisfc_host *host)
{
    struct mtd_info *mtd[SFC_CS_CNT] = {NULL};
    hisfc_spi *spi[SFC_CS_CNT] = {NULL};

    spi[1] = &(host->core_host->spi[1]);
    spi[0] = &(host->core_host->spi[0]);

    mtd[1] = &(host->mtd[1]);
    mtd[0] = &(host->mtd[0]);

    mtd[1]->size = spi[1]->chipsize;
    mtd[1]->erasesize = spi[1]->erasesize;

    mtd[0]->size = spi[0]->chipsize;
    mtd[0]->erasesize = spi[0]->erasesize;

    return;
}

#ifdef CONFIG_PM
static s32 hisfc_drv_suspend(struct platform_device *pltdev, pm_message_t state)
{
    struct hisfc_host *host = (struct hisfc_host *)platform_get_drvdata(pltdev);

    if ((host == NULL) || (host->core_host == NULL)) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "suspend para is null");
        return -EFAULT;
    }

    hisfc_suspend_single_controller(host->core_host);

    return 0;
}

static s32 hisfc_drv_resume(struct platform_device *pltdev)
{
    struct hisfc_host *host = (struct hisfc_host *)platform_get_drvdata(pltdev);
    if ((host == NULL) || (host->core_host == NULL)) {
        return -EFAULT;
    }

    return hisfc_resume(host->core_host);
}

#endif /* CONFIG_PM */

s32 hisfc_drv_reg_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen, const u8 *data)
{
    sfc_bus_info info;
    struct hisfc_host *host = (struct hisfc_host *)mtd->priv;

    if (mtd == &(host->mtd[1])) {
        info.cs = 1;
    } else {
        info.cs = 0;
    }

    info.bus = host->core_host->bus;

    return hisfc_reg_write(info, to, len, retlen, data);
}

s32 hisfc_drv_init_mtd_info(u32 bus)
{
    u32 cs;
    u32 index;
    u32 dev_flag = 0;
    struct mtd_info *mtd[SFC_CTRL_CNT * SFC_CS_CNT] = {NULL};

    if (bus >= SFC_CTRL_CNT) {
        return -EINVAL;
    }
    for (cs = 0; cs < SFC_CS_CNT; cs++) {
        index = bus * SFC_CTRL_CNT + cs;
        if (sfc_core_cs_is_used(bus, cs, g_host[bus].core_host->cfg_use_cs)) {
            mtd[index] = &(g_host[bus].mtd[cs]);
            if (memset_s(mtd[index], sizeof(struct mtd_info), 0, sizeof(struct mtd_info))) {
                sfc_log(LOG_ERROR_F, SFC_PLAT_C, "memset_s mtd index %u failed", index);
            }
            if (index == 0) { // cs 0
                mtd[index]->name = "hi_sfc0.0";
            } else if (index == 1) { // cs 1
                mtd[index]->name = "hi_sfc0.1";
            } else if (index == 2) { // cs 2
                mtd[index]->name = "hi_sfc1.0";
            } else if (index == 3) { // cs 3
                mtd[index]->name = "hi_sfc1.1";
            }
            mtd[index]->type = MTD_NORFLASH;
            mtd[index]->writesize = 1;
            mtd[index]->flags = MTD_CAP_NORFLASH;
            mtd[index]->owner = THIS_MODULE;
            mtd[index]->priv = &g_host[bus];
            dev_flag = 1;
        }
    }

    if (dev_flag == 0) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "not find dev");
        return -ENODEV;
    }

    return 0;
}

static u32 hisfc_drv_get_cfg_mask(u32 bus, u32 cs)
{
    u32 cfg_mask;

    switch (cs) {
        case SFC_CSID0:
            cfg_mask = SFC_CS0_USED;
            break;
        case SFC_CSID1:
            cfg_mask = SFC_CS1_USED;
            break;
        default:
            cfg_mask = SFC_CS_NOUSE;
            break;
    }

    return cfg_mask;
}

static s32 hisfc_drv_register_single_mtd_device(u32 bus, u32 cs)
{
    s32 ret = 0;
    u32 cfg_mask;
    s32 nr_parts; /* Number of partitions */
    struct mtd_partition *parts = NULL;
    const char *part_probes[] = { "ofpart", NULL, };

    cfg_mask = hisfc_drv_get_cfg_mask(bus, cs);
    parts = hisfc_drv_parse_spi_partition(bus, cs, g_host[bus].mtd[cs].size, &nr_parts);
    if ((parts == NULL) || (nr_parts <= 0)) {
        return -EINVAL;
    }
    g_host[bus].add_partition = 1;

    if ((g_host[bus].mtd[cs].size) && ((g_host[bus].core_host->cfg_use_cs & cfg_mask) == cfg_mask)) {
        ret = mtd_device_parse_register(&(g_host[bus].mtd[cs]), part_probes, 0, parts, nr_parts);
        if (ret) {
            g_host[bus].core_host->spi[cs].mtd_device_regis = 0;
            sfc_log(LOG_ERROR_F, SFC_PLAT_C, "parse register sfc(%u) CS(%u) failed(%d)", bus, cs, ret);
            return ret;
        }
        g_host[bus].core_host->spi[cs].mtd_device_regis = 1;
    }

    return ret;
}

s32 hisfc_drv_unregister_single_mtd_device(u32 bus, u32 cs)
{
    s32 ret = 0;
    u32 cfg_mask;

    cfg_mask = hisfc_drv_get_cfg_mask(bus, cs);
    if ((g_host[bus].core_host->spi[cs].mtd_device_regis == 1) && (g_host[bus].mtd[cs].size)
        && ((g_host[bus].core_host->cfg_use_cs & cfg_mask) == cfg_mask)) {
        ret = mtd_device_unregister(&(g_host[bus].mtd[cs]));
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_PLAT_C, "mtd_device_unregister sfc(%u) cs(%u) ret:%d", bus, cs, ret);
            return ret;
        }
    }

    return ret;
}

void hisfc_drv_mtd_device_unregister(u32 bus)
{
    s32 ret;
    u32 cs;

    for (cs = 0; cs < SFC_CS_CNT; cs++) {
        ret = hisfc_drv_unregister_single_mtd_device(bus, cs);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_PLAT_C, "unregister_single_mtd_device sfc(%u) CS(%u) failed(%d)", bus, cs, ret);
        }
    }

    return;
}

s32 hisfc_drv_mtd_device_register(u32 bus)
{
    u32 cs;
    s32 ret;

    for (cs = 0; cs < SFC_CS_CNT; cs++) {
        ret = hisfc_drv_register_single_mtd_device(bus, cs);
        if (ret) {
            sfc_log(LOG_ERROR_F, SFC_PLAT_C, "register_single_mtd_device sfc(%u) CS(%u) failed(%d)", bus, cs, ret);
            goto FAIL;
        }
    }

    return 0;
FAIL:
    hisfc_drv_mtd_device_unregister(bus);

    return ret;
}

static void hisfc_drv_modify_mtd_write_cb(hisfc_spi *spi, struct mtd_info *mtd, u32 mtd_flag)
{
    if ((mtd == NULL) || (spi == NULL) || (spi->driver == NULL)) {
        sfc_log(LOG_PRINT_F, SFC_PLAT_C, "mtd or spi is null not cfg read cb!");
        return;
    }

    switch (mtd_flag) {
        case USE_DMA_FLAG:
            /* If DMA write is supported, use the DMA write mode. Otherwise, retain the original mode. */
            if (spi->driver->support_dma_bus_flag & SUPPORT_DMA_WR) {
                mtd->_write = hisfc_drv_dma_write;
                break;
            }
            sfc_log(LOG_PRINT_F, SFC_PLAT_C, "dma write not support,write cfg not changed!");
            break;
        case USE_REG_FLAG:
            mtd->_write = hisfc_drv_reg_write;
            break;
        case USE_BUS_FLAG:
            /* If the bus write mode is supported, use the bus write mode. Otherwise, retain the original mode. */
            if (spi->driver->support_dma_bus_flag & SUPPORT_BUS_WR) {
                mtd->_write = hisfc_drv_bus_write;
                break;
            }
            sfc_log(LOG_PRINT_F, SFC_PLAT_C, "bus write not support,write cfg not changed!");
            break;
        default:
            sfc_log(LOG_PRINT_F, SFC_PLAT_C, "mtd flag %u write not support!", mtd_flag);
            break;
    }

    return;
}

static void hisfc_drv_modify_mtd_read_cb(struct mtd_info *mtd, u32 mtd_flag)
{
    if (mtd == NULL) {
        sfc_log(LOG_PRINT_F, SFC_PLAT_C, "mtd is null not cfg write cb!");
        return;
    }

    switch (mtd_flag) {
        case USE_DMA_FLAG:
            mtd->_read = hisfc_drv_dma_read;
            break;
        case USE_REG_FLAG:
            mtd->_read = hisfc_drv_reg_read;
            break;
        case USE_BUS_FLAG:
            mtd->_read = hisfc_drv_bus_read;
            break;
        default:
            sfc_log(LOG_PRINT_F, SFC_PLAT_C, "mtd flag %u attr read not support!", mtd_flag);
            break;
    }

    return;
}

/*
 * Modify the read/write callback based on the mtd flag. If the DMA and bus write is supported,
 * check whether the callback is supported,
 * If the configuration is not supported, the previous configuration is used.
 */
static void hisfc_drv_plat_modify_mtd_cb(u8 *plat_data, u32 bus, u32 cs, u32 is_write, u32 mtd_flag)
{
    struct mtd_info *mtd = NULL;
    struct hisfc_host *host_info = NULL;

    host_info = (struct hisfc_host *)plat_data;
    if ((bus >= SFC_CTRL_CNT) || (cs >= SFC_CS_CNT)) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "para bus(%u) cs(%u) is out of range!", bus, cs);
        return;
    }

    mtd = &(host_info->mtd[cs]);
    if (is_write) {
        hisfc_drv_modify_mtd_write_cb(&host_info->core_host->spi[cs], mtd, mtd_flag);

        return;
    }

    hisfc_drv_modify_mtd_read_cb(mtd, mtd_flag);

    return;
}

s32 hisfc_drv_mtd_scan_init(void *info)
{
    /* Host pointer of a single controller */
    struct hisfc_host *host = (struct hisfc_host *)info;
    struct mtd_info *mtd[SFC_CS_CNT] = {NULL};
    u32 i;

    if (hisfc_drv_init_mtd_info(host->core_host->bus)) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "hisfc_drv_init_mtd_info bus %u failed", host->core_host->bus);
        return -1;
    }

    mtd[1] = &(host->mtd[1]);
    mtd[0] = &(host->mtd[0]);

    for (i = 0; i < SFC_CS_CNT; i++) {
        if (!sfc_core_cs_is_used(host->core_host->bus, i, host->core_host->cfg_use_cs)) {
            continue;
        }
        mtd[i]->_erase = hisfc_drv_reg_erase;
        mtd[i]->_read = hisfc_drv_reg_read;
        /* If DMA write is supported, use the DMA write mode. Otherwise, use the register mode. */
        if (host->core_host->spi[i].driver
            && (host->core_host->spi[i].driver->support_dma_bus_flag & SUPPORT_DMA_WR)) {
            mtd[i]->_write = hisfc_drv_dma_write;
            continue;
        }
        mtd[i]->_write = hisfc_drv_reg_write; /* Some devices do not support DMA write bus hisfc350_dma_write. */
    }
    hisfc_drv_probe_mtd_size(host);

    return 0;
}

struct platform_device *hisfc_drv_get_platform_device(const hisfc_core_host *c_host)
{
    struct hisfc_host *drv_host = NULL;

    if ((c_host == NULL) || (c_host->plat_ops == NULL)) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "hisfc_drv_get_platform_device err");
        return NULL;
    }

    drv_host = (struct hisfc_host *)hisfc_get_plat_data(c_host);
    if (drv_host == NULL) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "hisfc_drv_get_platform_device drv_host err");
        return NULL;
    }

    return drv_host->pltdev;
}

static s32 hisfc_drv_check_permission(u32 bus)
{
    if (hisfc_bus_check(bus)) {
        return -EFAULT;
    }

    if (g_host[bus].permis_info.use_hw == NOT_USE_HW_LOCK) {
        return 0;
    }

    if (g_host[bus].permis_info.can_access == A55_CAN_ACCESS) {
        return 0;
    }

    return -EACCES;
}

static s32 hisfc_drv_request_permission(u32 bus, struct device_node *node)
{
    s32 ret;

    ret = of_property_read_u32_index(node, "use_hw_lock", 0, &g_host[bus].permis_info.use_hw);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "of_property_read_u32_index bus failed. ret%d", ret);
        return ret;
    }

    if (g_host[bus].permis_info.use_hw == NOT_USE_HW_LOCK) {
        g_host[bus].permis_info.can_access = A55_CAN_ACCESS;
        return 0;
    }

    ret = of_property_read_u32_index(node, "hw_lock_id", 0, &g_host[bus].permis_info.hw_id);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "of_property_read_u32_index bus failed. ret%d", ret);
        return ret;
    }

    sfc_log(LOG_PRINT_F, SFC_PLAT_C, "sfc%u use hwid %u", bus, g_host[bus].permis_info.hw_id);
    ret = hw_acquire_hard_lock(g_host[bus].permis_info.hw_id);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "hw_acquire_hard_lock bus %u failed,ret%d", bus, ret);
        return ret;
    }

    g_host[bus].permis_info.can_access = A55_CAN_ACCESS;

    return ret;
}

static void hisfc_drv_release_permission(u32 bus)
{
    if (g_host[bus].permis_info.use_hw == NOT_USE_HW_LOCK) {
        return;
    }

    hw_release_hard_lock(g_host[bus].permis_info.hw_id);

    return;
}

__weak sfc_plat_operations g_sfc_platdrv_ops = { NULL };
__weak sfc_chip_operations g_sfc_chip_ops = { NULL };

s32 hisfc_drv_plat_linux_init(u8 *data)
{
    u32 bus;
    struct hisfc_host *host = (struct hisfc_host *)data;

    bus = host->bus;
    g_host[bus].core_host = hisfc_get_core_host(bus);
    g_host[bus].core_host->platform_data = data;
    g_host[bus].core_host->mtd_data = host;
    g_host[bus].core_host->mtd_init = hisfc_drv_mtd_scan_init;
    g_sfc_chip_ops.sfc_check_permission = hisfc_drv_check_permission;
    hisfc_core_register_chip_ops(bus, &g_sfc_chip_ops);
    g_sfc_platdrv_ops.plat_modify_mtd_cb = hisfc_drv_plat_modify_mtd_cb;
    hisfc_core_register_plat_ops(bus, &g_sfc_platdrv_ops);

    return 0;
}

static int sfc_get_flash_id_cmd(u32 cmd, unsigned long arg)
{
    int ret;
    struct sfc_id_info ids = {0};
    hisfc_core_host *sfc_host = NULL;

    ret = copy_from_user(&ids, (void __user *)arg, sizeof(struct sfc_id_info));
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "copy from user error");
        return -EFAULT;
    }

    if (ids.ctrl_id >= SFC_CTRL_CNT || ids.cs >= SFC_CS_CNT) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "cs(%u) or ctrl(%u) is invalid", ids.cs, ids.ctrl_id);
        return -EINVAL;
    }

    sfc_host = hisfc_get_core_host(ids.ctrl_id);
    memcpy_s(&ids.flash_id[0], SFC_MAX_ID_LEN, sfc_host->flash_id[ids.cs], SFC_MAX_ID_LEN);

    ret = copy_to_user((void __user *)arg, &ids, sizeof(struct sfc_id_info));
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "copy to user error");
        return -EFAULT;
    }

    return 0;
}

static int sfc_set_clk_cmd(u32 cmd, unsigned long arg)
{
    struct sfc_clk_info clk_info = { 0 };
    int ret;

    ret = copy_from_user(&clk_info, (void __user *)arg, sizeof(struct sfc_clk_info));
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "copy from user error");
        return -EFAULT;
    }

    return sfc_sys_set_controller_clk(clk_info.ctrl_id, clk_info.clk_mhz);
}

static int hisfc_cmd_set_addr_mode(hisfc_spi *cur_spi, struct hisfc_flash_conf *sfc_msg)
{
    int ret;
    s32 enable_4byte;
    u32 tmp_addrcycle;

    if (cur_spi->driver == NULL) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "error: driver is NULL");
        return -EFAULT;
    }

    if (cur_spi->original_addrcycle == SFC_3BYTE_ADDR) {
        if (sfc_msg->addrcycle == SFC_4BYTE_ADDR) {
            sfc_log(LOG_ERROR_F, SFC_PLAT_C, "error: can not switch to 4byte mode");
            return -EFAULT;
        }

        return 0;
    }

    enable_4byte = (sfc_msg->addrcycle == SFC_4BYTE_ADDR) ? SFC_TRUE : SFC_FALSE;
    if (cur_spi->driver->entry_4addr == NULL) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "error: entry_4addr is NULL");
        return -EFAULT;
    }

    tmp_addrcycle = cur_spi->addrcycle;
    cur_spi->addrcycle = cur_spi->original_addrcycle;
    ret = cur_spi->driver->entry_4addr(cur_spi, enable_4byte);
    if (ret) {
        cur_spi->addrcycle = tmp_addrcycle;
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "switch to %u byte mode failed", sfc_msg->addrcycle);
        return ret;
    }

    cur_spi->addrcycle = sfc_msg->addrcycle;

    return 0;
}

static int sfc_set_flash_addr_mode_cmd(u32 cmd, unsigned long arg)
{
    int ret;
    hisfc_core_host *sfc_host = NULL;
    struct hisfc_flash_conf sfc_conf = {0};

    ret = copy_from_user(&sfc_conf, (void __user *)arg, sizeof(struct hisfc_flash_conf));
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "copy from user error");
        return -EINVAL;
    }

    if (sfc_conf.ctrl_id >= SFC_CTRL_CNT || sfc_conf.cs >= SFC_CS_CNT) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "cs(%u) or ctrl(%u) is invalid", sfc_conf.cs, sfc_conf.ctrl_id);
        return -EINVAL;
    }

    if (sfc_conf.addrcycle != SFC_4BYTE_ADDR && sfc_conf.addrcycle != SFC_3BYTE_ADDR) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "addrcycle(%u) is invaild", sfc_conf.addrcycle);
        return -EINVAL;
    }

    sfc_host = hisfc_get_core_host(sfc_conf.ctrl_id);
    if (sfc_host == NULL) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "error: sfc_host(%u) is NULL", sfc_conf.ctrl_id);
        return -EFAULT;
    }

    sfc_osadp_mutex_lock(&sfc_host->lock);
    ret = hisfc_cmd_set_addr_mode(&sfc_host->spi[sfc_conf.cs], &sfc_conf);
    sfc_osadp_mutex_unlock(&sfc_host->lock);

    return ret;
}

static long sfc_ioctl(struct file *filp, u32 cmd, unsigned long arg)
{
    int ret;

    switch (cmd) {
        case SFC_GET_ID:
            ret = sfc_get_flash_id_cmd(cmd, arg);
            break;
        case SFC_SET_CLK:
            ret = sfc_set_clk_cmd(cmd, arg);
            break;
        case SFC_SET_FALSH_ADDR_MODE:
            ret = sfc_set_flash_addr_mode_cmd(cmd, arg);
            break;
        default:
            ret = -EINVAL;
    }

    return ret;
}

static const struct file_operations g_sfc_fops = {
    .unlocked_ioctl = sfc_ioctl,
    .owner = THIS_MODULE,
};

static s32 hisfc_drv_probe(struct platform_device *pltdev)
{
    s32 ret;
    u32 bus = 0;

    if (of_property_read_u32_index(pltdev->dev.of_node, "busid", 0, &bus)) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "read sfc bus id failed");
        return -ENOSYS;
    }

    if (hisfc_bus_check(bus)) {
        return -EINVAL;
    }

    sfc_log(LOG_ERROR_F, SFC_PLAT_C, "probe bus %u", bus);
    /* Save device information. */
    platform_set_drvdata(pltdev, &g_host[bus]);
    if (memset_s(&g_host[bus], sizeof(struct hisfc_host), 0, sizeof(struct hisfc_host))) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "warning:probe g_host %u failed", bus);
    }
    g_host[bus].bus = bus;
    g_host[bus].pltdev = pltdev;

    if (hisfc_drv_request_permission(bus, pltdev->dev.of_node)) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "request sfc bus(%u) permission bus failed", bus);
        return -EINVAL;
    }

    ret = hisfc_std_init(bus, hisfc_drv_plat_linux_init, (u8 *)&g_host[bus]);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "hisfc_std_init failed %d", ret);
        goto RELEASR_PERM;
    }

    ret = dma_set_mask_and_coherent(&pltdev->dev, SFC_DMA_MASK);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "set mask to 64bit failed,coherent_mask:0x%llx, dma mask:0x%llx",
                pltdev->dev.coherent_dma_mask, *(pltdev->dev.dma_mask));
        goto ERR_OUT;
    }

    ret = hisfc_drv_mtd_device_register(bus);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "hisfc_drv_mtd_device_register failed, result:%d", ret);
        goto ERR_OUT;
    }

    ret = comm_init_dev(&sfc_dev, &g_sfc_fops, DEV_SFC_NAME_KER);
    if (ret) {
        LOG(LOG_ERROR, "comm_init_dev err");
        goto ERR_OUT;
    }

    return ret;

ERR_OUT:
    hisfc_std_release(bus);
RELEASR_PERM:
    hisfc_drv_release_permission(bus);

    return ret;
}

static s32 hisfc_drv_remove(struct platform_device *pltdev)
{
    s32 ret;
    u32 bus;

    sfc_log(LOG_ERROR_F, SFC_PLAT_C, "hisfc_drv_remove");

    ret = of_property_read_u32_index(pltdev->dev.of_node, "busid", 0, &bus);
    if (ret) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "of_property_read_u32_index bus failed. ret %d", ret);
        return ret;
    }

    sfc_log(LOG_ERROR_F, SFC_PLAT_C, "hisfc_drv_remove bus %u", bus);
    hisfc_drv_mtd_device_unregister(bus);
    hisfc_std_release(bus);
    hisfc_drv_release_permission(bus);
    comm_cleanup_dev(&sfc_dev);

    return 0;
}

static void hisfc_drv_shutdown(struct platform_device *pltdev)
{
    struct hisfc_host *host_s = (struct hisfc_host *)platform_get_drvdata(pltdev);

    sfc_log(LOG_ERROR_F, SFC_PLAT_C, "hisfc_drv_shutdown");

    if ((host_s == NULL) || (host_s->core_host == NULL)) {
        sfc_log(LOG_ERROR_F, SFC_PLAT_C, "shutdown para is null");
        return;
    }

    hisfc_shutdown_single_controler(host_s->core_host);

    return;
}
#ifdef SFC0_COMPILE
static const struct of_device_id g_sfc_dt_ids[] = {
    { .compatible = "hisilicon,hi1711-sfc0" }, {}
};
#else
static const struct of_device_id g_sfc_dt_ids[] = {
    { .compatible = "hisilicon,hi1711-sfc1" }, {}
};
#endif

static struct platform_driver g_hisfc351_driver_pltdrv = {
    .probe = hisfc_drv_probe,
    .remove = hisfc_drv_remove,
    .shutdown = hisfc_drv_shutdown,
#ifdef CONFIG_PM
    .suspend = hisfc_drv_suspend,
    .resume = hisfc_drv_resume,
#endif /* CONFIG_PM */
    .driver = {
#ifdef SFC0_COMPILE
    .name = "hi_sfc0",
#else
    .name = "hi_sfc1",
#endif
    .of_match_table = g_sfc_dt_ids,
    .owner = THIS_MODULE,
    .bus = &platform_bus_type,
    },
};

module_platform_driver(g_hisfc351_driver_pltdrv);

MODULE_AUTHOR("HiSilicon Technologies Co., Ltd");
MODULE_LICENSE("GPL v2");
#ifdef SFC0_COMPILE
MODULE_DESCRIPTION("Hisilicon Spi Flash Controller 0 Device Driver");
#else
MODULE_DESCRIPTION("Hisilicon Spi Flash Controller 1 Device Driver");
#endif