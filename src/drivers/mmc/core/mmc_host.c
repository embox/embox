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

static int mmc_send_cmd(struct mmc_host *host, int cmd, int arg, int flags, uint32_t *resp);

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
static int mmc_send_cmd(struct mmc_host *host, int cmd, int arg, int flags, uint32_t *resp) {
	struct mmc_request req;

	assert(resp);
	assert(host);
	assert(host->ops);
	assert(host->ops->request);

	memset(&req, 0, sizeof(req));

	req.cmd.opcode = cmd;
	req.cmd.arg = arg;
	req.cmd.flags = flags;

	host->ops->request(host, &req);

	memcpy(resp, req.cmd.resp, sizeof(uint32_t) * 4);

	return 0;
}

int mmc_sw_reset(struct mmc_host *host) {
	uint32_t resp[4];
	uint64_t size;
	int man_year, man_mon;
	static const char mon_name[12][4] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};

	/* CMD0 sets card in an inactive state */
	mmc_send_cmd(host, 0, 0, 0, resp);

	mmc_send_cmd(host, 55, 0, MMC_RSP_R1, resp);

#define VOLTAGE_WINDOW_MMC	0x00FF8080 /* Taken from u-boot */
#define VOLTAGE_WINDOW_SD	0x80010000

	mmc_send_cmd(host, 41, VOLTAGE_WINDOW_MMC & 0xff8000, MMC_RSP_R3, resp);

	mmc_send_cmd(host, 1, 0, 0, resp);

	/* Send CID (get manifacture ID etc.) */
	mmc_send_cmd(host, 2, 0, MMC_RSP_R2, resp);

	log_debug("MMC CID: %08x %08x %08x %08x",
			resp[0], resp[1], resp[2], resp[3]);

	log_debug("MMC info (parsed CID):");
	log_debug("Manufacturer ID =0x%2x", resp[0] >> 24);
	log_debug("OEM/OID         =0x%2x%2x", (resp[0] >> 16) & 0xFF, (resp[0] >> 8) & 0xFF);
	log_debug("Product name    =%c%c%c%c%c",
			(char) (resp[0] & 0xFF),
			(char) ((resp[1] >> 24) & 0xFF),
			(char) ((resp[1] >> 16) & 0xFF),
			(char) ((resp[1] >> 8) & 0xFF),
			(char) (resp[1] & 0xFF));
	log_debug("Revision        =0x%02x", (resp[2] >> 24) & 0xFF);
	log_debug("Serial number   =0x%2x%2x%2x%2x",
			(resp[2] >> 16) & 0xFF,
			(resp[2] >> 8) & 0xFF,
			resp[2] & 0xFF,
			(resp[3] >> 24) & 0xFF);

	man_year = 2000 + 10 * ((resp[3] >> 16) & 0xFF) +
			(((resp[3] >> 8) & 0xFF) >> 4);
	man_mon = (resp[3] >> 8) & 0xF;

	assert(man_mon < 12);

	log_debug("Date             %s %d", mon_name[man_mon], man_year);

	/* Avoid warnings if log_level = 0 */
	(void) man_year, (void) man_mon, (void) mon_name;

	/* CMD3 sets stand-by mode */
	mmc_send_cmd(host, 3, 0, MMC_RSP_R6, resp);

	host->rca = resp[0] >> 16;

	log_debug("MMC RCA: %04x", host->rca);

	/* Send CSD (get device size and so on) */
	/* TODO setup RCA for MMC cards? */
	mmc_send_cmd(host, 9, host->rca << 16, MMC_RSP_R2, resp);
	log_debug("MMC CSD: %08x %08x %08x %08x",
			resp[0], resp[1], resp[2], resp[3]);

	if (!(resp[0] & 0x40000000)) {
		host->high_capacity = 1;
		size = ((resp[1] >> 8) & 0x3) << 10;
		size |= (resp[1] & 0xFF) << 2;
		//size |= (resp[2] & 0x3) << 10;

		size = 256 * 1024 * (size + 1);
		log_debug("Size = %lld bytes (High-Capacity SD)", size);
	} else {
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

	mmc_send_cmd(host, 7, host->rca << 16, 0, resp);

	return 0;
}
