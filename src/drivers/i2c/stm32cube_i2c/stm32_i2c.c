/**
 * @file
 * @brief I2C driver for STM32
 *
 * @date    10.12.2018
 * @author  Alexander Kalmuk
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/i2c/i2c.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <util/log.h>

#include "stm32_i2c.h"

#define I2C_XMIT_TIMEOUT 1000

/* FIXME from cube B-L475E-IOT01/stm32l475e_iot01.h */
#ifndef DISCOVERY_I2Cx_TIMING
#define DISCOVERY_I2Cx_TIMING ((uint32_t)0x00702681)
#endif /* DISCOVERY_I2Cx_TIMING */

static int stm32_i2c_slave_select(struct stm32_i2c_priv *priv, int slave_addr) {
	I2C_HandleTypeDef *i2c_handle = priv->i2c_handle;

	i2c_handle->Init.OwnAddress1 = slave_addr;
	i2c_handle->Init.OwnAddress2 = 0;

	if (HAL_I2C_Init(i2c_handle) != HAL_OK) {
		log_error("HAL_I2C_Init failed\n");
		return -1;
	}

	return 0;
}

static int stm32_i2c_current_slave_addr(struct stm32_i2c_priv *priv) {
	I2C_HandleTypeDef *i2c_handle = priv->i2c_handle;

	return i2c_handle->Init.OwnAddress1;
}

static int stm32_i2c_rx(struct stm32_i2c_priv *priv, uint16_t addr,
    uint8_t *buf, size_t len) {
	I2C_HandleTypeDef *i2c_handle = priv->i2c_handle;

	while (HAL_I2C_GetState(i2c_handle) != HAL_I2C_STATE_READY) {}

	/**
	 * I2C address _must_ be shifted as it stated in
	 * Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c:
	 *
	 * @param  DevAddress Target device address: The device 7 bits address value
	 *      in datasheet must be shift at right before call interface
	 */
#if USE_I2C_IRQ
	if (HAL_I2C_Master_Receive_IT(i2c_handle, addr << 1, buf, len) != HAL_OK) {
		return -1;
	}
#else
	if (HAL_I2C_Master_Receive(i2c_handle, addr << 1, buf, len, I2C_XMIT_TIMEOUT)
	    != HAL_OK) {
		return -1;
	}
#endif

	while (HAL_I2C_GetState(i2c_handle) != HAL_I2C_STATE_READY)
		;
	return HAL_I2C_GetError(i2c_handle) == HAL_I2C_ERROR_AF ? -1 : len;
}

static int stm32_i2c_tx(struct stm32_i2c_priv *priv, uint16_t addr,
    uint8_t *buf, size_t len) {
	I2C_HandleTypeDef *i2c_handle = priv->i2c_handle;

	while (HAL_I2C_GetState(i2c_handle) != HAL_I2C_STATE_READY)
		;

		/*
	 * I2C address _must_ be shifted as it stated in
	 * Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c:
	 *
	 * @param  DevAddress Target device address: The device 7 bits address value
	 *      in datasheet must be shift at right before call interface
	 */
#if USE_I2C_IRQ
	if (HAL_I2C_Master_Transmit_IT(i2c_handle, addr << 1, buf, len) != HAL_OK) {
		return -1;
	}
#else
	if (HAL_I2C_Master_Transmit(i2c_handle, addr << 1, buf, len, I2C_XMIT_TIMEOUT)
	    != HAL_OK) {
		return -1;
	}
#endif
	while (HAL_I2C_GetState(i2c_handle) != HAL_I2C_STATE_READY)
		;
	return HAL_I2C_GetError(i2c_handle) == HAL_I2C_ERROR_AF ? -1 : len;
}

#if USE_I2C_IRQ
irq_return_t i2c_ev_irq_handler(unsigned int irq_nr, void *data) {
	I2C_HandleTypeDef *i2c_handle = data;
	HAL_I2C_EV_IRQHandler(i2c_handle);
	return IRQ_HANDLED;
}

irq_return_t i2c_er_irq_handler(unsigned int irq_nr, void *data) {
	I2C_HandleTypeDef *i2c_handle = data;
	HAL_I2C_ER_IRQHandler(i2c_handle);
	return IRQ_HANDLED;
}
#endif

static int stm32_i2c_irq_attach(struct stm32_i2c_priv *priv,
    I2C_HandleTypeDef *i2c_handle) {
#if USE_I2C_IRQ
	int res = 0;

	res |= irq_attach(priv->event_irq, i2c_ev_irq_handler, 0, i2c_handle,
	    "I2C events");
	if (res < 0) {
		log_error("irq_attach failed\n");
		return -1;
	}
	res |= irq_attach(priv->error_irq, i2c_er_irq_handler, 0, i2c_handle,
	    "I2C errors");
	if (res < 0) {
		log_error("irq_attach failed\n");
		return -1;
	}
#endif
	return 0;
}

static int stm32_i2c_init(const struct i2c_bus *bus) {
	struct stm32_i2c_priv *priv;
	I2C_HandleTypeDef *i2c_handle;

	priv = bus->i2c_priv;
	i2c_handle = priv->i2c_handle;

	i2c_handle->Instance = priv->i2c;
	i2c_handle->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	i2c_handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c_handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c_handle->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
#if defined(STM32F4_CUBE) || defined(STM32F2_CUBE) || defined(STM32L1_CUBE)
	i2c_handle->Init.ClockSpeed = 400000;
	i2c_handle->Init.DutyCycle = I2C_DUTYCYCLE_16_9;
#elif defined(STM32L4_CUBE)
	i2c_handle->Init.Timing = DISCOVERY_I2Cx_TIMING;
	i2c_handle->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
#else
#error Unsupported platform
#endif

	if (priv->init_pins) {
		priv->init_pins();
	}

	return stm32_i2c_irq_attach(priv, i2c_handle);
}

static int stm32_i2c_master_xfer(const struct i2c_bus *bus,
    struct i2c_msg *msgs, size_t num_msgs) {
	struct stm32_i2c_priv *priv;
	int i;
	int res = -1;

	priv = bus->i2c_priv;
	assert(priv);
	assert(priv->i2c_handle);

	for (i = 0; i < num_msgs; i++) {
		if (stm32_i2c_current_slave_addr(priv) != msgs->addr) {
			stm32_i2c_slave_select(priv, msgs->addr);
		}

		if (msgs[i].flags & I2C_M_RD) {
			res = stm32_i2c_rx(priv, msgs->addr, msgs->buf, msgs->len);
		}
		else {
			res = stm32_i2c_tx(priv, msgs->addr, msgs->buf, msgs->len);
		}
	}

	return res;
}

const struct i2c_ops stm32_i2c_ops = {
    .i2c_master_xfer = stm32_i2c_master_xfer,
    .i2c_init = stm32_i2c_init,
};
