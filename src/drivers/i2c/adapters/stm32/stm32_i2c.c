/**
 * @file
 * @brief I2C driver for STM32
 *
 * @date    10.12.2018
 * @author  Alexander Kalmuk
 */
#include <assert.h>
#include <kernel/irq.h>
#include <util/log.h>

#include <drivers/i2c/i2c.h>

#include "stm32_i2c.h"

static int stm32_i2c_slave_select(struct stm32_i2c *adapter, int slave_addr) {
	I2C_HandleTypeDef *i2c_handle = adapter->i2c_handle;

	i2c_handle->Init.OwnAddress1 = slave_addr;
	i2c_handle->Init.OwnAddress2 = 0;

	if (HAL_I2C_Init(i2c_handle) != HAL_OK) {
		log_error("HAL_I2C_Init failed\n");
		return -1;
	}

	return 0;
}

static int stm32_i2c_current_slave_addr(struct stm32_i2c *adapter) {
	I2C_HandleTypeDef *i2c_handle = adapter->i2c_handle;

	return i2c_handle->Init.OwnAddress1;
}

static int stm32_i2c_rx(struct stm32_i2c *adapter, uint16_t addr,
		uint8_t *buf, size_t len) {
	I2C_HandleTypeDef *i2c_handle = adapter->i2c_handle;

	while (HAL_I2C_GetState(i2c_handle) != HAL_I2C_STATE_READY)
		;
	if (HAL_I2C_Master_Receive_IT(i2c_handle, addr, buf, len) != HAL_OK) {
		return -1;
	}

	while (HAL_I2C_GetState(i2c_handle) != HAL_I2C_STATE_READY)
		;
	return HAL_I2C_GetError(i2c_handle) == HAL_I2C_ERROR_AF ? -1 : len;
}

static int stm32_i2c_tx(struct stm32_i2c *adapter, uint16_t addr,
		uint8_t *buf, size_t len) {
	I2C_HandleTypeDef *i2c_handle = adapter->i2c_handle;

	while (HAL_I2C_GetState(i2c_handle) != HAL_I2C_STATE_READY)
		;
	if (HAL_I2C_Master_Transmit_IT(i2c_handle, addr, buf, len) != HAL_OK) {
		return -1;
	}
	while (HAL_I2C_GetState(i2c_handle) != HAL_I2C_STATE_READY)
		;
	return HAL_I2C_GetError(i2c_handle) == HAL_I2C_ERROR_AF ? -1 : len;
}

static irq_return_t i2c_ev_irq_handler(unsigned int irq_nr, void *data) {
	I2C_HandleTypeDef *i2c_handle = data;
	HAL_I2C_EV_IRQHandler(i2c_handle);
	return IRQ_HANDLED;
}

static irq_return_t i2c_er_irq_handler(unsigned int irq_nr, void *data) {
	I2C_HandleTypeDef *i2c_handle = data;
	HAL_I2C_ER_IRQHandler(i2c_handle);
	return IRQ_HANDLED;
}

int stm32_i2c_common_init(struct stm32_i2c *adapter) {
	int res = 0;
	I2C_TypeDef *i2c = adapter->i2c;
	I2C_HandleTypeDef *i2c_handle = adapter->i2c_handle;

	i2c_handle->Instance             = i2c;

	i2c_handle->Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
	i2c_handle->Init.ClockSpeed      = 400000;
	i2c_handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c_handle->Init.DutyCycle       = I2C_DUTYCYCLE_16_9;
	i2c_handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c_handle->Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

	res |= irq_attach(adapter->event_irq, i2c_ev_irq_handler, 0,
		i2c_handle, "I2C events");
	if (res < 0) {
		log_error("irq_attach failed\n");
		return -1;
	}
	res |= irq_attach(adapter->error_irq, i2c_er_irq_handler, 0,
		i2c_handle, "I2C errors");
	if (res < 0) {
		log_error("irq_attach failed\n");
		return -1;
	}
	return 0;
}

static int stm32_i2c_master_xfer(struct i2c_adapter *adapter, struct i2c_msg *msgs,
		int num) {
	struct stm32_i2c *stm32_adapter;
	int i;
	int res = -1;

	stm32_adapter = adapter->i2c_algo_data;
	assert(stm32_adapter);
	assert(stm32_adapter->i2c_handle);

	for (i = 0; i < num; i++) {
		if (stm32_i2c_current_slave_addr(stm32_adapter) != msgs->addr) {
			stm32_i2c_slave_select(stm32_adapter, msgs->addr);
		}

		if (msgs[i].flags & I2C_M_RD) {
			res = stm32_i2c_rx(stm32_adapter, msgs->addr, msgs->buf, msgs->len);
		} else {
			res = stm32_i2c_tx(stm32_adapter, msgs->addr, msgs->buf, msgs->len);
		}
	}

	return res;
}

const struct i2c_algorithm stm32_i2c_algo = {
	.i2c_master_xfer = stm32_i2c_master_xfer,
};
