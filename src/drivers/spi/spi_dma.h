/**
 * @file spi.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 03.12.2018
 */

#ifndef DRIVERS_SPI_DMA_H_
#define DRIVERS_SPI_DMA_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <kernel/irq.h>

#include <util/macro.h>


struct spi_device;

typedef void (*irq_spi_event_t)(struct spi_device *data, int cnt);

struct dma_ctrl_blk;
struct dma_mem_handle;


/* DMA Levels 
 * rcPanic: DMA Read Panic Threshold. Generate the Panic signal to 
 * 		the RX DMA engine whenever the RX FIFO level is greater than this amount. 
 * rdReq:  DMA Read Request Threshold. Generate A DREQ to the RX DMA engine 
 * 		whenever the RX FIFO level is greater than this amount, (RX DREQ 
 * 		is also generated if the transfer has finished but the RXFIFO isn t empty).
 * wrPanic: DMA Write Panic Threshold. Generate the Panic signal to the TX DMA 
 * 		engine whenever the TX FIFO level is less than or equal to this amount
 * wrReq: DMA Write Request Threshold. Generate a DREQ signal to the TX DMA 
 * 		engine whenever the TX FIFO level is less than or equal to this amount.
 */
#define DMA_LEVELS(rdPanic, rdReq, wrPanic, wrReq)                         \
	((rdPanic & 0xFF) << 24 | (rdReq & 0xFF) << 16 | (wrPanic & 0xff) << 8 \
	    | (wrReq & 0xff))

/* Initializes a control block in the dma allocated memory with sensible default values
 * for a single block transfer to send.
 *
 * @param dev - structure returned from spi_dev_by_id()
 * @param mem_handle - memory allocated from dma_malloc()
 * @param offset - offset into allocated mem_handle memory in which to initialize a control block
 * @param src - pointer to source physical memory location 
 * @param bytes - bytes to copy from src pointer being first byte
 * @param next_conbk - pointer to next conbk, null if this is last one 
 * @param int_enable - enable interrupt at end of transmission/receipt for conbk
 */
extern struct dma_ctrl_blk *init_dma_block_spi_in(struct spi_device *dev,
    struct dma_mem_handle *mem_handle, uint32_t offset, void *src,
    uint32_t bytes, struct dma_ctrl_blk *next_conbk, bool int_enable);

/* Initializes a control block in the dma allocated memory with sensible default values
 * for a single block transfer to receive to.
 *
 * @param dev - structure returned from spi_dev_by_id()
 * @param mem_handle - memory allocated from dma_malloc()
 * @param offset - offset into allocated mem_handle memory in which to initialize a control block
 * @param dest - pointer to source physical memory location 
 * @param bytes - bytes to copy to dest pointer being first byte
 * @param next_conbk - pointer to next conbk, null if this is last one 
 * @param int_enable - enable interrupt at end of transmission/receipt for conbk
 */
extern struct dma_ctrl_blk *init_dma_block_spi_out(struct spi_device *dev,
    struct dma_mem_handle *mem_handle, uint32_t offset, void *dest,
    uint32_t bytes, struct dma_ctrl_blk *next_conbk, bool int_enable);

extern int spi_dma_prepare(struct spi_device *dev,
    irq_return_t (*dma_complete)(unsigned int, void *), int dma_chan_out,
    int dma_chan_in, uint32_t dma_levels);

extern int spi_irq_prepare(struct spi_device *dev,
    irq_spi_event_t send_complete, irq_spi_event_t received_data);

#endif /* DRIVERS_SPI_DMA_H_ */
