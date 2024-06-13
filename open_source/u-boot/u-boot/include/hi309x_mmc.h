#ifndef __MMC_H__
#define __MMC_H__

#define POWER_ON         1
#define POWER_OFF        0

#define MMC_RETRY        3

/*LP SUBYS CSR*/
#define LP_SUBSYS_CSR_BASE          0x0871D000
#define LPSC_CLKEN_CTRL0            (LP_SUBSYS_CSR_BASE + 0x10)
#define LPSC_CLKEN_CTRL1            (LP_SUBSYS_CSR_BASE + 0x14)
#define LPSC_CRG_DIV_CTRL           (LP_SUBSYS_CSR_BASE + 0x18)
#define LPSC_EMMC_FUNC_CTRL_ADDR    (LP_SUBSYS_CSR_BASE + 0xF0)
#define LPSC_EMMC_CRG_CTRL_ADDR     (LP_SUBSYS_CSR_BASE + 0xF8)

extern int emmc_init(void);
extern int mmc_non_dma_change_area(unsigned char index, unsigned int retry);
extern int mmc_non_dma_read(unsigned char *buf,unsigned int start,unsigned int num, unsigned int retry);
extern int mmc_non_dma_write(unsigned char *buf,unsigned int start,unsigned int num, unsigned int retry);

#endif