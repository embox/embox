/**
 * @file bcm283x_spi0.h
 * @brief
 * @author kpishere
 * @version
 * @date 2021.07.13
 */

#ifndef BCM283X_SPI0_H
#define BCM283X_SPI0_H

#include <drivers/dma/dma.h>

//
// Control and Status register
//
#define SPI0_CS_RESERVED ( 0x3F << 26 ) /* Reserved - Write as 0, read as don't care */
#define SPI0_CS_LENLONG ( 1 << 25 ) /* RW: Enable Long data word in Lossi mode if DMA_LEN is set \
										0= writing to the FIFO will write a single byte \
										1= writing to the FIFO will write a 32 bit word */
#define SPI0_CS_DMA_LEN ( 1 << 24 ) /* RW: Enable DMA mode in Lossi mode */
#define SPI0_CS_CSPOL2 ( 1 << 23 ) /* RW: Chip Select 2 Polarity \
									0= Chip select is active low. 1= Chip select is active high */
#define SPI0_CS_CSPOL1 ( 1 << 22 ) /* RW: Chip Select 1 Polarity \
									0= Chip select is active low. 1= Chip select is active high */
#define SPI0_CS_CSPOL0 ( 1 << 21 ) /* RW: Chip Select 0 Polarity \
									0= Chip select is active low. 1= Chip select is active high. */
#define SPI0_CS_RXF ( 1 << 20 )  /* RO: RXF - RX FIFO Full  0 = RXFIFO is not full. \
								1 = RX FIFO is full. No further serial data will be sent/ \
									received until data is read from FIFO. */
#define SPI0_CS_RXR ( 1 << 19 ) /* RO: RXR RX FIFO needs Reading ( full) \
								0 = RX FIFO is less than full (or not active TA = 0) \
								1 = RX FIFO is or more full. Cleared by reading \
									sufficient data from the RX FIFO or setting TA to 0. */
#define SPI0_CS_TXD ( 1 << 18 ) /* RO: TXD TX FIFO can accept Data \
								0 = TX FIFO is full and so cannot accept more data. \
								1 = TX FIFO has space for at least 1 byte. */
#define SPI0_CS_RXD ( 1 << 17 ) /* RO: RXD RX FIFO contains Data \
								0 = RX FIFO is empty. \
								1 = RX FIFO contains at least 1 byte. */
#define SPI0_CS_DONE ( 1 << 16 ) /* RO: Done transfer Done \
								0 = Transfer is in progress (or not active TA = 0) \
								1 = Transfer is complete. Cleared by writing more data \
								to the TX FIFO or setting TA to 0.  */
#define SPI0_CS_TE_EN ( 1 << 15 ) /* RW: Unused */
#define SPI0_CS_LMONO ( 1 << 14 ) /* RW: Unused */
#define SPI0_CS_LEN ( 1 << 13 ) /* RW: LEN LoSSI enable \
								The serial interface is configured as a LoSSI master. \
								0 = The serial interface will behave as an SPI master. \
								1 = The serial interface will behave as a LoSSI master. */
#define SPI0_CS_REN ( 1 << 12 ) /* RW: REN Read Enable read enable if you are using \
								bidirectional mode. If this bit is set, the SPI peripheral \
								will be able to send data to this device. \
								0 = We intend to write to the SPI peripheral. \
								1 = We intend to read from the SPI peripheral. */
#define SPI0_CS_ADCS ( 1 << 11 ) /* RW: ADCS Automatically Deassert Chip Select \
								0 = Dont automatically deassert chip select at the end of a DMA \
								transfer chip select is manually controlled by software. \
								1 = Automatically deassert chip select at the end of a DMA \
								transfer (as determined by SPIDLEN) */
#define SPI0_CS_INTR ( 1 << 10 ) /* RW: INTR Interrupt on RXR \
								0 = Dont generate interrupts on RX FIFO condition. \
								1 = Generate interrupt while RXR = 1. */
#define SPI0_CS_INTD ( 1 << 9 ) /* RW: INTD Interrupt on Done \
								0 = Don t generate interrupt on transfer complete. \
								1 = Generate interrupt when DONE = 1.  */
#define SPI0_CS_DMAEN ( 1 << 8 ) /* RW: DMAEN DMA Enable 0 = No DMA requests will be issued. \
								1 = Enable DMA operation. Peripheral generates data requests. \
								These will be taken in four-byte words until the \
								SPIDLEN has been reached.  */
#define SPI0_CS_TA ( 1 << 7 ) /* RW: Transfer Active \
								0 = Transfer not active./CS lines are all high \
								(assuming CSPOL = 0). RXR and DONE are 0. Writes to \
								SPIFIFO write data into bits -0 of SPICS allowing DMA \
								data blocks to set mode before sending data. \
								1 = Transfer active. /CS lines are set according to CS \
								bits and CSPOL. Writes to SPIFIFO write data to TX FIFO. \
								TA is cleared by a dma_frame_end pulse from the DMA controller */
#define SPI0_CS_CSPOL ( 1 << 6 ) /* RW: Chip Select Polarity \
								0 = Chip select lines are active low \
								1 = Chip select lines are active high  */
typedef enum {
	SPI0_no_action = 0x0
,	SPI0_tx_fifo = 0x1 /* Clear TX FIFO. One shot operation. */
,	SPI0_rx_fifo = 0x2 /* Clear RX FIFO. One shot operation. */
} SPI0_fifo_clear; /* If CLEAR and TA are both set in the same operation, \
					the FIFOs are cleared before the new frame is started. 
					Read back as 0.*/
#define SPI0_CS_CLEAR(a) ( ((a) & 0x03) << 4)
#define SPI0_CS_CPOL ( 1 << 3 ) /* RW: Clock Polarity \
					0 = Rest state of clock = low. 1 = Rest state of clock = high. */
#define SPI0_CS_CPHA ( 1 << 2 ) /* RW: Clock Phase \
					0 = First SCLK transition at middle of data bit. \
					1 = First SCLK transition at beginning of data bit. */

#define SPI0_CS(n) ( ((n) & 0x03) << 0 ) /* RW: Chip Select \
					00 = Chip select 0 01 = Chip select 1 10 = Chip select 2 11 = Reserved */

//
// SPI DMA DREQ Controls Register
//
#define SPI0_DC_RPANIC(l) ( ((l) & 0xff) << 24 ) /* RW: DMA Read Panic Threshold. \
					Generate the Panic signal to the RX DMA engine whenever the RX \
					FIFO level is greater than this amount. */
#define SPI0_DC_RDREQ(t) ( ((t) & 0xff) << 16 ) /* RW: DMA Read Request Threshold. \
					Generate A DREQ to the RX DMA engine whenever the RX FIFO level \
					is greater than this amount, (RX DREQ is also generated if the \
					transfer has finished but the RXFIFO isn t empty). */
#define  SPI0_DC_TPANIC(p) ( ((p) & 0xff) << 8 ) /* RW: DMA Write Panic Threshold. \
					Generate the Panic signal to the TX DMA engine whenever the \
					TX FIFO level is less than or equal to this amount. */
#define  SPI0_DC_TDREQ(r) ( ((r) & 0xff) << 0 ) /* RW: DMA Write Request Threshold. \
					Generate a DREQ signal to the TX DMA engine whenever the TX FIFO \
					level is less than or equal to this amount. */

#endif /* BCM283X_SPI_H */
