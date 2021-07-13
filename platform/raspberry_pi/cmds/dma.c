#include <drivers/bcm283x_dma.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mem/sysmalloc.h>
#include <assert.h>

#define MEM_SIZE 1024
#define CPY_BLK 256
#define DMA_CHAN 5

void fill_test(uint32_t *memLoc, uint32_t bytes) {
    uint32_t pattern = 0xDEADBEEF;
    for(uint32_t i = 0 ; i < bytes / sizeof(uint32_t) ; i+= 2 ) {
        memLoc[i] = pattern;
        memLoc[i+1] = i;
    }
}


// DMA memory-to-memory test
int dma_test_mem_transfer(Dma_mem_handle *mem)
{
    Dma_conbk *cbp = mem->physical_addr;
    // Pointers for DMA 
    uint32_t srce = ((uint32_t)(mem->bus_addr)+sizeof(Dma_conbk));
    uint32_t dest = srce + CPY_BLK;
 
    // ARM uses physical addr pointer to same memory
    memset((void *)cbp, 0, MEM_SIZE);   // clear all working memory
    fill_test(DMA_BUS_TO_PHYS(srce),CPY_BLK);    // set values to copy

    // DMA will use bus address values, were writing to memory with the physical address pointer
    cbp->ti = DMA_TI_SRC_INC | DMA_TI_DEST_INC;
    cbp->source_ad = srce;
    cbp->dest_ad = dest;
    cbp->txfr_len = CPY_BLK;
    bcm283x_dma_transfer_conbk(DMA_CHAN, cbp);

    while(bcm283x_dma_in_progress(DMA_CHAN)) {
#if 1
        bcm283x_dma_disp(DMA_CHAN);
#endif
    }

    return memcmp(DMA_BUS_TO_PHYS(srce),DMA_BUS_TO_PHYS(dest),CPY_BLK);
}


int main(int argc, char **argv) {
    int res = -1;

    bcm283x_dma_config(DMA_CHAN);
    Dma_mem_handle *mem_handle = dma_malloc(MEM_SIZE);
    if(mem_handle != 0x00) res = dma_test_mem_transfer(mem_handle);
    if(res == 0) {
        printf("\n * Success *\n");
    } else {
        printf("\n * Fail *\n");
    }
    dma_free(mem_handle);
	return 0;
}

