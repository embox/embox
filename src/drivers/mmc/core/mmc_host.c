/**
 * @file mmc_simple.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 18.10.2019
 */
#include <errno.h>
#include <limits.h>

#include <drivers/block_dev.h>
#include <drivers/mmc/mmc.h>
#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>
#include <mem/misc/pool.h>
#include <util/indexator.h>
#include <util/log.h>

#define MMC_DEV_QUANTITY OPTION_GET(NUMBER, dev_quantity)
#define MMC_DEFAULT_NAME "mmc#" /* Sharp symbol is used for enumeration */

POOL_DEF(mmc_dev_pool, struct mmc_host, MMC_DEV_QUANTITY);
INDEX_DEF(mmc_idx, 0, MMC_DEV_QUANTITY);

static int mmc_block_ioctl(struct block_dev *bdev, int cmd, void *args, size_t size) {
	log_debug("NIY");
	return 0;
}

static int mmc_block_read(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno) {
	struct mmc_host *mmc;
	uint32_t arg;
	struct mmc_request req;

	log_debug("count %d blkno %d", count, blkno);

	assert(bdev);
	assert(buffer);

	mmc = bdev->privdata;
	assert(mmc);

	/* TODO may be different for high capacity storage */
	arg = blkno * bdev->block_size;

	memset(&req, 0, sizeof(req));

	req.cmd.opcode = 17;
	req.cmd.arg = arg;
	req.cmd.flags = MMC_RSP_R1B | MMC_DATA_READ;

	req.data.addr = (uintptr_t) buffer;
	req.data.blksz = bdev->block_size;
	req.data.blocks = count;

	assert(mmc->ops);
	assert(mmc->ops->request);
	mmc->ops->request(mmc, &req);

	return bdev->block_size;
}

static int mmc_block_write(struct block_dev *bdev, char *buffer, size_t count, blkno_t blkno) {
	struct mmc_host *mmc;
	uint32_t arg;
	struct mmc_request req;

	log_debug("count %d blkno %d", count, blkno);

	assert(bdev);
	assert(buffer);

	mmc = bdev->privdata;
	assert(mmc);

	/* TODO may be different for high capacity storage */
	arg = blkno * bdev->block_size;

	memset(&req, 0, sizeof(req));

	req.cmd.opcode = 24;
	req.cmd.arg = arg;
	req.cmd.flags = MMC_RSP_R1B | MMC_DATA_WRITE;

	req.data.addr = (uintptr_t) buffer;
	req.data.blksz = bdev->block_size;
	req.data.blocks = count;

	assert(mmc->ops);
	assert(mmc->ops->request);
	mmc->ops->request(mmc, &req);

	return bdev->block_size;
}

static int mmc_block_probe(void *args) {
	log_debug("NIY");
	return 0;
}

static const struct block_dev_driver mmc_block_driver = {
	.name = "MMC driver",
	.ioctl = mmc_block_ioctl,
	.read = mmc_block_read,
	.write = mmc_block_write,
	.probe = mmc_block_probe,
};

struct mmc_host *mmc_alloc_host(void) {
	struct mmc_host *mmc;
	char buf[PATH_MAX];

	mmc = pool_alloc(&mmc_dev_pool);
	if (mmc == NULL) {
		log_error("Failed to alloc mmc_host from pool");
		errno = ENOMEM;
		return NULL;
	}

	strncpy(buf, MMC_DEFAULT_NAME, sizeof(buf) - 1);
	mmc->idx = block_dev_named(buf, &mmc_idx);
	if (mmc->idx < 0) {
		log_error("Failed to alloc index for mmc_host");
		errno = -mmc->idx;
		goto free_mmc;
	}

	mmc->bdev = block_dev_create(buf, (void *) &mmc_block_driver, mmc);
	if (mmc->bdev == NULL) {
		log_error("Failed to create block_device for mmc_host");
		goto free_idx;
	}

	return mmc;

free_idx:
	assert(mmc);
	assert(mmc->idx > 0);
	index_free(&mmc_idx, mmc->idx);
free_mmc:
	assert(mmc);
	pool_free(&mmc_dev_pool, mmc);

	return NULL;
}

int mmc_dev_destroy(struct mmc_host *mmc) {
	pool_free(&mmc_dev_pool, mmc);
	return 0;
}

/* Send simple command without data */
int mmc_send_cmd(struct mmc_host *host, int cmd, int arg, int flags, uint32_t *resp) {
	struct mmc_request req;

	assert(host);
	assert(host->ops);
	assert(host->ops->request);

	memset(&req, 0, sizeof(req));

	req.cmd.opcode = cmd;
	req.cmd.arg = arg;
	req.cmd.flags = flags;

	host->ops->request(host, &req);

	if (resp) {
		memcpy(resp, req.cmd.resp, sizeof(uint32_t) * 4);
	}

	return 0;
}

int mmc_sw_reset(struct mmc_host *host) {
	log_debug("NIY");
	return 0;
}

static void mmc_go_idle(struct mmc_host *host) {
	/* CMD0 sets card in an inactive state */
	mmc_send_cmd(host, 0, 0, 0, NULL);
}

int mmc_scan(struct mmc_host *host) {
	assert(host);

	mmc_go_idle(host);

	/* The order is important! */
	if (!mmc_try_sdio(host)) {
		log_debug("SDIO detected");
		return 0;
	}

	if (!mmc_try_sd(host)) {
		log_debug("SD detected");
		return 0;
	}

	if (!mmc_try_mmc(host)) {
		log_debug("MMC detected");
		return 0;
	}

	log_debug("Failed to detect any memory card");
	return -1;
}
