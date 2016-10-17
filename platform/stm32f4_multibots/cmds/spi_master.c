
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <embox/unit.h>

#include "stm32f4_discovery.h"

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
#else
#error Unsupported SPI
#endif

#define SPI_SLAVE_SYNBYTE         0x53
#define SPI_MASTER_SYNBYTE        0xAC

#define SPI_TIMEOUT_MAX           1000000000

static SPI_HandleTypeDef SpiHandle;

static int spi_init(void) {
	memset(&SpiHandle, 0, sizeof(SpiHandle));

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

static void spi_delay(int n) {
	int i = n;
	while (i--)
		;
}

static void spi_sync(void) {
	uint8_t txackbytes = SPI_MASTER_SYNBYTE;
	uint8_t rxackbytes = 0x00;

	do {
		rxackbytes = 0x0;
		/* Call SPI write function to send command to slave */
		if (HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t *)&txackbytes, (uint8_t *)&rxackbytes, 1, 10000000) != HAL_OK) {
			printf("%s\n", ">>> spi_sync error");
		}
	} while(rxackbytes != SPI_SLAVE_SYNBYTE);
}

static int spi_test(void) {
	uint8_t res = 0;
	uint8_t cmd = 0x57;

	printf("SPI test\n");

	while(1) {
		spi_delay(1000000);
		res = 0;
		// sync
		spi_sync();
		printf(">> synchronized\n");
		// request data
		if (HAL_SPI_Transmit(&SpiHandle, &cmd, 1, SPI_TIMEOUT_MAX) != HAL_OK) {
			printf("HAL SPI Transmit error\n");
			return -1;
		}
		// data from slave
		if (HAL_SPI_Receive(&SpiHandle, &res, 1, SPI_TIMEOUT_MAX) != HAL_OK) {
			printf("HAL_SPI_Receive error 1\n");
			return -1;
		}
		printf("0x%x ", res);
	}

	return 0;
}

static void init_leds() {
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
}


int main(int argc, char *argv[]) {
	int res;

	printf("SPI master start!\n");

	//HAL_Init();

	init_leds();
	res = spi_init();
	if (res < 0) {
		return -1;
	}
    BSP_LED_Toggle(LED3);
	spi_test();

	return 0;
}
