#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mem/sysmalloc.h>
#include <assert.h>

#include <drivers/dma/dma.h>

#define MEM_SIZE 1024
#define CPY_BLK 256
#define DMA_CHAN 5

static void fill_test(uint32_t *memLoc, uint32_t bytes) {
    uint32_t pattern = 0xDEADBEEF;
    for(uint32_t i = 0 ; i < bytes / sizeof(uint32_t) ; i+= 2 ) {
        memLoc[i] = pattern;
        memLoc[i+1] = i;
    }
}

// DMA memory-to-memory test
static int dma_test_mem_transfer(Dma_mem_handle *mem)
{
	struct dma_ctrl_blk *cbp = mem->physical_addr;
    // Pointers for DMA 
    uint32_t srce = ((uint32_t)(mem->physical_addr)+sizeof(struct dma_ctrl_blk));
    uint32_t dest = srce + CPY_BLK;

    // ARM uses physical addr pointer to same memory
    memset((void *)cbp, 0, MEM_SIZE);   // clear all working memory
    fill_test((uint32_t *)srce,CPY_BLK);    // set values to copy

    dma_transfer(DMA_CHAN, dest, srce, CPY_BLK / sizeof(uint32_t));

    uint32_t error_flags;
    while(dma_in_progress_status(DMA_CHAN, &error_flags));

    return memcmp((uint32_t *)srce,(uint32_t *)dest,CPY_BLK);
}


int main(int argc, char **argv) {
    int res = -1;

    uint32_t channel_bitfield = dma_channels_free();
    
    printf("\nDMA Channels available: ");
    for(int i=0; i<32; i++) {
        if( ( channel_bitfield >> i) & 0x01 ) printf(" %d", i); 
    }
    printf("\n");

    dma_config(DMA_CHAN);
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

