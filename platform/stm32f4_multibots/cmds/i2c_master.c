/**
 * @file
 * @brief
 *
 * @date    29.03.2017
 * @author  Alex Kalmuk
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <kernel/irq.h>
#include <embox/unit.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4_discovery.h"

#include <drivers/i2c/stm32f4/stm32f4_i2c.h>

#define SLAVE_ADDR       0x23
#define OWN_ADDR         0x22

#define TXBUFFERSIZE  16
#define RXBUFFERSIZE  TXBUFFERSIZE

static uint8_t tx_buff[TXBUFFERSIZE];
static uint8_t rx_buff[RXBUFFERSIZE];

static I2C_HandleTypeDef I2cHandle;

static void init_leds(void) {
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
}

static void i2c_master_run(void) {
	uint32_t i = 0, j = 0;

	for (j = 0; j < 10; j++) {
		for (i = 0; i < TXBUFFERSIZE; i++) {
			tx_buff[i] = 0xAB;
			rx_buff[i] = 0x00;
		}

		do {
			while (HAL_I2C_Master_Transmit_IT(&I2cHandle, (uint16_t)SLAVE_ADDR, tx_buff, TXBUFFERSIZE) != HAL_OK)
				;
			while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
				;
		} while(HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);
		printf("%s\n", ">> HAL_I2C_Master_Transmit_IT OK\n");

		do {
			while (HAL_I2C_Master_Receive_IT(&I2cHandle, (uint16_t)SLAVE_ADDR, rx_buff, RXBUFFERSIZE) != HAL_OK)
				;
		} while(HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);
		printf("%s\n", ">> HAL_I2C_Master_Receive_IT OK\n");

		printf("rx_buf: ");
		for (i = 0; i < RXBUFFERSIZE; i++) {
			printf(" %02x ", rx_buff[i]);
		}
		printf("\n");
	}
}

int main(int argc, char *argv[]) {
	int res;

	printf("I2C master running\n");

	init_leds();
	res = stm32f4_i2c_init(&I2cHandle, OWN_ADDR);
	if (res < 0) {
		return -1;
	}
	BSP_LED_Toggle(LED3);

	i2c_master_run();

	return 0;
}
