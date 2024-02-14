/**
 * @file spi.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 03.12.2018
 */

#ifndef DRIVERS_SPI_H_
#define DRIVERS_SPI_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/char_dev.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <lib/libds/array.h>
#include <util/macro.h>
/* #include <drivers/dma/dma.h> */

#include <config/embox/driver/spi/core.h>

#define SPI_REGISTRY_SZ \
	OPTION_MODULE_GET(embox__driver__spi__core, NUMBER, spi_quantity)

struct spi_ops;

/* Host modes */
enum spi_mode_t {
	/* It could be just SPI_MODE_MASTER, but
	 * it conflicts with STM32 macros */
	SPI_MODE_T_MASTER,
	SPI_MODE_T_SLAVE,
};

enum spi_pol_phase_t {
	SPI_MODE_0 = 0x00, /* CPOL = 0, CPHA = 0 */
	SPI_MODE_1 = 0x01, /* CPOL = 0, CPHA = 1 */
	SPI_MODE_2 = 0x02, /* CPOL = 1, CPHA = 0 */
	SPI_MODE_3 = 0x03, /* CPOL = 1, CPHA = 1 */
};

struct spi_device;

typedef void (*irq_spi_event_t)(struct spi_device *data, int cnt);

struct spi_device {
	struct dev_module *dev;

	uint32_t flags;
	bool is_master;
	int bits_per_word;

	struct spi_ops *spi_ops;
	void *priv;
};

struct dma_ctrl_blk;
struct dma_mem_handle;
struct spi_ops {
	int (*init)(struct spi_device *dev);
	int (*select)(struct spi_device *dev, int cs);
	int (*set_mode)(struct spi_device *dev, bool is_master);
	int (*transfer)(struct spi_device *dev, uint8_t *in, uint8_t *out, int cnt);
};

extern struct spi_device *spi_dev_by_id(int id);
extern int spi_dev_id(struct spi_device *dev);

/* In polling mode: 
 * - either of *in or *out buffers can be set, cnt is zero to positive
 *
 * In interrupt mode:  
 * - call with cnt < 0, set to -N bytes to write out. Interrupts will fire
 *   until N bytes are written out to make cnt == 0
 * - within interrupt handlers, function can be called as expected in polling mode
 *   where cnt is positive.
 * 
 *   NB For BCM283X implementation: 
 *   - count must not be larger than 16 bytes for sending within interrupt call 
 *   - takes 12 bytes received to trigger interrupt
 */
extern int spi_transfer(struct spi_device *dev, uint8_t *in, uint8_t *out,
    int cnt);

/* Set extra options in struct *dev before calling this function 
 */
extern int spi_select(struct spi_device *dev, int cs);
extern int spi_set_master_mode(struct spi_device *dev);
extern int spi_set_slave_mode(struct spi_device *dev);

extern const struct idesc_ops spi_iops;

/* Note: if you get linker error like "redefinition of 'spi_device0'"
 * then you should reconfig system so SPI bus indecies do not overlap */
#define SPI_DEV_DEF(dev_name, spi_dev_ops, dev_priv, idx)                   \
	struct spi_device MACRO_CONCAT(spi_device, idx);                        \
	CHAR_DEV_DEF(dev_name, &spi_iops, &MACRO_CONCAT(spi_device, idx));      \
	struct spi_device MACRO_CONCAT(spi_device, idx) = {                     \
	    .spi_ops = spi_dev_ops,                                             \
	    .priv = dev_priv,                                                   \
	    .dev = (void *)MACRO_CONCAT(__char_device_registry_ptr_, dev_name), \
	}

/* IOCTL-related stuff */
#define SPI_IOCTL_CS       0x1
#define SPI_IOCTL_TRANSFER 0x2
#define SPI_IOCTL_CS_MODE  0x3

struct spi_transfer_arg {
	uint8_t *in, *out;
	ssize_t count;
};

/* CS modes */
#define SPI_CS_ACTIVE    (1 << 0)
#define SPI_CS_INACTIVE  (1 << 1)
/* x is one of enum spi_pol_phase_t */
#define SPI_CS_MODE(x)   ((0x03 & (x)) << 2)
#define SPI_CS_IRQD      (1 << 4)
#define SPI_CS_IRQR      (1 << 5)
#define SPI_CS_DMAEN     (1 << 6)
/* Upper 16 bits used to set clock divisor */
#define SPI_CS_DIVSOR(x) (((x)&0xFFFF) << 16)

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

#endif /* DRIVERS_SPI_H_ */
