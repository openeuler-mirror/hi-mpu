// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * sfc core opr
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef _SFC0_CORE_H_
#define _SFC0_CORE_H_
#include "sfc_addr_define.h"
#include "sfc_diff.h"
#include "spi351_ids.h"

#define sfc_section(str) (1)

#define SFC_3BYTE_ADDR    (3)
#define SFC_4BYTE_ADDR    (4)
#define SFC_MAX_ID_LEN    8
#define SFC_4BYTE_DISABLE 0

#define SFC0_INTR_MASK       (0x1e0) /* bit5-8 */
#define SFC0_ARB_CFG_MASK    (0x3)
#define SFC_DIS_ALL_INT_MASK (0x7)
#define SFC_PP_FAIL_MASK     (1UL << 2)
#define SFC_DMA_DONE_MASK    (1UL << 1)
#define SFC_CMD_OP_END_MASK  (1UL << 0)

#define SFC_BUSID0                   (0)
#define SFC_BUSID1                   (1)
#define SFC_CSID0                    (0)
#define SFC_CSID1                    (1)
#define SFC_CS0_USED                 BIT(0)
#define SFC_CS1_USED                 BIT(1)
#define SFC_CS_NOUSE                 (0xFFFFFF)
#define BYTES_TO_U32(v3, v2, v1, v0) (((u32)(v3) << 0) | ((u32)(v2) << 8) | ((u32)(v1) << 16) | ((u32)(v0) << 24))

#define HISFC350_GLOBAL_CONFIG_READ_DELAY(_n)    (((_n) & 0x03) << 3)
#define HISFC350_GLOBAL_CONFIG_WRITE_PROTECT     BIT(1)
#define HISFC350_GLOBAL_CONFIG_SPI_MODE3         BIT(0)
#define HISFC350_GLOBAL_CONFIG_ADDR_MODE_4B      BIT(2)
#define HISFC350_TIMING_TSHSL(_n)                ((_n) & 0xF)
#define HISFC350_TIMING_TCSS(_n)                 (((_n) & 0x7) << 8)
#define HISFC350_TIMING_TCSH(_n)                 (((_n) & 0x7) << 12)
#define HISFC_INT_STA_PP_FAIL                    BIT(2)
#define HISFC_INT_STA_DMA_RESP_ERR               BIT(3)
#define HISFC350_INT_RAW_STATUS_DMA_DONE         BIT(1)
#define HISFC350_INT_CLEAR_DMA_DONE              BIT(1)
#define HISFC350_BUS_DMA_CTRL_START              BIT(0)
#define HISFC350_BUS_DMA_CTRL_RW(_rw)            ((_rw) << 1)
#define HISFC350_BUS_DMA_CTRL_CS(_cs)            (((_cs) & 0x01) << 4)
#define HISFC350_BUS_DMA_LEN_DATA_CNT(_n)        (((_n) - 1) & 0x0FFFFFFF)
#define HISFC350_BUS_DMA_AHB_CTRL_INCR4_EN       BIT(0)
#define HISFC350_BUS_DMA_AHB_CTRL_INCR8_EN       BIT(1)
#define HISFC350_BUS_DMA_AHB_CTRL_INCR16_EN      BIT(2)
#define HISFC350_DMA_ALIGN_SIZE                  (256)
#define HISFC350_DMA_ALIGN_MASK                  (HISFC350_DMA_ALIGN_SIZE - 1)
#define HISFC350_DMA_MIN_SIZE                    (4)
#define HISFC350_DMA_MAX_SIZE                    (4096)
#define HISFC350_DMA_MAX_MASK                    (HISFC350_DMA_MAX_SIZE - 1)
#define HISFC350_BUS_CONFIG1_READ_EN             BIT(31)
#define HISFC350_BUS_CONFIG1_WRITE_EN            BIT(30)
#define HISFC350_BUS_CONFIG1_WRITE_INS(_n)       (((_n) & 0xFF) << 22)
#define HISFC350_BUS_CONFIG1_WRITE_DUMMY_CNT(_n) (((_n) & 0x7) << 19)
#define HISFC350_BUS_CONFIG1_WRITE_IF_TYPE(_n)   (((_n) & 0x7) << 16)
#define HISFC350_BUS_CONFIG1_READ_INS(_n)        (((_n) & 0xFF) << 8)
#define HISFC350_BUS_CONFIG1_READ_PREF_CNT(_n)   (((_n) & 0x3) << 6)
#define HISFC350_BUS_CONFIG1_READ_DUMMY_CNT(_n)  (((_n) & 0x7) << 3)
#define HISFC350_BUS_CONFIG1_READ_IF_TYPE(_n)    ((_n) & 0x7)
#define SPI_OPR_LOCK                             (0)
#define SPI_OPR_NOLOCK                           (0)
#define HISFC351_CMD_CONFIG_LOCK_FLASH(_n)       (((_n) & 0x01) << 20)
#define HISFC351_CMD_CONFIG_MEM_IF_TYPE(_n)      (((_n) & 0x07) << 17)
#define HISFC351_CMD_CONFIG_DATA_CNT(_n)         ((((_n) - 1) & 0xFF) << 9)
#define HISFC351_CMD_CONFIG_RW_READ              BIT(8)
#define HISFC351_CMD_CONFIG_DATA_EN              BIT(7)
#define HISFC351_CMD_CONFIG_DUMMY_CNT(_n)        (((_n) & 0x07) << 4)
#define HISFC351_CMD_CONFIG_ADDR_EN              BIT(3)
#define HISFC351_CMD_CONFIG_SEL_CS(_cs)          (((_cs) & 0x01) << 1)
#define HISFC351_CMD_CONFIG_START                BIT(0)
#define HISFC350_CMD_ADDR_MASK                   (0x3FFFFFFF)

#define HISFC350_REG_BUF_SIZE ((SFC_CMD_DATABUF_31_OFFSET_ADDR - SFC_CMD_DATABUF_0_OFFSET_ADDR) + 0x04)

#define HISFC350_BUS_FLASH_SIZE_CS0_MASK (0x0F)
#define HISFC350_BUS_FLASH_SIZE_CS1_MASK (0x0F << 8)

#define HISFC350_IFCYCLE_STD       (0)
#define HISFC350_IFCYCLE_DUAL      (1)
#define HISFC350_IFCYCLE_DUAL_ADDR (2)
#define HISFC350_IFCYCLE_DUAL_CMD  (3)
#define HISFC350_IFCYCLE_QUAD      (5)
#define HISFC350_IFCYCLE_QUAD_ADDR (6)
#define HISFC350_IFCYCLE_QUAD_CMD  (7)

/* Don't change the follow config */
#ifdef SFC_SUPPORT_MUL_MODE
#define HISFC351_SUPPORT_READ (SPI_IF_READ_STD \
                                | SPI_IF_READ_FAST \
                                | SPI_IF_READ_DUAL \
                                | SPI_IF_READ_DUAL_ADDR \
                                | SPI_IF_READ_QUAD \
                                | SPI_IF_READ_QUAD_ADDR)

#define HISFC351_SUPPORT_WRITE (SPI_IF_WRITE_STD \
                                 | SPI_IF_WRITE_DUAL \
                                 | SPI_IF_WRITE_DUAL_ADDR \
                                 | SPI_IF_WRITE_QUAD \
                                 | SPI_IF_WRITE_QUAD_ADDR)
#else
#define HISFC351_SUPPORT_READ  SPI_IF_READ_STD
#define HISFC351_SUPPORT_WRITE SPI_IF_WRITE_STD
#endif
#define HISFC_SUPPORT_MAX_DUMMY (7)

#define SFC_SERACH_READ  1
#define SFC_SERACH_WRITE 0
#define SFC_ENABLE       1
#define SFC_DISABLE      0
#define SFC_WP_EN        BIT(1)

#define HISFC350_MAX_READY_WAIT_JIFFIES (5 * HZ)  // 40
#define SFC_RW_LEN_ZERO                 (1000)
#define SPI_FLASH_PAGE_SIZE             (256)

extern struct spi_info g_hisfc_spi_info_table[];

typedef struct {
    u32 bus;
    u32 cs;
    u32 if_type;
    u32 addr_cycle;
} sfc_com_info;

/* protect_para.pro_op_mode */
enum SFC_PRO_MODE {
    SFC_PRO_MODE_ALL = 0,
    SFC_PRO_MODE_SEC = 1,
    SFC_PRO_MODE_OTHER
};

typedef struct {
    u32 tb;
    u32 pro_sec_cnt;
} mode_block;

#define OTHER_MOD_CNT 5
/* Define the union u_pro_config */
typedef union {
    mode_block block;
    /* Define an unsigned arr member */
    u32 other_mdoe[OTHER_MOD_CNT];
} u_pro_config;

typedef struct {
    u32 pro_op_mode;
    u32 enable;
    u_pro_config pro_config;
} protect_para;

#define SFC_TEST
#ifdef SFC_TEST
/* for test start */
typedef struct {
    sfc_com_info com_info;
    u8 ids[SFC_MAX_ID_LEN];
} sfc_rdid;

typedef struct {
    sfc_com_info com_info;
    u32 use_set_addrcycle;
} sfc_set_addrcycle;

#define SFC_MAX_RW_DATA_SIZE 2048
#define SFC_REG_READ         0
#define SFC_REG_WRITE        1
#define SFC_DMA_WRITE        2
#define SFC_DMA_READ         3
#define SFC_BUS_WRITE        4
#define SFC_BUS_READ         5
#define SFC_DMA_SEC_WRITE    6
#define SFC_DMA_SEC_READ     7
#define SFC_IRQ_WRITE        8
#define SFC_IRQ_READ         9

typedef struct {
    u32 chipselect;
    u32 use_int_mode;
    unsigned long chipsize;
    u32 erasesize;
    u32 addrcycle;
    struct spi_operation read[1];
    struct spi_operation write[1];
    struct spi_operation erase[1];
} base_debug_info;

typedef struct {
    sfc_com_info com_info;
    base_debug_info f_base_info;
} sfc_flash_info;

typedef struct {
    u8 read_data_buf[SFC_CS_CNT][SFC_MAX_RW_DATA_SIZE];
    u8 write_data_buf[SFC_CS_CNT][SFC_MAX_RW_DATA_SIZE];
} sfc_buf;

#define USE_DMA_FLAG BIT(0)
#define USE_REG_FLAG BIT(1)
#define USE_BUS_FLAG BIT(2)

/* for test end */
#endif

struct spi_driver;

typedef struct {
    s8 *name;
    u32 chipselect;
    u32 use_int_mode;
    u32 mtd_device_regis;
    u64 chipsize;
    u32 erasesize;
    void __iomem *iobase;
    u32 addrcycle;
    u32 original_addrcycle;
    struct spi_operation read[1];
    struct spi_operation write[1];
    struct spi_operation erase[1];
    void *host;
    struct spi_driver *driver;
} hisfc_spi;

typedef struct spi_driver_test_s {
    s32 (*protect_process)(hisfc_spi *spi, protect_para *p_protect);
} spi_driver_test;

#define SUPPORT_DMA_WR  (1UL << 0)
#define SUPPORT_DMA_RD  (1UL << 1)
#define SUPPORT_BUS_WR  (1UL << 2)
#define SUPPORT_BUS_RD  (1UL << 3)
#define SUPPORT_REG_WR  (1UL << 4)
#define SUPPORT_REG_RD  (1UL << 5)
#define SUPPORT_ALL_MOD (SUPPORT_DMA_WR | SUPPORT_DMA_RD | SUPPORT_BUS_WR |  \
    SUPPORT_BUS_RD | SUPPORT_REG_WR | SUPPORT_REG_RD)

struct spi_driver {
    u32 support_dma_bus_flag;
    s32 (*wait_ready)(hisfc_spi *spi, u8 is_lock);
    s32 (*write_enable)(hisfc_spi *spi);
    s32 (*entry_4addr)(hisfc_spi *spi, s32 enable);
    s32 (*bus_prepare)(hisfc_spi *spi, s32 op);
    s32 (*spi_opr_finish_call)(hisfc_spi *spi);
    s32 (*erase_process)(hisfc_spi *spi, unsigned long long addr, unsigned long long len, u8 is_lock);
    s32 (*read_process)(hisfc_spi *spi, loff_t from, size_t len, size_t *retlen, u8 *buf);
    s32 (*init_flash_mem)(hisfc_spi *spi, u8 is_lock);
    s32 (*spec_cfg)(hisfc_spi *spi, u8 is_lock);
    s32 (*write_any_reg)(hisfc_spi *spi, u8 iftype, u8 len, u32 value, u32 addr);
    s32 (*read_any_reg)(hisfc_spi *spi, u32 addr, u8 iftype, u8 *value, u8 is_lock);
    s32 (*modify_any_reg)(hisfc_spi *spi, u32 addr, u8 mask, u8 val, u8 is_lock);
    spi_driver_test driver_test;
};

#define SCM3_CORE 1
#define UNSC_CORE 2

typedef struct {
    u32 use_set_addrcycle;
    u32 addr_cycle;
} hisfc_addrmode_ctl;

typedef struct {
    hisfc_addrmode_ctl addrmode_ctl[SFC_CS_CNT];
} hisfc_extra_ctl;
#define MAX_SFC_NAME_LEN 8
#define SFC_NAME_LEN     4

struct s_hisfc_core_host;

enum sfc_dma_type {
    SFC_DMA_TO_DEVICE,
    SFC_DMA_FROM_DEVICE,
};

enum sfc_mem_type {
    SFC_TYPE_NONE = 0,
    SFC_TYPE_HOST_BUF,
};

typedef struct {
    s32 (*sfc_clock_switch_ctrl)(u32 bus, u32 enable);
    s32 (*sfc_set_clock)(u32 bus, u32 clk_mhz);
    s32 (*sfc_reset_handle)(u32 bus);
    void (*sfc_init_extend_addr_mode)(void);
    u32 (*sfc_get_dma_cfg_addr)(u32 bus, dma_addr_t phy_addr); /* force */
    s32 (*sfc_set_iocfg)(u32 bus, u32 cs, u32 enable);
    void (*sfc_set_iodrive)(u32 bus, u32 cs);
    s32 (*sfc_check_permission)(u32 bus);
} sfc_chip_operations;

typedef struct {
    s32 (*plat_get_cfg_cs)(struct s_hisfc_core_host *c_host, u32 *cfg_cs);
    u32 (*plat_get_irq_num)(struct s_hisfc_core_host *c_host);
    void __iomem *(*plat_get_reg_base)(struct s_hisfc_core_host *c_host);
    void __iomem *(*plat_get_io_base)(struct s_hisfc_core_host *c_host);
    s8 *(*plat_mem_zalloc)(struct s_hisfc_core_host *c_host, u32 mtype, u32 size);
    void (*plat_mem_free)(struct s_hisfc_core_host *c_host, u32 mtype, const u8 *ptr);
    dma_addr_t (*plat_dma_map)(const struct s_hisfc_core_host *c_host, u8 *dma_buf, size_t size,
                               enum sfc_dma_type type);
    void (*plat_dma_unmap)(const struct s_hisfc_core_host *c_host, dma_addr_t addr, size_t size,
                           enum sfc_dma_type type);
    s8 *(*plat_ioremap_wc)(const struct s_hisfc_core_host *c_host, unsigned long phy_addr, size_t len);
    void (*plat_modify_mtd_cb)(u8 *plat_data, u32 bus, u32 cs, u32 is_write, u32 mtd_flag);
} sfc_plat_operations;

typedef struct s_hisfc_core_host {
    u32 sc_un_core; /* 1:scm3  2:a55 */
    u32 bus;        /* 0 sfc0, 1 sfc1 */
    s8 sfc_name[MAX_SFC_NAME_LEN];
    u8 flash_id[SFC_CS_CNT][SFC_MAX_ID_LEN];
    u32 irq_init_flag; /* success:1 */
    u32 cfg_use_cs;
    volatile u32 int_status;
    char *buffer;
    struct mutex lock;
    wait_queue_head_t sfc_wait_queue;
    void *iobase;
    void *regbase;
    void (*set_system_clock)(struct s_hisfc_core_host *host, struct spi_operation *op, s32 clk_en);
    void (*set_host_addr_mode)(struct s_hisfc_core_host *host, s32 enable);
    s32 num_chip;
    hisfc_spi spi[SFC_CS_CNT];
    void *platform_data;
    s32 (*mtd_init)(void *mtd_para);
    void *mtd_data;
    const sfc_chip_operations *chip_ops;
    const sfc_plat_operations *plat_ops;
    /* for test start */
    hisfc_extra_ctl extra_ctl;
    /* for test end */
} hisfc_core_host;

typedef struct {
    hisfc_core_host mgr_core_host[SFC_CTRL_CNT];
    u32 com_res_init_flag;
} hisfc_mgr;

void hisfc351_set_system_clock(hisfc_core_host *host, struct spi_operation *op, s32 clk_en);
void hisfc351_get_best_clock(u32 *clock);

#ifdef CONFIG_HISFC351_SHOW_CYCLE_TIMING
char *hisfc351_get_clock_str(u32 clk_reg);
#endif

#define SFC_FALSE 0
#define SFC_TRUE  1

#define hisfc_read(_host, _reg)          readl((void *)((char *)(_host)->regbase + (_reg)))
#define hisfc_write(_host, _reg, _value) writel((unsigned)(_value), (void *)((char *)(_host)->regbase + (_reg)))

#define SFC_CPU_FINISH_MAX_CNT 0x10000000
#define BUS_ADDR_ALIGN_MASK    (0x7)
#define SPANSION_ID_LEN        (0x6)
#define SECTOR_ARCH_MASK       (0x8)
#define SECTOR_POS_MASK        (0x4)

#define HYBRID_SECTOR_ARCH  (0UL << 3) /* 4KB erase enable */
#define UNIFORM_SECTOR_ARCH (1UL << 3) /* 4KB erase disable */

#define SECTOR_4KB_AT_TOP    (1UL << 2) /* high address */
#define SECTOR_4KB_AT_BOTTOM (0UL << 2) /* low address */

#define SECTOR_TYPE_DEFAULT 0
#define SECTOR_TYPE_4KB     4
#define SECTOR_TYPE_32KB    32
#define SECTOR_TYPE_64KB    64
#define SECTOR_TYPE_256KB   256
#define SECTOR_4KB_CNT      8
#define MIN_BUF_LEN         4
#define BUF_READ_OFF        4
#define REG_WR_CMD_LEN      4
#define CMD_SEND_WAIT_CNT   100

typedef struct {
    u32 dma_phy_addr;
    u32 spi_flash_addr;
    u32 len;
    u32 is_read;
    u32 cs;
} dma_rw_addr;

void sfc_core_wait_cpu_finish(const hisfc_core_host *c_host);
void sfc_core_dma_wait_cpu_finish(const hisfc_core_host *c_host);
s32 sfc_core_spi_probe(hisfc_core_host *host);
u32 sfc_core_map_chipsize(unsigned long long chipsize);
u8 *sfc_core_read_ids(hisfc_core_host *host, u32 chipselect, u8 *buffer);
s32 sfc_core_check_addr_len(hisfc_spi *spi, loff_t addr, size_t len, u8 is_erase);
void sfc_core_dma_rw_config(const hisfc_core_host *c_host, dma_rw_addr rm_addr);
s32 sfc_core_reg_read(hisfc_spi *spi, loff_t from, size_t len, size_t *retlen, u8 *buf);
s32 sfc_core_reg_read_for_all(hisfc_spi *spi, loff_t from, size_t len, size_t *retlen, u8 *buf);
s32 sfc_core_reg_write(hisfc_spi *spi, loff_t to, size_t len, size_t *retlen, const u8 *data);
s32 sfc_core_reg_erase(hisfc_spi *spi, unsigned long long addr, unsigned long long len);
s32 sfc_core_bus_addr_align_detect(loff_t addr);
s32 sfc_core_reg_erase_one_block(hisfc_core_host *host, hisfc_spi *spi,
                                 u32 offset, u8 is_lock, u8 sector_type);
s32 sfc_core_read_reg_by_cmd(hisfc_spi *spi, u32 cmd, u8 iftype, u32 *value);
s32 sfc_core_write_reg_by_cmd(hisfc_spi *spi, u8 cmd, u8 iftype, u8 len, u32 value);
s32 sfc_core_release_spi_lock(hisfc_spi *spi);
s32 sfc_core_iftype_check(u32 iftype, u32 is_write);
s32 sfc_core_spi_iftype_update(hisfc_core_host *host, u32 chipselect, u32 is_write, u32 iftype);
void sfc_core_set_host_addr_mode(hisfc_core_host *host_s, s32 enable);
void sfc_core_addr_mode_set(hisfc_spi *spi);
u32 sfc_core_cs_is_used(u32 bus, u32 cs, u32 dts_cfg);
u32 sfc_core_bus_is_used(u32 bus, u32 dts_cfg);
s32 sfc_core_com_para_check(sfc_com_info *com_para);
void sfc_core_bus_write_enable(const hisfc_core_host *host, u32 enable);
u32 sfc_core_is_wpen(const hisfc_core_host *host);
void sfc_core_timing_cfg(const hisfc_core_host *c_host);
void sfc_core_suspend_shutdown_com_process(hisfc_core_host *c_host, u32 is_suspend);
s32 sfc_core_resume(hisfc_core_host *c_host);

/* start for spansion or cypress (s70fs01gs) */
s32 sfc_core_write_any_reg(hisfc_spi *spi, u8 iftype, u8 len, u32 value, u32 addr);
s32 sfc_core_read_any_reg(hisfc_spi *spi, u32 reg_addr, u8 iftype, u8 *value, u8 is_lock);
s32 sfc_core_modify_any_reg(hisfc_spi *spi, u32 reg_addr, u8 mask, u8 val, u8 is_lock);
s32 sfc_core_spansionfsxx_erase(hisfc_spi *spi, unsigned long long addr, unsigned long long len, u8 is_lock);
s32 sfc_core_fs01gs_init_flash_mem(hisfc_spi *spi, u8 is_lock);
s32 sfc_core_s70fs01gs_reg_read(hisfc_spi *spi, loff_t from, size_t len,
                                size_t *retlen, u8 *buf);
/* end for spansion or cypress (s70fs01gs) */
s32 sfc_core_fs512s_init_flash_mem(hisfc_spi *spi, u8 is_lock);

#endif
