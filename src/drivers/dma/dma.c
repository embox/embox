/**
 * @file
 * @brief
 *
 * @date    2021.08.05
 * @author  kpishere
 */
#include <drivers/dma/dma.h>
#include <assert.h>
#include <string.h>

struct dma_ops dma_dev  __attribute__ ((aligned (0x20)));

int dma_config(int dma_chan) { return dma_config_extended(dma_chan, NULL, 0x00); };

int dma_config_extended(int dma_chan, irq_handler_t irqhandler, uint32_t cs_panic_opts) {
    assert(dma_dev.config_extended != NULL);

    return dma_dev.config_extended(dma_chan, irqhandler, cs_panic_opts);
}

int dma_transfer(int dma_chan, uint32_t dst, uint32_t src, int words) {
    assert(dma_dev.transfer != NULL);

    return dma_dev.transfer(dma_chan, dst, src, words);
}

int dma_transfer_conbk(int dma_chan, volatile Dma_conbk *conbk) {
    assert(dma_dev.transfer_conbk != NULL);

    return dma_dev.transfer_conbk(dma_chan,conbk);
}

int dma_in_progress_status(int dma_chan, uint32_t *error_flags) {
    assert(dma_dev.in_progress_status != NULL);

    return dma_dev.in_progress_status(dma_chan, error_flags);
}

// Default implementation is to just use malloc
Dma_mem_handle *dma_malloc(size_t size) {
    if(dma_dev.malloc == NULL) {
        Dma_mem_handle *memh = malloc(size + sizeof(Dma_mem_handle));
        memh->physical_addr = (void *)( memh + sizeof(Dma_mem_handle) );
        memh->bus_addr = (uintptr_t)memh->physical_addr;
        memh->mb_handle = memh->bus_addr;
        memh->size = size;        
        return memh;
    }
    return dma_dev.malloc(size);
}
// Default implementation is to just use free
void dma_free(Dma_mem_handle *mem) {
    if(dma_dev.free == NULL) {
        if(mem->physical_addr != NULL) {
            free(mem);
            mem->physical_addr = NULL;
        }
    } else {
        dma_dev.free(mem);
    }     
}

extern uint32_t dma_channels_free(void) {
    assert(dma_dev.channels_free != NULL);

    return dma_dev.channels_free();
}
