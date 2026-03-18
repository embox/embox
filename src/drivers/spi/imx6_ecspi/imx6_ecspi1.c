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
#include <drivers/gpio.h>
#include <drivers/clk/ccm_imx6.h>
#include <drivers/iomuxc.h>

#include "imx6_ecspi.h"

EMBOX_UNIT_INIT(imx6_ecspi1_init);

#define BASE_ADDR OPTION_GET(NUMBER, base_addr)

/* cs_count controlled via Mybuild option (default 1 for sabrelite CS0).
 * Set cs_count=4 to restore legacy 4-CS behaviour. */
#define ECSPI1_CS_COUNT  OPTION_GET(NUMBER, cs_count)

static struct imx6_ecspi imx6_ecspi1 = {
	.base_addr     = BASE_ADDR,
	.cs_count      = ECSPI1_CS_COUNT,
	.cs_array      = { {2, 19}, {1, 30}, {2, 24}, {2, 25} },
	.use_configreg = OPTION_GET(NUMBER, use_configreg),
};

static void imx_ecspi1_pins_init(void) {
	int i, gpio_n, port;
	/* Configure all CS pins as GPIO output, pre-driven HIGH (deasserted).
	 * imx6_ecspi_set_cs() drives them LOW to assert and HIGH to deassert.
	 * Without this setup the GPIO is in input mode and gpio_set() is a no-op. */
	for (i = 0; i < (int)imx6_ecspi1.cs_count; i++) {
		gpio_n = imx6_ecspi1.cs_array[i][0];
		port   = imx6_ecspi1.cs_array[i][1];
		gpio_setup_mode(gpio_n, 1 << port, GPIO_MODE_OUT);
		gpio_set(gpio_n, 1 << port, 1); /* deasserted = high */
	}
}

static int imx6_ecspi1_init(void) {
	log_debug("");
	clk_enable("ecspi1");
	imx_ecspi1_pins_init();
	return imx6_ecspi_init(&imx6_ecspi1);
}

#define SPI_BUS_NAME      ixm6_ecspi1

PERIPH_MEMORY_DEFINE(imx6_ecspi1, BASE_ADDR, 0x44);

SPI_CONTROLLER_DEF(SPI_BUS_NAME, &imx6_ecspic_ops, &imx6_ecspi1, 1);
