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

struct dma_ctrl_blk *init_dma_block_spi_in(struct spi_controller *dev, struct dma_mem_handle *mem_handle,
		uint32_t offset, void *src, uint32_t bytes,
		struct dma_ctrl_blk *next_conbk, bool int_enable) {
	struct bcm283x_spi_dev *bcm283x_spi_dev;

	assert(dev);
	assert(dev->spi_ops);
	bcm283x_spi_dev = dev->priv;

	if(bcm283x_spi_dev->init_dma_block_spi_in == NULL) {
		log_debug("Select operation is not supported for SPI%d", dev->spic_bus_num);
		return NULL;
	}
	return bcm283x_spi_dev->init_dma_block_spi_in(dev, mem_handle, offset, src,bytes, next_conbk, int_enable);
}

struct dma_ctrl_blk *init_dma_block_spi_out(struct spi_controller *dev, struct dma_mem_handle *mem_handle,
		uint32_t offset, void *dest, uint32_t bytes,
		struct dma_ctrl_blk *next_conbk, bool int_enable) {
	struct bcm283x_spi_dev *bcm283x_spi_dev;

	assert(dev);
	assert(dev->spi_ops);
	bcm283x_spi_dev = dev->priv;

	if(bcm283x_spi_dev->init_dma_block_spi_out == NULL) {
		log_debug("Select operation is not supported for SPI%d", dev->spic_bus_num);
		return NULL;
	}
	return bcm283x_spi_dev->init_dma_block_spi_out(dev, mem_handle, offset, dest, bytes, next_conbk, int_enable);
}

int spi_dma_prepare(struct spi_controller *dev,
		irq_return_t (*dma_complete)(unsigned int,  void *),
		int dma_chan_out, int dma_chan_in,
		uint32_t dma_levels) {
	struct bcm283x_spi_dev *bcm283x_spi_dev;

	assert(dev);

	bcm283x_spi_dev = dev->priv;

	bcm283x_spi_dev->dma_complete = dma_complete;
	bcm283x_spi_dev->dma_chan_out = dma_chan_out;
	bcm283x_spi_dev->dma_chan_in = dma_chan_in;
	bcm283x_spi_dev->dma_levels = dma_levels;

	return 0;
}

int spi_irq_prepare(struct spi_controller *dev,
		irq_spi_event_t send_complete, irq_spi_event_t received_data) {
	struct bcm283x_spi_dev *bcm283x_spi_dev;

	assert(dev);

	bcm283x_spi_dev = dev->priv;

	bcm283x_spi_dev->send_complete = send_complete;
	bcm283x_spi_dev->received_data = received_data;

	return 0;
}
