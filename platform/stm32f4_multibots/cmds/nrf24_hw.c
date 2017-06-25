
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <embox/unit.h>

#include "stm32f4_discovery.h"

#include "nrf24.h"

#define MODOPS_SPI OPTION_GET(NUMBER, spi)

#if MODOPS_SPI == 1
#define SPIx                             SPI1

#define SPIx_CS_PIN                      GPIO_PIN_4
#define SPIx_CS_GPIO_PORT                GPIOE

#define NRF24_CE_PIN                     2

#elif MODOPS_SPI == 2
#define SPIx                             SPI2
#define SPIx_CS_PIN                      GPIO_PIN_12
#define SPIx_CS_GPIO_PORT                GPIOB

#define NRF24_CE_PIN                     11

#else
#error Unsupported SPI
#endif

#define SPI_TIMEOUT_MAX           1000000000

#define CS_L() \
	HAL_GPIO_WritePin(GPIOB, SPIx_CS_PIN, GPIO_PIN_RESET)

#define CS_H() \
	HAL_GPIO_WritePin(GPIOB, SPIx_CS_PIN, GPIO_PIN_SET)

SPI_HandleTypeDef SpiHandle;

int spi_init(void) {
	SpiHandle.Instance               = SPIx;
	SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
	SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
	SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	SpiHandle.Init.CRCPolynomial     = 7;
	SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
	SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	SpiHandle.Init.NSS               = SPI_NSS_SOFT;
	SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
	SpiHandle.Init.Mode              = SPI_MODE_MASTER;
	
	if (HAL_SPI_Init(&SpiHandle) != HAL_OK) {
		printf("%s\n", "HAL_SPI_Init error\n");
		return -1;
	}

	return 0;
}

void nrf24_setupPins() {
	GPIO_InitTypeDef  GPIO_InitStruct;

	printf("NRF24 init\n");

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

	__GPIOD_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();
	__GPIOE_CLK_ENABLE();

	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;

	printf(">> nrf24: Init CS pin\n");
	GPIO_InitStruct.Pin = SPIx_CS_PIN;
#if MODOPS_SPI == 2
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#else
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
#endif

	printf(">> nrf24: Init CE pin\n");
	GPIO_InitStruct.Pin = NRF24_CE_PIN;
#if MODOPS_SPI == 2
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#else
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
#endif

	//CS_H();
#if MODOPS_SPI == 2
	HAL_GPIO_WritePin(GPIOB, SPIx_CS_PIN, GPIO_PIN_SET);
#else
	HAL_GPIO_WritePin(GPIOE, SPIx_CS_PIN, GPIO_PIN_SET);
#endif

#if MODOPS_SPI == 2
	HAL_GPIO_WritePin(GPIOD, NRF24_CE_PIN, GPIO_PIN_RESET);
#else
	HAL_GPIO_WritePin(GPIOE, NRF24_CE_PIN, GPIO_PIN_RESET);
#endif

	printf("NRF24 inited\n");
}

void nrf24_ce_digitalWrite(uint8_t state) {
#if MODOPS_SPI == 2
	if (state == HIGH) {
		HAL_GPIO_WritePin(GPIOD, NRF24_CE_PIN, GPIO_PIN_SET);
	} else if (state == LOW) {
		HAL_GPIO_WritePin(GPIOD, NRF24_CE_PIN, GPIO_PIN_RESET);
	}
#else
	if (state == HIGH) {
		HAL_GPIO_WritePin(GPIOE, NRF24_CE_PIN, GPIO_PIN_SET);
	} else if (state == LOW) {
		HAL_GPIO_WritePin(GPIOE, NRF24_CE_PIN, GPIO_PIN_RESET);
	}
#endif
}

void nrf24_csn_digitalWrite(uint8_t state) {
#if MODOPS_SPI == 2
	if (state == HIGH) {
		HAL_GPIO_WritePin(GPIOB, SPIx_CS_PIN, GPIO_PIN_SET);
	} else if (state == LOW) {
		HAL_GPIO_WritePin(GPIOB, SPIx_CS_PIN, GPIO_PIN_RESET);
	}
#else
	if (state == HIGH) {
		HAL_GPIO_WritePin(GPIOE, SPIx_CS_PIN, GPIO_PIN_SET);
	} else if (state == LOW) {
		HAL_GPIO_WritePin(GPIOE, SPIx_CS_PIN, GPIO_PIN_RESET);
	}
#endif
}
