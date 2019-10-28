/**
 * @file
 *
 * @date Oct 9, 2019
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_MMC_CORE_MMC_CARD_H_
#define SRC_DRIVERS_MMC_CORE_MMC_CARD_H_


struct mmc_host;

/*
 * MMC device
 */
struct mmc_card {
	struct mmc_host *host; /* the host this device belongs to */
};

#endif /* SRC_DRIVERS_MMC_CORE_MMC_CARD_H_ */
