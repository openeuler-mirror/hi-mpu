/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: DDR driver APIs
 * Create: 2018-05-10
 */
#ifdef CONFIG_HI3093_SPI_BOOT
#include <hi309x_common.h>
#else
#include <common.h>
#endif
#include <dm.h>
#include <asm/io.h>
#include "bd_info.h"
#ifdef CONFIG_HI3093_SPI_BOOT
#include "ddr.h"

typedef struct {
    u32 row_bits;
    u32 col_bits;
    u32 bank_bits;
    u32 width;
    u32 ecc_on;
} dram_param;

u8 g_wdq_bdl_ini[DDRPHY_BYTELANE_MAX][DDRPHY_BYTELANE_DQ_MAX];
u8 g_wdq_bdl_jump[DDRPHY_BYTELANE_MAX];
u8 g_full_flag[DDRPHY_BYTELANE_MAX];
u32 g_best_vref[DDRPHY_BYTELANE_MAX] = {MIN_WRITE_VREF_SCAN, MIN_WRITE_VREF_SCAN};

/* bit28-bit31:sfc_bank_addr bit10-bit27: sfc_row/col_addr bit5-bit9:tim_para bit0-bit4: cmd_code */
#define GENERATE_MRS(bank, mode, para, code)  \
    (((bank) << 28) | (((mode) & 0x3FFFF) << 10) | (((para) & 0x1f) << 5) | ((code) & 0x1f))

/* 32个bit位，先将数值不需要的高位置0，再整体偏移lowPos位 */
#define READ_REG32_BITS_VAL(val, pos, width)  \
    (((val) & (0xFFFFFFFFU >> (32 - ((pos) + (width))))) >> (pos))

#define WRITE_REG32_BITS_VAL(val, pos, width, data) \
    (((val) & (~(((0x1U << (width)) - 1) << (pos)))) | ((data) << (pos)))


/* 读取寄存器对应位区间整体值 */
static inline u32 dram_reg32_read_bits(u32 addr, u32 low_pos, u32 bit_width)
{
    u32 reg_val;

    reg_val = readl(addr);

    return  READ_REG32_BITS_VAL(reg_val, low_pos, bit_width);
}

/* 将寄存器对应位区间置为data值 */
static inline void dram_reg32_write_bits(u32 addr, u32 data, u32 low_pos, u32 bit_width)
{
    u32 reg_val;

    reg_val = readl(addr);
    /* 对应位区域清0，再赋值 */
    reg_val = WRITE_REG32_BITS_VAL(reg_val, low_pos, bit_width, data);
    writel(reg_val, addr);
    return;
}

void dram_exmbist_set_pattern(u32 pattern)
{
    /* set pattern */
    writel(0x00000013, MDDRC_EXMBIST_CMD_REG);
    writel(pattern, MDDRC_EXMBIST_DAT_REG);
    writel(pattern, MDDRC_EXMBIST_DAT_REG);
    writel(pattern, MDDRC_EXMBIST_DAT_REG);
    writel(pattern, MDDRC_EXMBIST_DAT_REG);
    writel(pattern, MDDRC_EXMBIST_DAT_REG);
    writel(pattern, MDDRC_EXMBIST_DAT_REG);
    writel(pattern, MDDRC_EXMBIST_DAT_REG);
    writel(pattern, MDDRC_EXMBIST_DAT_REG);
}

void dram_exmbist_set_datamask(void)
{
    /* set mask */
    writel(0x00000019, MDDRC_EXMBIST_CMD_REG);
    writel(0xffffffff, MDDRC_EXMBIST_DAT_REG);
    writel(0xffffffff, MDDRC_EXMBIST_DAT_REG);
    writel(0xffffffff, MDDRC_EXMBIST_DAT_REG);
    writel(0xffffffff, MDDRC_EXMBIST_DAT_REG);
    writel(0xffffffff, MDDRC_EXMBIST_DAT_REG);
    writel(0xffffffff, MDDRC_EXMBIST_DAT_REG);
    writel(0xffffffff, MDDRC_EXMBIST_DAT_REG);
    writel(0xffffffff, MDDRC_EXMBIST_DAT_REG);
}

void print_bist_error(int stg)
{
    u32 i;
    u32 status;
    u32 write_value;
    u32 err_info;
    char buf[DDRC_PRINT_BUF_SIZE] = {0};

    snprintf(buf, sizeof(buf), "[DDR]bist failed stg%d\r\n", stg);
    uart_early_puts(CONFIG_SYS_UART_NUM, buf);

    status = readl(MDDRC_EXMBIST_STS_REG);
    write_value = 0x80;
    writel(write_value, MDDRC_EXMBIST_CMD_REG);
    err_info = readl(MDDRC_EXMBIST_CMD_REG);

    snprintf(buf, sizeof(buf), "[DDR]status(0x%x) diag_addr(0x%x)\r\n", status, err_info);
    uart_early_puts(CONFIG_SYS_UART_NUM, buf);

    for (i = 0; i < DRAM_CFG_ITL_BITS; i++) {
        write_value = ((i << DRAM_CFG_ITL_SHIFT) | 0x81);
        writel(write_value, MDDRC_EXMBIST_CMD_REG);
        err_info = readl(MDDRC_EXMBIST_CMD_REG);
        snprintf(buf, sizeof(buf), "[DDR]actual_value%d(0x%x) \r\n", i, err_info);
        uart_early_puts(CONFIG_SYS_UART_NUM, buf);
    }

    for (i = 0; i < DRAM_CFG_COL_BITS; i++) {
        write_value = ((i << DRAM_CFG_COL_SHIFT) | 0x82);
        writel(write_value, MDDRC_EXMBIST_CMD_REG);
        err_info = readl(MDDRC_EXMBIST_CMD_REG);
        snprintf(buf, sizeof(buf), "[DDR]exp_value%d(0x%x) \r\n", i, err_info);
        uart_early_puts(CONFIG_SYS_UART_NUM, buf);
    }

    write_value = 0x83;
    writel(write_value, MDDRC_EXMBIST_CMD_REG);
    err_info = readl(MDDRC_EXMBIST_CMD_REG);
    snprintf(buf, sizeof(buf), "[DDR]error cnt(0x%x)\r\n", err_info);
    uart_early_puts(CONFIG_SYS_UART_NUM, buf);
}

s32 dram_exmbist_check_result(int stg)
{
    while ((readl(MDDRC_EXMBIST_STS_REG) & 0x1) != 0x1) {
        early_udelay(1);
    }
    if ((readl(MDDRC_EXMBIST_STS_REG) & 0x2) != 0x2) {
        print_bist_error(stg);
        return -1;
    }

    return 0;
}

s32 dram_exmbist_MarchIC(void)
{
    s32 ret;

    dram_exmbist_set_pattern(0x55aa55aa);

    dram_exmbist_set_datamask();

    /* M0 is {->(wA);} */
    writel(CMD_ASCEND_TEST_DDR, MDDRC_EXMBIST_CMD_REG);
    writel(CMD_IME_WR_PAT, MDDRC_EXMBIST_CMD_REG);
    ret = dram_exmbist_check_result(0); /* exmbist stage0 */
    if (ret) {
        return ret;
    }

    /* M1 is {->(rA,w(!A));} */
    writel(CMD_RD_PAT, MDDRC_EXMBIST_CMD_REG);
    writel(CMD_IME_WR_INV_PAT, MDDRC_EXMBIST_CMD_REG);
    ret = dram_exmbist_check_result(1); /* exmbist stage1 */
    if (ret) {
        return ret;
    }
    /* M2 is {->(r(!A),wA);} */
    writel(CMD_RD_INV_PAT, MDDRC_EXMBIST_CMD_REG);
    writel(CMD_IME_WR_PAT, MDDRC_EXMBIST_CMD_REG);
    ret = dram_exmbist_check_result(2); /* exmbist stage2 */
    if (ret) {
        return ret;
    }

    /* M3 is {<-(rA,w(!A));} */
    writel(CMD_DESCEND_TEST_DDR, MDDRC_EXMBIST_CMD_REG);
    writel(CMD_RD_PAT, MDDRC_EXMBIST_CMD_REG);
    writel(CMD_IME_WR_INV_PAT, MDDRC_EXMBIST_CMD_REG);
    ret = dram_exmbist_check_result(3); /* exmbist stage3 */
    if (ret) {
        return ret;
    }
    /* M4 is {<-(r(!A),wA);} */
    writel(CMD_RD_INV_PAT, MDDRC_EXMBIST_CMD_REG);
    writel(CMD_IME_WR_PAT, MDDRC_EXMBIST_CMD_REG);
    ret = dram_exmbist_check_result(4); /* exmbist stage4 */
    if (ret) {
        return ret;
    }
    /* M5 is {<-(rA);} */
    writel(CMD_IME_RD_PAT, MDDRC_EXMBIST_CMD_REG);
    ret = dram_exmbist_check_result(5); /* exmbist stage5 */
    if (ret) {
        return ret;
    }

    return 0;
}

s32 dram_clear(void)
{
    s32 ret;

    dram_exmbist_set_pattern(0x00000000);

    dram_exmbist_set_datamask();

    /* M0 is {->(wA);} */
    writel(CMD_ASCEND_TEST_DDR, MDDRC_EXMBIST_CMD_REG);
    writel(CMD_IME_WR_PAT, MDDRC_EXMBIST_CMD_REG);
    ret = dram_exmbist_check_result(0);
    if (ret) {
        return ret;
    }

    /* M1 is {->(rA);} */
    writel(CMD_IME_RD_PAT, MDDRC_EXMBIST_CMD_REG);
    ret = dram_exmbist_check_result(1);
    if (ret) {
        return ret;
    }

    return 0;
}

void dram_exmbist_set_ddr_area(u32 value)
{
    writel(0x00000011, HI309x_DDRC_REG_BASE + 0x8158);
    writel(0x00000000, HI309x_DDRC_REG_BASE + 0x815c);
    writel(0x00000000, HI309x_DDRC_REG_BASE + 0x815c);
    writel(value, HI309x_DDRC_REG_BASE + 0x815c);
    writel(0x00000000, HI309x_DDRC_REG_BASE + 0x815c);

    return;
}

s32 dram_exmbist(u32 row_bits, u32 col_bits, u32 bank_bits, u32 width)
{
    s32 ret;
    /* dram col bits trans to exmbist col bits, minus 10 then add 6 */
    u32 exmbist_col = (col_bits - 10) + 6;
    /* exmbist bank bits is always 4 if DDR4 */
    u32 exmbist_bank = 4;
    u32 value;

    /* exmbist init */
    writel(0x00000100, HI309x_DDRC_REG_BASE + 0x8030);
    writel(0x00000c0c, HI309x_DDRC_REG_BASE + 0x814c);
    value = readl(HI309x_DDRC_REG_BASE + 0x8040);
    value |= (1 << EXMBIST_CLKON_SHIFT);
    writel(value, HI309x_DDRC_REG_BASE + 0x8040);
    value = readl(HI309x_DDRC_REG_BASE + 0x8160);
    value &= ~(0xfU << MBIST_RANK_SHIFT);
    writel(value, HI309x_DDRC_REG_BASE + 0x8160);

    writel(0x00000001, HI309x_DDRC_REG_BASE + 0x8014);

    /* set row/col/bank bits */
    writel(0x04040018, HI309x_DDRC_REG_BASE + 0x8158);
    value = (exmbist_bank << 24U) | (exmbist_col << 16U) | (row_bits << 8U) | 0x12;
    writel(value, HI309x_DDRC_REG_BASE + 0x8158);

    /* max address: (((max_row +1)<<cfg_col_bits)-1)<<cfg_bank_bits, max bank just 0x1 if dram_x16 */
    if (width == DRAM_X16) {
        value = ((((1U << row_bits) << exmbist_col) - 1) << exmbist_bank) | 0x1;
    } else {
        value = (((1U << row_bits) << exmbist_col) << exmbist_bank) - 1;
    }
    /* set address area */
    dram_exmbist_set_ddr_area(value);

    /* marchIC test */
    ret = dram_exmbist_MarchIC();
    if (ret) {
        goto out;
    }

    /* set clear ddr area */
    value = (((1U << row_bits) << exmbist_col) << exmbist_bank) - 1;
    dram_exmbist_set_ddr_area(value);

    ret = dram_clear();
    if (ret) {
        goto out;
    }
out:
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8014) = 0x00000000;

    if (!ret) {
        writel(0x1, DDR_RSV_MEM_BIST_RESULT);
        uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] dram bist test success.\r\n");
    } else {
        writel(0x0, DDR_RSV_MEM_BIST_RESULT);
        uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] dram bist test fail.\r\n");
    }

    return ret;
}
#endif // CONFIG_HI3093_SPI_BOOT end

static unsigned long get_ddr_size(void)
{
    bdinfo_init();
    return bdinfo_get_ddr_size();
}

static u32 get_ddr_ecc_state(void)
{
    bdinfo_init();
    return bdinfo_get_ddr_ecc();
}

unsigned long get_ramdisk_size(void)
{
#ifdef IBMC_RAMDISK_SIZE
    return IBMC_RAMDISK_SIZE;
#endif

    return 0;
}

#ifdef CONFIG_HI3093_SPI_BOOT
u32 hw_efuse_read_chip_name()
{
    /* efuse[355:340]: chip name total 16bits
     * efuse[367:356]: chip version total 12bits
     * SYSCTL_DIEID10[31:0]: efuse[351:320]
     * SYSCTL_DIEID11[24:0]: efuse[376:352]
     */
    u32 chip_name;
    u32 die_id10_vaule = readl(SYSCTL_DIEID10_BASE);
    u32 die_id11_vaule = readl(SYSCTL_DIEID11_BASE);

    chip_name = (die_id10_vaule >> 20) & 0xFFF;  /* chip_name[11:0] = SYSCTL_DIEID10[31:20]*/
    chip_name |= ((die_id11_vaule & 0xF) << 12); /* chip_name[15:12] =  SYSCTL_DIEID11[3:0] */

    return chip_name;
}

u32 hw_efuse_read_chip_version()
{
    /* efuse[355:340]: chip name total 16bits
     * efuse[367:356]: chip version total 12bits
     * SYSCTL_DIEID10[31:0]: efuse[351:320]
     * SYSCTL_DIEID11[24:0]: efuse[376:352]
     */
    u32 chip_version;
    u32 die_id11_vaule = readl(SYSCTL_DIEID11_BASE);

    chip_version = (die_id11_vaule >> 4) & 0xFFF; /* chip_version[11:0] = SYSCTL_DIEID11[15:4] */

    return chip_version;
}
#endif // CONFIG_HI3093_SPI_BOOT end

unsigned long get_ddr_rsv_size(void)
{
    return IBMC_SYS_RSV_SIZE + get_ramdisk_size();
}

#ifdef CONFIG_HI3093_SPI_BOOT
static u32 dram_get_valid_phase(u32 phase)
{
    return phase - ((phase + 1) >> SHIFT_2);
}

static u32 dram_phy_phase_check(u32 phase_min, u32 phase_max, u32 phase_mid)
{
    u32 valid_phase_min, valid_phase_max, valid_phase_mid;
    u32 dif0, dif1;

    valid_phase_min = dram_get_valid_phase(phase_min);
    valid_phase_max = dram_get_valid_phase(phase_max);
    valid_phase_mid = dram_get_valid_phase(phase_mid);

    dif0 = valid_phase_mid - valid_phase_min;
    dif1 = valid_phase_max - valid_phase_mid;

    if (dif0 == dif1) {
        return 0;
    } else {
        return ((dif0 > dif1) ? DDRPHY_PHASE_LEFT_SHIFT : DDRPHY_PHASE_RIGHT_SHIFT);
    }
}

static void dram_assert_phyupd(void)
{
    writel(0x8000, (HI309x_DDRC_REG_BASE + 0xc004));
}

static void dram_deassert_phyupd(void)
{
    u32 reg;

    reg = readl(HI309x_DDRC_REG_BASE + 0xc070);
    reg |= CONFIG_DLYUPD;
    writel(reg, (HI309x_DDRC_REG_BASE + 0xc070));
    early_udelay(1);

    reg &= ~CONFIG_DLYUPD;
    writel(reg, (HI309x_DDRC_REG_BASE + 0xc070));
    writel(0x0, (HI309x_DDRC_REG_BASE + 0xc004));
}

/* trainning完成后需要对写方向时序配置调整 */
void dram_phy_bdl_adjust(u32 bytelane)
{
    u32 ret, reg_val;
    u32 right_ph = readl(HI309x_DDRC_REG_BASE + 0xc254 + (bytelane * 0x80)) & 0x1FU;
    u32 left_ph = (readl(HI309x_DDRC_REG_BASE + 0xc254 + (bytelane * 0x80)) >> SHIFT_16) & 0x1FU;
    u32 wdq_ph = (readl(HI309x_DDRC_REG_BASE + 0xc234 + (bytelane * 0x80)) >> SHIFT_8) & 0x1FU;
    u32 rdqsbdl30 = readl(HI309x_DDRC_REG_BASE + 0xc248 + (bytelane * 0x80)) & 0x1FFU;
    u32 wdq0bdl = readl(HI309x_DDRC_REG_BASE + 0xc210 + (bytelane * 0x80)) & 0x7FU;
    u32 wdq1bdl = (readl(HI309x_DDRC_REG_BASE + 0xc210 + (bytelane * 0x80)) >> SHIFT_8) & 0x7FU;
    u32 wdq2bdl = (readl(HI309x_DDRC_REG_BASE + 0xc210 + (bytelane * 0x80)) >> SHIFT_16) & 0x7FU;
    u32 wdq3bdl = (readl(HI309x_DDRC_REG_BASE + 0xc210 + (bytelane * 0x80)) >> SHIFT_24) & 0x7FU;
    u32 wdq4bdl = readl(HI309x_DDRC_REG_BASE + 0xc214 + (bytelane * 0x80)) & 0x7FU;
    u32 wdq5bdl = (readl(HI309x_DDRC_REG_BASE + 0xc214 + (bytelane * 0x80)) >> SHIFT_8) & 0x7FU;
    u32 wdq6bdl = (readl(HI309x_DDRC_REG_BASE + 0xc214 + (bytelane * 0x80)) >> SHIFT_16) & 0x7FU;
    u32 wdq7bdl = (readl(HI309x_DDRC_REG_BASE + 0xc214 + (bytelane * 0x80)) >> SHIFT_24) & 0x7FU;
    u32 wdqmbdl = readl(HI309x_DDRC_REG_BASE + 0xc218 + (bytelane * 0x80)) & 0x7FU;
    u32 half_rdqsbdl = rdqsbdl30 >> 1;

    ret = dram_phy_phase_check(right_ph, left_ph, wdq_ph);
    if (ret) {
        dram_assert_phyupd();
        if (ret == DDRPHY_PHASE_LEFT_SHIFT) {
            if ((wdq_ph & DDRPHY_PHASE_VALID_MASK) == DDRPHY_PHASE_VALID_MASK) {
                wdq_ph--;
            }
            wdq_ph--;
            writel((wdq_ph << SHIFT_8), HI309x_DDRC_REG_BASE + 0xc234 + (bytelane * 0x80));
        }

        wdq0bdl += half_rdqsbdl;
        wdq1bdl += half_rdqsbdl;
        wdq2bdl += half_rdqsbdl;
        wdq3bdl += half_rdqsbdl;
        wdq4bdl += half_rdqsbdl;
        wdq5bdl += half_rdqsbdl;
        wdq6bdl += half_rdqsbdl;
        wdq7bdl += half_rdqsbdl;
        wdqmbdl += half_rdqsbdl;

        reg_val = (wdq3bdl << SHIFT_24) | (wdq2bdl << SHIFT_16) | (wdq1bdl << SHIFT_8) | wdq0bdl;
        writel(reg_val, HI309x_DDRC_REG_BASE + 0xc210 + (bytelane * 0x80));
        reg_val = (wdq7bdl << SHIFT_24) | (wdq6bdl << SHIFT_16) | (wdq5bdl << SHIFT_8) | wdq4bdl;
        writel(reg_val, HI309x_DDRC_REG_BASE + 0xc214 + (bytelane * 0x80));
        writel(wdqmbdl, HI309x_DDRC_REG_BASE + 0xc218 + (bytelane * 0x80));

        dram_deassert_phyupd();
    }
}

void dram_init_cfg_axi(u32 ecc_on)
{
    if (ecc_on == 0) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0020) = 0x00000001;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0104) = 0x00140004;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0114) = 0x00140004;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0124) = 0x00142004;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0134) = 0x00144004;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0144) = 0x00146004;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0800) = 0x00001ffe;
    } else {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0020) = 0x01f00005;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0104) = 0x00140004;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0114) = 0x00140004;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0124) = 0x00142004;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0134) = 0x00144004;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0144) = 0x00146004;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0800) = 0x00001f81;
    }
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0100) = 0x00001500;  // region size 2G
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0110) = 0x00001520;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0120) = 0x00001540;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0130) = 0x00001560;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0140) = 0x00001580;

    /* stepX cfg axi_qos_wrpri */
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0204) = 0x00000000;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0214) = 0x00000001;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0224) = 0x00000004;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0234) = 0x00000003;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0244) = 0x00000002;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0254) = 0x00000005;

    /* stepX cfg axi_qos_rdpri */
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0208) = 0x00000000;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0218) = 0x00000001;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0228) = 0x00000004;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0238) = 0x00000003;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0248) = 0x00000002;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0258) = 0x00000005;

    uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] cfg axi success.\r\n");

    return;
}

void dram_init_cfg_sec(void)
{
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x1004) = 0x00000001;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x1404) = 0x00000001;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x1804) = 0x00000001;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x1c04) = 0x00000001;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x2004) = 0x00000001;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x2404) = 0x00000001;

    uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] cfg sec success.\r\n");

    return;
}

void dram_inif_cfg_qosbuf(void)
{
    uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] cfg qosbuf success.\r\n");

    return;
}

static void dram_init_cfg_dmc1_2400M()
{
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8100) = 0x444a5d14;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8058) = 0x000efc00;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8104) = 0x80400c14;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8108) = 0x62012000;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x810c) = 0xffd00a45;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8110) = 0x10cf2018;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8114) = 0x00004009;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8118) = 0x004540ff;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x811c) = 0x00004444;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8120) = 0x06934809;

    return;
}

static void dram_init_cfg_dmc1_2133M()
{
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8100) = 0x43421a12;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8104) = 0x80400b0f;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8108) = 0x62010000;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x810c) = 0xffd00944;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8110) = 0x10cf2018;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8114) = 0x00004008;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8118) = 0x004430ff;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x811c) = 0x00004444;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8120) = 0x05daec08;

    return;
}

static void dram_init_cfg_dmc1_1866M()
{
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8100) = 0x4339d710;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8104) = 0x80400a0d;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8108) = 0x5200e000;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x810c) = 0xffd00834;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8110) = 0x10cf2018;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8114) = 0x00004007;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8118) = 0x003430ff;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x811c) = 0x00004444;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8120) = 0x05229007;

    return;
}

static void dram_init_cfg_dmc1_1600M()
{
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8100) = 0x4331940e;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8104) = 0x8040090b;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8108) = 0x4200e000;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x810c) = 0xffd00733;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8110) = 0x10cf2018;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8114) = 0x00004006;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8118) = 0x003330ff;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x811c) = 0x00004444;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8120) = 0x04623006;

    return;
}

s32 dram_init_cfg_dmc1(u32 row_bits, u32 col_bits, u32 bank_bits, s32 rate)
{
    u32 delay_cnt = 0;
    u32 row, col;
    u32 bank, bank_grp;
    char buf[DDRC_PRINT_BUF_SIZE] = {0};

    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8030) = 0x00000100;  // ecc bypass, 此寄存器只影响带外ecc
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8040) = 0x0001663f;  // ddrc workmode
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8044) = 0x0000fffe;  // rank1~15 disable
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8050) = 0x30c10017;  // bank mode

    /* col bits 8~12 in bits0~2, row bits 11~18 in bits4~6, bank num 4/8/16 in bits8~9, bank group1/2/4 in bits10~11 */
    row = (row_bits - DRAM_ROW_BITS_MIN) & 0x7U;
    col = (col_bits - DRAM_COL_BITS_MIN) & 0x7U;
    bank = (bank_bits - DRAM_BANK_BITS_MIN) & 0x3U;
    bank_grp = (bank_bits - DRAM_BANK_BITS_MIN) & 0x3U;
    /* col bit[0-2], row bit[4-6], bank bit[8-9] bank_grp bit [10-11] */
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8060) = ((bank_grp << 10) | (bank << 8) | (row << 4) | col);
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x80a0) = 0x00000001;  // enable wodt

    if (rate == HI309x_DDRC_2400M) {
        dram_init_cfg_dmc1_2400M();
    } else if (rate == HI309x_DDRC_2133M) {
        dram_init_cfg_dmc1_2133M();
    } else if (rate == HI309x_DDRC_1866M) {
        dram_init_cfg_dmc1_1866M();
    } else if (rate == HI309x_DDRC_1600M) {
        dram_init_cfg_dmc1_1600M();
    } else {
        snprintf(buf, sizeof(buf), "[DDR] init with bad rate %d\r\n", rate);
        uart_early_puts(CONFIG_SYS_UART_NUM, buf);
        return -1;
    }

    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x814c) = 0x00000c0c;  // cmd fifo depth
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8198) = 0x00000000;  // crc not bypass
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8000) = 0x00000002;  // ddrphy sref done

    while (*(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8294) & 0x1) {
        early_udelay(10);              /* 延时 10 微秒，等寄存器生效 */
        if (++delay_cnt > 100) { /* 循环100次，总共延时1毫秒 */
            uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] wait self refresh timeout.\r\n");
            return -1;
        }
    }

    uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] cfg dmc1 success.\r\n");

    return 0;
}

void dram_init_cfg_ddrphy_stage1(u32 rate)
{
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc074) = 0x00000000; /* rank to rank delay */
    if (ddrc_is_1711v110()) {
        if (rate == HI309x_DDRC_2400M) {
            *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc208) = 0x02f80400; /* data block ctrl */
            *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc288) = 0x02f80400;
        } else {
            *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc208) = 0x02f80000; /* data block ctrl */
            *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc288) = 0x02f80000;
        }
    } else {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc208) = 0x00000000; /* data block ctrl */
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc288) = 0x00000000;
    }
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc018) = 0xf3000307; /* timing para for pll */
    /* delay 2us */
    early_udelay(2);

    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc018) = 0xf3000302;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc01c) = 0x4e200080; /* pll ctrl */
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc010) = 0x000920ff; /* timing para for phy */
    if (ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc014) = 0x2001d4c0; /* phy timer1 */
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc02c) = 0x001c0c2a; /* dram configure */
    } else {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc014) = 0x0001d4c0; /* phy timer1 */
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc02c) = 0x0000002a; /* dram configure */
    }
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc030) = 0x6aa59993; /* dram timer0 */
    if (ddrc_is_1711v110()) {
        if (rate == HI309x_DDRC_2400M) {
            *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc034) = 0x1b4860c4; /* dram timer1 */
        } else {
            *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc034) = 0x154860c4; /* dram timer1 */
        }
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc038) = 0x3ff20880; /* dram timer2 */
    } else {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc034) = 0x154860c4; /* dram timer1 */
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc038) = 0x3ff81080; /* dram timer2 */
    }

    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc03c) = 0xf0100200; /* dram timer3 */
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc040) = 0x603464e8; /* dram timer4 */
    if (!ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0dc) = 0xea620333;
    }
    uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] ddrphy stage1.\r\n");

    return;
}

static void dram_init_cfg_ddrphy_speed_2400M(void)
{
    if (!ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc208) = 0x00000400;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc288) = 0x00000400;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc034) = 0x1b4860c4; /* dram timer1 */
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc040) = 0x803464e8; /* dram timer4 */
    }
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0bc) = 0x0000000c;
    if (ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc064) = 0x01010944;
    } else {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc064) = 0x02010944;
    }
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc068) = 0x02000018;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0e0) = 0x14800000;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0e4) = 0x00001010;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc070) = 0x0000100b;
    if (!ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0dc) = 0xea720333;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc11c) = 0x0000034d;
    }
}

static void dram_init_cfg_ddrphy_speed_2133M(void)
{
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0bc) = 0x0000000b;
    if (ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc064) = 0x01010730;
    } else {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc064) = 0x02010730;
    }
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc068) = 0x02000010;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0e0) = 0x04800000;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0e4) = 0x00000810;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc070) = 0x00000b0a;
}

static void dram_init_cfg_ddrphy_speed_1866M(void)
{
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0bc) = 0x0000000a;
    if (ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc064) = 0x01010520;
    } else {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc064) = 0x02010720;
    }
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc068) = 0x02000008;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0e0) = 0x04800000;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0e4) = 0x00000410;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc070) = 0x00000909;
}

static void dram_init_cfg_ddrphy_speed_1600M(void)
{
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0bc) = 0x00000009;  // dram timer5
    if (ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc064) = 0x01010310;  // mode reg 01
    } else {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc064) = 0x02010310;  // mode reg 01
    }
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc068) = 0x00000000;  // mode reg 23
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0e0) = 0x04800000;  // mode reg 45
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0e4) = 0x00000410;  // mode reg 67
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc070) = 0x00000708;  // misc ctrl, read/write latency
}

s32 dram_init_cfg_ddrphy_speed(s32 rate)
{
    if (rate == HI309x_DDRC_2400M) {
        dram_init_cfg_ddrphy_speed_2400M();
    } else if (rate == HI309x_DDRC_2133M) {
        dram_init_cfg_ddrphy_speed_2133M();
    } else if (rate == HI309x_DDRC_1866M) {
        dram_init_cfg_ddrphy_speed_1866M();
    } else if (rate == HI309x_DDRC_1600M) {
        dram_init_cfg_ddrphy_speed_1600M();
    } else {
        return -1;
    }

    uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] ddrphy speed.\r\n");

    return 0;
}

static s32 dram_check_training_success(void)
{
    u32 delay_cnt = 0;
    u32 phy_status;
    char buf[DDRC_PRINT_BUF_SIZE] = {0};

    while (readl(HI309x_DDRC_REG_BASE + 0xc004)) {
        early_udelay(10);              /* 延时 10 微秒，等寄存器生效 */
        if (++delay_cnt > 100) { /* 循环100次，总共延时1毫秒 */
            uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] wait timeout.\r\n");
            return -1;
        }
    }

    phy_status = readl(HI309x_DDRC_REG_BASE + 0xc008);
    if (phy_status) {
        writel(phy_status, (HI309x_DDRC_REG_BASE + 0xc008));
        snprintf(buf, sizeof(buf), "[DDR] training failed(0x%x)\r\n", phy_status);
        uart_early_puts(CONFIG_SYS_UART_NUM, buf);
        return -1;
    } else {
        dram_phy_bdl_adjust(0);
        dram_phy_bdl_adjust(1);
        uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] training success\r\n");
    }

    return 0;
}

static void dram_init_cfg_ddrphy_trainctrl(u32 rate)
{
    if (ddrc_is_1711v110()) {
        if (rate == HI309x_DDRC_2400M) {
            *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0dc) = 0xca520000;
        } else {
            *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0dc) = 0xca420000;
        }
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc048) = 0x60cc8090;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc114) = 0x28066100;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc118) = 0x2a046203;
        if (rate == HI309x_DDRC_2400M) {
            *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc11c) = 0x0360035d;
        } else {
            *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc11c) = 0x03600010;
        }
    } else {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc048) = 0x60441090;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc114) = 0x2a066021;
    }
}

s32 dram_init_cfg_ddrphy_stage2(u32 rate)
{
    dram_init_cfg_ddrphy_trainctrl(rate);
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xd25c) = 0x00000000;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc044) = 0x0000000f;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc080) = 0x1fffc3ff;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc078) = 0x00003984;
    if (ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc084) = 0x00104100;
    } else {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc084) = 0x00004100;
    }
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xd208) = 0x0002ff00;
    if (ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xd238) = 0x20220000;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xd2b8) = 0x20220000;
    }
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xd248) = 0x00303652;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xd2c8) = 0x00303652;
    if (!ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xd064) = 0xffff0770;
    }
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc1d4) = 0x00000000;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc020) = 0x0c000421;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc270) = 0x00000002;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc2f0) = 0x00000002;
    if (ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc274) = 0x0000000c;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc2f4) = 0x0000000c;
    } else {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc274) = 0x00000010;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc2f4) = 0x00000010;
    }
    if (!ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc0d0) = 0xf066f3ff;
    }
    if (ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xd018) = 0x0fff0ff0;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xd204) = 0x9001f805;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xd078) = 0xffff0000;
    } else {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xd018) = 0x02db0000;
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xd204) = 0x6c01b005;
    }
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc004) = 0x0000140f;
    if (!ddrc_is_1711v110()) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0xc00c) = 0x40003f03;
    }

    return dram_check_training_success();
}

s32 dram_cfg_ddrphy_vref(u32 width, s32 rate, u32 vref)
{
    u32 delay_cnt = 0;
    u32 mode_reg;
    u32 value;

    writel(0x00010000, HI309x_DDRC_REG_BASE + 0x8218);
    if ((rate == HI309x_DDRC_2400M) || (rate == HI309x_DDRC_2133M)) {
        value = (width == DRAM_X8) ? 0x900200e0 : 0x500200e0;
    } else if ((rate == HI309x_DDRC_1866M) || (rate == HI309x_DDRC_1600M)) {
        value = (width == DRAM_X8) ? 0x900200e0 : 0x500200e0;
    } else {
        return -1;
    }

    mode_reg = readl(MODEREG67_BASE_ADDR) >> DDRPHY_MR6_MODE_SHIFT;
    value |= (mode_reg & DDRPHY_MR6_MODE_MASK) << DDRC_CFG_SFC_MR6_SHIFT;
    value |= ((vref & DDRPHY_VREF_MASK) << DDRPHY_VREF_SHIFT);

    writel(value, HI309x_DDRC_REG_BASE + 0x8214);
    writel(value, HI309x_DDRC_REG_BASE + 0x8214);
    value &= ~DDRPHY_VREF_TRAINING_ENABLE;
    writel(value, HI309x_DDRC_REG_BASE + 0x8214);

    writel(0x00000001, HI309x_DDRC_REG_BASE + 0x800c);
    while (readl(HI309x_DDRC_REG_BASE + 0x800c) & 0x1) {
        early_udelay(10);              /* 延时 10 微秒，等寄存器生效 */
        if (++delay_cnt > 100) { /* 循环100次，总共延时1毫秒 */
            uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] wait complete timeout.\r\n");
            return -1;
        }
    }

    writel(0x00000200, HI309x_DDRC_REG_BASE + 0x8210);
    writel(0x00000000, HI309x_DDRC_REG_BASE + 0x8210);

    uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] ddrphy vref.\r\n");

    return 0;
}

s32 dram_init_cfg_ddrphy_stage3(void)
{
    writel(0x000003f1, (HI309x_DDRC_REG_BASE + 0xc004));

    return dram_check_training_success();
}

u32 ddrphy_wdq_bdl_get(u32 count, u32 dir, u32 byte_lane, u32 dq_num)
{
    u32 bdl_index;

    bdl_index = g_wdq_bdl_ini[byte_lane][dq_num];
    if (dir == DDRPHY_DIRECTION_LEFT) {
        bdl_index += (g_wdq_bdl_jump[byte_lane] + count);
        if (bdl_index >= DDRPHY_WDQ_BDL_MAX) {
            bdl_index = DDRPHY_WDQ_BDL_MAX;
            g_full_flag[byte_lane] = 1;
        }
    } else {
        if (bdl_index < (g_wdq_bdl_jump[byte_lane] + count)) {
            bdl_index = 0;
            g_full_flag[byte_lane] = 1;
        } else {
            bdl_index -= (g_wdq_bdl_jump[byte_lane] + count);
        }
    }

    return bdl_index;
}

void ddrphy_wdq_bdl_set(u32 count, u32 *bit_map, u32 dir)
{
    u32 reg_val, tmp;
    u32 dq_num, byte_lane;
    u32 bdl_index;

    dram_assert_phyupd();

    for (byte_lane = 0; byte_lane < DDRPHY_BYTELANE_MAX; byte_lane++) {
        for (dq_num = 0; dq_num < DDRPHY_BYTELANE_DQ_MAX; dq_num++) {
            if (bit_map[byte_lane] & (1U << dq_num)) {
                continue;
            }
            bdl_index = ddrphy_wdq_bdl_get(count, dir, byte_lane, dq_num);
            tmp = DDRPHY_WDQ_BDL_SHIFT(dq_num);
            reg_val = readl((uintptr_t)DDRPHY_WDQ_BDL(byte_lane, dq_num));
            reg_val &= ~((u32)DDRPHY_WDQ_BDL_MASK << tmp);
            reg_val |= (bdl_index << tmp) & ((u32)DDRPHY_WDQ_BDL_MASK << tmp);
            writel(reg_val, (uintptr_t)DDRPHY_WDQ_BDL(byte_lane, dq_num));
        }
    }

    dram_deassert_phyupd();
}

static s32 dram_phase_switch_bdl(void)
{
    u32 i, j;
    u32 phase, wdq_bdl;
    u32 cyc, count_bdl;
    u32 reg_val;

    dram_assert_phyupd();

    for (i = 0; i < DDRPHY_BYTELANE_MAX; i++) {
        cyc = (readl(DDRPHY_RDQS_CYCLE(i)) & DDRPHY_RDQS_CYCLE_MASK) >> DDRPHY_RDQS_CYCLE_SHIFT;
        g_wdq_bdl_jump[i] = (u8)(cyc / 10); /* 配置jump 为1/10时钟周期 */
        reg_val = readl(DDRPHY_WDQ_PHASE(i));
        phase = (reg_val & DDRPHY_WDQ_PHASE_MASK) >> DDRPHY_WDQ_PHASE_SHIFT;
        if (phase < DDRPHY_SWITCH_PHASE_NUM) {
            return -1;
        }

        phase -= DDRPHY_SWITCH_PHASE_NUM;

        reg_val &= ~DDRPHY_WDQ_PHASE_MASK;
        reg_val |= (phase << DDRPHY_WDQ_PHASE_SHIFT) & DDRPHY_WDQ_PHASE_MASK;
        writel(reg_val, DDRPHY_WDQ_PHASE(i));
        count_bdl = (DDRPHY_SWITCH_VALID_PHASE * cyc) / DDRPHY_PHASE_NUM_MAX;

        for (j = 0; j < DDRPHY_BYTELANE_DQ_MAX; j++) {
            wdq_bdl = (readl((uintptr_t)DDRPHY_WDQ_BDL(i, j)) >> DDRPHY_WDQ_BDL_SHIFT(j)) & DDRPHY_WDQ_BDL_MASK;
            g_wdq_bdl_ini[i][j] = (u8)(wdq_bdl + count_bdl);
            reg_val = readl((uintptr_t)DDRPHY_WDQ_BDL(i, j));
            reg_val &= ~((u32)DDRPHY_WDQ_BDL_MASK << DDRPHY_WDQ_BDL_SHIFT(j));
            reg_val |= ((u32)g_wdq_bdl_ini[i][j] << DDRPHY_WDQ_BDL_SHIFT(j));
            writel(reg_val, (uintptr_t)DDRPHY_WDQ_BDL(i, j));
        }
    }

    dram_deassert_phyupd();

    return 0;
}

static void ddrphy_bist_stop(void)
{
    /* DDR PHY BIST Stop */
    writel(0x3e008260, HI309x_DDRC_REG_BASE + 0xc054);
}

static void ddrphy_bist_reset(void)
{
    /* DDR PHY BIST Reset */
    writel(0x3e008262, HI309x_DDRC_REG_BASE + 0xc054);
}

static void ddrphy_bist_start(void)
{
    ddrphy_bist_reset();
    /* DDR PHY BIST Start */
    writel(0x3e008261, HI309x_DDRC_REG_BASE + 0xc054);
}

static void dram_cal_wdls(u32 *status, u32 *bit_map, u32 index, u32 *cal_wdl)
{
    u32 dq_num, byte_lane;
    bool st = true;

    for (byte_lane = 0; byte_lane < DDRPHY_BYTELANE_MAX; byte_lane++) {
        for (dq_num = 0; dq_num < DDRPHY_BYTELANE_DQ_MAX; dq_num++) {
            st = (status[byte_lane] & (1U << dq_num)) ||
                 (status[byte_lane] & (1U << (dq_num + DDRPHY_BYTELANE_DQ_MAX)));
            if ((st && !(bit_map[byte_lane] & (1U << dq_num))) ||
                (!st && g_full_flag[byte_lane])) {
                cal_wdl[byte_lane] += index;
                bit_map[byte_lane] |= (1U << dq_num);
            }
        }
    }
}

static void ddrphy_write_scan(u32 *cal_wbdl)
{
    s32 bdl_index;
    u8 direction;
    u32 bist_status_buf[DDRPHY_BYTELANE_MAX], error_bit_map_buf[DDRPHY_BYTELANE_MAX];
    u32 *bist_status = bist_status_buf;
    u32 *error_bit_map = error_bit_map_buf;

    for (direction = 0; direction < DDRPHY_DIRECTION_MAX; direction++) {
        error_bit_map[0] = error_bit_map[1] = 0;
        g_full_flag[0] = g_full_flag[1] = 0;
        bdl_index = 0;
        while ((error_bit_map[0] != 0xff) || (error_bit_map[1] != 0xff)) {
            bdl_index += 1;

            ddrphy_wdq_bdl_set(bdl_index, error_bit_map, direction);

            /* 启动DDR PHY BIST 测试 */
            ddrphy_bist_start();

            /* 延时1ms */
            early_udelay(1000);

            /* 停止BIST 测试 */
            ddrphy_bist_stop();

            bist_status[0] = readl(DDRPHY_BIST_STATUS(0));
            bist_status[1] = readl(DDRPHY_BIST_STATUS(1));

            /* 复位BIST 模块 */
            ddrphy_bist_reset();
            dram_cal_wdls(bist_status, error_bit_map, bdl_index, cal_wbdl);
        }
    }
}

static void ddrphy_training_best_vref(u32 width, u32 rate, u32 vref)
{
    u32 byte_lane;
    u32 cal_wbdl_buf[DDRPHY_BYTELANE_MAX] = { 0 };
    u32 *cal_wbdl = cal_wbdl_buf;
    static u32 max_wdl[DDRPHY_BYTELANE_MAX] = { 0 };

    dram_cfg_ddrphy_vref(width, rate, vref);
    ddrphy_write_scan(cal_wbdl);

    if (width == DRAM_X8) {
        for (byte_lane = 0; byte_lane < DDRPHY_BYTELANE_MAX; byte_lane++) {
            if (cal_wbdl[byte_lane] > max_wdl[byte_lane]) {
                max_wdl[byte_lane] = cal_wbdl[byte_lane];
                g_best_vref[byte_lane] = vref;
            }
        }
    } else {
        if (cal_wbdl[0] + cal_wbdl[1] > max_wdl[0]) {
            max_wdl[0] = cal_wbdl[0] + cal_wbdl[1];
            g_best_vref[0] = vref;
        }
    }
}

static void ddrphy_vref_write_training(u32 width, u32 rate)
{
    u32 bist_ctrl_reg0, bist_ctrl_reg1;
    u32 val;

    bist_ctrl_reg0 = readl(DDRPHY_DXN_BIST_CTRL(0));
    writel(0x1ff01, DDRPHY_DXN_BIST_CTRL(0));
    bist_ctrl_reg1 = readl(DDRPHY_DXN_BIST_CTRL(1));
    writel(0x1ff01, DDRPHY_DXN_BIST_CTRL(1));

    /* RANG1 vref training 扫描出最佳的vref */
    for (val = MIN_WRITE_VREF_SCAN; val < MAX_RANG1_WRITE_VREF_SCAN; val += VREF_STEP_JUMP) {
        ddrphy_training_best_vref(width, rate, DDRPHY_VREF_RANG1 | val);
    }

    /* RANG2 vref training 扫描出最佳的vref */
    for (val = MIN_WRITE_VREF_SCAN; val < MAX_RANG2_WRITE_VREF_SCAN; val += VREF_STEP_JUMP) {
        ddrphy_training_best_vref(width, rate, DDRPHY_VREF_RANG2 | val);
    }

    writel(bist_ctrl_reg0, DDRPHY_DXN_BIST_CTRL(0));
    writel(bist_ctrl_reg1, DDRPHY_DXN_BIST_CTRL(1));
}

u32 dram_generate_mrs_val(u32 mr_reg_id, u32 value, u32 tim_para, u32 cmd_code)
{
    u32 sfc_bank;
    u32 mrs;
    /* MR0: 0000, MR1: 0100, MR2: 1000, MR3: 1100,  MR4: 0001, MR5: 0101, MR6: 1001, MR7: 1101 */
    u32 ddr4_sfc_bank[DDR_SFC_MR_MAX] = {0, 0x4, 0x8, 0xc, 0x1, 0x5, 0x9, 0xd};

    sfc_bank = ddr4_sfc_bank[mr_reg_id];
    mrs = GENERATE_MRS(sfc_bank, value, tim_para, cmd_code);

    return mrs;
}

void dram_set_pda_mode_int(u32 *mode, u32 dq)
{
    u32 idx;
    u32 sfc_reg_value;
    u32 dq_select[DDRPHY_BYTELANE_MAX] = { 0xff00ff00, 0x00ff00ff }; /* 2X8颗粒对应的值写入寄存器 */

    /* 参考SFC配置流程中emrs_3的bit4 打开PDA */
    *mode = dram_reg32_read_bits(MODEREG_BASE_ADDR, MR3_LOWPOS, MR3_WIDTH);
    *mode = (*mode) | 0x10;

    /* open PDA mode-- modereg3 sfc_tim_para：0x12(tMOD) sfc_cmd_code：0x0(MR write) */
    sfc_reg_value = dram_generate_mrs_val(DDR_SFC_MR3, *mode, 0x12, 0);

    writel(sfc_reg_value, DDRC_CFG_SFC_ADDR0);
    /* 循环写4次，配置DQ0/DQ1 */
    for (idx = 0; idx < DDR_BURST_NUM; idx++) {
        writel(dq_select[dq], DDRC_CFG_SFC_WDATA_ADDR);
    }

    /* 写指针归零 */
    writel(0x0, DDRC_CFG_SFC_WCTRL_ADDR);
}

void dram_set_pda_mode_out(u32 *modereg23, u32 dq)
{
    u32 idx;
    u32 sfc_reg_value;

    *modereg23 = (*modereg23) & 0xffef;
    /* sfc_tim_para 0x12(tMOD) sfc_cmd_code：0x9 */
    sfc_reg_value = dram_generate_mrs_val(DDR_SFC_MR3, *modereg23, 0x12, 0x9);

    writel(sfc_reg_value, DDRC_CFG_SFC_ADDR0);
    for (idx = 0; idx < DDR_BURST_NUM; idx++) {
        writel(0x0, DDRC_CFG_SFC_WDATA_ADDR);
    }

    /* 写指针归零 */
    writel(0x0, DDRC_CFG_SFC_WCTRL_ADDR);
}

s32 wait_check_operation_finished(u32 wait_time)
{
    u32 status;
    u32 delay = 0;

    do {
            early_udelay(10); /* wait 10us */
            status = dram_reg32_read_bits(DDRC_CURR_FUNC_BASE_ADDR, IN_SFC_LOWPOS, IN_SFC_WIDTH);
            if (status == 0) {
                return 0;
            }
            delay++;
    } while (delay < wait_time);

    return -1;
}

u32 dram_set_vref_by_sfc(u32 vref_value)
{
    u32 ret;
    u32 sfc_reg_value;
    u32 mode_reg;

    mode_reg = dram_reg32_read_bits(MODEREG67_BASE_ADDR, MR6_LOWPOS, MR6_WIDTH);
    /* 0x80即bit7位：代表MR6 verf training 开关 */
    mode_reg = (mode_reg & 0xff00) | vref_value | 0x80;
    /* sfc_tim_para 0x12(tMOD) sfc_cmd_code：0x9 */
    sfc_reg_value = dram_generate_mrs_val(DDR_SFC_MR6, mode_reg, 0x12, 0x9);
    writel(sfc_reg_value, DDRC_CFG_SFC_ADDR0);
    writel(sfc_reg_value, DDRC_CFG_SFC_ADDR0);

    /* 关闭verf training */
    mode_reg = (mode_reg & 0xff00) | vref_value;
    /* sfc_tim_para 0x12(tMOD) sfc_cmd_code：0x9 */
    sfc_reg_value = dram_generate_mrs_val(DDR_SFC_MR6, mode_reg, 0x12, 0x9);

    writel(sfc_reg_value, DDRC_CFG_SFC_ADDR0);

    /* 请求执行sfc命令 */
    dram_reg32_write_bits(DDRC_CTRL_SFC_ADDR, 0x1, CMD_REQ_LOWPOS, CMD_REQ_WIDTH);
    ret = wait_check_operation_finished(DDRPHY_PLL_INIT_DELAY_CNT);
    if (ret) {
        uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] Do Sfc setting Verf Operation fail\r\n");
        return ret;
    }

    return 0;
}

static s32 dram_set_write_vref(u32 dq, u32 vref)
{
    u32 ret;
    u32 mode_reg = 0;

    /* 选择rank */
    dram_reg32_write_bits(DDRC_CFG_SFC_ADDR1, 0x1, SFC_RANK_LOWPOS, SFC_RANK_WIDTH);

    /* cmdque指针清零 */
    writel(0x200, HI309x_DDRC_REG_BASE + 0x8210);
    writel(0x0, HI309x_DDRC_REG_BASE + 0x8210);

    /* 配置值0x100001，启动发送pre-all指令 */
    writel(0x100001, DDRC_CFG_SFC_ADDR0);

    /* 使能PDA模式 */
    dram_set_pda_mode_int(&mode_reg, dq);

    /* 配置Vref MR6寄存器 */
    ret = dram_set_vref_by_sfc(vref);
    if (ret) {
        uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] Sfc Set MR6 Fail\r\n");
        return ret;
    }

    /* 退出PDA */
    dram_set_pda_mode_out(&mode_reg, dq);

    /* 请求执行sfc配置命令 */
    dram_reg32_write_bits(DDRC_CTRL_SFC_ADDR, 0x1, CMD_REQ_LOWPOS, CMD_REQ_WIDTH);
    ret = wait_check_operation_finished(DDRPHY_PLL_INIT_DELAY_CNT);
    if (ret) {
        uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] Do Sfc setting Verf Operation fail\r\n");
        return ret;
    }

    return 0;
}

static s32 dram_set_vref_by_pda_mode(u32 dq, u32 vref)
{
    u32 ret;
    u32 refresh_v;
    u32 pd_en_v;
    char buf[DDRC_PRINT_BUF_SIZE] = {0};

    refresh_v = dram_reg32_read_bits(DDRC_CFG_TIMING2_ADDR, TAREF_LOWPOS, TAREF_WIDTH);
    pd_en_v = dram_reg32_read_bits(DDRC_CFG_PD_ADDR, PD_EN_LOWPOS, PD_EN_WIDTH);

    dram_reg32_write_bits(DDRC_CFG_TIMING2_ADDR, 0x0, TAREF_LOWPOS, TAREF_WIDTH);
    dram_reg32_write_bits(DDRC_CFG_PD_ADDR, 0x0, PD_EN_LOWPOS, PD_EN_WIDTH);

    ret = dram_set_write_vref(dq, vref);
    if (ret) {
        snprintf(buf, sizeof(buf), "[DDR] set write vref fail dq = %d, vref = %d\r\n", dq, vref);
        uart_early_puts(CONFIG_SYS_UART_NUM, buf);
        return ret;
    }

    dram_reg32_write_bits(DDRC_CFG_TIMING2_ADDR, refresh_v, TAREF_LOWPOS, TAREF_WIDTH);
    dram_reg32_write_bits(DDRC_CFG_PD_ADDR, pd_en_v, PD_EN_LOWPOS, PD_EN_WIDTH);

    return 0;
}

static s32 dram_config_best_verf(u32 width, s32 rate)
{
    u32 value, tmp, i;
    s32 ret;

    ret = dram_phase_switch_bdl();
    if (ret) {
        return 0;
    }

    ddrphy_vref_write_training(width, rate);
    if (width == DRAM_X8) {
        ret = dram_set_vref_by_pda_mode(0, g_best_vref[0]);
        ret += dram_set_vref_by_pda_mode(1, g_best_vref[1]);
    } else {
        ret = dram_cfg_ddrphy_vref(width, rate, g_best_vref[0]);
    }

    if (ret) {
        return ret;
    }

    dram_assert_phyupd();
    for (i = 0; i < DDRPHY_BYTELANE_MAX; i++) {
        value = readl((uintptr_t)HI309x_DDRC_REG_BASE + 0xc230 + i * 0x80);
        tmp = (value >> DDRPHY_WDQSPHASE_OFFSET) & DDRPHY_WDQSPHASE_MASK;
        value = ((tmp + DDRPHY_WDQPHASE_DEFAULT_VAL) & DDRPHY_WDQPHASE_MASK) << DDRPHY_WDQPHASE_OFFSET;

        writel(value, (uintptr_t)(HI309x_DDRC_REG_BASE + 0xc234 + i * 0x80));
        writel(0x0, (uintptr_t)(HI309x_DDRC_REG_BASE + 0xc210 + i * 0x80));
        writel(0x0, (uintptr_t)(HI309x_DDRC_REG_BASE + 0xc214 + i * 0x80));
        writel(0x0, (uintptr_t)(HI309x_DDRC_REG_BASE + 0xc218 + i * 0x80));
    }
    dram_deassert_phyupd();

    writel(0x00000201, (HI309x_DDRC_REG_BASE + 0xc004));

    ret = dram_check_training_success();
    if (ret) {
        return ret;
    }

    writel(0x0c000401, (HI309x_DDRC_REG_BASE + 0xc020));
    return 0;
}

s32 dram_init_cfg_ddrphy(u32 width, s32 rate)
{
    s32 ret;

    dram_init_cfg_ddrphy_stage1(rate);

    ret = dram_init_cfg_ddrphy_speed(rate);
    if (ret) {
        return ret;
    }

    ret = dram_init_cfg_ddrphy_stage2(rate);
    if (ret) {
        return ret;
    }

    /* wait 500us to ensure the ddr init complete */
    early_udelay(500);

    /* verf */
    ret = dram_cfg_ddrphy_vref(width, rate, DEFAULT_VREF_CONFIG);
    if (ret) {
        return ret;
    }

    ret = dram_init_cfg_ddrphy_stage3();
    if (ret) {
        return ret;
    }

    return dram_config_best_verf(width, rate);
}

s32 dram_init_cfg_dmc2(s32 rate)
{
    if (rate == HI309x_DDRC_2400M) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8108) = 0x62012092;
    } else if (rate == HI309x_DDRC_2133M) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8108) = 0x62010081;
    } else if (rate == HI309x_DDRC_1866M) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8108) = 0x5200e071;
    } else if (rate == HI309x_DDRC_1600M) {
        *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8108) = 0x4100e061;
    } else {
        return -1;
    }

    uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] cfg dmc2 success.\r\n");

    return 0;
}

void dram_init_cfg_clrint(void)
{
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8284) = 0xffffffff;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x4174) = 0xffffffff;
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x0870) = 0xffffffff;

    return;
}

void dram_init_cfg_auto_low_power(void)
{
    *(volatile u32 *)(HI309x_DDRC_REG_BASE + 0x8028) = 0xfff1;

    return;
}

void dram_init_cfg_clr_ecc(void)
{
    writel(0x3, HI309x_DDRC_REG_BASE + 0x870);
    readl(HI309x_DDRC_REG_BASE + 0x890);
    readl(HI309x_DDRC_REG_BASE + 0x894);

    return;
}

void dram_init_param_fix(dram_param *param, u32 *ddr_size)
{
    u32 pcb_ver;
    char buf[DDRC_PRINT_BUF_SIZE] = {0};

    bdinfo_init();

    pcb_ver = board_info_pcb_ver_get();

    snprintf(buf, sizeof(buf),
                  "pcb_ver:     0x%x\r\n"
                  "ecc_on:      0x%x\r\n"
                  "width:       0x%x\r\n"
                  "ddr_size:    0x%x\r\n",
                  pcb_ver, param->ecc_on, param->width, *ddr_size);
    uart_early_puts(CONFIG_SYS_UART_NUM, buf);

    if ((pcb_ver == EVB_V110_PCB_VERSION)||(pcb_ver == 0x7)) {
        return;
    }

    param->width = DRAM_X8;
    param->ecc_on = 1;
    param->col_bits = 10; /* col bits is 10 for allsize */
    *ddr_size = DDR_SIZE_1G;

    return;
}

void dram_init_param_get(dram_param *param)
{
    u32 initial_info = readl(SYSCTL_INITIAL_INF_BASE);
    u32 ddr_size;

    param->ecc_on = (initial_info >> 31) & 0x1U;  /* initial_info's bit31 is ecc flag */
    param->col_bits = 10;                         /* col bits is 10 for allsize */
    param->width = (initial_info >> 28) & 0x1U;   /* initial_info's bit28 is width flag */
    ddr_size = (initial_info >> 26) & 0x3U; /* initial_info's bit26 27 is size */

    dram_init_param_fix(param, &ddr_size);

    switch (ddr_size) {
        /* row bits range from 14 to 17, depends on totalsize and width */
        case DDR_SIZE_1G:
            /* 1G size, width:16-->row_bits:16, width:8-->row_bits:15 */
            param->row_bits = (param->width == DRAM_X16) ? 16 : 15;
            /* 1G size, width:16-->bank_bits:3, width:8-->bank_bits:4 */
            param->bank_bits = (param->width == DRAM_X16) ? 3 : 4;
            break;

        case DDR_SIZE_2G:
            /* 2G size, width:16-->row_bits:17, width:8-->row_bits:16 */
            param->row_bits = (param->width == DRAM_X16) ? 17 : 16;
            /* 2G size, width:16-->bank_bits:3, width:8-->bank_bits:4 */
            param->bank_bits = (param->width == DRAM_X16) ? 3 : 4;
            break;

        case DDR_SIZE_512M:
            /* 512M size, width:16-->row_bits:15, width:8-->row_bits:14 */
            param->row_bits = (param->width == DRAM_X16) ? 15 : 14;
            /* 512M size, width:16-->bank_bits:3, width:8-->bank_bits:4 */
            param->bank_bits = (param->width == DRAM_X16) ? 3 : 4;
            break;

        /* only 2Gb X8 dram supported with totalsize 4GB */
        case DDR_SIZE_4G:
            param->width = DRAM_X8;
            param->row_bits = 17; /* 4G size, row_bits:17 */
            param->bank_bits = 4; /* 4G size, bank_bits:4 */
            break;
        default:
            break;
    }
}

s32 dram_init_cfg(dram_param *param, s32 rate)
{
    s32 ret;

    /* step1 axi */
    dram_init_cfg_axi(param->ecc_on);

    /* step2 sec */
    dram_init_cfg_sec();

    /* step3 QOSBUF */
    dram_inif_cfg_qosbuf();

    /* step4 dmc */
    ret = dram_init_cfg_dmc1(param->row_bits, param->col_bits, param->bank_bits, rate);
    if (ret) {
        return ret;
    }

    /* step5 ddrphy */
    ret = dram_init_cfg_ddrphy(param->width, rate);
    if (ret) {
        return ret;
    }
    uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] cfg ddrphy success.\r\n");

    /* step6 dmc */
    ret = dram_init_cfg_dmc2(rate);
    if (ret) {
        return ret;
    }

    /* step7 clear int */
    dram_init_cfg_clrint();

    /* step8 sdram auto low power */
    dram_init_cfg_auto_low_power();

    uart_early_puts(CONFIG_SYS_UART_NUM, "[DDR] ddr init success.\r\n");

    dram_exmbist(param->row_bits, param->col_bits, param->bank_bits, param->width);

    /* clear ecc int and cnt */
    dram_init_cfg_clr_ecc();

    return 0;
}
#endif // CONFIG_HI3093_SPI_BOOT end

int dram_init_banksize(void)
{
    unsigned char *spin_table_addr = (unsigned char *)CPU_RELEASE_ADDR;
    unsigned long ddr_size;
    unsigned long ddr_rsv_size;
#ifdef CONFIG_HI3093_SPI_BOOT
    ddr_rsv_size = IBMC_SYS_RSV_SIZE;
    char buf[DDRC_PRINT_BUF_SIZE] = {0};
#else
    ddr_rsv_size = get_ddr_rsv_size();
#endif
    unsigned long ddr_ecc_size = 0;

    ddr_size = get_ddr_size();
    /* 计算ecc空间 */
    if (get_ddr_ecc_state()) {
        ddr_ecc_size = ddr_size >> 3; /* ecc开启, 右移3位 1/8空间是计算ecc空间 */
    }
    gd->bd->bi_dram[0].start = CONFIG_SYS_SDRAM_BASE;
    gd->bd->bi_dram[0].size = (ddr_size - ddr_rsv_size) - ddr_ecc_size;

#ifdef CONFIG_HI3093_SPI_BOOT
    memset(spin_table_addr, 0, CPU_RELEASE_ADDR_LEN);

    snprintf(buf, sizeof(buf), "[dram_init_banksize]ddr_size:0x%x, ddr_ecc_size:0x%x, "
                                "rsv_size:0x%x,sys_start:0x%x,sys_size:0x%x\r\n",
                                ddr_size, ddr_ecc_size, ddr_rsv_size, CONFIG_SYS_SDRAM_BASE, gd->bd->bi_dram[0].size);
    uart_early_puts(CONFIG_SYS_UART_NUM, buf);
#else
    (void)memset(spin_table_addr, 0, CPU_RELEASE_ADDR_LEN);
#endif

    return 0;
}

#ifdef CONFIG_HI3093_SPI_BOOT
int dram_init(void)
{
    s32 ret;
    dram_param param = { 0 };

    /* 已完成初始化直接返回，未完成初始化 需要复位DDRC */
    if (sys_init_flag_check(DDR_INIT_DONE)) {
        uart_early_puts(CONFIG_SYS_UART_NUM, "DDR has been init, don't need to init again!\r\n");
        return 0;
    } else if (sys_init_flag_check(DDR_INIT_DOING)) {
        uart_early_puts(CONFIG_SYS_UART_NUM, "DDR is initing\r\n");
        writel(0x1, HI309x_DDRC_REG_BASE + 0xa004);

        /* wait 20us */
        early_udelay(20);

        writel(0x1, HI309x_DDRC_REG_BASE + 0xa008);

        /* wait 20us */
        early_udelay(20);
    }

    uart_early_puts(CONFIG_SYS_UART_NUM, "Begin to init DDR!\r\n");

    /* 已开始DDR初始化 */
    sys_init_flag_set(DDR_INIT_MASK, DDR_INIT_DOING);

    dram_init_param_get(&param);

    ret = dram_init_cfg(&param, HI309x_DDRC_2400M);
    if (ret) {
        return ret;
    }

    /* 已完成DDR初始化 */
    sys_init_flag_set(DDR_INIT_MASK, DDR_INIT_DONE);
    writel(0x1, DDR_RSV_MEM_INIT_RESULT);

    return 0;
}
#else
int dram_init(void)
{
    unsigned long ddr_size;
    unsigned long ddr_rsv_size;
    unsigned long ddr_ecc_size = 0;

    ddr_rsv_size = get_ddr_rsv_size();

    ddr_size = get_ddr_size();

    /* 计算ecc空间 */
    if (get_ddr_ecc_state()) {
        ddr_ecc_size = ddr_size >> 3; /* ecc开启, 右移3位 1/8空间是计算ecc空间 */
    }

    gd->ram_size = (ddr_size - ddr_rsv_size) - ddr_ecc_size;

    return 0;
}
#endif
