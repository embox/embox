/**
 * @file
 * @brief  Adoptaion of NRF24 library to STM32F4 Cube
 *
 * @date   27.06.18
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <string.h>

#include <kernel/irq.h>
#include <util/log.h>
#include <embox/unit.h>

#include "stm32f4_discovery.h"

#include "nrf24.h"

#define MODOPS_SPI OPTION_GET(NUMBER, spi)

#if MODOPS_SPI == 1
#define SPIx                     SPI1

#define SPIx_CS_PIN              GPIO_PIN_4
#define SPIx_CS_GPIO_PORT        GPIOE

#define NRF24_CE_PIN             2

#elif MODOPS_SPI == 2
#define SPIx                     SPI2
#define SPIx_CS_PIN              GPIO_PIN_12
#define SPIx_CS_GPIO_PORT        GPIOB

#define NRF24_CE_PIN             10

#else
#error Unsupported SPI
#endif

#define NRF24_SPI_TIMEOUT        1000000

#define CS_L() \
	HAL_GPIO_WritePin(GPIOB, SPIx_CS_PIN, GPIO_PIN_RESET)

#define CS_H() \
	HAL_GPIO_WritePin(GPIOB, SPIx_CS_PIN, GPIO_PIN_SET)

static SPI_HandleTypeDef SpiHandle;

static int exti_line0_init(void);

static int spi_init(void) {
	SpiHandle.Instance               = SPIx;
	SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
	SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
	SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
	SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	SpiHandle.Init.CRCPolynomial     = 7;
	SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
	SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
	SpiHandle.Init.NSS               = SPI_NSS_SOFT;
	SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
	SpiHandle.Init.Mode              = SPI_MODE_MASTER;
	
	if (HAL_SPI_Init(&SpiHandle) != HAL_OK) {
		log_error("%s\n", "HAL_SPI_Init error\n");
		return -1;
	}

	return 0;
}

int nrf24_hw_init(void) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

	__GPIOD_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();
	__GPIOE_CLK_ENABLE();

	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;

	GPIO_InitStruct.Pin = SPIx_CS_PIN;
#if MODOPS_SPI == 2
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#else
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
#endif

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

	if (exti_line0_init() < 0) {
		log_error("Error exti_line0_init\n");
		return -1;
	}

	if (spi_init() < 0) {
		log_error("Error spi_init\n");
		return -1;
	}

	return 0;
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

void nrf24_spi_transfer(uint8_t *tx, uint8_t *rx, uint8_t len) {
	if (HAL_SPI_TransmitReceive(&SpiHandle, tx, rx, len, NRF24_SPI_TIMEOUT)
			!= HAL_OK) {
		log_error("spi_sync error\n");
	}
}

void nrf24_spi_transmit(uint8_t *tx, uint8_t len) {
	if (HAL_SPI_Transmit(&SpiHandle, tx, len, NRF24_SPI_TIMEOUT) != HAL_OK) {
		log_error("spi_sync error\n");
	}
}

void nrf24_spi_receive(uint8_t *rx, uint8_t len) {
	if (HAL_SPI_Receive(&SpiHandle, rx, len, NRF24_SPI_TIMEOUT) != HAL_OK) {
		log_error("spi_sync error\n");
	}
}

static irq_return_t exti0_handler(unsigned int irq_nr, void *data) {
	if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET) {
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	}

	return nrf24_handle_interrupt() == 0 ? IRQ_HANDLED : -1;
}

static int inited;

static int exti_line0_init(void) {
	GPIO_InitTypeDef   GPIO_InitStructure;

	/* Enable GPIOA clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* Configure PA0 pin as input floating */
	GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin  = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	if (!inited) {
		irq_attach(EXTI0_IRQn + 16, exti0_handler, 0, NULL, "NRF24");
		inited = 1;
	}

	return 0;
}
