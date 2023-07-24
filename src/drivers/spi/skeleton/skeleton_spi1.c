/**
 * @file skeleton_spi.c
 * @brief It is template, change "skeleton" to "your-device"
 * @author Andrew Bursian
 * @version
 * @date 20.06.2023
 */

/* If there is only one SPI port in your system, you can merge this file with 
skeleton_spi_common.c and use simple "your-dev_spi" instead of "your-dev_spi1" */

#include <string.h>
#include <drivers/spi.h>

#include <embox/unit.h>

#include "skeleton_spi.h"

/* You may define some board specific confuguration (set 
of CONF_SPI1_... defines), for example - #include <config/board_config.h> */

/* This define have to be replaced by #include <your-system-defines.h>
It is used to pick out particular spi port among others, so in case 
of single SPI used you can exclude both SPI1_BASE_ADDR constant and 
.instance structure field of struct skeleton_spi */
#define SPI1_BASE_ADDR	NULL

EMBOX_UNIT_INIT(skeleton_spi1_init);

static struct skeleton_spi skeleton_spi1 = {
	.instance = SPI1_BASE_ADDR,
//#if defined(CONF_SPI1_PIN_CS_PORT)
//	.nss_port = CONF_SPI1_PIN_CS_PORT,
//	.nss_pin  = CONF_SPI1_PIN_CS_NR,
//#endif
//	.bits_per_word = CONF_SPI1_BITS_PER_WORD,
//	.clk_div       = CONF_SPI1_CLK_DIV,
};

static int skeleton_spi1_init(void) {
	skeleton_spi_init(&skeleton_spi1);

//	gpio_setup_mode(CONF_SPI1_PIN_SCK_PORT,  CONF_SPI1_PIN_SCK_NR,  CONF_SPI1_PIN_SCK_AF);
//	gpio_setup_mode(CONF_SPI1_PIN_MISO_PORT, CONF_SPI1_PIN_MISO_NR, CONF_SPI1_PIN_MISO_AF);
//	gpio_setup_mode(CONF_SPI1_PIN_MOSI_PORT, CONF_SPI1_PIN_MOSI_NR, CONF_SPI1_PIN_MOSI_AF);
//#if defined(CONF_SPI1_PIN_CS_PORT)
//	/* Chip Select is usual GPIO pin. */
//	gpio_setup_mode(CONF_SPI1_PIN_CS_PORT, CONF_SPI1_PIN_CS_NR, GPIO_MODE_OUT);
//	/* Chip Select is in inactive state by default. */
//	gpio_set(CONF_SPI1_PIN_CS_PORT, CONF_SPI1_PIN_CS_NR, GPIO_PIN_HIGH);
//#endif

	return 0;
}

#define SPI_DEV_NAME      skeleton_spi_1

SPI_DEV_DEF(SPI_DEV_NAME, &skeleton_spi_ops, &skeleton_spi1, 1);
