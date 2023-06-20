/**
 * @file
 *
 * @date Jun 19, 2023
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <stdbool.h>

#include <drivers/dma/dma.h>

#include <drivers/spi.h>

#include "bcm283x_spi_dev.h"

struct dma_ctrl_blk *init_dma_block_spi_in(struct spi_device *dev, struct dma_mem_handle *mem_handle,
		uint32_t offset, void *src, uint32_t bytes,
		struct dma_ctrl_blk *next_conbk, bool int_enable) {
	struct bcm283x_spi_dev *bcm283x_spi_dev;

	assert(dev);
	assert(dev->spi_ops);
	bcm283x_spi_dev = dev->priv;

	if(bcm283x_spi_dev->init_dma_block_spi_in == NULL) {
		log_debug("Select operation is not supported for SPI%d",
				spi_dev_id(dev));
		return NULL;
	}
	return bcm283x_spi_dev->init_dma_block_spi_in(dev, mem_handle, offset, src,bytes, next_conbk, int_enable);
}

struct dma_ctrl_blk *init_dma_block_spi_out(struct spi_device *dev, struct dma_mem_handle *mem_handle,
		uint32_t offset, void *dest, uint32_t bytes,
		struct dma_ctrl_blk *next_conbk, bool int_enable) {
	struct bcm283x_spi_dev *bcm283x_spi_dev;

	assert(dev);
	assert(dev->spi_ops);
	bcm283x_spi_dev = dev->priv;

	if(bcm283x_spi_dev->init_dma_block_spi_out == NULL) {
		log_debug("Select operation is not supported for SPI%d",
				spi_dev_id(dev));
		return NULL;
	}
	return bcm283x_spi_dev->init_dma_block_spi_out(dev, mem_handle, offset, dest, bytes, next_conbk, int_enable);
}
