/**
 * @file
 *
 * @date Oct 9, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_MMC_CORE_MMC_HOST_H_
#define SRC_DRIVERS_MMC_CORE_MMC_HOST_H_

#include <stdint.h>
#include <stdbool.h>

struct mmc_host;
struct mmc_card;
struct mmc_request;

struct mmc_ios {
	unsigned int clock; /* clock rate */
	unsigned short vdd;

/* vdd stores the bit number of the selected voltage range from below. */

	unsigned char bus_mode; /* command output mode */
};


struct mmc_host_ops {
	/*
	 * It is optional for the host to implement pre_req and post_req in
	 * order to support double buffering of requests (prepare one
	 * request while another request is active).
	 * pre_req() must always be followed by a post_req().
	 * To undo a call made to pre_req(), call post_req() with
	 * a nonzero err condition.
	 */
	void (*post_req)(struct mmc_host *host, struct mmc_request *req, int err);
	void (*pre_req)(struct mmc_host *host, struct mmc_request *req, bool is_first_req);
	void (*request)(struct mmc_host *host, struct mmc_request *req);

	/*
	 * Avoid calling the next three functions too often or in a "fast
	 * path", since underlaying controller might implement them in an
	 * expensive and/or slow way. Also note that these functions might
	 * sleep, so don't call them in the atomic contexts!
	 */

	/*
	 * Notes to the set_ios callback:
	 * ios->clock might be 0. For some controllers, setting 0Hz
	 * as any other frequency works. However, some controllers
	 * explicitly need to disable the clock. Otherwise e.g. voltage
	 * switching might fail because the SDCLK is not really quiet.
	 */
	void (*set_ios)(struct mmc_host *host, struct mmc_ios *ios);

	/*
	 * Return values for the get_ro callback should be:
	 *   0 for a read/write card
	 *   1 for a read-only card
	 *   -ENOSYS when not supported (equal to NULL callback)
	 *   or a negative errno value when something bad happened
	 */
	int (*get_ro)(struct mmc_host *host);

	/*
	 * Return values for the get_cd callback should be:
	 *   0 for a absent card
	 *   1 for a present card
	 *   -ENOSYS when not supported (equal to NULL callback)
	 *   or a negative errno value when something bad happened
	 */
	int (*get_cd)(struct mmc_host *host);

	void (*enable_sdio_irq)(struct mmc_host *host, int enable);

	/* optional callback for HC quirks */
	void (*init_card)(struct mmc_host *host, struct mmc_card *card);

	int (*start_signal_voltage_switch)(struct mmc_host *host, struct mmc_ios *ios);

	/* Check if the card is pulling dat[0:3] low */
	int (*card_busy)(struct mmc_host *host);

	/* The tuning command opcode value is different for SD and eMMC cards */
	int (*execute_tuning)(struct mmc_host *host, uint32_t opcode);

	/* Prepare HS400 target operating frequency depending host driver */
	int (*prepare_hs400_tuning)(struct mmc_host *host, struct mmc_ios *ios);
	/* Prepare enhanced strobe depending host driver */
	void (*hs400_enhanced_strobe)(struct mmc_host *host, struct mmc_ios *ios);
	int (*select_drive_strength)(struct mmc_card *card,
			unsigned int max_dtr, int host_drv,
			int card_drv, int *drv_type);
	void (*hw_reset)(struct mmc_host *host);
	void (*card_event)(struct mmc_host *host);

	/*
	 * Optional callback to support controllers with HW issues for multiple
	 * I/O. Returns the number of supported blocks for the request.
	 */
	int (*multi_io_quirk)(struct mmc_card *card,
			unsigned int direction, int blk_size);
};

struct mmc_host {
	const struct mmc_host_ops *ops;

	struct block_dev *bdev;

	int idx;

	uint16_t rca;

	int high_capacity;

	void *priv;
#if 1
	struct mmc_ios ios; /* current io bus settings */

	unsigned int f_min;
	unsigned int f_max;
	unsigned int f_init;
	uint32_t ocr_avail;
	uint32_t ocr_avail_sdio;	/* SDIO-specific OCR */
	uint32_t ocr_avail_sd;	/* SD-specific OCR */
	uint32_t ocr_avail_mmc;	/* MMC-specific OCR */

	/* host specific block data */
	unsigned int     max_seg_size;	/* see blk_queue_max_segment_size */
	unsigned short   max_segs;	/* see blk_queue_max_segments */
	unsigned short   unused;
	unsigned int     max_req_size;	/* maximum number of bytes in one req */
	unsigned int     max_blk_size;	/* maximum size of one mmc block */
	unsigned int     max_blk_count;	/* maximum number of blocks in one req */
	unsigned int     max_busy_timeout; /* max busy timeout in ms */

	unsigned long private[0] /*____cacheline_aligned*/;
#endif
};

/* Clean part */
extern struct mmc_host *mmc_alloc_host(void);
extern int mmc_dev_destroy(struct mmc_host *mmc);
extern int mmc_hw_reset(struct mmc_host *host);
extern int mmc_sw_reset(struct mmc_host *host);
extern int mmc_send_cmd(struct mmc_host *host, int cmd, int arg,
		int flags, uint32_t *resp);

static inline void *mmc_priv(struct mmc_host *mmc) {
	return mmc->priv;
}

extern int mmc_scan(struct mmc_host *host);

/* Version-specific functions */
extern int mmc_try_sdio(struct mmc_host *host);
extern int mmc_try_sd(struct mmc_host *host);
extern int mmc_try_mmc(struct mmc_host *host);

#endif /* SRC_DRIVERS_MMC_CORE_MMC_HOST_H_ */
