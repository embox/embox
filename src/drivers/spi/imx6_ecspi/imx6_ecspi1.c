/**
 * @file
 * @brief i.MX6 Enhanced Configurable SPI driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 12.06.2017
 */

#include <embox/unit.h>
#include <util/log.h>
#include <framework/mod/options.h>
#include <drivers/common/memory.h>
#include <drivers/spi.h>
#include <drivers/clk/ccm_imx6.h>
#include <drivers/iomuxc.h>

#include "imx6_ecspi.h"

EMBOX_UNIT_INIT(imx6_ecspi1_init);

#define BASE_ADDR OPTION_GET(NUMBER, base_addr)

static struct imx6_ecspi imx6_ecspi1 = {
	.base_addr = BASE_ADDR,
	.cs_count  = 4,
	.cs_array  = { {1, 30}, {2, 19}, {2, 24}, {2, 25} }
};

static void imx_ecspi1_pins_init(void) {
	/* TODO Make init like for escpi2.
	 * Currenly it is inited by uboot. */
}

static int imx6_ecspi1_init(void) {
	log_debug("");
	clk_enable("ecspi1");
	imx_ecspi1_pins_init();
	return imx6_ecspi_init(&imx6_ecspi1);
}

#define SPI_BUS_NAME      ixm6_ecspi1

PERIPH_MEMORY_DEFINE(imx6_ecspi1, BASE_ADDR, 0x44);

SPI_CONTROLLER_DEF(SPI_BUS_NAME, &imx6_ecspi_ops, &imx6_ecspi1, 0);
