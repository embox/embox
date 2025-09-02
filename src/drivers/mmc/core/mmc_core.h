/**
 * @file
 *
 * @date Oct 9, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_MMC_CORE_MMC_CORE_H_
#define SRC_DRIVERS_MMC_CORE_MMC_CORE_H_

#include <stdint.h>

struct mmc_command {
	uint32_t opcode;
	uint32_t arg;

	uint32_t resp[4];

	int flags;
#define MMC_RSP_PRESENT (1 << 0)
#define MMC_RSP_136     (1 << 1)
#define MMC_RSP_CRC     (1 << 2)
#define MMC_RSP_BUSY    (1 << 3)
#define MMC_RSP_OPCODE  (1 << 4)
#define MMC_RSP_MASK    (0x1F)

#define MMC_CMD_MASK    (3 << 5) /* non-SPI command type */
#define MMC_CMD_AC      (0 << 5)
#define MMC_CMD_ADTC    (1 << 5)
#define MMC_CMD_BC      (2 << 5)
#define MMC_CMD_BCR     (3 << 5)

#define MMC_RSP_SPI_S1   (1 << 7)  /* one status byte */
#define MMC_RSP_SPI_S2   (1 << 8)  /* second byte */
#define MMC_RSP_SPI_B4   (1 << 9)  /* four data bytes */
#define MMC_RSP_SPI_BUSY (1 << 10) /* card may send busy */

/*
 * These are the native response types, and correspond to valid bit
 * patterns of the above flags.  One additional valid pattern
 * is all zeros, which means we don't expect a response.
 */
#define MMC_RSP_NONE    (0)
#define MMC_RSP_R1      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1B     (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE|MMC_RSP_BUSY)
#define MMC_RSP_R2      (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
#define MMC_RSP_R3      (MMC_RSP_PRESENT)
#define MMC_RSP_R4      (MMC_RSP_PRESENT)
#define MMC_RSP_R5      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R6      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R7      (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_DATA_WRITE  (1 << 8)
#define MMC_DATA_READ   (1 << 9)
#define MMC_DATA_XFER   (MMC_DATA_READ | MMC_DATA_WRITE)

	int error;
};

struct mmc_data {
	uintptr_t addr;

	unsigned int blksz;  /* data block size */
	unsigned int blocks; /* number of blocks */

	unsigned int flags;

	unsigned int bytes_xfered;
};

struct mmc_request {
	struct mmc_command sbc; /* SET_BLOCK_COUNT for multiblock */
	struct mmc_command cmd;
	struct mmc_data data;
};

extern void mmc_dump_cid(uint32_t *cid);

#endif /* SRC_DRIVERS_MMC_CORE_MMC_CORE_H_ */
