/**
 * @file stm32.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 25.12.2019
 */

#include <assert.h>
#include <string.h>

#include "stm32_spi.h"

#include <drivers/spi.h>
#include <kernel/irq.h>
#include <util/log.h>

#include <drivers/gpio/gpio.h>
#include <drivers/gpio/gpio_driver.h>

static int stm32_spi_setup(struct stm32_spi *dev, void *instance, bool is_master) {
	SPI_HandleTypeDef *handle = &dev->handle;

	memset(handle, 0, sizeof(*handle));

	handle->Instance               = instance;
	handle->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	handle->Init.Direction         = SPI_DIRECTION_2LINES;
	handle->Init.CLKPhase          = SPI_PHASE_1EDGE;
	handle->Init.CLKPolarity       = SPI_POLARITY_LOW;
	handle->Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	handle->Init.CRCPolynomial     = 7;
	handle->Init.DataSize          = SPI_DATASIZE_8BIT;
	handle->Init.FirstBit          = SPI_FIRSTBIT_MSB;
	handle->Init.NSS               = SPI_NSS_SOFT;
	handle->Init.TIMode            = SPI_TIMODE_DISABLE;

	if (is_master) {
		handle->Init.Mode      = SPI_MODE_MASTER;
	} else {
		handle->Init.Mode      = SPI_MODE_SLAVE;
	}

	if (HAL_OK != HAL_SPI_Init(handle)) {
		log_error("Failed to init SPI!");
		return -1;
	}

	return 0;
}

int stm32_spi_init(struct stm32_spi *dev, void *instance) {
	return stm32_spi_setup(dev, instance, true);
}

static int stm32_spi_select(struct spi_device *dev, int cs) {
	log_debug("NIY");

	return 0;
}

static int stm32_spi_set_mode(struct spi_device *dev, bool is_master) {
	struct stm32_spi *s = dev->priv;
	SPI_HandleTypeDef *handle = &s->handle;
	void *instance = handle->Instance;

	HAL_SPI_DeInit(handle);

	return stm32_spi_setup(s, instance, is_master);
}

static int stm32_spi_transfer(struct spi_device *dev, uint8_t *inbuf,
		uint8_t *outbuf, int count) {
	int ret;
	struct stm32_spi *priv = dev->priv;
	SPI_HandleTypeDef *handle = &priv->handle;

	if (dev->flags & SPI_CS_ACTIVE && dev->is_master) {
		/* Note: we suppose that there's a single slave device
		 * on the SPI bus, so we lower the same pin all the tiem */
		HAL_GPIO_WritePin(priv->nss_port, priv->nss_pin, GPIO_PIN_RESET);
	}

	ret = HAL_SPI_TransmitReceive(handle, inbuf, outbuf, count, 5000);
	if (ret != HAL_OK) {
		log_error("fail %d", ret);
	}

	while (HAL_SPI_GetState(handle) != HAL_SPI_STATE_READY) {
	}

	if (dev->flags & SPI_CS_INACTIVE && dev->is_master) {
		/* Note: we suppose that there's a single slave device
		 * on the SPI bus, so we raise the same pin all the tiem */
		HAL_GPIO_WritePin(priv->nss_port, priv->nss_pin, GPIO_PIN_SET);
	}

	return 0;
}

struct spi_ops stm32_spi_ops = {
	.select   = stm32_spi_select,
	.set_mode = stm32_spi_set_mode,
	.transfer = stm32_spi_transfer
};
