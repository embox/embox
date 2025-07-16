/**
 * @file
 *
 * @date Jun 20, 2023
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_SPI_BCM283X_BCM283X_SPI_DEV_H_
#define SRC_DRIVERS_SPI_BCM283X_BCM283X_SPI_DEV_H_


#include <stdint.h>

#include <drivers/spi.h>

struct bcm283x_spi_regs;

struct dma_mem_handle;
typedef struct dma_ctrl_blk *(*init_dma_block_spi_func_t) (
		struct spi_controller *dev,
		struct dma_mem_handle *mem_handle, uint32_t offset, void *src, uint32_t bytes,
		struct dma_ctrl_blk *next_conbk, bool int_enable);

struct bcm283x_spi_dev {
	struct bcm283x_spi_regs *regs;

	irq_spi_event_t send_complete;
	irq_spi_event_t received_data;
	irq_handler_t dma_complete;
	uint8_t *in;
	uint8_t *out;
	int      count;
	int      dma_chan_out;
	int      dma_chan_in;
	uint32_t dma_levels;

	init_dma_block_spi_func_t init_dma_block_spi_in;
	init_dma_block_spi_func_t init_dma_block_spi_out;
};


#endif /* SRC_DRIVERS_SPI_BCM283X_BCM283X_SPI_DEV_H_ */
