/**
 * @file
 * @brief ne2k register
 * @date 01.07.2011
 * @author Biff
 *         - Initial implementation
 * @author Nikolay Korotky
 *         Ilia Vaprol
 *         - Adaptation for embox
 */

#ifndef NET_NE2K_PCI_H_
#define NET_NE2K_PCI_H_

/* Port addresses */
#define NE_CMD          0x00 /* The command register (for all pages) */
/* Page 0 register offsets */
#define EN0_STARTPG     0x01 /* Starting page of ring buffer */
#define EN0_STOPPG      0x02 /* Ending page +1 of ring buffer */
#define EN0_BOUNDARY    0x03 /* Boundary page of ring buffer */
#define EN0_TSR		    0x04 /* Transmit status reg RD */
#define EN0_TPSR        0x04
#define EN0_TBCR_LO     0x05 /* Low  byte of tx byte count */
#define EN0_TBCR_HI     0x06 /* High byte of tx byte count */
#define EN0_ISR         0x07 /* Interrupt status reg */
/* Where to DMA data to/from */
#define EN0_RSARLO      0x08 /* Remote start address reg 0 */
#define EN0_RSARHI      0x09 /* Remote start address reg 1 */
/* How much data to DMA */
#define EN0_RCNTLO      0x0a /* Remote byte count reg */
#define EN0_RCNTHI      0x0b /* Remote byte count reg */

#define EN0_RXCR        0x0c /* RX configuration reg */
#define EN0_TXCR        0x0d /* TX configuration reg WR */
#define EN0_COUNTER0    0x0d /* Rcv alignment error counter RD */
#define EN0_DCFG        0x0e /* Data configuration reg WR */
#define EN0_COUNTER1    0x0e /* Rcv CRC error counter RD */
#define EN0_IMR	        0x0f /* Interrupt mask reg WR */
#define EN0_COUNTER2    0x0f /* Rcv missed frame error counter RD */

/* Bits in receive configuration byte and EN0_RXCR */
#define ENRCR_SEP       0x01 /* Accept packets with receive errors */
#define ENRCR_AR        0x02 /* Accept packets with len < 64 */
#define ENRCR_AB        0x04 /* Accept packets with bcast dest address */
#define ENRCR_AM        0x08 /* Accept packets with mcast dest address */
#define ENRCR_PRO       0x10 /* Accept packets with physical dest address */
#define ENRCR_MON       0x20 /* Monitor mode (disable buffered to memory) */

/* Bits in received packet status byte and EN0_RSR */
#define ENRSR_RXOK      0x01 /* Received a good packet */
#define ENRSR_CRC       0x02 /* CRC error */
#define ENRSR_FAE       0x04 /* frame alignment error */
#define ENRSR_FO        0x08 /* FIFO overrun */
#define ENRSR_MPA       0x10 /* missed pkt */
#define ENRSR_PHY       0x20 /* physical/multicase address */
#define ENRSR_DIS       0x40 /* receiver disable. set in monitor mode */
#define ENRSR_DEF       0x80 /* deferring */

/* Bits in EN0_ISR - Interrupt status register */
#define ENISR_RX        0x01 /* Receiver, no error */
#define ENISR_TX        0x02 /* Transmitter, no error */
#define ENISR_RX_ERR    0x04 /* Receiver, with error */
#define ENISR_TX_ERR    0x08 /* Transmitter, with error */
#define ENISR_OVER      0x10 /* Receiver overwrote the ring */
#define ENISR_COUNTERS  0x20 /* Counters need emptying */
#define ENISR_RDC       0x40 /* remote dma complete */
#define ENISR_RESET     0x80 /* Reset completed */
#define ENISR_ALL       0x3f /* Interrupts we will enable */

/* Transmitted packet status, EN0_TSR. */
#define ENTSR_PTX       0x01 /* Packet transmitted without error */
#define ENTSR_ND        0x02 /* The transmit wasn't deferred. */
#define ENTSR_COL       0x04 /* The transmit collided at least once. */
#define ENTSR_ABT       0x08 /* The transmit collided 16 times, and was deferred. */
#define ENTSR_CRS       0x10 /* The carrier sense was lost. */
#define ENTSR_FU        0x20 /* A "FIFO underrun" occured during transmit. */
#define ENTSR_CDH       0x40 /* The collision detect "heartbeat" signal was lost. */
#define ENTSR_OWC       0x80 /* There was an out-of-window collision. */

/* Page 1 register offsets */
#define EN1_PHYS          0x01       /* This board's physical eth addr */
#define EN1_PHYS_SHIFT(i) (0x01 + i) /* Get and set mac address */
#define EN1_CURPAG        0x07       /* Current memory page RD WR */
#define EN1_MULT          0x08       /* Multicast filter mask array (8 bytes) */
#define EN1_MULT_SHIFT(i) (0x08 + i) /* Get and set multicast filter */

#define NE_DATAPORT       0x10 /* NatSemi-defined port window offset */
#define NE_RESET          0x1f /* Reset command */

/* Commands to select the different pages. */
#define NE_PAGE0_STOP     0x21
#define NE_PAGE1_STOP     0x61

#define NE_PAGE0          0x20
#define NE_PAGE1          0x60

#define NE_START          0x02
#define NE_STOP           0x01

#define NE_PAR0           0x01
#define NE_PAR1           0x02
#define NE_PAR2           0x03
#define NE_PAR3           0x04
#define NE_PAR4           0x05
#define NE_PAR5           0x06

//#define MEMBASE            (16 * 1024)
//#define NE_PAGE_SIZE       256
//#define TX_BUFFER_START    (MEMBASE / NE_PAGE_SIZE)
//#define NE_TXBUF_SIZE      6
//#define RX_BUFFER_START    (TX_BUFFER_START + NE_TXBUF_SIZE)
//#define RX_BUFFER_END      ((32 * 1024) / NE_PAGE_SIZE)

/* Applies to ne2000 version of the card. */
#define NESM_START_PG_TX  0x40 /* First page of TX buffer */
#define NESM_START_PG_RX  0x4C /* First page of RX buffer */
#define NESM_STOP_PG      0x80 /* Last page +1 of RX ring */

/* Some constants of e8390 family */
#define E8390_STOP        0x01 /* Stop and reset the chip */
#define E8390_START       0x02 /* Start the chip, clear reset */
#define E8390_TXOFF       0x02 /* EN0_TXCR: Transmitter off */
#define E8390_TRANS       0x04 /* Transmit a frame */
#define E8390_RREAD       0x08 /* Remote read */
#define E8390_RWRITE      0x10 /* Remote write  */
#define E8390_NODMA       0x20 /* Remote DMA */
#define E8390_RXOFF       0x20 /* EN0_RXCR: Accept no packets */
#define E8390_PAGE0       0x00 /* Select page chip registers */
#define E8390_PAGE1       0x40 /* using the two high-order bits */
#define E8390_PAGE2       0x80 /* Page 3 is invalid. */
#define E8390_CMD         0x00 /* The command register (for all pages) */
#define E8390_RXCONFIG    (ENRCR_AB | ENRCR_PRO) /* EN0_RXCR */
#define E8390_TXCONFIG    0x00 /* EN0_TXCR: Normal transmit mode */

#endif /* NET_NE2K_PCI_H_ */
