/**
 * @file
 * @brief Communicate two boards over I2C
 *
 * @date    29.03.2017
 * @author  Alex Kalmuk
 */

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <kernel/irq.h>

#include "stm32f4_discovery.h"

#define I2C_MODE_MASTER  0
#define I2C_MODE_SLAVE   1

#define I2C_BUFFERSIZE   256

#define I2C_ADDR         0x30F

static I2C_HandleTypeDef I2cHandle;

static void init_leds(void) {
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED6);
}

static irq_return_t i2c_ev_irq_handler(unsigned int irq_nr, void *data) {
	HAL_I2C_EV_IRQHandler(&I2cHandle);
	return IRQ_HANDLED;
}

static irq_return_t i2c_er_irq_handler(unsigned int irq_nr, void *data) {
	HAL_I2C_ER_IRQHandler(&I2cHandle);
	return IRQ_HANDLED;
}

static int i2c_init(int i2c_nr) {
	int res = 0;
	int ev_irq_nr, er_irq_nr;
	I2C_TypeDef *i2c;

	switch (i2c_nr) {
	case 1:
		i2c = I2C1;
		ev_irq_nr = 47;
		er_irq_nr = 48;
		break;
	default:
		return -1;
	}

	I2cHandle.Instance             = i2c;

	I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_10BIT;
	I2cHandle.Init.ClockSpeed      = 400000;
	I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
	I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
	I2cHandle.Init.OwnAddress1     = I2C_ADDR;
	I2cHandle.Init.OwnAddress2     = 0;

	if (HAL_I2C_Init(&I2cHandle) != HAL_OK) {
		printf("HAL_I2C_Init error\n");
		return -1;
	}

	res |= irq_attach(ev_irq_nr, i2c_ev_irq_handler, 0, NULL, "I2C events");
	if (res < 0) {
		printf("irq_attach\n");
	}
	res |= irq_attach(er_irq_nr, i2c_er_irq_handler, 0, NULL, "I2C errors");
	if (res < 0) {
		printf("irq_attach\n");
	}

	return res;
}

static int i2c_buffer_cmp(uint8_t *tx, uint8_t *rx, size_t sz) {
	int i;

	for (i = 0; i < I2C_BUFFERSIZE; i++) {
		if (tx[i] != rx[i]) {
			return -1;
		}
	}

	return 0;
}

static int i2c_master(uint8_t *tx_buf, uint8_t *rx_buf) {
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);

	printf("Press USER button to start transmittion\n");
	/* Wait until pressed */
	while (BSP_PB_GetState(BUTTON_KEY) != 1)
		;
	/* Wait until released */
	while (BSP_PB_GetState(BUTTON_KEY) != 0)
		;

	do {
		if (HAL_I2C_Master_Transmit_IT(&I2cHandle, (uint16_t)I2C_ADDR,
										tx_buf, I2C_BUFFERSIZE) != HAL_OK) {
			printf("HAL_I2C_Master_Transmit_IT failed\n");
			return -1;
		}

		while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
			;
	/* When Acknowledge failure occurs (Slave don't acknowledge its address)
	 * Master restarts communication */
	} while (HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);

	printf("Press USER button to start receiving\n");
	/* Wait until pressed */
	while (BSP_PB_GetState(BUTTON_KEY) != 1)
		;
	/* Wait until released */
	while (BSP_PB_GetState(BUTTON_KEY) != 0)
		;

	do {
		if (HAL_I2C_Master_Receive_IT(&I2cHandle, (uint16_t)I2C_ADDR,
										rx_buf, I2C_BUFFERSIZE) != HAL_OK) {
			printf("HAL_I2C_Master_Receive_IT failed\n");
			return -1;
		}
	/* When Acknowledge failure occurs (Slave don't acknowledge its address)
	 * Master restarts communication */
	} while (HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);

	return 0;
}

static int i2c_slave(uint8_t *tx_buf, uint8_t *rx_buf) {
	if (HAL_I2C_Slave_Receive_IT(&I2cHandle,
								 rx_buf, I2C_BUFFERSIZE) != HAL_OK) {
		printf("HAL_I2C_Slave_Receive_IT failed\n");
		return -1;
	}

	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
		;

	if (HAL_I2C_Slave_Transmit_IT(&I2cHandle,
								 tx_buf, I2C_BUFFERSIZE) != HAL_OK) {
		printf("HAL_I2C_Slave_Transmit_IT failed\n");
		return -1;
	}
	return 0;
}

static int i2c_test(int mode) {
	int i;
	int res = 0;
	uint8_t tx_buf[I2C_BUFFERSIZE];
	uint8_t rx_buf[I2C_BUFFERSIZE];

	for (i = 0; i < I2C_BUFFERSIZE; i++) {
		tx_buf[i] = i;
	}

	switch (mode) {
	case I2C_MODE_MASTER:
		if (i2c_master(tx_buf, rx_buf) < 0) {
			return -1;
		}
		break;
	case I2C_MODE_SLAVE:
		if (i2c_slave(tx_buf, rx_buf) < 0) {
			return -1;
		}
		break;
	default:
		printf("Unknown i2c mode\n");
		return -1;
	}

	while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
		;

	/* Finally, compare TX and RX buffers */
	res = i2c_buffer_cmp(tx_buf, rx_buf, I2C_BUFFERSIZE);
	if (res < 0) {
		printf("Error: TX data differs from RX data\n");
	}

	return res;
}

static void print_usage(void) {
	printf("Usage: i2c_connect_boards [-h] -s <i2c_nr> -m <mode> n\n"
			"Example:\n"
			"  First board: i2c_connect_boards -s 1 -m master\n"
			"  Second board: i2c_connect_boards -s 1 -m slave\n");
}

int main(int argc, char *argv[]) {
	int res = 0, opt;
	int i2c_nr, mode;
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
			if ((optarg == NULL) || (!sscanf(optarg, "%d", &i2c_nr))) {
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
				mode = I2C_MODE_MASTER;
			} else if (!strcmp(mode_str, "slave")) {
				mode = I2C_MODE_SLAVE;
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

	res = i2c_init(i2c_nr);
	if (res < 0) {
		printf("Error: I2C initialization failed\n");
		return -EBUSY;
	}
	printf("I2C %s started on I2C%d...\n\n", mode_str, i2c_nr);
	
	res = i2c_test(mode);
	if (res < 0) {
		BSP_LED_On(LED5);
		return -EAGAIN;
	}

	printf("SUCCESS\n");
	BSP_LED_On(LED6);

	return 0;
}
