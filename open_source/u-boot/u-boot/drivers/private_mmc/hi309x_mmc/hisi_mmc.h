#ifndef _HISI_MMC_H_
#define _HISI_MMC_H_
#include <common.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/errno.h>

#define __EMMC_DMA__
#define MMC_CLK           6250000

#define F_MIN             400000
#define F_MAX             6250000
#define MAX_CLK_COUNT     4

#define ENABLE            1
#define DISABLE           0

#ifdef EDA_SIMULATION
#define MAX_RETRY_COUNT   20000000
#else
#define MAX_RETRY_COUNT   10000000
#endif

#define CARD_RETRY_COUNT  100

#define MAX_DES  100

#define EMMC_READ 0
#define EMMC_WRITE 1

#define MMC_DATA_WRITE (1 << 8)
#define MMC_DATA_READ (1 << 9)

#define USE_INTERNAL_SLAVE      0xFDFFFFFF
#define EMMC_RCA 0x01     /*emmc Relative addr*/

/* EXT_CSD command definations*/
#define EXT_CSD_CMD_SET_NORMAL    (1 << 0)  /*NORMAL EXT_CSD_CMD*/
#define EXT_CSD_CMD_SET_SECURE    (1 << 1)
#define EXT_CSD_CMD_SET_CPSECURE  (1 << 2)
#define PART_ACCESS_MASK    (0x7)
#define USER_PART            0x0
#define BOOT_PARTITION_1     0x1
#define BOOT_PARTITION_2     0x2
#define REPLAY_PROT_MEM_BLK  0x3
#define USER_GPP_1           0x4
#define USER_GPP_2           0x5
#define USER_GPP_3           0x6
#define USER_GPP_4           0x7
/*
 * EXT_CSD fields
 */
#define EXT_CSD_GP_SIZE_MULT    143 /* Read and Write */
#define EXT_CSD_PARTITIONING_SUPPORT    160 /* Read Only */
#define EXT_CSD_RST_N_FUNCTION    162 /*Read and Write*/
#define EXT_CSD_RPMB_MULT    168 /* Read Only */
#define EXT_CSD_ERASE_GROUP_DEF    175 /* Read and Write */
#define EXT_CSD_BOOT_BUS_WIDTH    177
#define EXT_CSD_PART_CONF    179 /* Read and Write */
#define EXT_CSD_BUS_WIDTH    183 /* Read and Write */
#define EXT_CSD_HS_TIMING    185 /* Read and Write */
#define EXT_CSD_REV    192 /* Read Only */
#define EXT_CSD_CARD_TYPE    196 /* Read Only */
#define EXT_CSD_SEC_CNT    212 /* Read Only, 4 bytes */
#define EXT_CSD_HC_WP_GRP_SIZE    221 /* Read Only */
#define EXT_CSD_HC_ERASE_GRP_SIZE    224 /* Read Only */
#define EXT_CSD_BOOT_MULT    226 /* Read Only */

#define MMC_MODE_SPI    0x400
#define mmc_host_is_spi(mmc)    ((mmc)->host_caps & MMC_MODE_SPI)
#define MMC_STATUS_MASK    (~0x0206BF7F)
#define MMC_STATUS_RDY_FOR_DATA (1 << 8)
#define MMC_STATUS_CURR_STATE   (0xf << 9)
#define MMC_STATE_PRG    (7 << 9)
/*cmd*/
#define MMC_GO_IDLE_STATE         0   /* bc                          */
#define MMC_SEND_OP_COND          1   /* bcr  [31:0] OCR         R3  */
#define MMC_ALL_SEND_CID          2   /* bcr                     R2  */
#define MMC_SET_RELATIVE_ADDR     3   /* ac   [31:16] RCA        R1  */
#define MMC_SWITCH                6   /* ac   [31:0] See below   R1b */
#define MMC_SELECT_CARD           7   /* ac   [31:16] RCA        R1  */
#define MMC_SEND_EXT_CSD          8   /* adtc                    R1  */
#define MMC_SEND_CSD              9   /* ac   [31:16] RCA        R2  */
#define MMC_SEND_CID             10   /* ac   [31:16] RCA        R2  */
#define MMC_SEND_STATUS          13   /* ac   [31:16] RCA        R1  */
#define MMC_READ_SINGLE_BLOCK    17   /* adtc [31:0] data addr   R1  */
#define MMC_READ_MULTIPLE_BLOCK  18   /* adtc [31:0] data addr   R1  */
#define MMC_WRITE_BLOCK          24   /* adtc [31:0] data addr   R1  */
#define MMC_WRITE_MULTIPLE_BLOCK 25   /* adtc                    R1  */

#define MMC_RSP_PRESENT (1 << 0)
#define MMC_RSP_136 (1 << 1) /* 136 bits long response */
#define MMC_RSP_CRC (1 << 2) /* CRC will be checked in response */
#define MMC_RSP_BUSY (1 << 3) /* Card may send busy in reponse */
#define MMC_RSP_OPCODE (1 << 4) /* Opcode is contained in response */
#define MMC_CMD_ADTC   (1 << 5)

#define MMC_RSP_NONE (0)
#define MMC_RSP_R1 (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1B (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_BUSY)
#define MMC_RSP_R2  (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
#define MMC_RSP_R3  (MMC_RSP_PRESENT)
#define MMC_RSP_R4  (MMC_RSP_PRESENT)
#define MMC_RSP_R5  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R6  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R7  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1_NO_CRC   (MMC_RSP_R1 & (~MMC_RSP_CRC))

#define mmc_resp_type(flags)  (flags & (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC|MMC_RSP_BUSY|MMC_RSP_OPCODE))
#define MMC_SWITCH_MODE_WRITE_BYTE  0x03 /* Set target byte to value */

#define OCR_BUSY    0x80000000
#define OCR_HCS     0x40000000

#define MMC_VDD_32_33    0x00100000 /* VDD voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34    0x00200000 /* VDD voltage 3.3 ~ 3.4 */

#define OCR_VOLTAGE_MASK    0x00FFFF80
#define OCR_ACCESS_MODE     0x60000000

/*set card's speed*/
#define MMC_MODE_HS    0x001
#define MMC_MODE_HS_52MHz    0x010
#define MMC_MODE_HC    0x800
#define MMC_HS_52MHZ   0x2

/*set card's bit*/
#define MMC_BUS_WIDTH_8 8
#define MMC_BUS_WIDTH_4 4

#define EXT_CSD_BUS_WIDTH_1 0 /* Card is in 1 bit mode */
#define EXT_CSD_BUS_WIDTH_4 1 /* Card is in 4 bit mode */
#define EXT_CSD_BUS_WIDTH_8 2 /* Card is in 8 bit mode */
#define EXT_CSD_DDR_BUS_WIDTH_4 5 /* Card is in 4 bit DDR mode */
#define EXT_CSD_DDR_BUS_WIDTH_8 6 /* Card is in 8 bit DDR mode */

/* Command register defines */
#define HISI_MMC_CMD_START         BIT(31)
#define HISI_MMC_CMD_USE_HOLD_REG  BIT(29)
#define HISI_MMC_CMD_VOLT_SWITCH   BIT(28)
#define HISI_MMC_CMD_CCS_EXP       BIT(23)
#define HISI_MMC_CMD_CEATA_RD      BIT(22)
#define HISI_MMC_CMD_UPD_CLK       BIT(21)
#define HISI_MMC_CMD_INIT          BIT(15)
#define HISI_MMC_CMD_STOP          BIT(14)
#define HISI_MMC_CMD_PRV_DAT_WAIT  BIT(13)
#define HISI_MMC_CMD_SEND_STOP     BIT(12)
#define HISI_MMC_CMD_STRM_MODE     BIT(11)
#define HISI_MMC_CMD_DAT_WR        BIT(10)
#define HISI_MMC_CMD_DAT_EXP       BIT(9)
#define HISI_MMC_CMD_RESP_CRC      BIT(8)
#define HISI_MMC_CMD_RESP_LONG     BIT(7)
#define HISI_MMC_CMD_RESP_EXP      BIT(6)

#define SYNC_DMA_RW_RETRY_COUNT          30000000 /* 30s timeout */
#define HISI_MMC_CTRL_USE_IDMAC BIT(25)
#define MAX_ONE_TIME_TRANSMIT_BLOCKS (128000) /* maxinum number of blocks in a DMA transfer*/
// #define MAX_ONE_TIME_TRANSMIT_BLOCKS (1280) /* maxinum number of blocks in a DMA transfer*/
#define EXT_INT0                 16 /* The 1st external interrupt number */
#define EXT_INT_EMMC             85
#define NUM_INTERRUPTS           256
#define NVIC_UNPEND0      0xE000E280
#define NVIC_EN0          0xE000E100
#define NVIC_DIS0         0xE000E180
#define LP_SUBSYS_CSR_LPSC_ADDR_EXTEND0_OFFSET_ADDR      0x258
#define LP_SUBSYS_CSR_LPSC_ADDR_SEL_OFFSET_ADDR          0x0
#define LP_SUBSYS_BASE 0x871d000
#define HISI_MMC_CTRL_DMA_RESET BIT(2)
#define HISI_MMC_CTRL_FIFO_RESET BIT(1)
#define HISI_MMC_IDMAC_SWRESET BIT(0)

#define DMA_ADDR 0x872a0000
#define DMA_SIZE 0x1000000
/* EMMC_BMOD_OFFSET_ADDR(0x80) details */
#define BMOD_SWR     BIT(0)
#define BURST_LENGTH (2 << 8)
#define BURST_TYPE   BIT(1)

#define MMC_RETRY_WAIT_CARD_IDLE        400000
#define SYNC_DMA_RW_RETRY_COUNT             (5000000)
#define WAIT_FIFO_RESET_RETRY_COUNT         (2000)
#define EMMC_CARDTHRCTL_VALUE           0x02000003

/* 链表模式下 一个BD最大携带的数据大小为4K */
#define MAX_ONE_DES_SIZE 0x1000

#define WAIT_DMA          0
#define MMC_BLK_SIZE    512

#define MMC_COMPATIBILITY_MODE 0 /* Compatible mode */
#define MMC_HS_SDR_MODE        1
#define MMC_HS_DDR_MODE        2
#define MMC_HS200_SDR_MODE     3
#define MMC_HS400_DDR_MODE     4

#define DEFAULT_PHASE_VALUE      2

/* 控制器DDR 模式 */
#define DDR_MODE_ENABLE (0x1 << 16)

#define MAX_DMA_DESC_NUM 16000

#ifndef dma_haddr_t
typedef unsigned long long dma_haddr_t;
#endif

typedef struct {
    unsigned int data_flag;/* 读/写*/
    unsigned int cmd_flag;/*response type*/
    unsigned int n_block;/*block number*/
    unsigned int arg;/*command arg*/
    unsigned int opcode;/*command id*/
    unsigned int rsp[4];/*response*/

}mmc_cmd;

typedef union{
    unsigned int cmd_arg;
    struct cmd_bits_arg{
        unsigned int cmd_index:6;
        unsigned int response_expect:1;
        unsigned int response_length:1;
        unsigned int check_response_crc:1;
        unsigned int data_transfer_expected:1;
        unsigned int read_write:1;
        unsigned int transfer_mode:1;
        unsigned int send_auto_stop:1;
        unsigned int wait_prvdata_complete:1;
        unsigned int stop_abort_cmd:1;
        unsigned int send_initialization:1;
        unsigned int card_number:5;
        unsigned int update_clk_reg_only:1; /* bit 21 */
        unsigned int reserved_1:7;
        unsigned int use_hold_reg:1;
        unsigned int reserved_2:1;
        unsigned int start_cmd:1; /* HSB */
    } bits;
} cmd_arg_s;

struct emmc_des {
    unsigned int idmac_des_ctrl;
    unsigned int idmac_des_buf_size;
    unsigned int idmac_des_buf_addr;
    unsigned int idmac_des_next_addr;
};

struct emmc_host {
    unsigned int *dma_addr;     /* DMA的描述符地址 */
    unsigned int dma_size;      /* DMA的描述符地址空间大小 */
    unsigned int global_clk;    /* emmc工作时钟 */
    unsigned int global_type;   /* emmc工作位宽 */
    unsigned int global_timing; /* emmc工作模式 */
    bool tuning_flag;  /* emmc是否在tuning流程中，false-未执行，true-正在执行 */
};

struct emmc_dma_rw_infos {
    unsigned int start_addr;        /* 颗粒读写的起始地址-BLOCK为单位 */
    unsigned int total_size;        /* 颗粒读写的数据大小-BLOCK为单位 */
    unsigned int cur_rw_times;      /* 当前读写的次数 */
    unsigned int dma_dir;           /* 读写操作 */
    dma_haddr_t data_buf;  /* 数据读写存取的DDR地址 */
    s32 dma_request_state; /* DMA传输的完成状态 */
};

typedef enum {
    MMC_DMA_NONE = 0,
    MMC_DMA_WRITE = 1,
    MMC_DMA_READ = 2,
} mmc_dma_dir;

typedef enum {
    MMC_REQUEST_FINISH = 0,
    MMC_REQUEST_BUSY = 1,
    MMC_REQUEST_NONE = 2,
} mmc_request_state;

extern struct emmc_dma_rw_infos g_dma_info;
extern struct emmc_host g_host;

extern int mmc_config_clock(unsigned int cclk);
extern int mmc_non_dma_get_ext_csd(unsigned char *buf);
extern int emmc_switch(unsigned char index, unsigned char value);
extern int mmc_non_dma_rw_blcoks( unsigned int start_addr, unsigned int type, unsigned int num, unsigned char *buf);
extern int mmc_card_ready(void);
extern int mmc_non_dma_get_ext_csd(unsigned char *buf);
int mmc_irq_rw_blcoks(unsigned int start_addr, unsigned int type, unsigned int num, dma_haddr_t buf);
void mmc_idma_stop(void);

#endif
