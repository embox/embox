
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <embox/unit.h>

#include "stm32f4_discovery.h"

#include "nrf24_test.h"

#define MODOPS_SPI OPTION_GET(NUMBER, spi)

#if MODOPS_SPI == 1
#define SPIx                             SPI1
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI1_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_3
#define SPIx_SCK_GPIO_PORT               GPIOB
#define SPIx_SCK_AF                      GPIO_AF5_SPI1
#define SPIx_MISO_PIN                    GPIO_PIN_4
#define SPIx_MISO_GPIO_PORT              GPIOB
#define SPIx_MISO_AF                     GPIO_AF5_SPI1
#define SPIx_MOSI_PIN                    GPIO_PIN_5
#define SPIx_MOSI_GPIO_PORT              GPIOB
#define SPIx_MOSI_AF                     GPIO_AF5_SPI1
#define SPIx_CS_PIN                      GPIO_PIN_6
#define SPIx_CS_GPIO_PORT                GPIOB
#define SPIx_CS_AF                       GPIO_AF5_SPI2
#elif MODOPS_SPI == 2
#define SPIx                             SPI2
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI2_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI2_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI2_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_13
#define SPIx_SCK_GPIO_PORT               GPIOB
#define SPIx_SCK_AF                      GPIO_AF5_SPI2
#define SPIx_MISO_PIN                    GPIO_PIN_14
#define SPIx_MISO_GPIO_PORT              GPIOB
#define SPIx_MISO_AF                     GPIO_AF5_SPI2
#define SPIx_MOSI_PIN                    GPIO_PIN_15
#define SPIx_MOSI_GPIO_PORT              GPIOB
#define SPIx_MOSI_AF                     GPIO_AF5_SPI2
#define SPIx_CS_PIN                      GPIO_PIN_12
#define SPIx_CS_GPIO_PORT                GPIOB
#define SPIx_CS_AF                       GPIO_AF5_SPI2
#else
#error Unsupported SPI
#endif

#define SPI_TIMEOUT_MAX           1000000000
#define NRF24_CE_PIN              11

#define CS_L() \
	HAL_GPIO_WritePin(GPIOB, SPIx_CS_PIN, GPIO_PIN_RESET)

#define CS_H() \
	HAL_GPIO_WritePin(GPIOB, SPIx_CS_PIN, GPIO_PIN_SET)

SPI_HandleTypeDef SpiHandle;

static int spi_init(void) {
	SpiHandle.Instance               = SPIx;
	SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
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

static void nrf24_init(void) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	printf("NRF24 init\n");

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

	__GPIOD_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();

	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;

	printf(">> nrf24: Init CS pin\n");
	GPIO_InitStruct.Pin = SPIx_CS_PIN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	printf(">> nrf24: Init CE pin\n");
	GPIO_InitStruct.Pin = NRF24_CE_PIN;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	CS_H();
	HAL_GPIO_WritePin(GPIOD, NRF24_CE_PIN, GPIO_PIN_RESET);

	printf("NRF24 inited\n");
}

static uint8_t nrf24_rw_reg(uint8_t reg) {
	uint8_t txbytes = reg;
	uint8_t rxbytes = 0x0;
	if (HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t *)&txbytes, (uint8_t *)&rxbytes, 1, 10000000) != HAL_OK) {
		printf("%s\n", ">>> spi_sync error");
	}
	//printf("nrf24_rw_reg: out (output = %x)\n", rxbytes);
	return rxbytes;
}

static uint8_t nrf24_read_reg(uint8_t reg) {
	uint8_t value;

	CS_L();
	nrf24_rw_reg(reg);
	value = nrf24_rw_reg(0x00);
	CS_H();

	return value;
}

//static uint8_t nrf24_write_reg(uint8_t reg, value) {
//	uint8_t status;
//
//	CS_L();
//	status = nrf24_rw_reg(nRF24_CMD_WREG | reg);
//	value = nrf24_rw_reg(value);
//	CS_H();
//
//	return status;
//}

static void spi_delay(int n) {
	int i = n;
	while (i--)
		;
}

static void nrf24_test(void) {
	uint8_t reg = 0x00;
	printf("Starting NRF24 test...\n");
	spi_delay(1000000);

	/* Returned value is not correct on the first read, who knows why... */
	reg = nrf24_read_reg(nRF24_REG_CONFIG);

	reg = nrf24_read_reg(nRF24_REG_CONFIG);
	printf("nRF24_REG_CONFIG = %x\n", reg);
	reg = nrf24_read_reg(nRF24_REG_EN_AA);
	printf("nRF24_REG_EN_AA = %x\n", reg);

	printf("SNRF24 test finished!\n");
}

static void init_leds() {
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
}

int main(int argc, char *argv[]) {
	int res;

	printf("NRF24 test start\n");

	init_leds();
	nrf24_init();
	res = spi_init();
	if (res < 0) {
		return -1;
	}
    BSP_LED_Toggle(LED5);
	nrf24_test();

	return 0;
}
