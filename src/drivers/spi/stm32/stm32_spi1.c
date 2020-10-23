/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 25.12.2019
 */

#include <string.h>
#include <drivers/spi.h>
#include <drivers/spi/stm32_spi_conf.h>

#include <embox/unit.h>

#if defined(STM32F407xx)
#include <framework/mod/options.h>
#include <config/platform/stm32/f4/stm32f4_discovery/arch.h>
#endif

#include "stm32_spi.h"

EMBOX_UNIT_INIT(stm32_spi1_init);

static struct stm32_spi stm32_spi1 = {
#if defined(SPI1_NSS_GPIO_PORT) && defined(SPI1_NSS_PIN)
	.nss_port = SPI1_NSS_GPIO_PORT,
	.nss_pin  = SPI1_NSS_PIN
#endif
};

static int stm32_spi1_init(void) {
	GPIO_InitTypeDef  GPIO_InitStruct;

#if defined(STM32F407xx) && !OPTION_MODULE_GET(platform__stm32__f4__stm32f4_discovery__arch,BOOLEAN,errata_spi_wrong_last_bit)
	#error errata_spi_wrong_last_bit is not enabled for SPI1! \
	       Please, enable this option in platform.stm32.f4.stm32f4_discovery.arch
#endif

	spi1_enable_gpio_clocks();
	spi1_enable_spi_clocks();

	stm32_spi_init(&stm32_spi1, SPI1);

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));
	GPIO_InitStruct.Pin       = SPI1_SCK_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = SPI1_SCK_AF;
	HAL_GPIO_Init(SPI1_SCK_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SPI1_MISO_PIN;
	GPIO_InitStruct.Alternate = SPI1_MISO_AF;
	HAL_GPIO_Init(SPI1_MISO_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SPI1_MOSI_PIN;
	GPIO_InitStruct.Alternate = SPI1_MOSI_AF;
	HAL_GPIO_Init(SPI1_MOSI_GPIO_PORT, &GPIO_InitStruct);

#if defined(SPI1_NSS_GPIO_PORT) && defined(SPI1_NSS_PIN)
	/* Chip Select is usual GPIO pin. */
	GPIO_InitStruct.Pin       = SPI1_NSS_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	HAL_GPIO_Init(SPI1_NSS_GPIO_PORT, &GPIO_InitStruct);

	/* Chip Select is in inactive state by default. */
	HAL_GPIO_WritePin(SPI1_NSS_GPIO_PORT, SPI1_NSS_PIN, GPIO_PIN_SET);
#endif

	return 0;
}

SPI_DEV_DEF("stm32_spi_1", &stm32_spi_ops, &stm32_spi1, 1);
