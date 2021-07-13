#pragma once
/**
 * @file
 * @brief
 *
 * @date    07.06.2021
 * @author  kpishere
 */

#include <stdlib.h>

/* Notes on channel usage :
 *
 * 0 - Use this channel for memory to memory transfers only, DREQ signal is always high
 * 0-6 - Full featured channels
 * 7-F - Lite featured channels
 * 16-31 - System used channels, don't use, you may want to disable them for power
 *      consumption reduction only.
 */

#define DMA_CHANNELS 15

//
// DMA Peripheral mapping
//
#define DMA_PERMAP_MEMORY    0
#define DMA_PERMAP_DSI       1
#define DMA_PERMAP_PCM_TX    2
#define DMA_PERMAP_PCM_RX    3
#define DMA_PERMAP_SMI       4
#define DMA_PERMAP_PWM       5
#define DMA_PERMAP_SPI_TX    6
#define DMA_PERMAP_SPI_RX    7
#define DMA_PERMAP_BSC_TX    8
#define DMA_PERMAP_BSC_RX    9
#define DMA_PERMAP_UNUSED    10
#define DMA_PERMAP_EMMC      11
#define DMA_PERMAP_UART_TX   12
#define DMA_PERMAP_SD_HOST   13
#define DMA_PERMAP_UART_RX   14
#define DMA_PERMAP_DSI2      15
#define DMA_PERMAP_SLIMBUS_MCTX     16
#define DMA_PERMAP_HDMI             17
#define DMA_PERMAP_SLIMBUS_MCRX     18
#define DMA_PERMAP_SLIMBUS_DC0      19
#define DMA_PERMAP_SLIMBUS_DC1      20
#define DMA_PERMAP_SLIMBUS_DC2      21
#define DMA_PERMAP_SLIMBUS_DC3      22
#define DMA_PERMAP_SLIMBUS_DC4      23
#define DMA_PERMAP_FIFO0_SMI        24
#define DMA_PERMAP_FIFO1_SMI        25
#define DMA_PERMAP_FIFO2_SMI        26
#define DMA_PERMAP_SLIMBUS_DC5      27
#define DMA_PERMAP_SLIMBUS_DC6      28
#define DMA_PERMAP_SLIMBUS_DC7      29
#define DMA_PERMAP_SLIMBUS_DC8      30
#define DMA_PERMAP_SLIMBUS_DC9      31

//
// Channels 0 thru E - Control and Status
//
#define DMA_CS_RESET        ( 1 << 31 )
#define DMA_CS_ABORT        ( 1 << 30 )
#define DMA_CS_DISDEBUG     ( 1 << 29 )
#define DMA_CS_WAIT_FOR_OUTSTANDING_WRITES ( 1 << 28 )
#define DMA_CS_PANIC_PRIORITY(x) (( (x) & 0x0F ) << 20 )
#define DMA_CS_PRIORITY(x)  (( (x) & 0x0F ) << 16 )
#define DMA_CS_ERROR        ( 1 << 8 )
#define DMA_CS_WAITING_FOR_OUTSTANDING_READS ( 1 << 6 )
#define DMA_CS_DREQ_STOPS_DMA (1 << 5 )
#define DMA_CS_PAUSED       ( 1 << 4 )
#define DMA_CS_DREQ         ( 1 << 3 )
#define DMA_CS_INT          ( 1 << 2 )
#define DMA_CS_END          ( 1 << 1 )
#define DMA_CS_ACTIVE       ( 1 )

//
// Channels 0 thru E - Transfer Information
//
#define DMA_TI_WAITS(x)       ( ( (x) & 0x1F ) << 21 )
#define DMA_TI_PERMAP(x)      ( ( (x) & 0x1F ) << 16 )
#define DMA_TI_BURST_LENGTH(x) ( ( (x) & 0x0F ) << 12 )
#define DMA_TI_SRC_IGNORE     ( 1 << 11 )
#define DMA_TI_SRC_DREQ       ( 1 << 10 )
#define DMA_TI_SRC_WIDTH      ( 1 << 9 )
#define DMA_TI_SRC_INC        ( 1 << 8 )
#define DMA_TI_DEST_IGNORE    ( 1 << 7 )
#define DMA_TI_DEST_DREQ      ( 1 << 6 )
#define DMA_TI_DEST_WIDTH     ( 1 << 5 )
#define DMA_TI_DEST_INC       ( 1 << 4 )
#define DMA_TI_WAIT_RESP      ( 1 << 3 )
#define DMA_TI_INTEN          ( 1 )
//// Channels 0 thru 6 Only
//
#define DMA_TI_NO_WIDE_BURSTS ( 1 << 26 )
#define DMA_TI_TDMODE         ( 1 << 1 )
//
////

//
// Channels 0 thru E - Transfer Length
//
#define DMA_TXFR_LEN_XLENGTH(x)     (   (x) & 0xFFFF )
// Channels 0 thru 6 - Transfer Length
//
#define DMA_TXFR_LEN_YLENGTH(x)     ( ( (x) & 0x3FFF ) << 16 )

//
// Channels 0 thru 6 - 2D Stride 
//
#define DMA_STRIDE_D_STRIDE(x)     ( ( (x) & 0xFFFF ) << 16 )
#define DMA_STRIDE_S_STRIDE(x)     (   (x) & 0xFFFF )

//
// Channels 0 thru E - 2D Debug 
//
#define DMA_DEBUG_LITE          ( 1 << 28 )
#define DMA_DEBUG_VERSION(x)    ( ( (x) & 0x03 ) << 25 )
#define DMA_DEBUG_DMA_STATE(x)  ( ( (x) & 0x007F ) << 16 )
#define DMA_DEBUG_DMA_ID(x)     ( ( (x) & 0xFF ) << 8 )
#define DMA_DEBUG_OUTSTANDING_WRITES(x)     ( ( (x) & 0x0F ) << 4 )
#define DMA_DEBUG_READ_ERROR    ( 1 << 2 )
#define DMA_DEBUG_FIFO_ERROR    ( 1 << 1 )
#define DMA_DEBUG_READ_LAST_NOT_SET_ERROR ( 1 )

//
// Interrupt Status 
//
#define DMA_INT_STATUS(x) ((x) & 0xFFFF)

//
// Enable
//
#define DMA_ENABLE(x) ((x) & 0xFFFF)

// DMA Control block
typedef struct {
    volatile uint32_t ti;
    volatile uint32_t source_ad;
    volatile uint32_t dest_ad; 
    volatile uint32_t txfr_len;
    volatile uint32_t stride;
    volatile uint32_t nextconbk;
    volatile uint32_t debug;
} Dma_conbk;

#define DMA_BUS_TO_PHYS(x) (void *)((x) & ~0xC0000000)

typedef struct {
    void *physical_addr; // Physical (ARM direct) base address of the page
    uint32_t bus_addr;  // Bus adress of the page, this is not a pointer because it does not point to valid physical address
    uint32_t mb_handle; // Used by mailbox property interface
    uint32_t size;
} Dma_mem_handle;

extern int bcm283x_dma_transfer_conbk(int dma_chan, volatile Dma_conbk *conbk);
extern int bcm283x_dma_in_progress(int dma_chan);
extern void bcm283x_dma_disp(int dma_chan);
extern int bcm283x_dma_config(int dma_chan);
extern Dma_mem_handle *dma_malloc(size_t size);
extern void dma_free(Dma_mem_handle *mem);

