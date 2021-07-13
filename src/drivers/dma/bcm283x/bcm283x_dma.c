/**
 * @file
 * @brief
 *
 * @date    07.06.2021
 * @author  kpishere
 */

#include <assert.h>
#include <kernel/printk.h>
#include <kernel/irq.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <drivers/dma/dma.h>
#include <module/embox/mem/bitmask.h>

#include <asm/cp15.h>
#include <kernel/printk.h>

#include <drivers/bcm283x_dma.h>
#include <drivers/mailbox/bcm2835_mailbox_property.h>

#define DMA_0_BASE OPTION_GET(NUMBER, dma0_base)
#define DMA_F_BASE OPTION_GET(NUMBER, dmaF_base)

#define DMA330_IRQ OPTION_GET(NUMBER,dma_interrupt_irq)

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

#define REGS_DMA(c) (( Dma *)( DMA_0_BASE + 0x100 * (c) ) )
#define REGS_DMA_F  (( Dma *)( DMA_F_BASE ))

#define REGS_DMA_STATUS ((volatile uint32_t *)( DMA_0_BASE + 0x0fe0))
#define REGS_DMA_ENABLE ((volatile uint32_t *)( DMA_0_BASE + 0x0ff0))

static volatile Dma_conbk shared_conbk; 

#if 0
static irq_return_t bcm283x_dma_irq_handler(unsigned int irq_num, void *dev_id) {
//	HAL_DMA_IRQHandler(&dma_handle);

	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(DMA330_IRQ, bcm283x_dma_irq_handler, NULL);
#endif


// Returns a handle to the allocated memory
//
uint32_t vc_malloc(size_t bytes, uint32_t align, uint32_t flags) {
    bcm2835_mailbox_property_t *resp;
    
    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_ALLOCATE_MEMORY,  bytes, align, flags );
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_ALLOCATE_MEMORY );
    uint32_t handle = (uint32_t)resp->data.value_32;

    return handle;
}

// Lock memory to physical address mapping, returns a bus address 
// accessable only by GPU and DMA (but corresponding physical address
// can now be accessed by ARM CPU)
//
uint32_t vc_mem_lock(uint32_t handle) {
    bcm2835_mailbox_property_t *resp;

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_LOCK_MEMORY,  handle );
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_LOCK_MEMORY );

    return resp->data.value_32;
}

// Unlock but don't de-allocate memory
// (can't be accessed by ARM CPU but still can by GPU and DMA)
//
int vc_mem_unlock(uint32_t handle) {
    bcm2835_mailbox_property_t *resp;

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_UNLOCK_MEMORY,  handle );
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_UNLOCK_MEMORY );

    return resp->data.value_32;
}

// Release the allocated memory
//
int vc_mem_free(uint32_t handle) {
    bcm2835_mailbox_property_t *resp;

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_RELEASE_MEMORY,  handle );
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_RELEASE_MEMORY );

    return resp->data.value_32;
}

// Allocate and lock memory for use by DMA
//
Dma_mem_handle *dma_malloc(size_t size)
{
    // Make `size` a multiple of PAGE_SIZE
    size = ((size + PAGE_SIZE() - 1) / PAGE_SIZE()) * PAGE_SIZE();

    Dma_mem_handle *mem = (Dma_mem_handle *)malloc(sizeof(Dma_mem_handle));
    mem->mb_handle = (uint32_t)vc_malloc(size, PAGE_SIZE(), BCM2835_MEM_FLAG_L1_NONALLOCATING);
    mem->bus_addr = vc_mem_lock(mem->mb_handle);
    mem->physical_addr = DMA_BUS_TO_PHYS(mem->bus_addr);
    mem->size = size;

    assert(mem->bus_addr != 0);
    return mem;
}

// Unlock and deallocate memory
//
void dma_free(Dma_mem_handle *mem)
{
    if (mem->physical_addr == NULL)
        return;

    vc_mem_unlock(mem->mb_handle);
    vc_mem_free(mem->mb_handle);
    mem->physical_addr = NULL;
    free(mem);
}

// Enable and reset DMA
int bcm283x_dma_config(int dma_chan) {
    assert(dma_chan <= DMA_CHANNELS);

    *(REGS_DMA_ENABLE) |= ( 1 << dma_chan );
    REGS_DMA(dma_chan)->cs = DMA_CS_RESET;
    return 0;
}

// Start DMA - basic memory copy
// dma_chan - integer 0-15
// dst, src - are destination and source memory locations in bus address space
// words - 4-byte aligned memory length
int bcm283x_dma_transfer(int dma_chan, uint32_t dst, uint32_t src, int words) {   
    assert(dma_chan <= DMA_CHANNELS);

    // A standard copy block to support basic implementation
    shared_conbk.source_ad = src;
    shared_conbk.dest_ad = dst;
    shared_conbk.txfr_len = words * sizeof(uint32_t);
    shared_conbk.ti = DMA_TI_SRC_INC | DMA_TI_DEST_INC;
    shared_conbk.nextconbk = 0;
    shared_conbk.stride = 0;
    shared_conbk.debug = 0;

    return bcm283x_dma_transfer_conbk(dma_chan, &shared_conbk);
}

// Start DMA
// dma_chan - integer 0-15
// conbk - pointer to data type in physical memory
int bcm283x_dma_transfer_conbk(int dma_chan, volatile Dma_conbk *conbk) {
    assert(dma_chan <= DMA_CHANNELS);

    REGS_DMA(dma_chan)->conblk_ad = (uint32_t)conbk;  
    REGS_DMA(dma_chan)->cs = DMA_CS_END;    // Clear 'end' flag
    REGS_DMA(dma_chan)->debug = DMA_DEBUG_READ_ERROR 
        | DMA_DEBUG_FIFO_ERROR 
        | DMA_DEBUG_READ_LAST_NOT_SET_ERROR; // clear error bits
    REGS_DMA(dma_chan)->cs = DMA_CS_ACTIVE; // Start DMA
    return 0;
}

int bcm283x_dma_in_progress(int dma_chan) {
    assert(dma_chan <= DMA_CHANNELS);

    return (REGS_DMA(dma_chan)->cs & DMA_CS_ACTIVE);
}

// Display DMA registers (for debugging)
void bcm283x_dma_disp(int dma_chan)
{
    assert(dma_chan <= DMA_CHANNELS);

    printk("\ncs(%d) %08X\n", dma_chan, REGS_DMA(dma_chan)->cs);
    printk("conblk_ad(%d) %08X\n", dma_chan, REGS_DMA(dma_chan)->conblk_ad);
    printk("ti(%d) %08X\n", dma_chan, REGS_DMA(dma_chan)->ti);
    printk("source_ad(%d) %08X\n", dma_chan, REGS_DMA(dma_chan)->source_ad);
    printk("dest_ad(%d) %08X\n", dma_chan, REGS_DMA(dma_chan)->dest_ad);
    printk("txfr_len(%d) %08X\n", dma_chan, REGS_DMA(dma_chan)->txfr_len);
    printk("stride(%d) %08X\n", dma_chan, REGS_DMA(dma_chan)->stride);
    printk("nextconbk(%d) %08X\n", dma_chan, REGS_DMA(dma_chan)->nextconbk);
    printk("debug(%d) %08X\n", dma_chan, REGS_DMA(dma_chan)->debug);
}

static int bcm283x_dma_init(void) {
	int ret = 0;
    
#if 0
	ret = irq_attach(DMA330_IRQ, bcm283x_dma_irq_handler,
			0, NULL, "dma330_all");
	if (ret < 0) {
		log_error("irq_attach failed\n");
	}
#endif
	return ret;
}

EMBOX_UNIT_INIT(bcm283x_dma_init);
