/**
 * @file stm32_spi3.c
 * @brief it's compatible but not the same as stm32_spi1.c, stm32_spi2.c and stm32_spi5.c
 * @author Andrew Bursian
 * @version
 * @date 14.06.2023
 */

#include <string.h>
#include <drivers/gpio/gpio.h>
#include <drivers/spi.h>

#include <embox/unit.h>

#include "stm32_spi.h"

#include <config/board_config.h>

#include <framework/mod/options.h>

EMBOX_UNIT_INIT(stm32_spi3_init);

static struct stm32_spi stm32_spi3 = {
#if defined(CONF_SPI3_PIN_CS_PORT)
	.nss_port = CONF_SPI3_PIN_CS_PORT,
	.nss_pin  = CONF_SPI3_PIN_CS_NR,
#endif
	.bits_per_word = CONF_SPI3_BITS_PER_WORD,
	.clk_div       = CONF_SPI3_CLK_DIV,
};

static int stm32_spi3_init(void) {
	CONF_SPI3_CLK_ENABLE_SPI();

	stm32_spi_init(&stm32_spi3, SPI3);

#if	OPTION_GET(NUMBER, pullup) == 1
	gpio_setup_mode(CONF_SPI3_PIN_SCK_PORT, CONF_SPI3_PIN_SCK_NR,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(CONF_SPI3_PIN_SCK_AF) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
	gpio_setup_mode(CONF_SPI3_PIN_MISO_PORT, CONF_SPI3_PIN_MISO_NR,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(CONF_SPI3_PIN_MISO_AF) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
	gpio_setup_mode(CONF_SPI3_PIN_MOSI_PORT, CONF_SPI3_PIN_MOSI_NR,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(CONF_SPI3_PIN_MOSI_AF) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
#else
	gpio_setup_mode(CONF_SPI3_PIN_SCK_PORT, CONF_SPI3_PIN_SCK_NR,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(CONF_SPI3_PIN_SCK_AF) |
		GPIO_MODE_OUT_PUSH_PULL);
	gpio_setup_mode(CONF_SPI3_PIN_MISO_PORT, CONF_SPI3_PIN_MISO_NR,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(CONF_SPI3_PIN_MISO_AF) |
		GPIO_MODE_OUT_PUSH_PULL);
	gpio_setup_mode(CONF_SPI3_PIN_MOSI_PORT, CONF_SPI3_PIN_MOSI_NR,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(CONF_SPI3_PIN_MOSI_AF) |
		GPIO_MODE_OUT_PUSH_PULL);
#endif

#if defined(CONF_SPI3_PIN_CS_PORT)
#if	CONF_SPI3_PIN_CS_AF == -1
	/* NOAF in board configuration file defined as -1 */
	gpio_setup_mode(CONF_SPI3_PIN_CS_PORT, CONF_SPI3_PIN_CS_NR,
		GPIO_MODE_OUT | GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

	/* Chip Select is in inactive state by default. */
	gpio_set(CONF_SPI3_PIN_CS_PORT, CONF_SPI3_PIN_CS_NR, GPIO_PIN_HIGH);
#else
	gpio_setup_mode(CONF_SPI3_PIN_CS_PORT, CONF_SPI3_PIN_CS_NR,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(CONF_SPI3_PIN_CS_AF) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
#endif // CONF_SPI3_PIN_CS_AF == -1
#endif // defined(CONF_SPI3_PIN_CS_PORT)

	return 0;
}

#define SPI_DEV_NAME      stm32_spi_3

SPI_DEV_DEF(SPI_DEV_NAME, &stm32_spi_ops, &stm32_spi3, 3);
