/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Hi309x uboot DTB cfg fixing APIs
 * Create: 2018-05-10
 */

#include "fdt_fix.h"
#include <common.h>
#include <fdt_support.h>
#include <image.h>
#include <hi309x_common.h>
#include <linux/libfdt.h>
#include <asm/io.h>
#include "bd_info.h"
#include <dm.h>

#define DDR_REG_SIZE 16 /* Up to 64-bit address + 64-bit size */

#define MONTH_IS_VALID(month) (((month) >= 1) && ((month) <= 12))
/*
 * Read original version & build time and convert them to formatted string
 * example version string:    5.0.1.1
 * example build time string: 12:00:00 Jan 1 2019
 */

typedef struct __version_info {
    char active_ver[SECFW_VER_LEN];
    char gold_ver[SECFW_VER_LEN];
    char active_build_time[SECFW_BUILD_TIME_LEN];
    char gold_build_time[SECFW_BUILD_TIME_LEN];
} version_info;

static void sec_fw_format_ver_time(unsigned char *ver_data, unsigned char *time_data, char *ver_str, char *time_str)
{
    const char *month_str[] = {
        "Nil", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    unsigned int year = 2000;
    const char *p_month = "Nil";
    int ret;

    (void)memset(ver_str, 0, SECFW_VER_LEN);
    (void)memset(time_str, 0, SECFW_BUILD_TIME_LEN);
    /*
     * The structure of ver_data:
     * Byte  Desc
     * 0     Major
     * 1     Minor
     * 2     Rev
     * 3     Build
     */
    /* param[0],  param[1],  param[2],  param[3] */
    ret = sprintf(ver_str, "%u.%u.%u.%u", ver_data[0], ver_data[1], ver_data[2], ver_data[3]);
    if (ret < 0) {
        printf("version sprintf err\n");
        return;
    }

    /*
     * The structure of time_data:
     * Byte   Desc
     * 0      Year(0 - 255) -> plus 2000 is the real year
     * 1      Month(1 - 12) -> Must convert to month_str[n](as month_str[] indicates, if not valid, string is "Nil")
     * 2      Day(1-31)
     * 3      Hour(0-23)
     * 4      Minute(0-59)
     * 5      Second(0-59)
     */
    year += time_data[0];

    if (MONTH_IS_VALID(time_data[1])) {
        p_month = month_str[time_data[1]];
    }

    ret = sprintf(time_str, "%02u:%02u:%02u %s %2u %04u",
                    time_data[3], time_data[4], time_data[5], /* param[3]->year,  param[4]->month,  param[5]->day */
                    p_month, time_data[2], year); /* param[2] */
    if (ret < 0) {
        printf("version time sprintf err\n");
        return;
    }

#ifdef Hi309x_DEBUG_EN
    printf("Version:\t%s\n", ver_str);
    printf("Build_Time:\t%s\n", time_str);
#endif

    return;
}

static void sec_l0fw_get_version_time(version_info *ver)
{
    secfw_inter_core_param_s *param = (secfw_inter_core_param_s *)DDR_A55_CONN_PARAM_ADDR;
    unsigned char *p_ver_data = NULL;
    unsigned char *p_time_data = NULL;
    char *version = NULL;
    char *build_time = NULL;

    version = ver->active_ver;
    build_time = ver->active_build_time;
    p_ver_data = param->ver_param.l0fw_ver_active;
    p_time_data = param->ver_param.l0fw_build_time_active;
    sec_fw_format_ver_time(p_ver_data, p_time_data, version, build_time);

    version = ver->gold_ver;
    build_time = ver->gold_build_time;
    p_ver_data = param->ver_param.l0fw_ver_gold;
    p_time_data = param->ver_param.l0fw_build_time_gold;
    sec_fw_format_ver_time(p_ver_data, p_time_data, version, build_time);
}

static void sec_l1fw_get_version_time(version_info *ver)
{
    secfw_inter_core_param_s *param = (secfw_inter_core_param_s *)DDR_A55_CONN_PARAM_ADDR;
    unsigned char *p_ver_data = NULL;
    unsigned char *p_time_data = NULL;
    char *version = NULL;
    char *build_time = NULL;

    version = ver->active_ver;
    build_time = ver->active_build_time;
    p_ver_data = param->ver_param.l1fw_ver_active;
    p_time_data = param->ver_param.l1fw_build_time_active;
    sec_fw_format_ver_time(p_ver_data, p_time_data, version, build_time);

    version = ver->gold_ver;
    build_time = ver->gold_build_time;
    p_ver_data = param->ver_param.l1fw_ver_gold;
    p_time_data = param->ver_param.l1fw_build_time_gold;
    sec_fw_format_ver_time(p_ver_data, p_time_data, version, build_time);
}

static void sec_uboot_get_version_time(version_info *ver)
{
    secfw_inter_core_param_s *param = (secfw_inter_core_param_s *)DDR_A55_CONN_PARAM_ADDR;
    unsigned char *p_ver_data = NULL;
    unsigned char *p_time_data = NULL;
    char *version = NULL;
    char *build_time = NULL;

    version = ver->active_ver;
    build_time = ver->active_build_time;
    p_ver_data = param->ver_param.uboot_ver_active;
    p_time_data = param->ver_param.uboot_build_time_active;
    sec_fw_format_ver_time(p_ver_data, p_time_data, version, build_time);

    version = ver->gold_ver;
    build_time = ver->gold_build_time;
    p_ver_data = param->ver_param.uboot_ver_gold;
    p_time_data = param->ver_param.uboot_build_time_gold;
    sec_fw_format_ver_time(p_ver_data, p_time_data, version, build_time);
}

static void fdt_adapt_phy_addr_buckle(unsigned int board_id, gmac_phy_addr_info_u *addr_info)
{
    unsigned int pcb_ver = board_info_pcb_ver_get();

    if (board_id == EVB_BOARD_ID) {
        if (pcb_ver == PCB_COMMON_VER5) {
            addr_info->bits.gmac1_phy_addr = 0x1; /* EVB V110 配置RMII1 PHY addr为0x1 */
            addr_info->bits.gmac2_phy_addr = 0x3; /* EVB V110 配置SGMII0 PHY addr为0x3 */
        } else {
            addr_info->bits.gmac1_phy_addr = 0x3; /* EVB V100 配置RMII1 PHY addr为0x3 */
            addr_info->bits.gmac2_phy_addr = 0x0; /* EVB V100 配置SGMII0 PHY addr为0x0 */
        }
        addr_info->bits.gmac0_phy_addr = 0x2; /* RMII0 PHY addr */
        addr_info->bits.gmac3_phy_addr = 0x7; /* SGMII1 PHY addr */
    }
}

static void fdt_adapt_phy_addr_taishan(unsigned int board_id, gmac_phy_addr_info_u *addr_info)
{
    if (board_id == FUSION_RM210) {
        addr_info->bits.gmac0_phy_addr = 0xff; /* RMII0 PHY addr */
        addr_info->bits.gmac1_phy_addr = 0xff; /* RMII1 PHY addr */
        addr_info->bits.gmac2_phy_addr = 0x1;  /* SGMII0 PHY addr */
        addr_info->bits.gmac3_phy_addr = 0x2;  /* SGMII1 PHY addr */
    } else if (board_id == RM211_IT31RMUC) {
        addr_info->bits.gmac0_phy_addr = 0x2; /* RMII0 PHY addr */
        addr_info->bits.gmac1_phy_addr = 0xff; /* RMII1 PHY addr */
        addr_info->bits.gmac2_phy_addr = 0x3;  /* SGMII0 PHY addr */
        addr_info->bits.gmac3_phy_addr = 0x7;  /* SGMII1 PHY addr */
    }
}

static void fdt_adapt_phy_addr_pangea_v6(unsigned int board_id, gmac_phy_addr_info_u *addr_info)
{
    if ((board_id == PANGEA_V6_BOARD_ID_STL6TSPCE_SAS) || (board_id == PANGEA_V6_BOARD_ID_STL6TSPCE_PALM) ||
        (board_id == PANGEA_V6_BOARD_ID_STL6SPCQ_T) || (board_id == PANGEA_V6_BOARD_ID_STL6SPCQ) ||
        (board_id == PANGEA_V6_BOARD_ID_STL6SPCQ10) || (board_id == PANGEA_V6_BOARD_ID_STL6SPCQ20)) {
        addr_info->bits.gmac0_phy_addr = 0x4;  /* RMII0 PHY addr */
        addr_info->bits.gmac1_phy_addr = 0xff; /* RMII1 PHY addr */
        addr_info->bits.gmac2_phy_addr = 0xff; /* SGMII0 PHY addr */
        addr_info->bits.gmac3_phy_addr = 0xff; /* SGMII1 PHY addr */
    } else if (board_id == PANGEA_V6_BOARD_ID_STL6ICBP) {
        addr_info->bits.gmac0_phy_addr = 0xff; /* RMII0 PHY addr */
        addr_info->bits.gmac1_phy_addr = 0xff; /* RMII1 PHY addr */
        addr_info->bits.gmac2_phy_addr = 0x1;  /* SGMII0 PHY addr */
        addr_info->bits.gmac3_phy_addr = 0xff; /* SGMII1 PHY addr */
    } else if ((board_id == PANGEA_V6_BOARD_ID_STL6EXCN) || (board_id == PANGEA_V6_BOARD_ID_STL6SPCD) ||
               (board_id == PANGEA_V6_BOARD_ID_STL6TSPCM_ATLA) || (board_id == PANGEA_V6_BOARD_ID_STL6SPCK) ||
               (board_id == PANGEA_V6_BOARD_ID_STL6SPCJ)) {
        addr_info->u32 = 0xffffffff; /* 调试网口不使用phy */
    } else if (board_id == PANGEA_V6_BOARD_ID_STL6EXCN01) {
        addr_info->bits.gmac0_phy_addr = 0x5;  /* RMII0 PHY addr */
        addr_info->bits.gmac1_phy_addr = 0xff; /* RMII1 PHY addr */
        addr_info->bits.gmac2_phy_addr = 0xff; /* SGMII0 PHY addr */
        addr_info->bits.gmac3_phy_addr = 0xff; /* SGMII1 PHY addr */
    } else if (board_id == PANGEA_V6_BOARD_ID_STL6TSPC_TEST) {
        addr_info->bits.gmac0_phy_addr = 0xff; /* RMII0 PHY addr */
        addr_info->bits.gmac1_phy_addr = 0xff; /* RMII1 PHY addr */
        addr_info->bits.gmac2_phy_addr = 0x3;  /* SGMII0 PHY addr */
        addr_info->bits.gmac3_phy_addr = 0x7;  /* SGMII1 PHY addr */
    } else if ((board_id == PANGEA_V6_BOARD_ID_BC13MBSBF) || (board_id == PANGEA_V6_BOARD_ID_BC82ADDRF) ||
               (board_id == PANGEA_V6_BOARD_ID_STL6ICBA01) || (board_id == PANGEA_V6_BOARD_ID_STL6ICBA02) ||
               (board_id == PANGEA_V6_BOARD_ID_BC13MBSBF2)) {
        addr_info->u32 = 0xffffffff;
    }
}

static void fdt_adapt_phy_addr_tce(unsigned int board_id, gmac_phy_addr_info_u *addr_info)
{
    if (board_id == TCE_BOARD_ID_SMMA) {
        addr_info->u32 = 0xffffffff; /* 调试网口不使用phy */
    } else if (board_id == TCE_BOARD_ID_GPUA) {
        addr_info->u32 = 0xffffffff; /* 调试网口不使用phy */
    } else if (board_id == TCE_BOARD_ID_CN90BMCBB) {
        addr_info->u32 = 0xffffffff; /* 调试网口不使用phy */
    } else if (board_id == TCE_BOARD_ID_CN90GPUB) {
        addr_info->u32 = 0xffffffff; /* 调试网口不使用phy */
    } else if (board_id == TCE_BOARD_ID_CN91SPUA) {
        addr_info->u32 = 0xffffffff; /* 调试网口不使用phy */
    } else if (board_id == TCE_BOARD_ID_CN90BMCC) {
        addr_info->u32 = 0xffffffff; /* 调试网口不使用phy */
    } else if (board_id == TCE_BOARD_ID_CN90GPUD) {
        addr_info->u32 = 0xffffffff; /* 调试网口不使用phy */
    } else if (board_id == TCE_BOARD_ID_CN90GPUE) {
        addr_info->u32 = 0xffffffff; /* 调试网口不使用phy */
    } else if (board_id == TCE_BOARD_ID_CN90GPUB_VB) {
        addr_info->u32 = 0xffffffff;
    } else if (board_id == TCE_BOARD_ID_CN90BMCBBA) {
        addr_info->u32 = 0xffffffff;
    }
}

static void fdt_adapt_phy_addr_ocsa(unsigned int board_id, gmac_phy_addr_info_u *addr_info)
{
    if (board_id == OSCA_E9000_V6_IT11SWCA) {
        addr_info->u32 = 0xffffffff; /* 调试网口不使用phy */
    }
}

static void fdt_adapt_phy_addr_x_series(unsigned int board_id, gmac_phy_addr_info_u *addr_info)
{
    if (board_id == X_SERIES_X6000_v6_BC23SPSA || board_id == X_SERIES_X6000_v6_BC23SPSB) {
        addr_info->bits.gmac0_phy_addr = 0xff; /* RMII0 PHY addr */
        addr_info->bits.gmac1_phy_addr = 0xff; /* RMII1 PHY addr */
        addr_info->bits.gmac2_phy_addr = 0xff; /* SGMII0 PHY addr */
        addr_info->bits.gmac3_phy_addr = 0x6;  /* SGMII1 PHY addr */
    }
}

static void fdt_adapt_phy_addr_chh(unsigned int board_id, gmac_phy_addr_info_u *addr_info)
{
    if (board_id == CHH_BOARD_ID_CHH1FPFMBA) {
        addr_info->u32 = 0xffffffff; /* 调试网口不使用phy */
    } else if (board_id == CHH_BOARD_ID_CHH1SPMNTA) {
        addr_info->bits.gmac0_phy_addr = 0x2; /* RMII0 PHY addr */
        addr_info->bits.gmac1_phy_addr = 0x3; /* RMII1 PHY addr */
        addr_info->bits.gmac2_phy_addr = 0x6; /* SGMII0 PHY addr */
        addr_info->bits.gmac3_phy_addr = 0x7; /* SGMII1 PHY addr */
    }
}

static void fdt_adapt_phy_addr_skylake_buckle(gmac_phy_addr_info_u *addr_info)
{
    addr_info->bits.gmac0_phy_addr = 0x2; /* RMII0 PHY addr */
    addr_info->bits.gmac1_phy_addr = 0x1; /* RMII1 PHY addr */
    addr_info->bits.gmac3_phy_addr = 0x7; /* SGMII1 PHY addr */
}

static void fdt_get_phy_addr(gmac_phy_addr_info_u *addr_info)
{
    unsigned int platform_id;
    unsigned int board_id;
    unsigned int gmac_type;

    /* 获取单板硬件信息 */
    platform_id = bdinfo_get_platform_id();
    board_id = bdinfo_get_board_id();
    gmac_type = bdinfo_get_debug_gmac_type();

    /* 默认配置RMII0、RMII1 、SGMII0、SGMII1的phy地址为0xff 0xff代表不使用phy */
    addr_info->u32 = 0xffffffff;

    if (gmac_type == 0) {
        addr_info->bits.gmac0_phy_addr = 0x2; /* RMII0 PHY addr */
    } else {
        addr_info->bits.gmac2_phy_addr = 0x3; /* SGMII0 PHY addr */
    }

    /* EVB特殊适配 */
    if (platform_id == BOARD_PLATM_BUCKLE) {
        fdt_adapt_phy_addr_buckle(board_id, addr_info);
    } else if (platform_id == BOARD_PLATM_TAISHAN) { /* 第一种类型: 贴片主板类型 */
        fdt_adapt_phy_addr_taishan(board_id, addr_info);
    } else if (platform_id == BOARD_PLATM_PANGEA_V6) {
        fdt_adapt_phy_addr_pangea_v6(board_id, addr_info);
    } else if (platform_id == BOARD_PLATM_TCE) {
        fdt_adapt_phy_addr_tce(board_id, addr_info);
    } else if (platform_id == BOARD_PLATM_BUCKLE_OSCA) {
        fdt_adapt_phy_addr_ocsa(board_id, addr_info);
    } else if (platform_id == BOARD_PLATM_BUCKLE_X_SERIES) {
        fdt_adapt_phy_addr_x_series(board_id, addr_info);
    } else if (platform_id == BOARD_PLATM_CHH) {
        fdt_adapt_phy_addr_chh(board_id, addr_info);
    } else if (platform_id == BOARD_PLATM_SKYLAKE_BUCKLE) {
        fdt_adapt_phy_addr_skylake_buckle(addr_info);
    }
}

static int fdt_add_fw_version_node(void *fdt, int root_node_offset, char *fw_name, version_info *ver)
{
    int ret;
    int node_offset;

    node_offset = fdt_add_subnode(fdt, root_node_offset, fw_name);
    if (node_offset < 0) {
        return node_offset;
    }

    ret = fdt_setprop_string(fdt, node_offset, "active", ver->active_ver);
    if (ret) {
        return ret;
    }

    ret = fdt_setprop_string(fdt, node_offset, "active_build_time", ver->active_build_time);
    if (ret) {
        return ret;
    }

    ret = fdt_setprop_string(fdt, node_offset, "gold", ver->gold_ver);
    if (ret) {
        return ret;
    }

    ret = fdt_setprop_string(fdt, node_offset, "gold_build_time", ver->gold_build_time);
    if (ret) {
        return ret;
    }

    return 0;
}

static int fdt_fix_fw_version(void *fdt, int parent_node_offset)
{
    int ret;
    version_info ver = { 0 };

    sec_l0fw_get_version_time(&ver);
    ret = fdt_add_fw_version_node(fdt, parent_node_offset, FDT_L0FW_VER_NAME, &ver);
    if (ret) {
        printf("Failed to add secure firmware(L0) version\n");
        return ret;
    }

    sec_l1fw_get_version_time(&ver);
    ret = fdt_add_fw_version_node(fdt, parent_node_offset, FDT_L1FW_VER_NAME, &ver);
    if (ret) {
        printf("Failed to add secure firmware(L1) version\n");
        return ret;
    }

    sec_uboot_get_version_time(&ver);
    ret = fdt_add_fw_version_node(fdt, parent_node_offset, FDT_UBOOT_VER_NAME, &ver);
    if (ret) {
        printf("Failed to add uboot version\n");
        return ret;
    }

    return 0;
}

/*
 * Fix up the arguments for non-volatile memory partitions
 */
static int fdt_fix_nvm_parts(void *fdt, int root_node_offset)
{
    int ret;
    int node_offset;
    unsigned int nvm_part_id = sys_inif_flag_get(NVM_PART_MASK, NVM_PART_OFFSET);
    struct nvm_partiton_id ids[] = {
        { "0", "1", "2" },
        { "0", "2", "1" },
        { "1", "2", "0" },
        { "1", "0", "2" },
        { "2", "1", "0" },
        { "2", "0", "1" },
        { NVM_PART_INVAL, NVM_PART_INVAL, NVM_PART_INVAL },
        { NVM_PART_INVAL, NVM_PART_INVAL, NVM_PART_INVAL }
    };

    node_offset = fdt_add_subnode(fdt, root_node_offset, FDT_NVM_PART_NAME);
    if (node_offset < 0) {
        return node_offset;
    }

    ret = fdt_setprop_string(fdt, node_offset, "active", ids[nvm_part_id].active_part_id);
    if (ret) {
        return ret;
    }

    ret = fdt_setprop_string(fdt, node_offset, "gold", ids[nvm_part_id].gold_part_id);
    if (ret) {
        return ret;
    }

    ret = fdt_setprop_string(fdt, node_offset, "temp", ids[nvm_part_id].temp_part_id);
    if (ret) {
        return ret;
    }

    return 0;
}

static int fdt_fix_nvm_sfc_parts(void *fdt, int root_node_offset)
{
    int ret;
    int node_offset;
    unsigned int nvm_part_id = sys_inif_flag_get(NVM_PART_MASK, NVM_PART_OFFSET);

    struct nvm_sfc_partiton_id ids[] = {
        { "0", "1"},
        { "1", "0"},
    };

    if (nvm_part_id >= 2) {
        return -1;
    }

    node_offset = fdt_add_subnode(fdt, root_node_offset, FDT_NVM_PART_NAME);
    if (node_offset < 0) {
        return node_offset;
    }

    ret = fdt_setprop_string(fdt, node_offset, "active", ids[nvm_part_id].active_part_id);
    if (ret) {
        return ret;
    }

    ret = fdt_setprop_string(fdt, node_offset, "gold", ids[nvm_part_id].gold_part_id);

    return ret;
}

static int fdt_fix_secure_cfg(void *fdt, int parent_node_offset)
{
    unsigned int secure_en;
    int node_offset;
    int ret;

    /* Boot define bit0 indicates whether efuse secure bits are burned */
    secure_en = readl(SYSCTL_BOOT_DEFINE_BASE) & 0x1;

    node_offset = fdt_add_subnode(fdt, parent_node_offset, FDT_SECURE_ENABLE);
    if (node_offset < 0) {
        return node_offset;
    }

    ret = fdt_setprop_u32(fdt, node_offset, "enable", secure_en);
    if (ret) {
        return ret;
    }

    return 0;
}

static int fdt_fix_ramdisk_cfg(void *fdt, int parent_node_offset)
{
    unsigned long long reg[2] = {0}; /* reg_size is 2 (1 is address, 2 is size) */
    unsigned long long ramdisk_base_addr;
    unsigned long ramdisk_size;
    int node_offset;
    int ret;

    ramdisk_size = get_ramdisk_size();
    if (!ramdisk_size) {
        return 0;
    }

    node_offset = fdt_add_subnode(fdt, parent_node_offset, "ramdisk_memory");
    if (node_offset < 0) {
        return node_offset;
    }

    ramdisk_base_addr = CONFIG_SYS_SDRAM_BASE + gd->bd->bi_dram[0].size;
    reg[0] = cpu_to_fdt64(ramdisk_base_addr);
    reg[1] = cpu_to_fdt64(ramdisk_size);
    ret = fdt_setprop(fdt, node_offset, "reg", reg, sizeof(reg));
    if (ret) {
        return ret;
    }

    return 0;
}

int fdt_fix_memory_prop(void *fdt, int parent_node_offset)
{
    int ret;
    int node_offset;
    unsigned long long reg[2] = {0}; /* reg_size is 2 (1 is address, 2 is size) */
    int vce_flag;

    vce_flag = bdinfo_get_vce_state();
    printf("fdt_fix_memory_prop: vce_flag is %x (SYSCTRL_INITIAL_INF: 0x%08x)\n", vce_flag,
        readl(SYSCTL_INITIAL_INF_BASE));
    if (vce_flag == UNUSE_VCE_FLAG) {
        /* VCE memory is not used, add memory node, tell linux to use VCE memory */
        node_offset = fdt_add_subnode(fdt, parent_node_offset, "memory_unused_vce");
        if (node_offset < 0) {
            printf("WARNING: could not add node(memory_unused_vce) in dtb(%s)\n", fdt_strerror(node_offset));
            return node_offset;
        }

        ret = fdt_setprop_string(fdt, node_offset, "device_type", "memory");
        if (ret) {
            printf("WARNING: could not set device_type for memory node(%s)\n", fdt_strerror(ret));
            return ret;
        }

        /* 1st u64 is address, 2nd u64 is size */
        reg[0] = cpu_to_fdt64(VCE_RESDDR_ADDR);
        reg[1] = cpu_to_fdt64(VCE_RESDDR_SIZE);
        ret = fdt_setprop(fdt, node_offset, "reg", reg, sizeof(reg));
        if (ret) {
            printf("WARNING: could not set memory node in dtb for vce memory(%s)\n", fdt_strerror(ret));
            return ret;
        }
    }

    return 0;
}

static int fdt_fix_gmac_phy_cfg(void *fdt)
{
    int nodeoffset;
    int soc_node_offset;
    int ret;
    gmac_phy_addr_info_u addr_info;

    soc_node_offset = fdt_path_offset(fdt, "/soc");

    nodeoffset = fdt_find_or_add_subnode(fdt, soc_node_offset, "gmac");
    if (nodeoffset < 0) {
        return nodeoffset;
    }

    fdt_get_phy_addr(&addr_info);

    ret = fdt_setprop_u32(fdt, nodeoffset, "phy_addr", addr_info.u32);
    if (ret) {
        return ret;
    }

    return 0;
}

int ft_board_setup(void *fdt, struct bd_info *bd)
{
    int ret;
    int root_node_offset;
    unsigned int boot_media = get_boot_media_type();

    root_node_offset = fdt_path_offset(fdt, "/");
    if (root_node_offset < 0) {
        return root_node_offset;
    }

    ret = fdt_fix_fw_version(fdt, root_node_offset);
    if (ret) {
        return ret;
    }

    if (boot_media == BOOT_MODE_SFC) {
        ret = fdt_fix_nvm_sfc_parts(fdt, root_node_offset);
    } else {
        ret = fdt_fix_nvm_parts(fdt, root_node_offset);
    }

    if (ret) {
        return ret;
    }

    ret = fdt_fix_secure_cfg(fdt, root_node_offset);
    if (ret) {
        return ret;
    }

    ret = fdt_fix_gmac_phy_cfg(fdt);
    if (ret) {
        return ret;
    }

    ret = fdt_fix_memory_prop(fdt, root_node_offset);
    if (ret) {
        return ret;
    }

    ret = fdt_fix_ramdisk_cfg(fdt, root_node_offset);
    if (ret) {
        return ret;
    }

    return 0;
}
