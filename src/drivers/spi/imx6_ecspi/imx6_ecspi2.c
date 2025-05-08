/**
 * @file
 * @brief i.MX6 Enhanced Configurable SPI driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 12.06.2017
 */

#include <embox/unit.h>
#include <framework/mod/options.h>
#include <util/log.h>
#include <drivers/common/memory.h>
#include <drivers/spi.h>
#include <drivers/clk/ccm_imx6.h>
#include <drivers/iomuxc.h>

#include "imx6_ecspi.h"

EMBOX_UNIT_INIT(imx6_ecspi2_init);

#define BASE_ADDR    OPTION_GET(NUMBER, base_addr)
#define PINS_GROUP   OPTION_GET(NUMBER, pins_group)

static struct imx6_ecspi imx6_ecspi2 = {
	.base_addr = BASE_ADDR,
	.cs_count  = 1,
	.cs_array  = {
		{4, 29}
	},
	.cs_iomux  = {
		IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11,
	}
};

static void imx_ecspi2_pins_init(void) {
	int gpio_n, port;
	int i;
	/* SS0 (CS0) */
	/* FIXME SPI NSS doesn't work somewhy, so we use gpio instead of:
	 * iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11, 0x12);
	 * iomuxc_write(IOMUXC_ECSPI2_SS0_SELECT_INPUT, 0x2);
	 */

	/* Init all Chip Selects */
	for (i = 0; i < imx6_ecspi2.cs_count; i++) {
		/* Force GPIO mode. */
		iomuxc_write(imx6_ecspi2.cs_iomux[i], 0x15);

		gpio_n = imx6_ecspi2.cs_array[i][0];
		port   = imx6_ecspi2.cs_array[i][1];
		gpio_setup_mode(gpio_n, 1 << port, GPIO_MODE_OUT);
		gpio_set(gpio_n, 1 << port, 1);
	}

	/* Init selected GPIO group. */
#if PINS_GROUP == 1
	/* MISO */
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA10, 0x12);
	iomuxc_write(IOMUXC_ECSPI2_MISO_SELECT_INPUT, 0x2);
	/* MOSI */
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA09, 0x12);
	iomuxc_write(IOMUXC_ECSPI2_MOSI_SELECT_INPUT, 0x2);
	/* CLK */
	iomuxc_write(IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA08, 0x12);
	iomuxc_write(IOMUXC_ECSPI2_CSPI_CLK_IN_SELECT_INPUT, 0x2);
#else
	#error "Wrong pins group for SPI2"
#endif
}

static int imx6_ecspi2_init(void) {
	log_debug("");
	clk_enable("ecspi2");
	imx_ecspi2_pins_init();
	return imx6_ecspi_init(&imx6_ecspi2);
}

#define SPI_BUS_NAME      ixm6_ecspi2

PERIPH_MEMORY_DEFINE(imx6_ecspi2, BASE_ADDR, 0x44);

SPI_CONTROLLER_DEF(SPI_BUS_NAME, &imx6_ecspi_ops, &imx6_ecspi2, 1);
