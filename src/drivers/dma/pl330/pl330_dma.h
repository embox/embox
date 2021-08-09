#ifndef _pl330_DMA_H_
#define _pl330_DMA_H_
/**
 * @file
 * @brief
 *
 * @date    07.06.2021
 * @author  kpishere
 */

#include <stdlib.h>
#include <kernel/irq.h>

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
/* DMA Channel Reset: W1SC Writing a 1 to this bit will reset 
 * the DMA. The bit cannot be read, and will self clear.
 */
#define DMA_CS_RESET        ( 1 << 31 )
/* Abort DMA Writing: W1SC a 1 to this bit will abort the 
 * current DMA CB. The DMA will load the next CB and 
 * attempt to continue. The bit cannot be read, and will
 * self clear.
 */
#define DMA_CS_ABORT        ( 1 << 30 )
/* Disable debug pause signal: RW When set to 1, the DMA
 * will not stop when the debug pause signal is asserted.
 */
#define DMA_CS_DISDEBUG     ( 1 << 29 )
/* Wait for outstanding writes: RW When set to 1, the DMA 
 * will keep a tally of the AXI writes going out and 
 * the write responses coming in. At the very end of
 * the current DMA transfer it will wait until the
 * last outstanding write response has been received
 * before indicating the transfer is complete. Whilst
 * waiting it will load the next CB address (but will 
 * not fetch the CB), clear the active flag (if the 
 * next CB address = zero), and it will defer setting 
 * the END flag or the INT flag until the last 
 * outstanding write response has been received. In 
 * this mode, the DMA will pause if it has more than 
 * 13 outstanding writes at any one time.
 */
#define DMA_CS_WAIT_FOR_OUTSTANDING_WRITES ( 1 << 28 )
/* AXI Panic Priority Level: RW Sets the priority of 
 * panicking AXI bus transactions. This value is used 
 * when the panic bit of the selected peripheral 
 * channel is 1. Zero is the lowest priority.
 */
#define DMA_CS_PANIC_PRIORITY(x) (( (x) & 0x0F ) << 20 )
/* AXI Priority Level: RW Sets the priority of normal AXI 
 * bus transactions. This value is used when the 
 * panic bit of the selected peripheral channel is zero.
 * Zero is the lowest priority.
 */
#define DMA_CS_PRIORITY(x)  (( (x) & 0x0F ) << 16 )
/* DMA Error: RO Indicates if the DMA has detected an error. 
 * The error flags are available in the debug register, 
 * and have to be cleared by writing to that register.
 * 1 = DMA channel has an error flag set.
 * 0 = DMA channel is ok.
 */
#define DMA_CS_ERROR        ( 1 << 8 )
/* DMA is Waiting for the Last Write to be Received: RO
 * Indicates if the DMA is currently waiting for any 
 * outstanding writes to be received, and is not 
 * transferring data. 1 = DMA channel is waiting.
 */
#define DMA_CS_WAITING_FOR_OUTSTANDING_READS ( 1 << 6 )
/* DMA Paused by DREQ State: RO Indicates if the 
 * DMA is currently paused and not transferring data due
 * to the DREQ being inactive..
 * 1 = DMA channel is paused.
 * 0 = DMA channel is running.
 */
#define DMA_CS_DREQ_STOPS_DMA (1 << 5 )
/* DMA Paused State: RO Indicates if the DMA is currently 
 * paused and not transferring data. This will occur if: 
 * the active bit has been cleared, if the DMA is currently 
 * executing wait cycles or if the debug_pause signal has 
 * been set by the debug block, or the number of 
 * outstanding writes has exceeded the max count.
 * 1 = DMA channel is paused.
 * 0 = DMA channel is running.
 */
#define DMA_CS_PAUSED       ( 1 << 4 )
/* DREQ State: RO Indicates the state of the selected DREQ 
 * (Data Request) signal, ie. the DREQ selected by the 
 * PERMAP field of the transfer info.
 * 1 = Requesting data. This will only be valid once the 
 *     DMA has started and the PERMAP field has been loaded 
 *     from the CB. It will remain valid, indicating the 
 *     selected DREQ signal, until a new CB is loaded. If 
 *     PERMAP is set to zero (un- paced transfer) then 
 *     this bit will read back as 1.
 * 0 = No data request.
 */
#define DMA_CS_DREQ         ( 1 << 3 )
/* Interrupt Status: W1C This is set when the transfer 
 * for the CB ends and INTEN is set to 1. Once set it 
 * must be manually cleared down, even if the next 
 * CB has INTEN = 0. Write 1 to clear.
 */
#define DMA_CS_INT          ( 1 << 2 )
/* DMA End Flag: W1C Set when the transfer described 
 * by the current control block is complete. Write 1 to clear.
 */
#define DMA_CS_END          ( 1 << 1 )
/* Activate the DMA: RW This bit enables the DMA. 
 * The DMA will start if this bit is set and the CB_ADDR 
 * is non zero. The DMA transfer can be paused and resumed 
 * by clearing, then setting it again. This bit is 
 * automatically cleared at the end of the complete 
 * DMA transfer, ie. after a NEXTCONBK = 0x0000_0000 
 * has been loaded.
 */
#define DMA_CS_ACTIVE       ( 1 << 0 )

//
// Channels 0 thru E - Transfer Information
//
/* Add Wait Cycles
 * This slows down the DMA throughput by setting the number of dummy 
 * cycles burnt after each DMA read or write operation is completed.
 * A value of 0 means that no wait cycles are to be added.
 */
#define DMA_TI_WAITS(x)       ( ( (x) & 0x1F ) << 21 )
/* Peripheral Mapping
 * Indicates the peripheral number (1-31) whose ready signal shall 
 * be used to control the rate of the transfers, and whose panic signals 
 * will be output on the DMA AXI bus. Set to 0 for a continuous 
 * un-paced transfer.
 */
#define DMA_TI_PERMAP(x)      ( ( (x) & 0x1F ) << 16 )
/* Burst Transfer Length
 * Indicates the burst length of the DMA transfers. The DMA will attempt 
 * to transfer data as bursts of this number of words. A value of zero
 * will produce a single transfer. Bursts are only produced for
 * specific conditions, see main text.
 */
#define DMA_TI_BURST_LENGTH(x) ( ( (x) & 0x0F ) << 12 )
#define DMA_TI_SRC_IGNORE     ( 1 << 11 )
/* Control Source Reads with DREQ 
 * 1 = The DREQ selected by PER_MAP will gate the source reads.
 * 0 = DREQ has no effect.
 */
#define DMA_TI_SRC_DREQ       ( 1 << 10 )
/* Source Transfer Width
 * 1 = Use 128-bit source read width. 0 = Use 32-bit source read width.
 */
#define DMA_TI_SRC_WIDTH      ( 1 << 9 )
/* Source Address Increment
 * 1 = Source address increments after each read. The address will 
 *     increment by 4, if S_WIDTH=0 else by 32. 
 * 0 = Source address does not change
 */
#define DMA_TI_SRC_INC        ( 1 << 8 )
/* Ignore Writes: RW 1 = Do not perform destination writes. 
 * 0 = Write data to destination.
 */
#define DMA_TI_DEST_IGNORE    ( 1 << 7 )
/* Control Destination Writes with DREQ
 * 1 = The DREQ selected by PERMAP will gate the destination writes.
 * 0 = DREQ has no effect.
 */
#define DMA_TI_DEST_DREQ      ( 1 << 6 )
/* Destination Transfer Width
 * 1 = Use 128-bit destination write width. 
 * 0 = Use 32-bit destination write width
 */
#define DMA_TI_DEST_WIDTH     ( 1 << 5 )
/* Destination Address Increment
 * 1 = Destination address increments after each write 
 *     The address will increment by 4, if DEST_WIDTH=0 else by 32.
 * 0 = Destination address does not change.
 */
#define DMA_TI_DEST_INC       ( 1 << 4 )
/* Wait for a Write Response
 * When set this makes the DMA wait until it receives the AXI 
 * write response for each write. This ensures that multiple 
 * writes cannot get stacked in the AXI bus pipeline.
 * 1 = Wait for the write response to be received before proceeding.
 * 0 = Don't wait; continue as soon as the write data is sent.
 */
#define DMA_TI_WAIT_RESP      ( 1 << 3 )
/* Interrupt Enable
 * 1 = Generate an interrupt when the transfer described by the current Control Block completes.
 * 0 = Do not generate an interrupt.
 */
#define DMA_TI_INTEN          ( 1 )
//// Channels 0 thru 6 Only
//
/* Don t Do wide writes as a 2 beat burst: RW
 * This prevents the DMA from issuing wide writes as 2 beat AXI bursts. 
 * This is an inefficient access mode, so the default is to use the bursts.
 */
#define DMA_TI_NO_WIDE_BURSTS ( 1 << 26 )
/* 2D Mode : RW 
 * 1 = 2D mode interpret the TXFR_LEN register as YLENGTH number 
 *   of transfers each of XLENGTH, and add the strides to the address 
 *   after each transfer.
 * 0 = Linear mode interpret the TXFR register as a single transfer 
 *   of total length {YLENGTH ,XLENGTH}.
 */
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

// Convert bus address to physical address 
#define DMA_BUS_TO_PHYS(x) (void *)((x) & ~0xC0000000)
// Convert uncached memory virtual address to bus address
#define DMA_PHYS_TO_BUS(x) ((uint32_t)(x) | 0xC0000000)
// Get bus address of register
#define DMA_PERF_TO_BUS(x) ((uint32_t)(x) | 0x7E000000)

#endif /* _pl330_DMA_H_ */