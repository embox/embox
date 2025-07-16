/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 25.12.2019
 */

#include <stddef.h>

#include <util/macro.h>

#include <drivers/gpio.h>
#include <drivers/spi.h>

#include "stm32_spi.h"

#include <config/board_config.h>

#define SPI_BUS_NUM        3

#if defined CONF_SPI3_REGION_BASE
#define SPI_REGION_BASE        ((uintptr_t)CONF_SPI3_REGION_BASE)
#else
#define SPI_REGION_BASE         SPI3
#endif /* defined CONF_SPI3_REGION_BASE */

#define CLK_ENABLE_SPI          CONF_SPI3_CLK_ENABLE_SPI

#define SPI_PIN_SCK_PORT        CONF_SPI3_PIN_SCK_PORT
#define SPI_PIN_SCK_NR          CONF_SPI3_PIN_SCK_NR
#define SPI_PIN_SCK_AF          CONF_SPI3_PIN_SCK_AF

#define SPI_PIN_MISO_PORT       CONF_SPI3_PIN_MISO_PORT
#define SPI_PIN_MISO_NR         CONF_SPI3_PIN_MISO_NR
#define SPI_PIN_MISO_AF         CONF_SPI3_PIN_MISO_AF

#define SPI_PIN_MOSI_PORT       CONF_SPI3_PIN_MOSI_PORT
#define SPI_PIN_MOSI_NR         CONF_SPI3_PIN_MOSI_NR
#define SPI_PIN_MOSI_AF         CONF_SPI3_PIN_MOSI_AF

static int stm32_spi3_init(void);
static struct stm32_spi stm32_spi3 = {
	.hw_init = stm32_spi3_init,
#if defined(CONF_SPI3_PIN_CS_PORT)
	.nss_port = CONF_SPI3_PIN_CS_PORT,
	.nss_pin  = CONF_SPI3_PIN_CS_NR,
#endif
	.bits_per_word = CONF_SPI3_BITS_PER_WORD,
	.clk_div       = CONF_SPI3_CLK_DIV,
};

static int stm32_spi3_init(void) {
	CLK_ENABLE_SPI();

	stm32_spi_init(&stm32_spi3, (void *) SPI_REGION_BASE);

	gpio_setup_mode(SPI_PIN_SCK_PORT, SPI_PIN_SCK_NR,
		GPIO_MODE_ALT_SET(SPI_PIN_SCK_AF) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

	gpio_setup_mode(SPI_PIN_MISO_PORT, SPI_PIN_MISO_NR,
		GPIO_MODE_ALT_SET(SPI_PIN_MISO_AF) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

	gpio_setup_mode(SPI_PIN_MOSI_PORT, SPI_PIN_MOSI_NR,
		GPIO_MODE_ALT_SET(SPI_PIN_MOSI_AF) |
		GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

#if defined(CONF_SPI3_PIN_CS_PORT)
	/* Chip Select is usual GPIO pin. */
	gpio_setup_mode(CONF_SPI3_PIN_CS_PORT, CONF_SPI3_PIN_CS_NR,
		GPIO_MODE_OUT | GPIO_MODE_OUT_PUSH_PULL | GPIO_MODE_IN_PULL_UP);

	/* Chip Select is in inactive state by default. */
	gpio_set(CONF_SPI3_PIN_CS_PORT, CONF_SPI3_PIN_CS_NR, GPIO_PIN_HIGH);
#endif

	return 0;
}

#define SPI_BUS_NAME      MACRO_CONCAT(spi,SPI_BUS_NUM)

SPI_CONTROLLER_DEF(SPI_BUS_NAME, &stm32_spi_ops, &stm32_spi3, SPI_BUS_NUM);

#define SPI_DEV_NUM        0
#define SPI_DEV_NAME       MACRO_CONCAT(MACRO_CONCAT(spi_,SPI_BUS_NUM),_)
#define SPI_DEV_FULL_NAME  MACRO_CONCAT(SPI_DEV_NAME,SPI_DEV_NUM)

SPI_DEV_DEF(SPI_DEV_FULL_NAME, NULL, &stm32_spi3, SPI_BUS_NUM, SPI_DEV_NUM, NULL);
