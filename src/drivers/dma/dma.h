/**
 * @file
 * @brief
 *
 * @date    26.07.2020, 2021.08.05
 * @author  Alexander Kalmuk, kpishere
 */

#ifndef DRIVERS_DMA_DMA_H_
#define DRIVERS_DMA_DMA_H_

#include <stdint.h>
#include <stdlib.h>
#include <kernel/irq.h>

typedef struct {
    void *physical_addr; // Physical (ARM direct) base address of the page
    uint32_t bus_addr;  // Bus adress of the page, this is not a pointer because it does not point to valid physical address
    uint32_t mb_handle; // Used by mailbox property interface
    uint32_t size;
} Dma_mem_handle;

/* txfr_len :  Generally, use is This specifies the amount of data to be transferred in bytes.
 * In normal (non 2D) mode this specifies the amount of bytes to be transferred.
 * In 2D mode it is interpreted as an X and a Y length, and the DMA will perform Y transfers, 
 * each of length X bytes and add the strides onto the addresses after each X leg of the transfer.
 * The length register is updated by the DMA engine as the transfer progresses, so it will indicate 
 * the data left to transfer.
 */

// DMA PL330 Device register 
typedef struct {
    volatile uint32_t cs;
    volatile uint32_t conblk_ad;
    volatile uint32_t ti;
    volatile uint32_t source_ad;
    volatile uint32_t dest_ad;
    volatile uint32_t txfr_len;     
    volatile uint32_t stride;
    volatile uint32_t nextconbk;
    volatile uint32_t debug;
} Dma;

// DMA PL330 Control block
typedef struct {
    volatile uint32_t ti;
    volatile uint32_t source_ad;
    volatile uint32_t dest_ad; 
    volatile uint32_t txfr_len;
    volatile uint32_t stride;
    volatile uint32_t nextconbk;
    volatile uint32_t debug[2];
} Dma_conbk;

struct dma_ops {
    int (*config_extended)(int dma_chan, irq_handler_t irqhandler, uint32_t cs_panic_opts);
    int (*transfer)(int dma_chan, uint32_t dst, uint32_t src, int words);
    int (*transfer_conbk)(int dma_chan, volatile Dma_conbk *conbk);
    int (*in_progress_status)(int dma_chan, uint32_t *error_flags);

    // Some DMA engines require memory allocated by specific mechanism to function reliably
    Dma_mem_handle *(*malloc)(size_t size);
    void (*free)(Dma_mem_handle *mem);

    uint32_t (*channels_free)(void);
};

extern struct dma_ops dma_dev;

extern int dma_config_extended(int dma_chan, irq_handler_t irqhandler, uint32_t cs_panic_opts);
extern int dma_transfer_conbk(int dma_chan, volatile Dma_conbk *conbk);
extern int dma_in_progress_status(int dma_chan, uint32_t *error_flags);

extern int dma_config(int dma_chan);
extern int dma_transfer(int dma_chan, uint32_t dst, uint32_t src, int words);
inline int dma_in_progress(int dma_chan) { return dma_in_progress_status(dma_chan, NULL); };

extern Dma_mem_handle *dma_malloc(size_t size);
extern void dma_free(Dma_mem_handle *mem);

extern uint32_t dma_channels_free(void);

#endif /* DRIVERS_DMA_DMA_H_ */
