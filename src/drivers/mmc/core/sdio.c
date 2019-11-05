/**
 * @file sdio.c
 * @brief SDIO-specific stuff
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 05.11.2019
 */

#include <drivers/mmc/mmc_host.h>
#include <util/log.h>

int mmc_try_sdio(struct mmc_host *host) {
	log_debug("NIY");
	return -1;
}
