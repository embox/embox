/**
 * @file
 * @brief i.MX6 Enhanced Configurable SPI driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 12.06.2017
 */

#include <drivers/common/memory.h>
#include <drivers/spi.h>
#include <embox/unit.h>
#include <framework/mod/options.h>

#include "imx6_ecspi.h"

EMBOX_UNIT_INIT(imx6_ecspi1_init);

#define BASE_ADDR OPTION_GET(NUMBER, base_addr)

static struct imx6_ecspi imx6_ecspi1 = {
	.base_addr = BASE_ADDR,
};

static int imx6_ecspi1_init(void) {
	imx6_ecspi_init(&imx6_ecspi1);
	return 0;
}

PERIPH_MEMORY_DEFINE(imx6_ecspi1, BASE_ADDR, 0x44);
SPI_DEV_DEF("ixm6 ecspi1", &imx6_ecspi_ops, &imx6_ecspi1, 0);
