#include "stdbool.h"
#include <hi309x_common.h>
#include "bd_info.h"
#include <asm/io.h>
#include <string.h>
#include <linux/delay.h>
#ifndef NULL
#define NULL ((void *)0)    /* Commonly used */
#endif

typedef unsigned int u32;
typedef unsigned char u8;
typedef int s32;
#define PCIE0_RST_POS_MASK          (1 << 9)
#define PCIE1_RST_POS_MASK          (1 << 10)
#define PCIE_INIT_DONE       (1U << 9)
#define PCIE1_LTSSM_ON       (1U << 10)
#define PCIE_INIT_MASK       (PCIE_INIT_DONE)
#define PCIE1_LTSSM_MASK     (PCIE1_LTSSM_ON)
#define PCIE0_PHY_RST_OFFSET             (30)
#define PCIE_PHY_RTUNE_REQ_OFFSET        (2)
#define SHIFT_INVAL 32
#define PRE_INVAL   16
#define PHY_RX0_EQ_OFFSET                (7)
#define PCIE_APP_LTSSM_ENABLE_OFFSET     (11)
#define POWER_UP_RST_N_EN_OFFSET         (12)
#define PCIE_NO_CREDIT_PRO_DIS_OFFSET    (17)
#define DELAY_US_SERDES 10
#define TYPE_KVM        0U

#define PCIE_BAR_ENV_NAME_LEN      10
#define PCIE_MAX_BAR_CNT           6
#define PCIE_MAX_64BIT_BAR_CNT     3
#define PCIE_DB2_ENABLE_SHIFT      (1U << 13)
#define PCIE_PHY_ARGS_ENV_NAME_LEN 31
#define PCIE_MAX_PHY_ARGS_CNT      6

#define PCIE_MSI_CAP_ID                 0x5
#define PCIE_MSI_CTRL_MASK_OFFSET       8
#define PCIE_MSI_CTRL_64BIT_ADDR_OFFSET 7
#define PCIE_MSI_CTRL_EN_OFFSET         0

#define PCIE_CMD_INTX_DIS_OFFSET 10
#define PCIE_SYS_INT_OFFSET      5

#define PCIE_BAR_ADDR_32BIT 0x32
#define PCIE_BAR_ADDR_64BIT 0x64

#define SYSCTL_INITIAL_INF_REG      (SYSCTL_REG_BASE + 0x8)
#define SYSCTL_SYS_INT_MASK         (SYSCTL_REG_BASE + 0x138)
#define SYSCTL_SYS_INT_STAUTS       (SYSCTL_REG_BASE + 0x13c)
#define SYSCTL_A55_ASSERT_INFO_BASE (SYSCTL_REG_BASE + 0x100)
#define SYSCTL_PROBE_CTRL           (SYSCTL_REG_BASE + 0x218)
#define HOST_SUBSYS_BASE      (0x08B20000)
#define HOST_RST_ENABLE       (HOST_SUBSYS_BASE + 0xc)
#define HOST_RST_DISABLE      (HOST_SUBSYS_BASE + 0x10)
#define GPU_DFX_CFG_0         (HOST_SUBSYS_BASE + 0x24)
#define GPU_DFX_CFG_1         (HOST_SUBSYS_BASE + 0x28)
#define HOSTRTC_ADDR_CFG      (HOST_SUBSYS_BASE + 0x40)
#define HOST_PCIE_PHY_RST_CFG (HOST_SUBSYS_BASE + 0x190)
#define CFG_RST_FIX_ENABLE    (HOST_SUBSYS_BASE + 0x194)

#define PCIE0_MISC_BASE         (0x08B21000)
#define PCIE0_CTRL0             (PCIE0_MISC_BASE + 0x0)
#define PCIE0_CTRL2_REG         (PCIE0_MISC_BASE + 0x8)
#define PCIE0_CTRL7             (PCIE0_MISC_BASE + 0x1c)
#define PCIE0_CTRL8             (PCIE0_MISC_BASE + 0x20)
#define PCIE0_CTRL10            (PCIE0_MISC_BASE + 0x28)
#define PCIE0_CTRL11            (PCIE0_MISC_BASE + 0x2c)
#define PCIE0_PHYCTL0           (PCIE0_MISC_BASE + 0x30)
#define PCIE0_PHYCTL1           (PCIE0_MISC_BASE + 0x34)
#define PCIE0_PHYCTL2           (PCIE0_MISC_BASE + 0x38)
#define PCIE0_ECC_INT           (PCIE0_MISC_BASE + 0x50)
#define PCIE0_LNKDN_INT         (PCIE0_MISC_BASE + 0x70)
#define PCIE0_STATUS4           (PCIE0_MISC_BASE + 0x94)
#define PCIE0_PHYSTAT0          (PCIE0_MISC_BASE + 0xB0)

#define PCIE1_MISC_BASE         (0x0CD0E000)
#define PCIE1_CTRL0             (PCIE1_MISC_BASE + 0x0)
#define PCIE1_CTRL2_REG         (PCIE1_MISC_BASE + 0x8)
#define PCIE1_CTRL7             (PCIE1_MISC_BASE + 0x1c)
#define PCIE1_CTRL8             (PCIE1_MISC_BASE + 0x20)
#define PCIE1_CTRL10            (PCIE1_MISC_BASE + 0x28)
#define PCIE1_CTRL11            (PCIE1_MISC_BASE + 0x2c)
#define PCIE1_PHYCTL0           (PCIE1_MISC_BASE + 0x30)
#define PCIE1_PHYCTL1           (PCIE1_MISC_BASE + 0x34)
#define PCIE1_PHYCTL2           (PCIE1_MISC_BASE + 0x38)
#define PCIE1_ECC_INT           (PCIE1_MISC_BASE + 0x50)
#define PCIE1_LNKDN_INT         (PCIE1_MISC_BASE + 0x70)
#define PCIE1_STATUS0           (PCIE1_MISC_BASE + 0x80)
#define PCIE1_STATUS4           (PCIE1_MISC_BASE + 0x94)
#define PCIE1_PHYSTAT0          (PCIE1_MISC_BASE + 0xB0)

#define PCIE0_DM_BASE           (0x08B10000)
#define PCIE0_CMD_REG           (PCIE0_DM_BASE + 0x4)
#define PCIE0_MSI_CAP           (PCIE0_DM_BASE + 0x50)
#define PCIE1_DM_BASE           (0x0CD10000)
#define PCIE1_CFG_BAR0BASE      (PCIE1_DM_BASE + 0x10)

typedef union {
    struct {
        u32 pcs_tx_swing_low : 7; /* bit0 ~ bit6 */
        u32 pcs_tx_swing_full : 7; /* bit7 ~ bit13 */
        u32 pcs_tx_deemph_gen2_6db : 6; /* bit14 ~ bit19 */
        u32 pcs_tx_deemph_gen2_3p5db : 6; /* bit20 ~ bit25 */
        u32 pcs_tx_deemph_gen1 : 6; /* bit26 ~ bit31 */
    } bits;

    unsigned int val;
} U_PCIE_PHYCTL0;

typedef struct {
    u32 bar_mask[PCIE_MAX_BAR_CNT];
    u32 bar_addr_mode;
} PCIE_RES;

/* gen1_deemph gen2_3p5db_deemph gen2_6db_deemph TxAmplitudefull TxAmplitudelow Rxeq */
typedef struct {
    u32 phy_parama[PCIE_MAX_PHY_ARGS_CNT];
} PCIE_PARAMA;


PCIE_RES g_pcie_res;
PCIE_PARAMA g_pcie0_param;
PCIE_PARAMA g_pcie1_param;
u32 g_pcie1_linkstatus;

#define MAX_PARAM_LENGTH 35
typedef struct{
    char name[MAX_PARAM_LENGTH];
    int  val;
}env_param;

env_param g_env_param_default_list[] = {
    {"pcie1flag",0},
    {"usbTxPreemph",0x15},
    {"usbTxAmplitude",0x7f},
    {"usbRxEq",0x3},
    {"sgmii0TxPreemph",0x13},
    {"sgmii0TxAmplitude",0x7f},
    {"sgmii0RxEq",0x0},
    {"sgmii1TxPreemph",0x13},
    {"sgmii1TxAmplitude",0x7f},
    {"sgmii1RxEq",0x0},
    {"dac0gc",0x1},
    {"dac1gc",0x1},
    {"dac2gc",0x1},
    {"pcie0TxPreemph",0x15},
    {"pcie0TxPreemph3p5",0x15},
    {"pcie0TxPreemph6",0x15},
    {"pcie0TxAmplitudefull",0x7f},
    {"pcie0TxAmplitudelow",0x7f},
    {"pcie0RxEq",0x0},
    {"pcie1TxPreemph",0x15},
    {"pcie1TxPreemph3p5",0x15},
    {"pcie1TxPreemph6",0x15},
    {"pcie1TxAmplitudefull",0x7f},
    {"pcie1TxAmplitudelow",0x7f},
    {"pcie1RxEq",0x0},
    {"pcie1bar0",0x3ffffff},
    {"pcie1bar1",0xfffff},
    {"pcie1bar2",0xffffff},
    {"pcie1bar3",0},
    {"pcie1bar4",0},
    {"pcie1bar5",0},
    {"usb2phyctrl4",0x9173c},
    {"pcie1baraddrmode",0x32},
    {"bmcUartPort",0x2},
    {"rsvDramSize",0x0}
};


int env_param_get_value(u8 *name, u32 len, u32 *val) {
    if (val == NULL || name == NULL) {
        return -1;
    }
    for (int i = 0; i < sizeof(g_env_param_default_list); i++) {
        if (strcmp(g_env_param_default_list[i].name, name) == 0) {
            *val = g_env_param_default_list[i].val;
            return 0;
        }
    }
    return -1;
}

int env_param_set_value(u8 *name, u32 len, u8 val) {
    if (name == NULL) {
        return -1;
    }
    for (int i = 0; i < sizeof(g_env_param_default_list); i++) {
        if (strcmp(g_env_param_default_list[i].name, name) == 0) {
            g_env_param_default_list[i].val = val;
            return 0;
        }
    }
    return -1;
}
#define BIT_VALUE(val, offset) (((val) >> (offset)) & 0x1)
#define SYSCTL_STRAP_STATUS_BASE    (SYSCTL_REG_BASE + 0x4)
#define SYSCTL_INITIAL_INF_REG2     (SYSCTL_REG_BASE + 0x7c)
#define SYSCTL_INITIAL_INF_REG3     (SYSCTL_REG_BASE + 0x80)

static u32 pcie_get_platform_info(void)
{
    return board_info_platform_id_get();
}

static void pcie_get_phy0_param(void)
{
    u32 i;
    u32 val = 0;
    u32 ret;
    u8 env0_name[PCIE_MAX_PHY_ARGS_CNT][PCIE_PHY_ARGS_ENV_NAME_LEN] = {
        "pcie0TxPreemph", "pcie0TxPreemph3p5",
        "pcie0TxPreemph6", "pcie0TxAmplitudefull",
        "pcie0TxAmplitudelow", "pcie0RxEq"
    };
    u8 phy_paramdef[PCIE_MAX_PHY_ARGS_CNT] = { 0x15, 0x15, 0x15, 0x7F, 0x7F, 0x0 };

    for (i = 0; i < PCIE_MAX_PHY_ARGS_CNT; i++) {
        ret = env_param_get_value(env0_name[i], PCIE_PHY_ARGS_ENV_NAME_LEN + 1, &val);
        if (ret) {
            g_pcie0_param.phy_parama[i] = phy_paramdef[i];
        } else {
            g_pcie0_param.phy_parama[i] = val;
        }
    }
}

static void pcie_get_phy1_param(void)
{
    u32 i;
    u32 val = 0;
    u32 ret;

    u8 env1_name[PCIE_MAX_PHY_ARGS_CNT][PCIE_PHY_ARGS_ENV_NAME_LEN] = {
        "pcie1TxPreemph", "pcie1TxPreemph3p5",
        "pcie1TxPreemph6", "pcie1TxAmplitudefull",
        "pcie1TxAmplitudelow", "pcie1RxEq"
    };
    u8 phy_paramdef[PCIE_MAX_PHY_ARGS_CNT] = { 0x15, 0x15, 0x15, 0x7F, 0x7F, 0x0 };

    for (i = 0; i < PCIE_MAX_PHY_ARGS_CNT; i++) {
        ret = env_param_get_value(env1_name[i], PCIE_PHY_ARGS_ENV_NAME_LEN + 1, &val);
        if (ret) {
            g_pcie1_param.phy_parama[i] = phy_paramdef[i];
        } else {
            g_pcie1_param.phy_parama[i] = val;
        }
    }
}

static u32 get_kvm_type(void)
{
    u32 initial_info = readl(SYSCTL_INITIAL_INF_REG);

    /* pf_15->boot_cfg30 */
    u32 pcie0_state = BIT_VALUE(initial_info, 30);

    return pcie0_state;
}


static void set_pcie_inited_state(void)
{
    sys_init_flag_set(PCIE_INIT_MASK, PCIE_INIT_DONE);
}

static u32 get_pcie_init_state(void)
{
    if (sys_init_flag_check(PCIE_INIT_DONE)) {
        return 1;
    }

    return 0;
}

/* pcie0 pcie1 串行 */
static void pcie0_serdes_tuning(void)
{
    u32 val;

    /* 1. derset phy */
    val = readl(SYSCTL_PROBE_CTRL);
    val |= 0x1U << PCIE0_PHY_RST_OFFSET;
    writel(val, SYSCTL_PROBE_CTRL);

    /* 2. derset pcie core */
    writel(0x1, HOST_RST_DISABLE);

    /* 3. able rtune req */
    val = readl(PCIE0_PHYCTL2);
    val |= 0x1U << PCIE_PHY_RTUNE_REQ_OFFSET;
    writel(val, PCIE0_PHYCTL2);

    udelay(DELAY_US_SERDES);
    val = readl(PCIE0_PHYSTAT0);

    /* 4. disable rtune req */
    val = readl(PCIE0_PHYCTL2);
    val &= ~(0x1U << PCIE_PHY_RTUNE_REQ_OFFSET);
    writel(val, PCIE0_PHYCTL2);

    /* 5. rset pcie core */
    writel(0x1, HOST_RST_ENABLE);

    /* 6. reset phy */
    val = readl(SYSCTL_PROBE_CTRL);
    val &= ~(0x1U << PCIE0_PHY_RST_OFFSET);
    writel(val, SYSCTL_PROBE_CTRL);
}

static void set_pcie_ltssm(u32 portid, u32 en)
{
    u32 val;
    u32 tmp;
    phys_addr_t paddr;

    if (portid == 0) {
        paddr = PCIE0_CTRL7;
    } else {
        paddr = PCIE1_CTRL7;
    }

    val = readl(paddr);
    tmp = val;
    if (en) {
        val |= 0x1 << PCIE_APP_LTSSM_ENABLE_OFFSET;
    } else {
        val &= ~(0x1 << PCIE_APP_LTSSM_ENABLE_OFFSET);
    }
    if (tmp != val) {
        writel(val, paddr);
    }
}

static void phy_si_config(u8 core)
{
    U_PCIE_PHYCTL0 phyctrl0;
    u32 pcie_rxeq;

    if (core == 0) {
        phyctrl0.bits.pcs_tx_deemph_gen1 = g_pcie0_param.phy_parama[0];
        phyctrl0.bits.pcs_tx_deemph_gen2_3p5db = g_pcie0_param.phy_parama[1];
        phyctrl0.bits.pcs_tx_deemph_gen2_6db = g_pcie0_param.phy_parama[2];
        phyctrl0.bits.pcs_tx_swing_full = g_pcie0_param.phy_parama[3];
        phyctrl0.bits.pcs_tx_swing_low = g_pcie0_param.phy_parama[4];
        pcie_rxeq = g_pcie0_param.phy_parama[5];
        writel(phyctrl0.val, PCIE0_PHYCTL0);
        phyctrl0.val = readl(PCIE0_PHYCTL2);
        phyctrl0.val &= ~(0x7 << PHY_RX0_EQ_OFFSET);
        phyctrl0.val |= (pcie_rxeq << PHY_RX0_EQ_OFFSET);
        writel(phyctrl0.val, PCIE0_PHYCTL2);
    } else {
        phyctrl0.bits.pcs_tx_deemph_gen1 = g_pcie1_param.phy_parama[0];
        phyctrl0.bits.pcs_tx_deemph_gen2_3p5db = g_pcie1_param.phy_parama[1];
        phyctrl0.bits.pcs_tx_deemph_gen2_6db = g_pcie1_param.phy_parama[2];
        phyctrl0.bits.pcs_tx_swing_full = g_pcie1_param.phy_parama[3];
        phyctrl0.bits.pcs_tx_swing_low = g_pcie1_param.phy_parama[4];
        pcie_rxeq = g_pcie1_param.phy_parama[5];
        writel(phyctrl0.val, PCIE1_PHYCTL0);
        phyctrl0.val = readl(PCIE1_PHYCTL2);
        phyctrl0.val &= ~(0x7 << PHY_RX0_EQ_OFFSET);
        phyctrl0.val |= (pcie_rxeq << PHY_RX0_EQ_OFFSET);
        writel(phyctrl0.val, PCIE1_PHYCTL2);
    }
}

static void pcie0_linkdown_deal(void)
{
    u32 val0;
    u32 val1;

    /* clean all int status */
    val0 = readl(PCIE0_LNKDN_INT);
    writel(0xffffffff, PCIE0_CTRL11);
    writel(0, PCIE0_CTRL11);
    val1 = readl(PCIE0_ECC_INT);
    writel(val1, PCIE0_ECC_INT);

}

static void pcie0_serdes_args_cfg(void)
{
    u32 val;
    u32 tmp;

    writel(PCIE0_RST_POS_MASK, SYSCTL_SYS_INT_STAUTS);

    /* 1. dereset pcie0 & phy0 */
    writel(0x81, HOST_RST_DISABLE);

    /* 2. reset phy */
    val = readl(SYSCTL_PROBE_CTRL);
    val &= ~0x40000000;
    writel(val, SYSCTL_PROBE_CTRL);

    /* 3. disable ltssm */
    val = readl(PCIE0_CTRL7);
    tmp = val;
    val &= ~(0x1 << PCIE_APP_LTSSM_ENABLE_OFFSET);
    writel(val, PCIE0_CTRL7);

    /* 4. reset pipe0 */
    val = readl(HOST_PCIE_PHY_RST_CFG);
    val |= 0x1;
    writel(val, HOST_PCIE_PHY_RST_CFG);

    val = readl(PCIE0_CTRL8);
    val |= 0x1 << POWER_UP_RST_N_EN_OFFSET;
    writel(val, PCIE0_CTRL8);

    /* 5. combophy */
    phy_si_config(0);

    udelay(DELAY_US_SERDES);

    /* 6. derset phy */
    val = readl(SYSCTL_PROBE_CTRL);
    val |= 0x40000000;
    writel(val, SYSCTL_PROBE_CTRL);

    /* 7. derest pipe0 */
    val = readl(HOST_PCIE_PHY_RST_CFG);
    val |= 0x1;
    writel(val, HOST_PCIE_PHY_RST_CFG);

    val = readl(PCIE0_CTRL8);
    val &= ~(0x1 << POWER_UP_RST_N_EN_OFFSET);
    writel(val, PCIE0_CTRL8);

    val = readl(HOST_PCIE_PHY_RST_CFG);
    val &= ~0x1;
    writel(val, HOST_PCIE_PHY_RST_CFG);

    /* 8. restore ltssm */
    writel(tmp, PCIE0_CTRL7);
}

void pcie0_rst_pos_isr(void)
{
    pcie0_serdes_tuning();
    pcie0_serdes_args_cfg();
}

static void config_pcie0_en(u32 bmc_type)
{
    /* config PCIe0 */
    if (bmc_type == TYPE_KVM) {
        /* dereset pcie0 & phy0 */
        writel(0x81, HOST_RST_DISABLE);
        pcie0_rst_pos_isr();
    } else {
        /* reset pcie0 & phy0 */
        writel(0x81, HOST_RST_ENABLE);
    }
}

static u32 pcie1_get_flag_val(void)
{
    u32 val = 0;
    u32 ret;
    u8 env_name[] = "pcie1flag";

    ret = env_param_get_value(env_name, sizeof(env_name), &val);
    if (!ret) {
        return val;
    }
    /* return default value */
    return 0;
}

static void pcie1_set_flag_val(u32 on) /* u8 *val */
{
    u8 env_name[] = "pcie1flag";

    env_param_set_value(env_name, sizeof(env_name), on ? 1 : 0);
}

/* Get ltssm store in A55_ASSERT_INFO0, which is power on reset!! */
static u32 get_pcie1_ltssm_state_porn(void)
{
    if (sys_init_flag_check(PCIE1_LTSSM_ON)) {
        return 1;
    }

    return 0;
}

/* Set ltssm store in A55_ASSERT_INFO0, which is power on reset!! */
static void set_pcie1_ltssm_state_porn(u32 on)
{
    if (on) {
        sys_init_flag_set(PCIE1_LTSSM_MASK, PCIE1_LTSSM_ON);
    } else {
        sys_init_flag_set(PCIE1_LTSSM_MASK, 0);
    }
}

static void config_pcie1_ltssm(void)
{
    u32 platform_id;
    u32 pcie1flag = 0xff;

    /* dereset pcie1 &phy1 */
    writel(0x102, HOST_RST_DISABLE);

    /* config PCIe1 */
    platform_id = pcie_get_platform_info();
    if ((platform_id == BOARD_PLATM_PANGEA_V5) || (platform_id == BOARD_PLATM_PANGEA_V6) ||
        (platform_id == BOARD_PLATM_BUCKLE_PANGEA)) {
        pcie1_set_flag_val(1);
        set_pcie1_ltssm_state_porn(1);
        set_pcie_ltssm(1, 1);
    } else {
        pcie1flag = pcie1_get_flag_val();
        if (pcie1flag) {
            set_pcie1_ltssm_state_porn(1);
            set_pcie_ltssm(1, 1);
        } else {
            set_pcie1_ltssm_state_porn(0);
            set_pcie_ltssm(1, 0);
        }
    }
}

static u32 pcie1_verify_bar_size(u32 bar_id, u32 shift)
{
    u32 shift_def[PCIE_MAX_BAR_CNT] = {
        0x3ffffff, /* 64MB */
        0xfffff,   /* 1MB */
        0xffffff,  /* 16MB */
        0,         /* 0MB */
        0,         /* 0MB */
        0,         /* 0MB */
    };

    if ((shift > 27) || (shift < 20)) {
        return shift_def[bar_id];
    } else {
        return ((1U << shift) - 1);
    }
}

static bool pcie1_is_bar_addr_mode_valid(u32 addr_mode)
{
    if ((addr_mode != PCIE_BAR_ADDR_64BIT) && (addr_mode != PCIE_BAR_ADDR_32BIT)) {
        return false;
    }

    return true;
}

static void pcie1_get_bar_res(void)
{
    u32 i;
    u32 val = 0;
    u32 ret;
    u8 env_bar_mask[PCIE_MAX_BAR_CNT][PCIE_BAR_ENV_NAME_LEN] = {
        "pcie1bar0", "pcie1bar1", "pcie1bar2",
        "pcie1bar3", "pcie1bar4", "pcie1bar5"
    };
    u8 env_bar_addr[] = "pcie1baraddrmode";

    for (i = 0; i < PCIE_MAX_BAR_CNT; i++) {
        ret = env_param_get_value(env_bar_mask[i], PCIE_BAR_ENV_NAME_LEN, &val);
        if (ret) {
            g_pcie_res.bar_mask[i] = pcie1_verify_bar_size(i, SHIFT_INVAL);
        } else {
            g_pcie_res.bar_mask[i] = pcie1_verify_bar_size(i, val);
        }
    }

    ret = env_param_get_value(env_bar_addr, sizeof(env_bar_addr), &val);
    /* make sure hardware has a default val to set */
    if (ret || !pcie1_is_bar_addr_mode_valid(val)) {
        g_pcie_res.bar_addr_mode = PCIE_BAR_ADDR_32BIT;
        return;
    }

    g_pcie_res.bar_addr_mode = val;

}

static void pcie1_serdes_args_cfg(void)
{
    u32 val;
    u32 ltssm_flag;

    writel(PCIE1_RST_POS_MASK, SYSCTL_SYS_INT_STAUTS);

    /* 1. dereset pcie1 &phy1 */
    writel(0x102, HOST_RST_DISABLE);

    /* 2. reset phy */
    val = readl(SYSCTL_PROBE_CTRL);
    val &= ~0x80000000;
    writel(val, SYSCTL_PROBE_CTRL);

    /* 3. disable ltssm */
    val = readl(PCIE1_CTRL7);
    val &= ~(0x1 << PCIE_APP_LTSSM_ENABLE_OFFSET);
    writel(val, PCIE1_CTRL7);

    /* 4. reset pipe1 */
    val = readl(HOST_PCIE_PHY_RST_CFG);
    val |= 0x2;
    writel(val, HOST_PCIE_PHY_RST_CFG);

    val = readl(PCIE1_CTRL8);
    val |= 0x1 << POWER_UP_RST_N_EN_OFFSET;
    writel(val, PCIE1_CTRL8);

    /* 5. combophy */
    phy_si_config(1);

    udelay(DELAY_US_SERDES);

    /* 6. derset phy */
    val = readl(SYSCTL_PROBE_CTRL);
    val |= 0x80000000;
    writel(val, SYSCTL_PROBE_CTRL);

    /* 7. derest pipe1 */
    val = readl(HOST_PCIE_PHY_RST_CFG);
    val |= 0x1 << 1;
    writel(val, HOST_PCIE_PHY_RST_CFG);

    val = readl(PCIE1_CTRL8);
    val &= ~(0x1 << POWER_UP_RST_N_EN_OFFSET);
    /* disable credit protect */
    val |= (0x1 << PCIE_NO_CREDIT_PRO_DIS_OFFSET);
    writel(val, PCIE1_CTRL8);

    val = readl(HOST_PCIE_PHY_RST_CFG);
    val &= ~(0x1 << 1);
    writel(val, HOST_PCIE_PHY_RST_CFG);

    /* 8. restore ltssm */
    val = readl(PCIE1_CTRL7);
    ltssm_flag = get_pcie1_ltssm_state_porn();
    if (ltssm_flag) {
        val |= (0x1 << PCIE_APP_LTSSM_ENABLE_OFFSET);
    } else {
        val &= ~(0x1 << PCIE_APP_LTSSM_ENABLE_OFFSET);
    }
    writel(val, PCIE1_CTRL7);

}


static void pcie1_linkdown_deal(void)
{
    u32 val0;
    u32 val1;

    /* clean all int status */
    val0 = readl(PCIE1_LNKDN_INT);
    writel(0xffffffff, PCIE1_CTRL11);
    writel(0, PCIE1_CTRL11);
    val1 = readl(PCIE1_ECC_INT);
    writel(val1, PCIE1_ECC_INT);

}

void set_gpu_dfx_en(void)
{
    writel(0x4001, GPU_DFX_CFG_0);
    writel(0x40, GPU_DFX_CFG_1);
}

void hi309x_pcie_init(void)
{
    u32 reg_val;
    u32 inited;
    u32 bmc_type;
    
    pcie_get_phy0_param();
    pcie_get_phy1_param();
    set_gpu_dfx_en();
    pcie1_get_bar_res();

    bmc_type = get_kvm_type();
    
    inited = get_pcie_init_state();
    if (!inited) {
        config_pcie0_en(bmc_type);
        config_pcie1_ltssm();
        pcie1_serdes_args_cfg();
        set_pcie_inited_state();
    }

    /* clear int */
    writel((PCIE0_RST_POS_MASK | PCIE1_RST_POS_MASK), SYSCTL_SYS_INT_STAUTS);

    /* enable pcie dereset interrupt */
    reg_val = readl(SYSCTL_SYS_INT_MASK);
    reg_val &= ~(PCIE0_RST_POS_MASK | PCIE1_RST_POS_MASK);
    writel(reg_val, SYSCTL_SYS_INT_MASK);
    pcie0_linkdown_deal();
    pcie1_linkdown_deal();
    printf("hi309x_pcie_init OK\n");
}
