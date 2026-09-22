/**
 * @file
 * @brief SD Host Controller 3.00 register map for the BCM2711 Arasan block
 *
 * Embox's embox.driver.mmc.host.sdhci is NOT this: it is an i.MX6 uSDHC
 * driver wearing the name. Do not mix the two headers.
 *
 * THE 32-BIT RULE. The Arasan controller drops 8- and 16-bit accesses, so
 * every narrow field is read-modify-written inside its containing word.
 * Two words must never be read back for that RMW:
 *
 *   0x04  BLOCK_SIZE | BLOCK_COUNT
 *   0x0C  TRANSFER_MODE | COMMAND
 *
 * Compose those in software and store the full word once -- writing COMMAND
 * is what starts the transaction. Linux does the same with shadow registers
 * in sdhci-iproc.c.
 *
 * The interrupt status words (0x30, 0x32) are write-1-to-clear: never RMW
 * them either, store an explicit mask instead.
 *
 * QEMU's generic-sdhci accepts narrow accesses and will not catch any of
 * that. Real silicon will.
 */

#ifndef BCM2711_SDHCI_STD_H_
#define BCM2711_SDHCI_STD_H_

#include <stdint.h>

#include <hal/mem_barriers.h>
#include <hal/reg.h>

/* ------------------------------------------------------------ registers */

#define SDHCI_SDMA_ADDRESS    0x00
#define SDHCI_BLOCK_SIZE      0x04 /* 16 bit */
#define SDHCI_BLOCK_COUNT     0x06 /* 16 bit */
#define SDHCI_ARGUMENT        0x08
#define SDHCI_TRANSFER_MODE   0x0C /* 16 bit */
#define SDHCI_COMMAND         0x0E /* 16 bit */
#define SDHCI_RESPONSE        0x10 /* 4 x 32 bit */
#define SDHCI_BUFFER          0x20
#define SDHCI_PRESENT_STATE   0x24
#define SDHCI_HOST_CONTROL    0x28 /* 8 bit */
#define SDHCI_POWER_CONTROL   0x29 /* 8 bit */
#define SDHCI_CLOCK_CONTROL   0x2C /* 16 bit */
#define SDHCI_TIMEOUT_CONTROL 0x2E /* 8 bit */
#define SDHCI_SOFTWARE_RESET  0x2F /* 8 bit */
#define SDHCI_INT_STATUS      0x30 /* normal 16 | error 16, write-1-to-clear */
#define SDHCI_INT_ENABLE      0x34
#define SDHCI_SIGNAL_ENABLE   0x38
#define SDHCI_HOST_CONTROL2   0x3E /* 16 bit */
#define SDHCI_CAPABILITIES    0x40
#define SDHCI_CAPABILITIES_1  0x44
#define SDHCI_HOST_VERSION    0xFE /* 16 bit */

/* PRESENT_STATE */
#define SDHCI_CMD_INHIBIT       (1u << 0)
#define SDHCI_DATA_INHIBIT      (1u << 1)
#define SDHCI_DOING_WRITE       (1u << 8)
#define SDHCI_DOING_READ        (1u << 9)
#define SDHCI_SPACE_AVAILABLE   (1u << 10)
#define SDHCI_DATA_AVAILABLE    (1u << 11)
#define SDHCI_CARD_PRESENT      (1u << 16)
#define SDHCI_CARD_STATE_STABLE (1u << 17)
#define SDHCI_CARD_DETECT_LEVEL (1u << 18)
#define SDHCI_WRITE_PROTECT     (1u << 19)

/* HOST_CONTROL */
#define SDHCI_CTRL_4BITBUS (1u << 1)
#define SDHCI_CTRL_HISPD   (1u << 2)

/* POWER_CONTROL */
#define SDHCI_POWER_ON  (1u << 0)
#define SDHCI_POWER_180 (5u << 1)
#define SDHCI_POWER_300 (6u << 1)
#define SDHCI_POWER_330 (7u << 1)

/* CLOCK_CONTROL. v3.00 10-bit divided clock: SDCLK = base / (2 * N),
 * N == 0 meaning base itself. N[7:0] goes to bits 15:8, N[9:8] to 7:6. */
#define SDHCI_CLOCK_INT_EN     (1u << 0)
#define SDHCI_CLOCK_INT_STABLE (1u << 1)
#define SDHCI_CLOCK_CARD_EN    (1u << 2)
#define SDHCI_DIV_MASK         0x3ff
#define SDHCI_MAKE_DIV(n)      ((((n) & 0xff) << 8) | ((((n) >> 8) & 0x3) << 6))

/* SOFTWARE_RESET */
#define SDHCI_RESET_ALL  (1u << 0)
#define SDHCI_RESET_CMD  (1u << 1)
#define SDHCI_RESET_DATA (1u << 2)

/* INT_STATUS, low half (normal) */
#define SDHCI_INT_RESPONSE      (1u << 0)
#define SDHCI_INT_DATA_END      (1u << 1)
#define SDHCI_INT_DMA_END       (1u << 3)
#define SDHCI_INT_SPACE_AVAIL   (1u << 4)
#define SDHCI_INT_DATA_AVAIL    (1u << 5)
#define SDHCI_INT_CARD_INSERT   (1u << 6)
#define SDHCI_INT_CARD_REMOVE   (1u << 7)
#define SDHCI_INT_ERROR         (1u << 15)
/* INT_STATUS, high half (error), shifted into the 32-bit word */
#define SDHCI_ERR_CMD_TIMEOUT   (1u << 16)
#define SDHCI_ERR_CMD_CRC       (1u << 17)
#define SDHCI_ERR_CMD_END_BIT   (1u << 18)
#define SDHCI_ERR_CMD_INDEX     (1u << 19)
#define SDHCI_ERR_DATA_TIMEOUT  (1u << 20)
#define SDHCI_ERR_DATA_CRC      (1u << 21)
#define SDHCI_ERR_DATA_END_BIT  (1u << 22)
#define SDHCI_ERR_CURRENT_LIMIT (1u << 23)

/* TRANSFER_MODE */
#define SDHCI_TRNS_DMA        (1u << 0)
#define SDHCI_TRNS_BLK_CNT_EN (1u << 1)
#define SDHCI_TRNS_AUTO_CMD12 (1u << 2)
#define SDHCI_TRNS_READ       (1u << 4)
#define SDHCI_TRNS_MULTI      (1u << 5)

/* COMMAND, as the low 6 bits of the 16-bit register hold the flags */
#define SDHCI_CMD_RESP_NONE        0x0
#define SDHCI_CMD_RESP_LONG        0x1
#define SDHCI_CMD_RESP_SHORT       0x2
#define SDHCI_CMD_RESP_SHORT_BUSY  0x3
#define SDHCI_CMD_CRC              (1u << 3)
#define SDHCI_CMD_INDEX            (1u << 4)
#define SDHCI_CMD_DATA             (1u << 5)
#define SDHCI_MAKE_CMD(idx, flags) ((((idx) & 0x3f) << 8) | (flags))

/* CAPABILITIES */
#define SDHCI_CAP_BASE_CLOCK(caps) (((caps) >> 8) & 0xff) /* MHz, 0 = unknown */
#define SDHCI_CAP_TIMEOUT_CLOCK(caps) ((caps) & 0x3f)
#define SDHCI_CAP_SDMA                (1u << 22)
#define SDHCI_CAP_HISPD               (1u << 21)
#define SDHCI_CAP_VDD_330             (1u << 24)

#define SDHCI_MAX_TIMEOUT_CONTROL 0x0e

#define SDHCI_MAKE_BLKSZ(boundary, blksz) \
	((((boundary) & 0x7) << 12) | ((blksz) & 0xfff))

/* ------------------------------------------------------------ accessors */

static inline uint32_t sdhci_rd32(uintptr_t base, int reg) {
	uint32_t v;

	v = REG32_LOAD(base + reg);
	dmb(sy);
	return v;
}

static inline void sdhci_wr32(uintptr_t base, int reg, uint32_t val) {
	dmb(sy);
	REG32_STORE(base + reg, val);
}

static inline uint16_t sdhci_rd16(uintptr_t base, int reg) {
	return (uint16_t)(sdhci_rd32(base, reg & ~3) >> ((reg & 3) * 8));
}

static inline uint8_t sdhci_rd8(uintptr_t base, int reg) {
	return (uint8_t)(sdhci_rd32(base, reg & ~3) >> ((reg & 3) * 8));
}

/* RMW. Legal for 0x28/0x29 and 0x2C/0x2E/0x2F; never for 0x04, 0x0C or the
 * write-1-to-clear status words. */
static inline void sdhci_wr16(uintptr_t base, int reg, uint16_t val) {
	int shift = (reg & 3) * 8;
	uint32_t word = sdhci_rd32(base, reg & ~3);

	word = (word & ~(0xffffu << shift)) | ((uint32_t)val << shift);
	sdhci_wr32(base, reg & ~3, word);
}

static inline void sdhci_wr8(uintptr_t base, int reg, uint8_t val) {
	int shift = (reg & 3) * 8;
	uint32_t word = sdhci_rd32(base, reg & ~3);

	word = (word & ~(0xffu << shift)) | ((uint32_t)val << shift);
	sdhci_wr32(base, reg & ~3, word);
}

#endif /* BCM2711_SDHCI_STD_H_ */
