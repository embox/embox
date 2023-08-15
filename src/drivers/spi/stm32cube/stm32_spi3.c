/**
 * @file stm32_spi3.c
 * @brief it's compatible but not the same as stm32_spi1.c, stm32_spi2.c and stm32_spi5.c
 * @author Andrew Bursian
 * @version
 * @date 14.06.2023
 */

#include <string.h>
#include <util/log.h>

#include <drivers/spi.h>
#include "stm32_spi.h"

#include <config/board_config.h>
#include <framework/mod/options.h>

#include <embox/unit.h>
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
	GPIO_InitTypeDef  GPIO_InitStruct;

	CONF_SPI3_CLK_ENABLE_SCK();
	CONF_SPI3_CLK_ENABLE_MISO();
	CONF_SPI3_CLK_ENABLE_MOSI();
#if defined(CONF_SPI3_PIN_CS_PORT)
	CONF_SPI3_CLK_ENABLE_CS();
#endif
	CONF_SPI3_CLK_ENABLE_SPI();

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));
	GPIO_InitStruct.Pin       = CONF_SPI3_PIN_SCK_NR;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
#if	OPTION_GET(NUMBER, pullup) == 1
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
#else
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
#endif
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = CONF_SPI3_PIN_SCK_AF;
	HAL_GPIO_Init(CONF_SPI3_PIN_SCK_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = CONF_SPI3_PIN_MISO_NR;
	GPIO_InitStruct.Alternate = CONF_SPI3_PIN_MISO_AF;
	HAL_GPIO_Init(CONF_SPI3_PIN_MISO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = CONF_SPI3_PIN_MOSI_NR;
	GPIO_InitStruct.Alternate = CONF_SPI3_PIN_MOSI_AF;
	HAL_GPIO_Init(CONF_SPI3_PIN_MOSI_PORT, &GPIO_InitStruct);

#if defined(CONF_SPI3_PIN_CS_PORT)
	/* Chip Select is usual GPIO pin. */
	GPIO_InitStruct.Pin       = CONF_SPI3_PIN_CS_NR;
	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;

#if	CONF_SPI3_PIN_CS_AF == -1
	GPIO_InitStruct.Alternate = 0; // NOAF in board configuration file defined as -1 and not suitable for GPIO_InitTypeDef init structure
#else
	GPIO_InitStruct.Alternate = CONF_SPI3_PIN_CS_AF;
#endif

	HAL_GPIO_Init(CONF_SPI3_PIN_CS_PORT, &GPIO_InitStruct);

	/* Chip Select is in inactive state by default. */
	HAL_GPIO_WritePin(CONF_SPI3_PIN_CS_PORT, CONF_SPI3_PIN_CS_NR, GPIO_PIN_SET);
#endif //defined(CONF_SPI3_PIN_CS_PORT)

	stm32_spi_init(&stm32_spi3, SPI3);

	return 0;
}

#define SPI_DEV_NAME      stm32_spi_1

SPI_DEV_DEF(SPI_DEV_NAME, &stm32_spi_ops, &stm32_spi3, 3);
