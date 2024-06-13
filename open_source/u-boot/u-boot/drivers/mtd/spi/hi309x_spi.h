/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 */
/* start spansion or cypress (s70fs01gs) */
#ifndef _HI309X_SPI_H_
#define _HI309X_SPI_H_
#define FS01GS_FLASH_TYPE (1)
#define FS512S_FLASH_TYPE (2)

#define _4K (0x1000)

#define SPEC_FLASH_CNT (2)

#define SPI_CMD_RDAR 0x65 /* read any register  对于s70fs01gs的dummy cycle=8 */
#define SPI_CMD_WRAR 0x71 /* write any register */
#define SPI_CMD_EN4B 0xB7 /* enter to 4 bytes mode and set 4 byte bit as '1' */
#define SPI_CMD_EX4B 0xE9 /* exit 4 bytes mode and clear 4 byte bit as '0' */

#define SPANSION_S70FS01GS_FIRST_HALF_ADDR (0)
#define SPANSION_S70FS01GS_SECOND_HALF_ADDR (0x4000000)
#define S70FS01GS_BOUND_ADDR (SPANSION_S70FS01GS_SECOND_HALF_ADDR)

#define SPANSION_REG_ADDR_MASK (1ul << 26)
#define SPANSION_REG_ADDR_CALC(flash_reg_addr, flash_mem_addr) \
    ((flash_reg_addr) | ((flash_mem_addr)&SPANSION_REG_ADDR_MASK))
#define SPANSION_SR1V_REG_ADDR(flash_mem_addr) SPANSION_REG_ADDR_CALC(0x00800000, flash_mem_addr)
#define SPANSION_SR2V_REG_ADDR(flash_mem_addr) SPANSION_REG_ADDR_CALC(0x00800001, flash_mem_addr)
#define SPANSION_CR1V_REG_ADDR(flash_mem_addr) SPANSION_REG_ADDR_CALC(0x00800002, flash_mem_addr)
#define SPANSION_CR2V_REG_ADDR(flash_mem_addr) SPANSION_REG_ADDR_CALC(0x00800003, flash_mem_addr)
#define SPANSION_CR3V_REG_ADDR(flash_mem_addr) SPANSION_REG_ADDR_CALC(0x00800004, flash_mem_addr)
#define SPANSION_CR4V_REG_ADDR(flash_mem_addr) SPANSION_REG_ADDR_CALC(0x00800005, flash_mem_addr)

#define SECTOR_ARCH_MASK (0x8)
#define SECTOR_POS_MASK (0x4)

#define HYBRID_SECTOR_ARCH (0ul << 3)  /* 4KB erase enable */
#define UNIFORM_SECTOR_ARCH (1ul << 3) /* 4KB erase disable */

#define SECTOR_4KB_AT_TOP (1ul << 2)    /* high address */
#define SECTOR_4KB_AT_BOTTOM (0ul << 2) /* low address */

/* end spansion or cypress (s70fs01gs) */

/* Common status */
#define STATUS_WIP BIT(0)
#define STATUS_QEB_WINSPAN BIT(1)
#define STATUS_QEB_MXIC BIT(6)
#define STATUS_PEC BIT(7)
#define SR_BP0 BIT(2) /* Block protect 0 */
#define SR_BP1 BIT(3) /* Block protect 1 */
#define SR_BP2 BIT(4) /* Block protect 2 */

/* Flash timeout values */
#define SPI_FLASH_PROG_TIMEOUT (2 * CONFIG_SYS_HZ)
#define SPI_FLASH_PAGE_ERASE_TIMEOUT (5 * CONFIG_SYS_HZ)
#define SPI_FLASH_SECTOR_ERASE_TIMEOUT (10 * CONFIG_SYS_HZ)

/* Erase commands */
#define CMD_ERASE_4K 0x20
#define CMD_ERASE_CHIP 0xc7
#define CMD_ERASE_64K 0xd8

/* Write commands */
#define CMD_WRITE_STATUS 0x01
#define CMD_PAGE_PROGRAM 0x02
#define CMD_WRITE_DISABLE 0x04
#define CMD_WRITE_ENABLE 0x06
#define CMD_QUAD_PAGE_PROGRAM 0x32

/* Read commands */
#define CMD_READ_ARRAY_SLOW 0x03
#define CMD_READ_ARRAY_FAST 0x0b
#define CMD_READ_DUAL_OUTPUT_FAST 0x3b
#define CMD_READ_DUAL_IO_FAST 0xbb
#define CMD_READ_QUAD_OUTPUT_FAST 0x6b
#define CMD_READ_QUAD_IO_FAST 0xeb
#define CMD_READ_ID 0x9f
#define CMD_READ_STATUS 0x05
#define CMD_READ_STATUS1 0x35
#define CMD_READ_CONFIG 0x35
#define CMD_FLAG_STATUS 0x70

/* Bank addr access commands */
#ifdef CONFIG_SPI_FLASH_BAR
#define CMD_BANKADDR_BRWR 0x17
#define CMD_BANKADDR_BRRD 0x16
#define CMD_EXTNADDR_WREAR 0xC5
#define CMD_EXTNADDR_RDEAR 0xC8
#endif

int spi_flash_cmd(struct spi_slave *spi, u8 cmd, void *response, size_t len);

/* Enable writing on the SPI flash */
static inline int spi_flash_cmd_write_enable(struct spi_flash *flash)
{
    return spi_flash_cmd(flash->spi, CMD_WRITE_ENABLE, NULL, 0);
}

/* Disable writing on the SPI flash */
static inline int spi_flash_cmd_write_disable(struct spi_flash *flash)
{
    return spi_flash_cmd(flash->spi, CMD_WRITE_DISABLE, NULL, 0);
}

static inline int spi_flash_cmd_enter_4b(struct spi_flash *flash)
{
    return spi_flash_cmd(flash->spi, flash->spi->spec_para.cmd_enter4b, NULL, 0);
}

static inline int spi_flash_cmd_exit_4b(struct spi_flash *flash)
{
    return spi_flash_cmd(flash->spi, flash->spi->spec_para.cmd_exit4b, NULL, 0);
}

/*
 * Used for spi_flash write operation
 * - SPI claim
 * - spi_flash_cmd_write_enable
 * - spi_flash_cmd_write
 * - spi_flash_wait_till_ready
 * - SPI release
 */
int spi_flash_write_common(struct spi_flash *flash, const u8 *cmd, size_t cmd_len, const void *buf, size_t buf_len);

/*
 * Flash write operation, support all possible write commands.
 * Write the requested data out breaking it up into multiple write
 * commands as needed per the write size.
 */
int spi_flash_cmd_write_ops(struct spi_flash *flash, u32 offset, size_t len, const void *buf);

/*
 * Same as spi_flash_cmd_read() except it also claims/releases the SPI
 * bus. Used as common part of the ->read() operation.
 */
int spi_flash_read_common(struct spi_flash *flash, const u8 *cmd, size_t cmd_len, void *data, size_t data_len);

/* Flash read operation, support all possible read commands */
int spi_flash_cmd_read_ops(struct spi_flash *flash, u32 offset, size_t len, void *data);

extern void exec_cmd_resume_bank0(struct spi_flash *flash);

extern int spi_flash_cmd_prep(struct spi_flash *flash);

extern int spi_flash_cmd_release(struct spi_flash *flash);

#endif /* _HI309X_SPI_H_ */
