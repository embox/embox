/**
 * @file
 * @brief  SPI MSP layer derived from Cube
 *
 * @date   23.06.18
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <string.h>
#include <assert.h>
#include <util/log.h>
#include <embox/unit.h>

#include "stm32f4_discovery.h"

static int spix_sck_pin(void *spi) {
	assert(spi == SPI1 || spi == SPI2);
	return spi == SPI1 ? GPIO_PIN_3 : GPIO_PIN_13;
}

static int spix_miso_pin(void *spi) {
	assert(spi == SPI1 || spi == SPI2);
	return spi == SPI1 ? GPIO_PIN_4 : GPIO_PIN_14;
}

static int spix_mosi_pin(void *spi) {
	assert(spi == SPI1 || spi == SPI2);
	return spi == SPI1 ? GPIO_PIN_5 : GPIO_PIN_15;
}

static int spix_sck_af(void *spi) {
	assert(spi == SPI1 || spi == SPI2);
	return spi == SPI1 ? GPIO_AF5_SPI1 : GPIO_AF5_SPI2;
}

static int spix_miso_af(void *spi) {
	assert(spi == SPI1 || spi == SPI2);
	return spi == SPI1 ? GPIO_AF5_SPI1 : GPIO_AF5_SPI2;
}

static int spix_mosi_af(void *spi) {
	assert(spi == SPI1 || spi == SPI2);
	return spi == SPI1 ? GPIO_AF5_SPI1 : GPIO_AF5_SPI2;
}

static GPIO_TypeDef *spix_sck_port(void *spi) {
	assert(spi == SPI1 || spi == SPI2);
	return GPIOB;
}

static GPIO_TypeDef *spix_miso_port(void *spi) {
	assert(spi == SPI1 || spi == SPI2);
	return GPIOB;
}

static GPIO_TypeDef *spix_mosi_port(void *spi) {
	assert(spi == SPI1 || spi == SPI2);
	return GPIOB;
}

static void spix_enable_clocks(void *spi) {
	assert(spi == SPI1 || spi == SPI2);
	if (spi == SPI1) {
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_SPI1_CLK_ENABLE();
	} else if (spi == SPI2) {
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_SPI2_CLK_ENABLE();
	}
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_TypeDef *spi;

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

	log_debug("");

	spi = hspi->Instance;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO TX/RX clock */
	/* Enable SPI clock */
	spix_enable_clocks(spi);

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* SPI SCK GPIO pin configuration  */
	GPIO_InitStruct.Pin       = spix_sck_pin(spi);
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = spix_sck_af(spi);

	HAL_GPIO_Init(spix_sck_port(spi), &GPIO_InitStruct);

	/* SPI MISO GPIO pin configuration  */
	GPIO_InitStruct.Pin = spix_miso_pin(spi);
	GPIO_InitStruct.Alternate = spix_miso_af(spi);

	HAL_GPIO_Init(spix_miso_port(spi), &GPIO_InitStruct);

	/* SPI MOSI GPIO pin configuration  */
	GPIO_InitStruct.Pin = spix_mosi_pin(spi);
	GPIO_InitStruct.Alternate = spix_mosi_af(spi);

	HAL_GPIO_Init(spix_mosi_port(spi), &GPIO_InitStruct);
}
