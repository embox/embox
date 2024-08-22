/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 25.12.2019
 */

#include <string.h>
#include <drivers/gpio/gpio.h>
#include <drivers/spi.h>

#include "stm32_spi.h"

#include <config/board_config.h>

static int stm32_spi5_init(void);
static struct stm32_spi stm32_spi5 = {
	.hw_init = stm32_spi5_init,
#if defined(CONF_SPI5_PIN_CS_PORT)
	.nss_port = CONF_SPI5_PIN_CS_PORT,
	.nss_pin  = CONF_SPI5_PIN_CS_NR,
#endif
	.bits_per_word = CONF_SPI5_BITS_PER_WORD,
	.clk_div       = CONF_SPI5_CLK_DIV,
};

static int stm32_spi5_init(void) {
	CONF_SPI5_CLK_ENABLE_SPI();

#if defined CONF_SPI5_REGION_BASE
	stm32_spi_init(&stm32_spi5, (void *) CONF_SPI5_REGION_BASE);
#else
	stm32_spi_init(&stm32_spi5, SPI5);
#endif	/* CONF_SPI5_REGION_BASE */

	gpio_setup_mode(CONF_SPI5_PIN_SCK_PORT, CONF_SPI5_PIN_SCK_NR,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(CONF_SPI5_PIN_SCK_AF) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

	gpio_setup_mode(CONF_SPI5_PIN_MISO_PORT, CONF_SPI5_PIN_MISO_NR,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(CONF_SPI5_PIN_MISO_AF) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

	gpio_setup_mode(CONF_SPI5_PIN_MOSI_PORT, CONF_SPI5_PIN_MOSI_NR,
		GPIO_MODE_OUT_ALTERNATE | GPIO_ALTERNATE(CONF_SPI5_PIN_MOSI_AF) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

#if defined(CONF_SPI5_PIN_CS_PORT)
	/* Chip Select is usual GPIO pin. */
	gpio_setup_mode(CONF_SPI5_PIN_CS_PORT, CONF_SPI5_PIN_CS_NR,
		GPIO_MODE_OUT | GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
		
	/* Chip Select is in inactive state by default. */
	gpio_set(CONF_SPI5_PIN_CS_PORT, CONF_SPI5_PIN_CS_NR, GPIO_PIN_HIGH);
#endif

	return 0;
}

#define SPI_DEV_NAME      stm32_spi_5

SPI_DEV_DEF(SPI_DEV_NAME, &stm32_spi_ops, &stm32_spi5, 5);
