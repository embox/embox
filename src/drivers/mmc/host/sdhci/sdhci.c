/**
 * @file
 * @brief base SDHCI driver for MMC card
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 01.05.2017
 */

#include <string.h>
#include <unistd.h>

#include <drivers/block_dev.h>
#include <drivers/common/memory.h>
#include <drivers/mmc/mmc_core.h>
#include <drivers/mmc/mmc_host.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <util/log.h>

#include <embox/unit.h>

#include "sdhci_ops.h"

#define BASE_ADDR  OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM    OPTION_GET(NUMBER, irq_num)
#define MAX_HC_NUM OPTION_GET(NUMBER, hc_quantity)

static struct sdhci_host sdhci_hosts[MAX_HC_NUM];

#define NEXT_SDHCI_OFFT 0x4000

PERIPH_MEMORY_DEFINE(imx6_usdhc, BASE_ADDR, NEXT_SDHCI_OFFT * MAX_HC_NUM);

EMBOX_UNIT_INIT(sdhci_init);
static int sdhci_init(void) {
	int i;

	for (i = 0; i < MAX_HC_NUM; i++) {
		sdhci_hosts[i].ioaddr = BASE_ADDR + i * NEXT_SDHCI_OFFT;
		sdhci_hw_probe(&sdhci_hosts[i]);
	}

	return 0;
}
