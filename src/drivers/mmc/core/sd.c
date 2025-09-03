/**
 * @file sd.c
 * @brief SD-specific stuff
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 05.11.2019
 */

 #include <util/log.h>

#include <unistd.h>
#include <assert.h>

#include <drivers/block_dev.h>
#include <drivers/mmc/mmc.h>
#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>


#define OCR_HCS           (1u << 30) /* Flag for high-capacity storage.
				     * We assume that HC supports it */
#define OCR_READY         (1u << 31) /* When set to 1, SD card powerup is finished */
#define SD_VOLTAGE_WINDOW 0xFF8000
#define CMD55_VALID_RESP  0x120

int mmc_try_sd(struct mmc_host *host) {
	uint32_t resp[4];
	uint64_t size;
	int retry = 5;

	mmc_send_cmd(host, 8, 0x1AA, MMC_RSP_R7 , resp);

	do {
		usleep(10 * USEC_PER_MSEC);

		if (retry-- < 0) {
			log_debug("Failed to detect SD card");
			return -1;
		}

		mmc_send_cmd(host, 55, 0, MMC_RSP_R1, resp);
		if (resp[0] != CMD55_VALID_RESP) {
			continue;
		}

		mmc_send_cmd(host, 41, OCR_HCS | SD_VOLTAGE_WINDOW, MMC_RSP_R3, resp);
	} while (!(resp[0] & OCR_READY));

	if ((resp[0] & SD_VOLTAGE_WINDOW) == 0) {
		log_error("SD card voltage window mismatch!");
		return -1;
	}

	/* Send CID (get manifacture ID etc.) */
	mmc_send_cmd(host, 2, 0, MMC_RSP_R2, resp);
	mmc_dump_cid(resp);

	/* CMD3 sets stand-by mode */
	mmc_send_cmd(host, 3, 0, MMC_RSP_R6, resp);

	host->rca = resp[0] >> 16;

	log_debug("MMC RCA: %04x", host->rca);

	/* Send CSD (get device size and so on) */
	mmc_send_cmd(host, 9, host->rca << 16, MMC_RSP_R2, resp);
	log_debug("MMC CSD: %08x %08x %08x %08x",
			resp[0], resp[1], resp[2], resp[3]);

	if (resp[0] & 0x40000000) {
		host->high_capacity = 1;
		size = (resp[1] & 0xFF) << 16;
		size |= ((resp[2] >> 24) & 0xFF) << 8;
		size |= (resp[2] >> 16) & 0xFF;
		size = 512 * 1024 * (size + 1);
		log_debug("Size = %lld bytes (High-Capacity SD)", size);
	} else {
		host->high_capacity = 0;
		size = ((resp[1] >> 8) & 0x3) << 10;
		size |= (resp[1] & 0xFF) << 2;

		size = 256 * 1024 * (size + 1);
		log_debug("Size = %lld bytes (Standart Capacity SD)", size);
	}

	assert(host->bdev);
	host->bdev->size = size;
	host->bdev->block_size = 512;

	mmc_send_cmd(host, 7, host->rca << 16, MMC_RSP_R1B, resp);

	return 0;
}
