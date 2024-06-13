// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * flash info
 *
 * Copyright (c) HiSilicon Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef SPI_IDSH
#define SPI_IDSH

#define _1K      0x400
#define _2K      0x800
#define _4K      0x1000
#define _8K      0x2000
#define _16K     0x4000
#define _32K     0x8000
#define _64K     0x10000
#define _128K    0x20000
#define _256K    0x40000
#define _512K    0x80000
#define _1M      0x100000
#define _2M      0x200000
#define _4M      0x400000
#define _8M      0x800000
#define _16M     0x1000000
#define _32M     0x2000000
#define _64M     0x4000000
#define _128M    0x08000000
#define _256M    0x10000000
#define _512M    0x20000000
#define INFINITE 0xFFFFFFFF

#define SPI_IF_READ_STD       0x01
#define SPI_IF_READ_FAST      0x02
#define SPI_IF_READ_DUAL      0x04
#define SPI_IF_READ_DUAL_ADDR 0x08
#define SPI_IF_READ_QUAD      0x10
#define SPI_IF_READ_QUAD_ADDR 0x20

#define SPI_IF_WRITE_STD       0x01
#define SPI_IF_WRITE_DUAL      0x02
#define SPI_IF_WRITE_DUAL_ADDR 0x04
#define SPI_IF_WRITE_QUAD      0x08
#define SPI_IF_WRITE_QUAD_ADDR 0x10

#define SPI_IF_ERASE_SECTOR_4K       0x01
#define SPI_IF_ERASE_SECTOR_32K      0x02
#define SPI_IF_ERASE_CHIP            0x02 /* chip erase */
#define SPI_IF_ERASE_SECTOR_64K      0x04
#define SPI_IF_ERASE_SECTOR          0x04 /* sector erase, 64K */
#define SPI_IF_ERASE_SECTOR_256K     0x08
#define SPI_IF_ERASE_SECTOR_S25_256K 0x04 /* S25FS512,S25FL512 */

#define SPI_CMD_WREN 0x06 /* Write Enable */

#define SPI_CMD_SE_4K   0x20 /* 4KB sector Erase */
#define SPI_CMD_SE_32K  0x52 /* 32KB sector Erase */
#define SPI_CMD_SE_64K  0xD8 /* 64KB sector Erase */
#define SPI_CMD_SE_256K 0xD8 /* 256KB sector Erase */
#define SPI_CMD_SE      0xD8 /* 64KB Sector Erase */
#define SPI_CMD_BE      0xC7 /* chip erase */

#define SPI_CMD_RDSR 0x05 /* Read Status Register */
#define SPI_CMD_RDID 0x9F /* Read Identification */

#define SPI_CMD_PP                        0x02 /* Page Programming */
#define SPI_CMD_WRITE_DUAL                0xA2 /* fast program dual input */
#define SPI_CMD_WRITE_QUAD                0x32 /* fast program quad input */
#define SPI_CMD_WRITE_DUAL_ADDR           0xD2 /* Dual I/O High Performance Write */
#define SPI_CMD_WRITE_QUAD_ADDR           0x12 /* Quad I/O High Performance Write */
#define SPI_CMD_WRITE_QUAD_ADDR_144       0x38 /* Quad I/O High Performance Write */
#define SPI_CMD_WRITE_QUAD_ADDR_144_4BYTE 0x3E /* Quad I/O High Performance Write only for 4byte */

#define SPI_CMD_READ           0x03 /* Read Data bytes */
#define SPI_CMD_FAST_READ      0x0B /* Read Data Bytes at Higher Speed */
#define SPI_CMD_READ_DUAL      0x3B /* fast read dual output */
#define SPI_CMD_READ_QUAD      0x6B /* fast read quad output */
#define SPI_CMD_READ_DUAL_ADDR 0xBB /* Dual I/O High Performance Read */
#define SPI_CMD_READ_QUAD_ADDR 0xEB /* Quad I/O High Performance Read */

#define SPI_CMD_SR_WIP 1 /* Write in Progress */
#define SPI_CMD_SR_WEL 2 /* Write Enable Latch */

#define SPI_CMD_EN4B      0xB7 /* enter to 4 bytes mode and set 4 byte bit as '1' */
#define SPI_CMD_EX4B      0xE9 /* exit 4 bytes mode and clear 4 byte bit as '0' */
#define SPI_ISSI_CMD_EX4B 0x29

/* Used for Micron flashes only. */
#define SPINOR_OP_RD_EVCR 0x65 /* Read EVCR register */
#define SPINOR_OP_WD_EVCR 0x61 /* Write EVCR register */
/* Enhanced Volatile Configuration Register bits */
#define EVCR_QUAD_EN_MICRON 0x80 /* Micron Quad I/O */
#define SR_QUAD_EN_MX       0x40 /* Macronix Quad I/O */

/* for test add start */
#define SPI_CMD_MT25_WRSR 0x01 /* write status reg */

/* start spansion or cypress (s70fs01gs) */
#define SPI_CMD_RDAR 0x65 /* read any register  the dummy cycle of s70fs01gs is cycle */
#define SPI_CMD_WRAR 0x71 /* write any register */

#define SPANSION_S70FS01GS_FIRST_HALF_ADDR  (0)
#define SPANSION_S70FS01GS_SECOND_HALF_ADDR (0x4000000)
#define S70FS01GS_BOUND_ADDR                (SPANSION_S70FS01GS_SECOND_HALF_ADDR)
#define SPANSION_FS512S_FIRST_HALF_ADDR     (SPANSION_S70FS01GS_FIRST_HALF_ADDR)

#define SPANSION_REG_ADDR_MASK                                 (1UL << 26)
#define spansion_reg_addr_calc(flash_reg_addr, flash_mem_addr) \
    ((flash_reg_addr) | ((flash_mem_addr) & SPANSION_REG_ADDR_MASK))

#define spansion_sr1v_reg_addr(flash_mem_addr)                 spansion_reg_addr_calc(0x00800000, flash_mem_addr)
#define spansion_sr2v_reg_addr(flash_mem_addr)                 spansion_reg_addr_calc(0x00800001, flash_mem_addr)
#define spansion_cr1v_reg_addr(flash_mem_addr)                 spansion_reg_addr_calc(0x00800002, flash_mem_addr)
#define spansion_cr2v_reg_addr(flash_mem_addr)                 spansion_reg_addr_calc(0x00800003, flash_mem_addr)
#define spansion_cr3v_reg_addr(flash_mem_addr)                 spansion_reg_addr_calc(0x00800004, flash_mem_addr)
#define spansion_cr4v_reg_addr(flash_mem_addr)                 spansion_reg_addr_calc(0x00800005, flash_mem_addr)

#define spansion_cr2nv_reg_addr(flash_mem_addr) spansion_reg_addr_calc(0x00000003, flash_mem_addr)

/* Length of the ID used by the default driver of the spansion. */
#define SPANSION_SPEC_ID_LEN 2

#ifndef BIT
#define BIT(nr) (1UL << (nr))
#endif
#define SPI_FLASH_MT25QU_BP_MASK 0x5c   /* block protection mask */
#define MT_SR_BP0                BIT(2) /* Block protect 0 */
#define MT_SR_BP1                BIT(3) /* Block protect 1 */
#define MT_SR_BP2                BIT(4) /* Block protect 2 */
#define MT_SR_BP3                BIT(6) /* Block protect 3 */
#define MT_SR_TB                 BIT(5) /* top/bottom */

/* for test add end */
struct spi_operation {
    unsigned char iftype;
    unsigned char cmd;
    unsigned char dummy;
    unsigned int size;
    unsigned int clock;
};

/* Definition of the length of the device ID */
#define DEVICE_ID_LEN  8
#define BUFFER_MAX_LEN 20

struct spi_info {
    char *name;

    unsigned char id[DEVICE_ID_LEN];
    unsigned char id_len;

    unsigned long chipsize;
    unsigned int erasesize;
    unsigned char addrcycle;

#define MAX_SPI_OP       (8)
#define MAX_SPI_ERASE_OP (4)

    struct spi_operation *read[MAX_SPI_OP];
    struct spi_operation *write[MAX_SPI_OP];
    struct spi_operation *erase[MAX_SPI_ERASE_OP];
    struct spi_driver *driver;
};

struct spi_info *sfc_spi_serach_ids(struct spi_info *spi_info_table, const unsigned char ids[DEVICE_ID_LEN]);
void sfc_spi_search_rw(struct spi_info *spiinfo, struct spi_operation *spiop_rw, unsigned int iftype,
                       unsigned int max_dummy, int is_read);
void sfc_spi_get_erase(const struct spi_info *spiinfo, struct spi_operation *spiop_erase);

#endif
