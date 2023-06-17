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

#define BITS_PER_WORD    OPTION_GET(NUMBER, baudrateprescaler)
#define CLK_DIV          OPTION_GET(NUMBER, baudrateprescaler)

static struct stm32_spi stm32_spi3 = {
#if defined(CONF_SPI3_PIN_CS_PORT)
	.nss_port = CONF_SPI3_PIN_CS_PORT,
	.nss_pin  = CONF_SPI3_PIN_CS_NR,
#endif
	.bits_per_word = BITS_PER_WORD,
	.clk_div       = CLK_DIV,
};

static int stm32_spi3_init(void) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	CONF_SPI3_CLK_ENABLE_SCK();
	CONF_SPI3_CLK_ENABLE_MISO();
	CONF_SPI3_CLK_ENABLE_MOSI();
	CONF_SPI3_CLK_ENABLE_CS();
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
#if 0
	memset(&stm32_spi3.handle, 0, sizeof(stm32_spi3.handle));

	stm32_spi3.handle.Instance               = SPI3;
#if	OPTION_GET(NUMBER, baudrateprescaler) == 2
	stm32_spi3.handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
#elif	OPTION_GET(NUMBER, baudrateprescaler) == 4
	stm32_spi3.handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
#elif	OPTION_GET(NUMBER, baudrateprescaler) == 8
	stm32_spi3.handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
#elif	OPTION_GET(NUMBER, baudrateprescaler) == 16
	stm32_spi3.handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
#elif	OPTION_GET(NUMBER, baudrateprescaler) == 32
	stm32_spi3.handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
#elif	OPTION_GET(NUMBER, baudrateprescaler) == 64
	stm32_spi3.handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
#elif	OPTION_GET(NUMBER, baudrateprescaler) == 128
	stm32_spi3.handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
#elif	OPTION_GET(NUMBER, baudrateprescaler) == 256
	stm32_spi3.handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
#else
	stm32_spi3.handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
#endif
	stm32_spi3.handle.Init.Direction         = SPI_DIRECTION_2LINES;
	stm32_spi3.handle.Init.CLKPhase          = SPI_PHASE_1EDGE;
	stm32_spi3.handle.Init.CLKPolarity       = SPI_POLARITY_LOW;
	stm32_spi3.handle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	stm32_spi3.handle.Init.CRCPolynomial     = 7;
#if	OPTION_GET(NUMBER, datasize) == 4
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_4BIT;
#elif	OPTION_GET(NUMBER, datasize) == 5
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_5BIT;
#elif	OPTION_GET(NUMBER, datasize) == 6
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_6BIT;
#elif	OPTION_GET(NUMBER, datasize) == 7
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_7BIT;
#elif	OPTION_GET(NUMBER, datasize) == 8
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_8BIT;
#elif	OPTION_GET(NUMBER, datasize) == 9
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_9BIT;
#elif	OPTION_GET(NUMBER, datasize) == 10
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_10BIT;
#elif	OPTION_GET(NUMBER, datasize) == 11
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_11BIT;
#elif	OPTION_GET(NUMBER, datasize) == 12
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_12BIT;
#elif	OPTION_GET(NUMBER, datasize) == 13
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_13BIT;
#elif	OPTION_GET(NUMBER, datasize) == 14
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_14BIT;
#elif	OPTION_GET(NUMBER, datasize) == 15
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_15BIT;
#elif	OPTION_GET(NUMBER, datasize) == 16
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_16BIT;
#else
	stm32_spi3.handle.Init.DataSize          = SPI_DATASIZE_8BIT;
#endif
	stm32_spi3.handle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	stm32_spi3.handle.Init.NSS               = SPI_NSS_SOFT;
	stm32_spi3.handle.Init.TIMode            = SPI_TIMODE_DISABLE;
	stm32_spi3.handle.Init.Mode              = SPI_MODE_MASTER;

	if (HAL_OK != HAL_SPI_Init(&stm32_spi3.handle)) {
		log_error("Failed to init SPI!");
		return -1;
	}
#endif
	return 0;
}

SPI_DEV_DEF("stm32_spi_3", &stm32_spi_ops, &stm32_spi3, 3);
