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
#include <drivers/block_dev/partition.h>
#include <drivers/mmc/mmc.h>
#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>
#include <mem/misc/pool.h>
#include <lib/libds/indexator.h>
#include <util/log.h>

#define MMC_DEV_QUANTITY OPTION_GET(NUMBER, dev_quantity)
#define MMC_DEFAULT_NAME "mmc#" /* Sharp symbol is used for enumeration */

#define MMC_BUS_WIDTH    OPTION_GET(NUMBER, bus_width)

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

	mmc = block_dev_priv(bdev);
	assert(mmc);

	if (mmc->high_capacity) {
		arg = blkno;
	} else {
		arg = blkno * bdev->block_size;
	}

	memset(&req, 0, sizeof(req));

	req.cmd.opcode = 17;
	req.cmd.arg = arg;
	req.cmd.flags = MMC_RSP_R1B | MMC_DATA_READ;

	req.data.addr = (uintptr_t) buffer;
	req.data.blksz = bdev->block_size;
	req.data.blocks = count / bdev->block_size;

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

	mmc = block_dev_priv(bdev);
	assert(mmc);

	if (mmc->high_capacity) {
		arg = blkno;
	} else {
		arg = blkno * bdev->block_size;
	}

	memset(&req, 0, sizeof(req));

	req.cmd.opcode = 24;
	req.cmd.arg = arg;
	req.cmd.flags = MMC_RSP_R1B | MMC_DATA_WRITE;

	req.data.addr = (uintptr_t) buffer;
	req.data.blksz = bdev->block_size;
	req.data.blocks = count / bdev->block_size;

	assert(mmc->ops);
	assert(mmc->ops->request);
	mmc->ops->request(mmc, &req);

	return bdev->block_size;
}

static int mmc_block_probe(struct block_dev *bdev, void *args) {
	log_debug("NIY");
	return 0;
}

static const struct block_dev_ops mmc_block_driver = {
	.bdo_ioctl = mmc_block_ioctl,
	.bdo_read = mmc_block_read,
	.bdo_write = mmc_block_write,
	.bdo_probe = mmc_block_probe,
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
	if (mmc->bdev) {
		block_dev_destroy(mmc->bdev);
	}
	index_free(&mmc_idx, mmc->idx);
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

void mmc_set_bus_width(struct mmc_host *host, int width) {
	uint32_t resp[4];

	mmc_send_cmd(host, 55, 0x10000, MMC_RSP_R1, resp);
	mmc_send_cmd(host, 6, width, MMC_RSP_R1, resp);
}

struct mmc_try_desc {
	int (*mmc_try_func)(struct mmc_host *host);
	char *type_name;
};

	/* The order is important! */
static const struct mmc_try_desc mmc_try_repo[3] = {
	{ mmc_try_sdio, "SDIO"},
	{ mmc_try_sd, "SD"},
	{ mmc_try_mmc, "MMC"},
};

int mmc_scan(struct mmc_host *host) {
	int res;
	int i;

	assert(host);

	for (i = 0; i < 3; i ++) {
		mmc_go_idle(host);
		res = mmc_try_repo[i].mmc_try_func(host);
		if (res == 0) {
			log_debug("%s detected", mmc_try_repo[i].type_name);
			break;
		}
	}
	if (res == 0) {
		if (MMC_BUS_WIDTH) {
			mmc_set_bus_width(host, MMC_BUS_WIDTH);
		}
		create_partitions(host->bdev);
	} else {
		log_debug("Failed to detect any memory card");
	}

	return res;
}

void mmc_dump_cid(uint32_t *cid) {
	int man_year, man_mon;
	static const char mon_name[12][4] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};

	assert(cid);

	log_debug("MMC CID: %08x %08x %08x %08x",
			cid[0], cid[1], cid[2], cid[3]);

	log_debug("MMC info (parsed CID):");
	log_debug("Manufacturer ID =0x%2x", cid[0] >> 24);
	log_debug("OEM/OID         =0x%2x%2x", (cid[0] >> 16) & 0xFF, (cid[0] >> 8) & 0xFF);
	log_debug("Product name    =%c%c%c%c%c",
			(char) (cid[0] & 0xFF),
			(char) ((cid[1] >> 24) & 0xFF),
			(char) ((cid[1] >> 16) & 0xFF),
			(char) ((cid[1] >> 8) & 0xFF),
			(char) (cid[1] & 0xFF));
	log_debug("Revision        =0x%02x", (cid[2] >> 24) & 0xFF);
	log_debug("Serial number   =0x%2x%2x%2x%2x",
			(cid[2] >> 16) & 0xFF,
			(cid[2] >> 8) & 0xFF,
			cid[2] & 0xFF,
			(cid[3] >> 24) & 0xFF);

	man_year = 2000 + 10 * ((cid[3] >> 16) & 0xFF) +
			(((cid[3] >> 8) & 0xFF) >> 4);
	man_mon = (cid[3] >> 8) & 0xF;

	assert(man_mon < 12);

	log_debug("Date             %s %d", mon_name[man_mon], man_year);

	/* Avoid warnings if log_level is low */
	(void) man_year, (void) man_mon, (void) mon_name;
}
