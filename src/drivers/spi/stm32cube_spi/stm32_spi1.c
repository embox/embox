/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 25.12.2019
 */

#include <string.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>

#if defined(STM32F407xx)
#include <framework/mod/options.h>
#include <config/platform/stm32/f4/stm32f4_discovery/bsp.h>
#endif

#include "stm32_spi.h"

#include <config/board_config.h>

static int stm32_spi1_init(void);
static struct stm32_spi stm32_spi1 = {
	.hw_init = stm32_spi1_init,
#if defined(CONF_SPI1_PIN_CS_PORT)
	.nss_port = CONF_SPI1_PIN_CS_PORT,
	.nss_pin  = CONF_SPI1_PIN_CS_NR,
#endif
	.bits_per_word = CONF_SPI1_BITS_PER_WORD,
	.clk_div       = CONF_SPI1_CLK_DIV,
};

static int stm32_spi1_init(void) {
#if defined(STM32F407xx) && !OPTION_MODULE_GET(platform__stm32__f4__stm32f4_discovery__bsp,BOOLEAN,errata_spi_wrong_last_bit)
	#error errata_spi_wrong_last_bit is not enabled for SPI1! \
	       Please, enable this option in platform.stm32.f4.stm32f4_discovery.arch
#endif

	CONF_SPI1_CLK_ENABLE_SPI();

#if defined CONF_SPI1_REGION_BASE
	stm32_spi_init(&stm32_spi1, (void *) CONF_SPI1_REGION_BASE);
#else
	stm32_spi_init(&stm32_spi1, SPI1);
#endif	/* CONF_SPI1_REGION_BASE */

	gpio_setup_mode(CONF_SPI1_PIN_SCK_PORT, CONF_SPI1_PIN_SCK_NR,
		GPIO_MODE_ALT_SET(CONF_SPI1_PIN_SCK_AF) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

	gpio_setup_mode(CONF_SPI1_PIN_MISO_PORT, CONF_SPI1_PIN_MISO_NR,
		GPIO_MODE_ALT_SET(CONF_SPI1_PIN_MISO_AF) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

	gpio_setup_mode(CONF_SPI1_PIN_MOSI_PORT, CONF_SPI1_PIN_MOSI_NR,
		GPIO_MODE_ALT_SET(CONF_SPI1_PIN_MOSI_AF) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

#if defined(CONF_SPI1_PIN_CS_PORT)
	/* Chip Select is usual GPIO pin. */
	gpio_setup_mode(CONF_SPI1_PIN_CS_PORT, CONF_SPI1_PIN_CS_NR,
		GPIO_MODE_OUT | GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);
		
	/* Chip Select is in inactive state by default. */
	gpio_set(CONF_SPI1_PIN_CS_PORT, CONF_SPI1_PIN_CS_NR, GPIO_PIN_HIGH);
#endif

	return 0;
}

#define SPI_DEV_NAME      stm32_spi_1

SPI_DEV_DEF(SPI_DEV_NAME, &stm32_spi_ops, &stm32_spi1, 1);
