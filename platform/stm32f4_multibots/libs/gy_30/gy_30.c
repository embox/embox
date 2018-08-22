/**
 * @file
 * @brief
 *
 * @date    29.03.2017
 * @author  Alex Kalmuk
 */

#include <unistd.h>

#include <kernel/irq.h>
#include <util/log.h>

#include "stm32f4_discovery.h"
#include "stm32f4xx_hal_i2c.h"
#include "gy_30.h"

#define GY30_ADDR       0x46

static I2C_HandleTypeDef I2cHandle;

uint16_t gy_30_read_light_level(void) {
	uint16_t level = 0;

	do {
		while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
			;
		if (HAL_I2C_Master_Receive_IT(&I2cHandle, (uint16_t)GY30_ADDR,
										(void *) &level, 2) != HAL_OK) {
			log_error("gy_30_read_light_level failed\n");
			return 0;
		}
		while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
			;
	} while(HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);

	/* The actual value in lx is level / 1.2 */
	return (((uint32_t) level) * 5) / 6;
}

void gy_30_setup_mode(uint8_t mode) {
	do {
		if (HAL_I2C_Master_Transmit_IT(&I2cHandle, (uint16_t)GY30_ADDR,
										(void *) &mode, 1) != HAL_OK) {
			log_error("gy_30_setup_mode failed\n");
			return;
		}
		while (HAL_I2C_GetState(&I2cHandle) != HAL_I2C_STATE_READY)
			;
	} while(HAL_I2C_GetError(&I2cHandle) == HAL_I2C_ERROR_AF);

	/* Documentation says that we need to wait approximately 180 ms here.*/
	usleep(180000);

	log_debug("%s\n", "> gy_30_setup_mode OK\n");
}

static irq_return_t i2c_ev_irq_handler(unsigned int irq_nr, void *data) {
	HAL_I2C_EV_IRQHandler(&I2cHandle);
	return IRQ_HANDLED;
}

static irq_return_t i2c_er_irq_handler(unsigned int irq_nr, void *data) {
	HAL_I2C_ER_IRQHandler(&I2cHandle);
	return IRQ_HANDLED;
}

int gy_30_init(int i2c_nr) {
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
		log_error("Unsupported I2C number\n");
		return -1;
	}

	I2cHandle.Instance             = i2c;

	I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	I2cHandle.Init.ClockSpeed      = 400000;
	I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
	I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
	I2cHandle.Init.OwnAddress1     = GY30_ADDR;
	I2cHandle.Init.OwnAddress2     = 0;

	if (HAL_I2C_Init(&I2cHandle) != HAL_OK) {
		log_error("HAL_I2C_Init failed\n");
		return -1;
	}

	res |= irq_attach(ev_irq_nr, i2c_ev_irq_handler, 0, NULL, "I2C events");
	if (res < 0) {
		log_error("irq_attach failed\n");
		return -1;
	}
	res |= irq_attach(er_irq_nr, i2c_er_irq_handler, 0, NULL, "I2C errors");
	if (res < 0) {
		log_error("irq_attach failed\n");
		return -1;
	}

	return 0;
}
