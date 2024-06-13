/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Hi309x ATF boot codes for uboot
 * Create: 2018-05-10
 */
#include <common.h>
#include <dm.h>
#include <asm/io.h>
#include <atf_common.h>
#include <image.h>

static struct bl31_params *g_bl2_to_bl31_params;
static struct entry_point_info g_bl33_ep_info;
static struct atf_image_info g_bl33_image_info;

#define JUMP_ATF_ADDR 0x87100000
#define ATF_FW_SIZE 0x19000
#define UBOOT_PARAM_TO_ATF_ADDR 0x8100000

/* boot load atf bl3;atf bl3 load linux kernel */
void boot_from_atf(bootm_headers_t *images, void *flag)
{
    unsigned int reg_val;
    void (*entry)(struct bl31_params *images, void *flag) = NULL;

    reg_val = readl(0x14000010);
    reg_val <<= 2;  /* FCM Core 0 Reset Vector base address [33:2] */

    /* copy atf to ddr */
    (void)memcpy((void *)JUMP_ATF_ADDR, (void *)(uintptr_t)(reg_val + CONFIG_ATF_FW_OFFSET), ATF_FW_SIZE);
    g_bl2_to_bl31_params = (struct bl31_params *)UBOOT_PARAM_TO_ATF_ADDR;

    (void)memset((void *)UBOOT_PARAM_TO_ATF_ADDR, 0, sizeof(struct bl31_params));
    (void)memset((void *)&g_bl33_ep_info, 0, sizeof(struct entry_point_info));
    (void)memset((void *)&g_bl33_image_info, 0, sizeof(struct atf_image_info));

    SET_PARAM_HEAD(g_bl2_to_bl31_params, ATF_PARAM_BL31, ATF_VERSION_1, 0);

    g_bl2_to_bl31_params->bl31_image_info = NULL;
    g_bl2_to_bl31_params->bl32_ep_info = NULL;
    g_bl2_to_bl31_params->bl32_image_info = NULL;

    g_bl2_to_bl31_params->bl33_ep_info = &g_bl33_ep_info;
    SET_PARAM_HEAD(g_bl2_to_bl31_params->bl33_ep_info, ATF_PARAM_EP, ATF_VERSION_1, ATF_EP_NON_SECURE);

    /* BL33 expects to receive the primary CPU MPID (through x0) */
    g_bl33_ep_info.args.arg0 = (u64)images->ft_addr;
    g_bl33_ep_info.args.arg1 = 0;
    g_bl33_ep_info.args.arg2 = 0;
    g_bl33_ep_info.args.arg3 = 0;
    g_bl33_ep_info.args.arg4 = images->os.load;
    g_bl33_ep_info.pc = images->os.load;
    g_bl33_ep_info.spsr = SPSR_64(MODE_EL2, MODE_SP_ELX, DISABLE_ALL_EXECPTIONS);

    g_bl2_to_bl31_params->bl33_image_info = &g_bl33_image_info;
    SET_PARAM_HEAD(g_bl2_to_bl31_params->bl33_image_info, ATF_PARAM_IMAGE_BINARY, ATF_VERSION_1, 0);

    entry = (void *)JUMP_ATF_ADDR; /* boot_jump_linux */
    printf("boot atf\n");
    entry(g_bl2_to_bl31_params, NULL);
}
