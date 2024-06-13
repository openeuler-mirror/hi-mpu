// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * dfx interface header files
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _DFX_USR_API_H_
#define _DFX_USR_API_H_

#define DEV_DFX_NAME  "/dev/dfx"
#define DFX_IOC_MAGIC 'd'

/* Use the standard ioctl command */
#define DFX_CMD_RESERVED1                   _IOR(DFX_IOC_MAGIC, 1, int)
#define DFX_CMD_GET_DIEID                   _IOR(DFX_IOC_MAGIC, 2, int)
#define DFX_CMD_GET_PLL_LOCK_STATUS         _IOR(DFX_IOC_MAGIC, 3, int)
#define DFX_CMD_RESERVED4                   _IOR(DFX_IOC_MAGIC, 4, int)
#define DFX_CMD_GET_PEIE0_STATUS            _IOR(DFX_IOC_MAGIC, 5, int)
#define DFX_CMD_GET_PEIE1_STATUS            _IOR(DFX_IOC_MAGIC, 6, int)
#define DFX_CMD_GET_PEIE_SERDES_LOCK_STATUS _IOR(DFX_IOC_MAGIC, 7, int)
#define DFX_CMD_GET_GMAC_SERDES_LOCK_STATUS _IOR(DFX_IOC_MAGIC, 8, int)
#define DFX_CMD_GET_RESET_INFO              _IOR(DFX_IOC_MAGIC, 9, int)
#define DFX_CMD_GET_DDR_ECC_ENABLED_CHECK   _IOR(DFX_IOC_MAGIC, 10, int)
#define DFX_CMD_GET_DDR_ECC_ERR_COUNT       _IOR(DFX_IOC_MAGIC, 11, int)
#define DFX_CMD_GET_DDR_ECC_ERR_STATUS      _IOR(DFX_IOC_MAGIC, 12, int)
#define DFX_CMD_GET_GMAC_ERR_STATUS         _IOR(DFX_IOC_MAGIC, 13, int)
#define DFX_CMD_GET_GMAC_SHORT_RUNTS_THR    _IOR(DFX_IOC_MAGIC, 14, int)
#define DFX_CMD_GET_GMAC_MAX_FRM_SIZE       _IOR(DFX_IOC_MAGIC, 15, int)
#define DFX_CMD_GET_USB_DEV_DSTS            _IOR(DFX_IOC_MAGIC, 16, int)
#define DFX_CMD_GET_USB_HOST_PORTSC         _IOR(DFX_IOC_MAGIC, 17, int)
#define DFX_CMD_GET_VCE_ENVC_FRM_ERR_TYPE   _IOR(DFX_IOC_MAGIC, 18, int)
#define DFX_CMD_DYING_SAYINGS               _IOR(DFX_IOC_MAGIC, 19, int)
#define DFX_CMD_SEND_HOST_LASTWORD          _IOW(DFX_IOC_MAGIC, 20, int)
#define DFX_CMD_SERDES_PRBS_LOOPBACK        _IOW(DFX_IOC_MAGIC, 21, dfx_serdes_prbs_s)
#define DFX_CMD_GET_SERDES_EYE_PATTERN      _IOR(DFX_IOC_MAGIC, 22, dfx_serdes_eye_pattern_s)
#define DFX_CMD_SERDES_CR_WRITE             _IOW(DFX_IOC_MAGIC, 23, dfx_serdes_cr_rdwr_s)
#define DFX_CMD_SERDES_CR_READ              _IOR(DFX_IOC_MAGIC, 24, dfx_serdes_cr_rdwr_s)
#define DFX_CMD_SERDES_PRBS_START           _IOW(DFX_IOC_MAGIC, 25, dfx_serdes_prbs_s)
#define DFX_CMD_SERDES_PRBS_STOP            _IOW(DFX_IOC_MAGIC, 26, dfx_serdes_prbs_s)
#define DFX_CMD_SERDES_LOOPBACK             _IOW(DFX_IOC_MAGIC, 27, dfx_serdes_loopback_s)

#define DFX_CMD_MAX_NR       27
#define DFX_DIEID_MAX_NUM    12

#define MAX_GMAC_NUM         4
#define MAX_USB_NUM          4

typedef struct {
    unsigned int module;
    unsigned int id;
} dfx_serdes_module_s;

typedef struct {
    dfx_serdes_module_s module;
    unsigned int type;
    unsigned int direction;
} dfx_serdes_prbs_s;

typedef struct {
    dfx_serdes_module_s module;
    unsigned int loop;
} dfx_serdes_loopback_s;

typedef struct {
    dfx_serdes_module_s module;
    unsigned int address;
    unsigned int value;
} dfx_serdes_cr_rdwr_s;

typedef struct {
    dfx_serdes_module_s module;
    unsigned int ph_inc;
    unsigned int dac_inc;
    unsigned int scope_sel;
    void *buffer;
    int x, y;
    unsigned int length;
} dfx_serdes_eye_pattern_s;

enum {
    DFX_SERDES_PCIE = 0,
    DFX_SERDES_GMAC,
    DFX_SERDES_USB
};

enum {
    DFX_SERDES_PRBS31 = 1,
    DFX_SERDES_PRBS23,
    DFX_SERDES_PRBS15,
    DFX_SERDES_PRBS7,
};

enum {
    DFX_SERDES_PRBS_DIRECTION_RX = 0,
    DFX_SERDES_PRBS_DIRECTION_TX,
    DFX_SERDES_PRBS_DIRECTION_BOTH,
};

enum ECC_RESULT {
    ECC_OK = 0,
    ECC_SINGLE_BIT_ERROR,
    ECC_MULTI_BIT_ERROR,
    ECC_NONE
};

enum ECC_ERR_RESULT {
    ECC_ERR_OK = 0,
    ECC_ERR_SINGLE_BIT_ERROR = 0x1,
    ECC_ERR_MULTI_BIT_ERROR = 0x2,
    ECC_ERR_SINGLE_MULTI_BIT_ERROR = (ECC_ERR_SINGLE_BIT_ERROR) | (ECC_ERR_MULTI_BIT_ERROR),
    ECC_ERR_NONE
};

enum PLL_LOCK_STATUS {
    PLL_UNLOCKED = 0,
    PLL_LOCKED = 1,
    PLL_NONE
};

#define PCIE_LINKUP (0x11)

enum SERDES_READY_STATUS {
    SERDES_UNREADY = 0,
    SERDES_READY = 1,
    SERDES_NONE
};

enum SERDES_PLL_LOCK_STATUS {
    SERDES_PLL_UNLOCKED = 0,
    SERDES_PLL_LOCKED = 1,
    SERDES_PLL_NONE
};

enum BMC_RESET_TYPE {
    CHIP_RESET = 0,
    BMC_RESET = 1,
    BMC_RESET_TYPE_NONE
};

enum HOST_RESET_STATUS {
    HOST_RESETING = 0,
    HOST_WORKING = 1,
    HOST_RESET_STATUS_NONE
};

enum BMC_LAST_RESET_TYPE {
    RESET_TYPE_POWER_UP = 0x1,
    RESET_TYPE_EXT_WDT_BUTTON = 0x2,
    RESET_TYPE_INNER_WDT = 0x4,
    RESET_TYPE_NONE
};

enum DDR_ECC_CHECK {
    DDR_ECC_DISABLED = 0,
    DDR_ECC_ENABLED = 1,
    DDR_ECC_CHECK_NONE
};

enum DDRC_INIT_CHECK {
    DDRC_INIT_WITHOUT_ECC = 0,
    DDRC_INIT_WITH_ECC = 1,
    DDRC_INIT_NONE
};

enum DDRC_SR_CHECK  {
    DDRC_SR_STOP = 0,
    DDRC_SR_WORKING = 1,
    DDRC_SR_NONE
};

enum USB_DEVICE_ERR {
    USB_DEVICE_WITHOUT_ERRTICERR = 0,
    USB_DEVICE_ERRTICERR = 1,
    USB_DEVICE_ERRTICERR_NONE
};

enum USB_HOST_OCA_CHECK {
    USB_HOST_WITHOUT_OCA = 0,
    USB_HOST_OCA = 1,
    USB_HOST_OCA_CHECK_NONE
};

enum VCE_ENVC_VSYNC_CHECK {
    VCE_ENVC_VSYNC_OK = 0,
    VCE_ENVC_VSYNC_LOST = 1,
    VCE_ENVC_VSYNC_NONE
};

typedef struct {
    /* DIE ID */
    unsigned int die_id[DFX_DIEID_MAX_NUM];
} dfx_die_id_s;

/* PLL Lock Status */
typedef union {
    struct {
        unsigned int cpll_lock : 1;
        unsigned int pll0_lock : 1;
        unsigned int pll1_lock : 1;
        unsigned int pll2_lock : 1;
        unsigned int reserved  : 28;
    } bits;

    u32 val;
} dfx_pll_lock_sts_s;

/* Reset status indication and statistics */
typedef union {
    struct {
        u32 pcie0_rst_cnt       : 4;
        u32 pcie1_rst_cnt       : 4;
        u32 bmc_unsec_rst_cnt   : 4;
        u32 bmc_rst_cnt         : 4;
        u32 host_rst_cnt        : 4;
        u32 chip_rst_cnt        : 4;
        u32 pcie0_rst_sts       : 1;
        u32 pcie1_rst_sts       : 1;
        u32 bmc_unsec_rst_sts   : 1;
        u32 bmc_rst_sts         : 1;
        u32 host_rst_sts        : 1;
        u32 chip_rst_sts        : 1;
        u32 rsv                 : 2;
    } bits;

    u32 val;
} dfx_rst_info_s;

typedef struct {
    unsigned int gsds_id; /* serdes ID */
    union {
        unsigned int value;
        struct {
            unsigned int tx0_state              : 1;
            unsigned int tx0_detect_rx_result   : 1;
            unsigned int tx0_detect_rx_ack      : 1;
            unsigned int tx0_cm_state           : 1;
            unsigned int rx0_pll_state          : 1;
            unsigned int mpll_state             : 1;
            unsigned int rx_valid               : 1;
            unsigned int rx_los                 : 1;
            unsigned int reserved               : 24;
        } reg;
    } serdes;
} dfx_gmac_serdes_lock_u;

/*
 * Indicates the number of received and transmitted errors of the GMAC module.
 * Note that this register is cleared by reading.
 */
typedef struct {
    unsigned int id;
    unsigned int rx_fcs_errors;
    unsigned int rx_data_err;
    unsigned int rx_short_err_cnt;
    unsigned int tx_excessive_length_drop;
    unsigned int tx_underrun;
    unsigned int tx_crc_error;
} dfx_gmac_err_count_s;

typedef struct {
    unsigned int id;
    union {
        unsigned int value;
        struct {
            unsigned int short_runts_thr    : 5;
            unsigned int reserved           : 27;
        } reg;
    } short_runts_thr;
} dfx_gmac_short_runts_thr_s;

typedef struct  {
    unsigned int id;
    union {
        unsigned int value;
        struct {
            unsigned int max_frm_size   : 16;
            unsigned int reserved       : 16;
        } reg;
    } max_frm_size_u;
} dfx_gmac_max_frm_size_s;

typedef union  {
    unsigned int value;
    struct {
        unsigned int exclusive          : 1;
        unsigned int rd_wrap_split_en   : 1;
        unsigned int ecc_rwm_en         : 1;
        unsigned int reserved0          : 5;
        unsigned int wr_rcv_mode        : 12;
        unsigned int ecc_rwm_en_other   : 11;
        unsigned int reserved           : 1;
    } reg;
} dfx_ddr_ecc_enable_u;

typedef struct {
    unsigned int single_ecc_count;
    unsigned int multi_ecc_count;
} dfx_ddr_ecc_err_count_s;

typedef struct {
    unsigned int usb_id;
    union {
        unsigned int value;
        struct {
            unsigned int connectedspd   : 3;  /* Connected Speed */
            unsigned int soffn          : 14; /* Frame/Microframe Number of the Received SOF */
            unsigned int rxfifoempty    : 1;  /* RxFIFO Empty */
            unsigned int usblnkst       : 4;  /* USB/Link State */
            unsigned int devctrlhlt     : 1;  /* Device Controller Halted */
            unsigned int coreidle       : 1;  /* Core Idle */
            unsigned int sss            : 1;  /* SSS Save State Statu */
            unsigned int rss            : 1;  /* RSS Restore State Status */
            unsigned int reserved0      : 2;
            unsigned int sre            : 1;  /* Save Restore Error */
            unsigned int dcnrd          : 1;  /* Device Controller Not Ready */
            unsigned int reserved1      : 2;
        } reg;
    } dsts;
} dfx_usb_dsts_s;

typedef struct {
    unsigned int usb_id;
    unsigned int port_id;
    union {
        unsigned int value;
        struct {
            unsigned int ccs        : 1;  /* Current Connect Status */
            unsigned int ped        : 1;  /* Port Enabled/Disabled */
            unsigned int reserved0  : 1;
            unsigned int oca        : 1;  /* Over-current Active */
            unsigned int pr         : 1;  /* Port Reset */
            unsigned int pls        : 4;  /* Port Link State */
            unsigned int pp         : 1;  /* Port Power */
            unsigned int port_speed : 4;  /* Port Speed */
            unsigned int pic        : 2;  /* Port Indicator Control */
            unsigned int lws        : 1;  /* Port Link State Write Strobe */
            unsigned int csc        : 1;  /* Connect Status Change */
            unsigned int pec        : 1;  /* Port Enabled/Disabled */
            unsigned int wrc        : 1;  /* Warm Port Reset Change */
            unsigned int occ        : 1;  /* Over-current Change */
            unsigned int prc        : 1;  /* Port Reset Change */
            unsigned int plc        : 1;  /* Port Link State Change */
            unsigned int cec        : 1;  /* Port Config Error */
            unsigned int cas        : 1;  /* Conld Attach Status */
            unsigned int wce        : 1;  /* Wake on Connect Enable */
            unsigned int wde        : 1;  /* Wake on Disconnect */
            unsigned int woe        : 1;  /* Wake on Over-current */
            unsigned int reserved1  : 2;
            unsigned int dr         : 1;  /* Device Removable */
            unsigned int wpr        : 1;  /* Warm Port Reset */
        } reg;
    } portsc;
} dfx_usb_xhci_portsc_s, dfx_usb_host_portsc_s;

typedef union { /* Video signal loss detection */
    u32 value;
    struct {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int unused1            : 30;
        unsigned int envc_vsync_lost    : 1;
        unsigned int unused0            : 1;
#else
        unsigned int unused0            : 1;
        unsigned int envc_vsync_lost    : 1;
        unsigned int unused1            : 30;
#endif
    } reg;
} dfx_vce_envc_frm_err_type;

typedef union { /* PCIe status detection */
    u32 value;
    struct {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int reserved           : 26;
        unsigned int xmlh_ltssm_state   : 6;
#else
        unsigned int xmlh_ltssm_state   : 6;
        unsigned int reserved           : 26;
#endif
    } reg;
} dfx_pcie_link_stat;


typedef union {
    u32 value;
    struct {
#ifdef CONFIG_CPU_BIG_ENDIAN
        unsigned int phy_mac_rxdata     : 16;
        unsigned int phy_mac_rxdatak    : 2;
        unsigned int phy_mac_rxvalid    : 1;
        unsigned int phy_mac_phystatus  : 1;
        unsigned int phy_mac_rxelecidle : 1;
        unsigned int phy_mac_rxstatus   : 3;
        unsigned int reserved           : 8;
#else
        unsigned int reserved           : 8;
        unsigned int phy_mac_rxstatus   : 3;
        unsigned int phy_mac_rxelecidle : 1;
        unsigned int phy_mac_phystatus  : 1;
        unsigned int phy_mac_rxvalid    : 1;
        unsigned int phy_mac_rxdatak    : 2;
        unsigned int phy_mac_rxdata     : 16;
#endif
    } reg;
} dfx_pcie_serdes_stat;

#endif
