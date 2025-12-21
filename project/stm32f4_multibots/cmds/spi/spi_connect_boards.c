/**
 * @file
 * @brief  SPI Master
 *
 * @date   23.06.18
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <kernel/irq.h>

#include "stm32f4_discovery.h"

#define SPI_TIMEOUT_MAX       1000000000
#define SPI_BUFFERSIZE        128

static SPI_HandleTypeDef SpiHandle;

static void init_leds() {
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
}

static irq_return_t spi_irq_handler(unsigned int irq_nr, void *data) {
	HAL_SPI_IRQHandler(&SpiHandle);
	return 0;
}

static int spi_init(int spi_nr, int mode) {
	int irq_nr;
	SPI_TypeDef *spi;

	switch (spi_nr) {
	case 1:
		spi = SPI1;
		irq_nr = SPI1_IRQn + 16;
		break;
	case 2:
		spi = SPI2;
		irq_nr = SPI2_IRQn + 16;
		break;
	default:
		return -1;
	}

	SpiHandle.Instance               = spi;
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
	SpiHandle.Init.Mode              = mode;
	
	if (HAL_SPI_Init(&SpiHandle) != HAL_OK) {
		printf("%s\n", "HAL_SPI_Init error\n");
		return -1;
	}

	return irq_attach(irq_nr, spi_irq_handler, 0, NULL, "SPI_IRQHandler");
}

static int spi_buffer_cmp(char *tx, char *rx, size_t sz) {
	int i;

	for (i = 0; i < SPI_BUFFERSIZE; i++) {
		if (tx[i] != rx[i]) {
			return -1;
		}
	}

	return 0;
}

static int spi_test(void) {
	int i;
	int res = 0;
	char tx_buf[SPI_BUFFERSIZE];
	char rx_buf[SPI_BUFFERSIZE];

	for (i = 0; i < SPI_BUFFERSIZE; i++) {
		tx_buf[i] = i;
	}

	if (HAL_SPI_TransmitReceive_IT(&SpiHandle, (uint8_t*)tx_buf,
			(uint8_t *)rx_buf, SPI_BUFFERSIZE) != HAL_OK) {
		printf("HAL_SPI_TransmitReceive_IT failed\n");
		return -1;
	}

	while (HAL_SPI_GetState(&SpiHandle) != HAL_SPI_STATE_READY)
		;

	res = spi_buffer_cmp(tx_buf, rx_buf, SPI_BUFFERSIZE);
	if (res < 0) {
		printf("Error: TX data differs from RX data\n");
	}

	return res;
}

static void print_usage(void) {
	printf("Usage: spi_connect_boards [-h] -s <spi_nr> -m <mode> n\n"
			"Example:\n"
			"  First board: spi_connect_boards -s 2 -m master\n"
			"  Second board: spi_connect_boards -s 2 -m slave\n");
}

int main(int argc, char *argv[]) {
	int res = 0, opt;
	int spi_nr, mode;
	char mode_str[16];

	if (argc <= 1) {
		print_usage();
		return -EINVAL;
	}

	while (-1 != (opt = getopt(argc, argv, "hs:m:"))) {
		printf("\n");
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 's':
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &spi_nr))) {
				print_usage();
				return -EINVAL;
			}
			break;
		case 'm':
			if ((optarg == NULL) || (!sscanf(optarg, "%s", mode_str))) {
				print_usage();
				return -EINVAL;
			}
			if (!strcmp(mode_str, "master")) {
				mode = SPI_MODE_MASTER;
			} else if (!strcmp(mode_str, "slave")) {
				mode = SPI_MODE_SLAVE;
			} else {
				print_usage();
				return -EINVAL;
			}
			break;
		default:
			print_usage();
			return -EINVAL;
		}
	}

	init_leds();

	if (mode == SPI_MODE_MASTER) {
		BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

		printf("Press USER button to start\n");

		/* Wait for USER Button press before starting the Communication */
		while (BSP_PB_GetState(BUTTON_KEY) != 1) {
			BSP_LED_Toggle(LED3);
			HAL_Delay(40);
		}

		BSP_LED_Off(LED3);
	}

	res = spi_init(spi_nr, mode);
	if (res < 0) {
		printf("Error: SPI initialization failed\n");
		return -EBUSY;
	}
	printf("SPI %s started on SPI%d...\n\n", mode_str, spi_nr);
	
	res = spi_test();
	if (res < 0) {
  		BSP_LED_On(LED5);
		return -EAGAIN;
	}

	printf("SUCCESS\n");
  	BSP_LED_On(LED6);

	return 0;
}
