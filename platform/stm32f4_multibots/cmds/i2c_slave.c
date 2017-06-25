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

#define I2CADDR       0x23
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

static void i2c_slave_run(void) {
	uint32_t i = 0;

	while (1) {
		for (i = 0; i < TXBUFFERSIZE; i++) {
			tx_buff[i] = 0xCD;
			rx_buff[i] = 0x01;
		}

		if (HAL_I2C_Slave_Receive_IT(&I2cHandle, (uint8_t *)rx_buff, RXBUFFERSIZE) != HAL_OK) {
			printf("%s\n", ">> HAL_I2C_Slave_Receive_IT error\n");
		}

		while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
			;

		if (HAL_I2C_Slave_Transmit_IT(&I2cHandle, (uint8_t*)tx_buff, TXBUFFERSIZE)!= HAL_OK) {
			printf("%s\n", ">> HAL_I2C_Slave_Transmit_IT error\n");
		}

		while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
			;
	}
}

int main(int argc, char *argv[]) {
	int res;

	printf("I2C slave running\n");

	init_leds();
	res = stm32f4_i2c_init(&I2cHandle, I2CADDR);
	if (res < 0) {
		return -1;
	}
	BSP_LED_Toggle(LED5);

	i2c_slave_run();

	return 0;
}
