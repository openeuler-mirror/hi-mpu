/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: Hi309x platform specific header files
 * Create: 2018-05-10
 */
#ifndef __HI309x_COMMON_H__
#define __HI309x_COMMON_H__

#define CONFIG_SYS_REF_CLOCK_FREQ           25000000
#define CONFIG_SYS_APB_BYPASS_CLOCK_FREQ    1250000 /* 1.25Mhz */

/* Hi309x APB clock frequency */
#define CONFIG_SYS_CPU_FREQ                 1000000000
#define CONFIG_SYS_APB_CLOCK_FREQ           100000000
#define CONFIG_SYS_APB_CLOCK_FREQ_LOW_TEMP  75000000
#define CONFIG_SYS_UART_NUM                 2

#define US_TICKS                            (CONFIG_SYS_CPU_FREQ / 1000000)
/* frequency 25M for wdg5, 60s = 60 * 25M */
#define WDG5_UBOOT_TIMEOUT                  0x59682F00
#define WDG_ENABLE_VALUE                    0x3

#define SLAVE_PD_LOCK_TIMEOUT_ADDR          0x08100000
#define SLACE_PD_SUCCESS_FLAG               0xe /* BIT1-3 represents CPU1-3 powered down */

/* boot core and boot media */
#define BOOT_CORE_A55       0
#define BOOT_CORE_SECURE_M3 1
#define BOOT_MODE_SFC       0
#define BOOT_MODE_LOCALBUS  1

/* localbus and sfc a55 start */
#define EXT4_IMG_FLASH_ADDR (CONFIG_ATF_FW_OFFSET == 450560 ? 0x80000 : 0x400000)
/* scm3 start img ddr addr */
#define EXT4_IMG_SCM3_LOAD_ADDR     0x88680000
/* scm3 start img max size */
#define EXT4_IMG_SCM3_LOAD_SIZE_MAX 0x01200000

/* System counter memory mapped regs */
#define SYSCNT_CTL_REG_BASE         0x14002000UL
#define SYSCNT_FREQ_TBL_REG_BASE    (SYSCNT_CTL_REG_BASE + 0x20)

/* Sysctrl registers */
#define SYSCTL_REG_BASE             0x08740000UL
#define SYSCTL_CHIP_ID_BASE         SYSCTL_REG_BASE
#define SYSCTL_STRAP_STATUS_BASE    (SYSCTL_REG_BASE + 0x4)
#define SYSCTL_INITIAL_INF_BASE     (SYSCTL_REG_BASE + 0x8)
#define SYSCTL_BOOT_DEFINE_BASE     (SYSCTL_REG_BASE + 0xc)
#define SYSCTL_REBOOT_CTRL_BASE     (SYSCTL_REG_BASE + 0x10)
#define SYSCTL_CLK_BYPASS_CTRL_BASE (SYSCTL_REG_BASE + 0x1c)
#define SYSCTL_CPLL_CTRL1_BASE      (SYSCTL_REG_BASE + 0x24)
#define SYSCTL_PPLL0_CTRL0_BASE     (SYSCTL_REG_BASE + 0x28)
#define SYSCTL_PPLL0_CTRL1_BASE     (SYSCTL_REG_BASE + 0x2c)
#define SYSCTL_PPLL1_CTRL0_BASE     (SYSCTL_REG_BASE + 0x30)
#define SYSCTL_PPLL1_CTRL1_BASE     (SYSCTL_REG_BASE + 0x34)
#define SYSCTL_PLL_LOCK_STATUS_BASE (SYSCTL_REG_BASE + 0x40)
#define SYSCTL_WDG_CTRL_BASE        (SYSCTL_REG_BASE + 0x48)
#define SYSCTL_DBOOT_STATUS_BASE    (SYSCTL_REG_BASE + 0x60)
#define SYSCTL_USB_REF_CLK_CFG_BASE (SYSCTL_REG_BASE + 0x64)
#define SYSCTL_INITIAL_INF2_BASE    (SYSCTL_REG_BASE + 0x7c)
#define SYSCTL_INITIAL_INF3_BASE    (SYSCTL_REG_BASE + 0x80)
#define SYSCTL_A55_INFO0_BASE       (SYSCTL_REG_BASE + 0x100)
#define SYSCTL_RST_REQ_BASE         (SYSCTL_REG_BASE + 0x148)
#define SYSCTL_RST_DREQ_BASE        (SYSCTL_REG_BASE + 0X14c)
#define SYSCTL_RST_ST_BASE          (SYSCTL_REG_BASE + 0X150)
#define BMC_RST_CFG_MASK            (1 << 3)
#define M3_BOOT_EN_MASK             (1 << 2)
#define SC_BOOT_EN_MASK             (1 << 3)
#define M3_ADDR0_REMAP_EN_MASK      (1 << 5)
#define WDG6_OUT_EN_MASK            (1 << 27)
#define USB_PHY_CLK_DIV_MASK        (0x3 | (0x3 << 4) | (0x3 << 8)) /* Bit[1:0] & [5:4] & [9:8] */
#define USB_PHY_CLK_DIV_LOW_TEMP    ((0x3 << 4) | 0x2)          /* Bit[9:8] = 0 [5:4] = 2'b11 [1:0] = 2'b10 */

/* SYS INIT FLAGS RECORDS in SYSCTRL_A55_ASSERT_INFO0 */
#define PPLL1_LOCK_TIMEOUT          (1U << 5)
#define PPLL0_LOCK_TIMEOUT          (1U << 4)
#define CPLL_LOCK_TIMEOUT           (1U << 3)
#define PLL_INIT_DONE               (1U << 2)
#define DDR_INIT_DONE               (1U << 1)
#define DDR_INIT_DOING              1U
#define DDR_INIT_MASK               (DDR_INIT_DONE | DDR_INIT_DOING)
#define PLL_INIT_MASK               PLL_INIT_DONE
#define NVM_PART_MASK               (7U << 6)
#define NVM_PART_OFFSET             6
#define NVM_PART_INVAL              "n"

/* Watchdog registers */
#define WDG0_CONTROL_BASE   0x8763008
#define WDG1_CONTROL_BASE   0x8764008
#define WDG2_CONTROL_BASE   0x8765008
#define WDG3_CONTROL_BASE   0x8766008
#define WDG4_CONTROL_BASE   0x8767008
#define WDG5_CONTROL_BASE   0x8768008
#define WDG5_FEED_BASE      0x8768000
#define WDG5_LOCK_BASE      0x8768c00
#define WDG_UNLOCK_VALUE    0x1acce551
#define WDG_LOCK_VALUE      0x5a5a5a5a

/* LP Subsys registers */
#define LP_SUBSYS_REG_BASE          0x0871D000
#define LP_SUBSYS_SRST_REQ0_BASE    (LP_SUBSYS_REG_BASE + 0x4)
#define LP_SUBSYS_SRST_REQ1_BASE    (LP_SUBSYS_REG_BASE + 0x8)
#define LP_SUBSYS_SRST_REQ2_BASE    (LP_SUBSYS_REG_BASE + 0xc)
#define LP_SUBSYS_CRG_DIV_CTRL_BASE (LP_SUBSYS_REG_BASE + 0x18)
#define LP_SUBSYS_LPSC_SUPERIO_CTRL (LP_SUBSYS_REG_BASE + 0x24)
#define LP_SUBSYS_PORT_SEL_REG      (LP_SUBSYS_REG_BASE + 0x2C) /* physical port to uart port mapping */
#define LP_SUBSYS_UART_SEL_REG      (LP_SUBSYS_REG_BASE + 0x30)
#define LP_SUBSYS_SRST_DREQ0_BASE   (LP_SUBSYS_REG_BASE + 0x1fc)
#define LP_SUBSYS_SRST_DREQ1_BASE   (LP_SUBSYS_REG_BASE + 0x200)
#define LP_SUBSYS_SRST_ST1_BASE     (LP_SUBSYS_REG_BASE + 0x20c)
#define M3_SRST_REQ_MASK            (1 << 2)

/* Host Subsys registers */
#define HOST_SUBSYS_REG_BASE        0x08B20000
#define HOST_RST_DISABLE_BASE       (HOST_SUBSYS_REG_BASE + 0x10)

/* HP Subsys registers */
#define HP_SUBSYS_REG_BASE          0x0cd0c000
#define HP_SRST_DIS1_BASE           (HP_SUBSYS_REG_BASE + 0x78)
#define HP_SRST_DIS2_BASE           (HP_SUBSYS_REG_BASE + 0x7c)

/* PLL macros */
#define PLL_LOCK_TIMEOUT_US         1000000
#define CPLL_CLK_OUT_BYPASS         (0x1 << 4)
#define CPLL_CORE_CLK_BYPASS        0x1
#define CPLL_CLK_BYPASS_MASK        0x11 /* bit0,4 0b00010001 -> 0x11 */
#define CPLL_PD_MASK                (1U << 23)
#define CPLL_FBDIV_MASK             0xFFF /* bit[11:0] */
#define CPLL_LOCK_MASK              (1U << 0)
#define PPLL0_CLK_OUT_BYPASS        (0x1 << 5)
#define PPLL0_BUS_CLK_BYPASS        0x2
#define PPLL0_CLK_BYPASS_MASK       0x22 /* bit1,5 0b00100010 -> 0x22 */
#define PPLL0_PD_MASK               (1U << 23)
#define PPLL0_FBDIV_MASK            0xFFF
#define PPLL0_LOCK_MASK             (1U << 1)
#define PPLL1_CLK_OUT_BYPASS        (0x1 << 6)
#define PPLL1_DDRC_CLK_BYPASS       0x4
#define PPLL1_CLK_BYPASS_MASK       0x44 /* bit2,6 0b01000100 -> 0x44 */
#define PPLL1_PD_MASK               (1U << 23)
#define PPLL1_FBDIV_MASK            0xFFF
#define PPLL1_LOCK_MASK             (1U << 2)
#define PPLL2_GPU_CLK_BYPASS        (1U << 3)

/* LP_SUBSYS macros */
#define LPSC_APB_DIV_MASK           (0xF << 20) /* Bit[23:20] */

/* begin hi309x SFC */
/* SFC0 寄存器 */
#ifndef CONFIG_HISFC350_REG_BASE_ADDRESS
#define CONFIG_HISFC350_REG_BASE_ADDRESS 0x08600000
#endif
#define HISFC0_REG_BASE_ADDRESS         0x08640000
#define SFC_GLOBAL_CONFIG_OFFSET        0x100
#define HISFC350_CMD_INS                0x0308
#define HISFC350_CMD_ADDR               0x030C
#define HISFC350_CMD_ADDR_MASK          0x3FFFFFFF
#define HISFC350_CMD_DATABUF0           0x0400
#define HISFC350_CMD_DATABUF15          0x043C
#define SFC_ADDR_MODE_MASK              (1U << 2)

#define HISFC350_VERSION                0x01F8
#define SFC_BUS_0                       0
#define SFC_BUS_1                       1

/* sfc iocfg*/
#define    IO_CONFIG_T_BASE_ADDR                    0x8745000
#define IO_CONFIG_T_IOMG002_OFFSET_ADDR  0x8
#define IO_CONFIG_T_IOMG003_OFFSET_ADDR  0xC
#define IO_CONFIG_T_IOMG004_OFFSET_ADDR  0x10
#define IO_CONFIG_T_IOMG005_OFFSET_ADDR  0x14
#define IO_CONFIG_T_IOMG006_OFFSET_ADDR  0x18
#define IO_CONFIG_T_IOMG007_OFFSET_ADDR  0x1C
#define IO_CONFIG_T_IOMG008_OFFSET_ADDR  0x20
#define IO_CONFIG_T_IOMG009_OFFSET_ADDR  0x24
#define IO_CONFIG_T_IOMG010_OFFSET_ADDR  0x28
#define IO_CONFIG_T_IOMG011_OFFSET_ADDR  0x2C

/* bus0,sfc1; bus1,sfc0 */
#define hisfc0_read(_bus, _reg) \
    readl((void *)(((SFC_BUS_0 == (_bus)) ? \
    ((char *)HISFC0_REG_BASE_ADDRESS) : ((char *)CONFIG_HISFC350_REG_BASE_ADDRESS)) + (_reg)))

#define hisfc0_write(_bus, _reg, _value) \
    writel((unsigned)(_value),           \
        (void *)(((SFC_BUS_0 == (_bus)) ? \
        ((char *)HISFC0_REG_BASE_ADDRESS) : ((char *)CONFIG_HISFC350_REG_BASE_ADDRESS)) + (_reg)))

#define HISFC350_CMD_CONFIG                 0x0300
#define HISFC350_CMD_CONFIG_MEM_IF_TYPE(_n) (((_n)&0x07) << 17)
#define HISFC350_CMD_CONFIG_DATA_CNT(_n)    (((_n - 1) & 0x3F) << 9)
#define HISFC350_CMD_CONFIG_RW_READ         (1 << 8)
#define HISFC350_CMD_CONFIG_DATA_EN         (1 << 7)
#define HISFC350_CMD_CONFIG_DUMMY_CNT(_n)   (((_n)&0x07) << 4)
#define HISFC350_CMD_CONFIG_ADDR_EN         (1 << 3)
#define HISFC350_CMD_CONFIG_SEL_CS(_cs)     (((_cs)&0x01) << 1)
#define HISFC350_CMD_CONFIG_START           (1 << 0)

#define HISFC350_REG_BUF_SIZE   ((HISFC350_CMD_DATABUF15 - HISFC350_CMD_DATABUF0) + 0x04)
#define SPI_FLASH_PAGE_SIZE     256

#define HISFC350_GLOBAL_CONFIG              0x0100
#define HISFC350_GLOBAL_CONFIG_ADDR_MODE_4B (1 << 2)

#define SPI_CMD_WREN 0x06 /* Write Enable */
#define SPI_CMD_EN4B 0xB7 /* enter to 4 bytes mode and set 4 byte bit as '1' */
#define SPI_CMD_EX4B 0xE9 /* exit 4 bytes mode and clear 4 byte bit as '0' */

#define CONFIG_IOCFG_BASE_ADDRESS   0x20005000
#define CONFIG_IOCFG_BASE_LEN       0x1000
#define SFC_IOCFG_OFFSET            0xa8

#define SYS_SFC_50MB_CONFIG_REG     0x30
#define SYS_SFC_50MB_CONFIG         0x1802e10

#define SYS_CONFIG_SFC_ADDRESS      0x20000000

#define SPI_FLASH_3B_ADDR_LEN       3
#define SPI_FLASH_CMD_LEN           (1 + SPI_FLASH_3B_ADDR_LEN)
#define SPI_FLASH_4B_CMD_LEN        (1 + SPI_FLASH_3B_ADDR_LEN + 1)


/*
 * Mapping bewteen the corresponding area(active,gold,temp) ID and
 * the current non-volatile memory partition(Mainly for EMMC's GPP partitions)
 */
struct nvm_partiton_id {
    char *active_part_id;
    char *gold_part_id;
    char *temp_part_id;
};

struct nvm_sfc_partiton_id {
    char *active_part_id;
    char *gold_part_id;
};

struct uart_cfg {
    unsigned char data_bits; /* Data bits: 8,7,6,5 */
    unsigned char stop;      /* Stop bit: 0 - 1 bit or 1 - 2 bits */
    unsigned char pen;       /* Parity Enable: 0 - disable or 1 - enable */
    unsigned char eps;       /* Parity Select: 0 - odd or 1 - even */
    unsigned int baudrate;   /* Baudrate: e.g: 9600 115200 */
};

extern void uart_port_map_init(void);
extern int uart_init(int uart_num, struct uart_cfg *cfg, unsigned int uart_src_clk);
extern void uart_set_baudrate(int uart_num, unsigned int baudrate, unsigned int uart_src_clk);
/* uart_tx/rx will wait untill tx/rx buffer ready */
extern int uart_tx(int uart_num, const char c);
extern char uart_rx(int uart_num);
/* uart_tx/rx_ready will not wait, only get instance status */
extern int uart_tx_ready(int uart_num);
extern int uart_rx_ready(int uart_num);
extern void uart_early_puts(int uart_num, const char *str);
extern void sdelay(unsigned long loops);
extern unsigned long get_mpidr_el1(void);
extern void dboot_info_show(void);
extern int get_boot_core(void);
extern unsigned int get_boot_media_type(void);
extern int sys_init_flag_check(unsigned int init_flag);
extern void sys_init_flag_set(unsigned int init_mask, unsigned int init_flag);
extern unsigned int sys_inif_flag_get(unsigned int mask, unsigned int bit_off);
extern void early_udelay(unsigned long usec);

#endif /* __HI309x_COMMON_H__ */
