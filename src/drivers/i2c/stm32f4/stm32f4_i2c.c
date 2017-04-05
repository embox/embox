/**
 * @file
 * @brief
 *
 * @date    29.03.2017
 * @author  Alex Kalmuk
 */

#include <string.h>
#include <util/log.h>

#include "stm32_i2c_conf_f4.h"

int stm32f4_i2c_init(I2C_HandleTypeDef *I2cHandle, uint8_t slave_addr) {
	I2cHandle->Instance             = I2Cx;

	I2cHandle->Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	I2cHandle->Init.ClockSpeed      = 400000;
	I2cHandle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2cHandle->Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
	I2cHandle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2cHandle->Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
	I2cHandle->Init.OwnAddress1     = slave_addr;
	I2cHandle->Init.OwnAddress2     = 0;

	if (HAL_I2C_Init(I2cHandle) != HAL_OK) {
		log_error("STM32F4 I2C initialization error\n");
		return -1;
	}

	return 0;
}
