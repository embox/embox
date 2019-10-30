/**
 * @file imx_usdhc.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 30.10.2019
 */

#include <drivers/clk/ccm_imx6.h>
#include <drivers/common/memory.h>

#include <embox/unit.h>

#include "sdhci.h"
#include "sdhci_ops.h"

#define BASE_ADDR  OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM    OPTION_GET(NUMBER, irq_num)
#define MAX_HC_NUM OPTION_GET(NUMBER, hc_quantity)

static struct sdhci_host sdhci_hosts[MAX_HC_NUM];

PERIPH_MEMORY_DEFINE(imx_usdhc, BASE_ADDR, 0x4000 * MAX_HC_NUM);

EMBOX_UNIT_INIT(imx_usdhci_init);
static int imx_usdhci_init(void) {
	int i;

	clk_enable("usdhc1");
	clk_enable("usdhc2");
	clk_enable("usdhc3");
	clk_enable("usdhc4");

	for (i = 0; i < MAX_HC_NUM; i++) {
		sdhci_hosts[i].ioaddr = BASE_ADDR + i * 0x4000;
		sdhci_hw_probe(&sdhci_hosts[i]);
	}

	return 0;
}
