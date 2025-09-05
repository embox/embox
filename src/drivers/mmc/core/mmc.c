/**
 * @file mmc.c
 * @brief MMC-specific stuff (i.e. non-SD, non-SDIO cards)
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 05.11.2019
 */
#include <assert.h>

#include <drivers/block_dev.h>
#include <drivers/mmc/mmc.h>
#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>
#include <util/log.h>

int mmc_try_mmc(struct mmc_host *host) {
	uint32_t resp[4];
	uint64_t size;
	mmc_send_cmd(host, MMC_APP_CMD, 0, MMC_RSP_R1, resp);

	mmc_send_cmd(host, MMC_SEND_OP_COND, VOLTAGE_WINDOW_MMC & 0xff8000,
	    MMC_RSP_R3, resp);

	mmc_send_cmd(host, MMC_SEND_OP_COND, 0, 0, resp);
	/* Send CID (get manifacture ID etc.) */
	mmc_send_cmd(host, MMC_ALL_SEND_CID, 0, MMC_RSP_R2, resp);

	mmc_dump_cid(resp);

	/* CMD3 sets stand-by mode */
	mmc_send_cmd(host, MMC_SET_RELATIVE_ADDR, 0, MMC_RSP_R1, resp);

	host->rca = resp[0] >> 16;

	log_debug("MMC RCA: %04x", host->rca);

	/* Send CSD (get device size and so on) */
	/* TODO setup RCA for MMC cards? */
	mmc_send_cmd(host, MMC_SEND_CSD, host->rca << 16, MMC_RSP_R2, resp);
	log_debug("MMC CSD: %08x %08x %08x %08x", resp[0], resp[1], resp[2], resp[3]);

	if ((resp[0] | resp[1] | resp[2] | resp[3]) == 0) {
		return -1;
	}

	if (!(resp[0] & 0x40000000)) {
		host->high_capacity = 1;
		size = ((resp[1] >> 8) & 0x3) << 10;
		size |= (resp[1] & 0xFF) << 2;

		size = 256 * 1024 * (size + 1);
		log_debug("Size = %lld bytes (High-Capacity SD)", size);
	}
	else {
		host->high_capacity = 0;
		size = (resp[1] & 0xFF) << 16;
		size |= ((resp[2] >> 24) & 0xFF) << 8;
		size |= (resp[2] >> 16) & 0xFF;
		size = 512 * 1024 * (size + 1);
		log_debug("Size = %lld bytes (Standart Capacity SD)", size);
	}

	assert(host->bdev);
	host->bdev->size = size;
	host->bdev->block_size = 512;

	mmc_send_cmd(host, MMC_SELECT_CARD, host->rca << 16, 0, resp);

	return 0;
}
