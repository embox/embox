/**
 * @file
 *
 * @date Oct 21, 2019
 * @date author Anton Bondarev
 */

#ifndef SRC_DRIVERS_MMC_HOST_SDHCI_SDHCI_H_
#define SRC_DRIVERS_MMC_HOST_SDHCI_SDHCI_H_

#include <stdint.h>

#include <hal/reg.h>

#define USDHC_DS_ADDR               (0x00)
#define USDHC_BLK_ATT               (0x04)
#define USDHC_CMD_ARG               (0x08)
#define USDHC_CMD_XFR_TYP           (0x0C)
#define USDHC_CMD_RSP0              (0x10)
#define USDHC_CMD_RSP1              (0x14)
#define USDHC_CMD_RSP2              (0x18)
#define USDHC_CMD_RSP3              (0x1C)
#define USDHC_DATA_BUFF_ACC_PORT    (0x20)
#define USDHC_PRES_STATE            (0x24)
# define USDHC_PRES_STATE_CIHB      (1 << 0) /* CMD line is active */
# define USDHC_PRES_STATE_CDIHB     (1 << 1) /* DAT line is active */
# define USDHC_PRES_STATE_CDPL      (1 << 18) /* Card detection pin level */
#define USDHC_PROT_CTRL             (0x28)
# define USDHC_PROT_CTRL_LCTL       (1 << 0) /* Led control */
# define USDHC_PROT_CTRL_DTW_1BIT   (0 << 1) /* Data transfer width */
# define USDHC_PROT_CTRL_DTW_4BIT   (1 << 1)
# define USDHC_PROT_CTRL_DTW_8BIT   (2 << 1)
# define USDHC_PROT_CTRL_DTW_MASK   (3 << 1)
#define USDHC_SYS_CTRL              (0x2C)
# define USDHC_SYS_CTRL_RSTA        (1 << 24)
# define USDHC_SYS_CTRL_RSTC        (1 << 25)
# define USDHC_SYS_CTRL_RSTD        (1 << 26)
# define USDHC_SYS_CTRL_INITA       (1 << 27)
#define USDHC_INT_STATUS            (0x30)
# define USDHC_INT_STATUS_CC        (1 << 0) /* Command complete */
#define USDHC_INT_STATUS_EN         (0x34)
#define USDHC_INT_SIGNAL_EN         (0x38)
#define USDHC_AUTOCMD12_ERR_STATUS  (0x3C)
#define USDHC_HOST_CTRL_CAP         (0x40)
#define USDHC_WTMK_LVL              (0x44)
#define USDHC_MIX_CTRL              (0x48)
#define USDHC_FORCE_EVENT           (0x50)
#define USDHC_ADMA_ERR_STATUS       (0x54)
#define USDHC_ADMA_SYS_ADDR         (0x58)
#define USDHC_DLL_CTRL              (0x60)
#define USDHC_DLL_STATUS            (0x64)
#define USDHC_CLK_TUNE_CTRL_STATUS  (0x68)
#define USDHC_VEND_SPEC             (0xC0)
#define USDHC_MMC_BOOT              (0xC4)
#define USDHC_VEND_SPEC2            (0xC8)


#define RSP_136         (1 << 16)
#define RSP_PRESENT     (2 << 16)
#define RSP_BUSY        (3 << 16)
#define RSP_CRC         (1 << 19)
#define RSP_OPCODE      (1 << 20)

#define RSP_NONE        0
#define RSP_R1          (RSP_PRESENT | RSP_CRC | RSP_OPCODE)
#define RSP_R1b         (RSP_CRC | RSP_OPCODE|RSP_BUSY)
#define RSP_R2          (RSP_136 | RSP_CRC)
#define RSP_R3          (RSP_PRESENT)
#define RSP_R4          (RSP_PRESENT)
#define RSP_R5          (RSP_PRESENT | RSP_CRC | RSP_OPCODE)
#define RSP_R6          (RSP_PRESENT | RSP_CRC | RSP_OPCODE)
#define RSP_R7          (RSP_PRESENT | RSP_CRC | RSP_OPCODE)

#define WCMD_DPSEL      (1 << 21)
#define MIX_CARD2HOST	(1 << 4)
#define MIX_DMAEN       (1 << 0)

struct sdhci_host {
	uintptr_t ioaddr;
};

static inline uint32_t sdhci_readl(struct sdhci_host *host, int reg) {
	return REG32_LOAD(host->ioaddr + reg);
}

static inline void sdhci_writel(struct sdhci_host *host, int reg, uint32_t val) {
	REG32_STORE(host->ioaddr + reg, val);
}

static inline void sdhci_orl(struct sdhci_host *host, int reg, uint32_t val) {
	REG32_ORIN(host->ioaddr + reg, val);
}

static inline uint8_t sdhci_readb(struct sdhci_host *host, int reg) {
	return REG8_LOAD(host->ioaddr + reg);
}

#endif /* SRC_DRIVERS_MMC_HOST_SDHCI_SDHCI_H_ */
