/**
 * @file
 * @brief
 *
 * @date    07.06.2021
 * @author  kpishere
 */

#include <assert.h>
#include <util/log.h>
#include <kernel/irq.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <module/embox/mem/bitmask.h>

#include <drivers/dma/dma.h>
#include <drivers/dma/pl330_dma.h>
#include <drivers/mailbox/bcm2835_mailbox_property.h>

#define DMA_0_BASE OPTION_GET(NUMBER, dma0_base)
#define DMA_F_BASE OPTION_GET(NUMBER, dmaF_base)

#define DMA330_IRQ OPTION_GET(NUMBER,dma_interrupt_irq)

#define REGS_DMA(c) (( Dma *)( DMA_0_BASE + 0x100 * (c) ) )
#define REGS_DMA_F  (( Dma *)( DMA_F_BASE ))
#define REGS_DMA_CHAN(r) ( (((uint32_t)(r))-DMA_0_BASE) / 0x100)

#define REGS_DMA_STATUS ((volatile uint32_t *)( DMA_0_BASE + 0x0fe0))
#define REGS_DMA_ENABLE ((volatile uint32_t *)( DMA_0_BASE + 0x0ff0))

static volatile struct dma_ctrl_blk shared_conbk;

// Returns a handle to the allocated memory
//
static uint32_t pl330_vc_malloc(size_t bytes, uint32_t align, uint32_t flags) {
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
static uint32_t pl330_vc_mem_lock(uint32_t handle) {
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
static int pl330_vc_mem_unlock(uint32_t handle) {
    bcm2835_mailbox_property_t *resp;

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_UNLOCK_MEMORY,  handle );
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_UNLOCK_MEMORY );

    return resp->data.value_32;
}

// Release the allocated memory
//
static int pl330_vc_mem_free(uint32_t handle) {
    bcm2835_mailbox_property_t *resp;

    bcm2835_property_init();
    bcm2835_property_add_tag( TAG_RELEASE_MEMORY,  handle );
    bcm2835_property_process();
    resp = bcm2835_property_get( TAG_RELEASE_MEMORY );

    return resp->data.value_32;
}

/* IRQ 16 / DMA 0, 17/1, 18/2, 19/3, 20/4, 21/5
 *   22/6, 23/7, 24/8, 25/9, 26/A
 * - IRQ number 27 is shared by channels B thru E 
 */
static uint8_t get_irq_from_channel(uint8_t ch) {
    uint8_t irq = ch + 16;

    if(ch > 0x0A) irq = 27;
    return irq;
}

// Allocate and lock memory for use by DMA
//
struct dma_mem_handle *pl330_dma_malloc(size_t size)
{
    // Make `size` a multiple of PAGE_SIZE
    size = ((size + PAGE_SIZE() - 1) / PAGE_SIZE()) * PAGE_SIZE();

    Dma_mem_handle *mem = (struct dma_mem_handle  *)malloc(sizeof(struct dma_mem_handle ));
    mem->mb_handle = (uint32_t)pl330_vc_malloc(size, PAGE_SIZE(), BCM2835_MEM_FLAG_DIRECT);
    mem->bus_addr = pl330_vc_mem_lock(mem->mb_handle);
    mem->physical_addr = DMA_BUS_TO_PHYS(mem->bus_addr);
    mem->size = size;

    assert(mem->bus_addr != 0);
    return mem;
}

// Unlock and deallocate memory
//
void pl330_dma_free(struct dma_mem_handle  *mem)
{
    if (mem->physical_addr == NULL)
        return;

    pl330_vc_mem_unlock(mem->mb_handle);
    pl330_vc_mem_free(mem->mb_handle);
    mem->physical_addr = NULL;
    free(mem);
}

// Enable and reset DMA
int pl330_dma_config(int dma_chan, irq_handler_t irqhandler, uint32_t cs_panic_opts) {
    assert(dma_chan <= DMA_CHANNELS);
    int res = 0, irq = get_irq_from_channel(dma_chan);
    Dma *dev = REGS_DMA(dma_chan);

    if ( irqhandler == NULL && irq_nr_valid(irq) == 0 ) {
        res = irq_detach(irq, REGS_DMA(dma_chan));
        dev->ti &= ~DMA_TI_INTEN;
    }

    *(REGS_DMA_ENABLE) |= ( 1 << dma_chan );
    dev->cs = DMA_CS_RESET | (cs_panic_opts & ( DMA_CS_DISDEBUG 
        | DMA_CS_WAIT_FOR_OUTSTANDING_WRITES | DMA_CS_PANIC_PRIORITY(0xff) 
        | DMA_CS_PRIORITY(0xff) | DMA_CS_INT | DMA_CS_END | DMA_CS_ACTIVE ) ) ;

    if ( irqhandler != NULL ) {
        res = irq_attach(get_irq_from_channel(dma_chan), irqhandler, 0x00, dev, "DMA IRQ");
    }

    return res;
}

// Start DMA
// dma_chan - integer 0-15
// conbk - pointer to data type in physical memory
int pl330_dma_transfer_conbk(int dma_chan, volatile struct dma_ctrl_blk *conbk) {
    assert(dma_chan <= DMA_CHANNELS);

    REGS_DMA(dma_chan)->conblk_ad = (uint32_t)DMA_PHYS_TO_BUS(conbk);  
    REGS_DMA(dma_chan)->cs = DMA_CS_END | DMA_CS_INT; // Clear 'end' flag and interrupt
    REGS_DMA(dma_chan)->debug = DMA_DEBUG_READ_ERROR 
        | DMA_DEBUG_FIFO_ERROR 
        | DMA_DEBUG_READ_LAST_NOT_SET_ERROR; // clear error bits
    REGS_DMA(dma_chan)->cs |= DMA_CS_ACTIVE; // Start DMA
    return 0;
}

// Start DMA - basic memory copy
// dma_chan - integer 0-15
// dst, src - are destination and source memory locations in bus address space
// words - 4-byte aligned memory length
int pl330_dma_transfer(int dma_chan, uint32_t dst, uint32_t src, int words) {   
    assert(dma_chan <= DMA_CHANNELS);

    // A standard copy block to support basic implementation
    shared_conbk.source_ad = (uint32_t)DMA_PHYS_TO_BUS(src);
    shared_conbk.dest_ad = (uint32_t)DMA_PHYS_TO_BUS(dst);
    shared_conbk.txfr_len = words * sizeof(uint32_t);
    shared_conbk.ti = DMA_TI_SRC_INC | DMA_TI_DEST_INC;
    shared_conbk.nextconbk = 0;
    shared_conbk.stride = 0;
    shared_conbk.debug[1] = shared_conbk.debug[0] = 0;

    return pl330_dma_transfer_conbk(dma_chan, &shared_conbk);
}

int pl330_dma_in_progress(int dma_chan, uint32_t *error_flags) {
    assert(dma_chan <= DMA_CHANNELS);

    *error_flags = REGS_DMA(dma_chan)->debug;

    return (REGS_DMA(dma_chan)->cs & DMA_CS_ACTIVE);
}

uint32_t pl330_dma_channels_free(void) {
    bcm2835_mailbox_tag_t queries[] = {TAG_GET_DMA_CHANNELS, 0x0};
    uint32_t answers[] = {0x0, 0x0};

    bcm2835_property_value32(queries, answers);

    return answers[0];
}

static int pl330_dma_init(void) {
    dma_dev.config_extended = pl330_dma_config;
    dma_dev.transfer = pl330_dma_transfer;
    dma_dev.transfer_conbk = pl330_dma_transfer_conbk;
    dma_dev.in_progress_status = pl330_dma_in_progress;
    dma_dev.malloc = pl330_dma_malloc;
    dma_dev.free = pl330_dma_free;
    dma_dev.channels_free = pl330_dma_channels_free;
    return 0;
}

EMBOX_UNIT_INIT(pl330_dma_init);
