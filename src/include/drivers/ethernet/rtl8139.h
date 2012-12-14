/**
 * @file
 * @brief Device driver for RealTek RTL8139 Fast Ethernet.
 *
 * @date 18.12.11
 * @author Nikolay Korotkiy
 *         - Initial implementation
 */

#ifndef NET_RTL8139_H_
#define NET_RTL8139_H_

/* Offsets to registers */
#define RTL8139_MAC0      0x00 /* Ethernet hardware address */
#define RTL8139_MAR0      0x08 /* Multicast Registers */
#define RTL8139_TX_STAT_0 0x10 /* Transmit status (Four 32bit registers) */
#define RTL8139_TX_ADDR_0 0x20 /* Tx descriptors (also four 32bit) */
#define RTL8139_RX_BUF    0x30 /* Rx Buffer Start Address */
#define RTL8139_CR        0x37 /* Command Register */
#define  CR_RESET         0x10
#define  CR_RX_ENABLE     0x08
#define  CR_TX_ENABLE     0x04
#define  CR_TX_BUF_EMPTY  0x01
#define RTL8139_RX_BUFP   0x38 /* Current Address of Reseived Packet */
#define RTL8139_IMR       0x3C /* Intrpt Mask Reg */
#define RTL8139_ISR       0x3E /* Intrpt Status Reg */
#define  ISR_PCI_ERR      0x8000
#define  ISR_PCSTIMEOUT   0x4000
#define  ISR_RX_FIFOOVER  0x40
#define  ISR_RX_UNDERRUN  0x20
#define  ISR_RX_OVERFLOW  0x10
#define  ISR_TX_ERR       0x08
#define  ISR_TX_OK        0x04
#define  ISR_RX_ERR       0x02
#define  ISR_RX_OK        0x01
#define  ISR_RX_ACK_BITS  (ISR_RX_FIFOOVER | ISR_RX_OVERFLOW | ISR_RX_OK)
#define RTL8139_TCR       0x40      /* Tx Config Reg */
#define  TCR_IFG96        (3 << 24) /* Interframe Gap Time: 9.6us/960ns */
#define  TCR_LOOPBACK     (1 << 18) | (1 << 17) /* enable loopback test mode */
#define  TCR_CRC          (1 << 16) /* DISABLE appending CRC to end of Tx packets */
#define  TCR_DMA_SHIFT    8 /* DMA burst value (0-7) is shifted this many bits */
#define  TCR_RETRY_SHIFT  4 /* TXRR value (0-15) is shifted this many bits */
#define RTL8139_RCR       0x44      /* Rx Config Reg */
#define  RCR_FIFO_NONE    (7 << 13) /* rx fifo threshold */
#define  RCR_DMA_UNLIM    (7 << 8)  /* Max DMA burst */
#define  RCR_RCV_8K       0x0
#define  RCR_RCV_16K      (1 << 11)
#define  RCR_RCV_32K      (1 << 12) /* rx ring buffer length */
#define  RCR_RCV_64K      (1 << 11) | (1 << 12)
#define  RCR_NO_WRAP      (1 << 7)  /* Disable packet wrap at end of Rx buffer */
#define  RCR_AC_ERR       0x20
#define  RCR_AC_RUNT      0x10
#define  RCR_AC_BROADCAST 0x08
#define  RCR_AC_MULTICAST 0x04
#define  RCR_AC_MYPHYS    0x02
#define  RCR_AC_ALLPHYS   0x01
#define RTL8139_MPC       0x4C /* Missed Packet Counter */
#define RTL8139_9346CR    0x50 /* 9346 Command Reg */
#define  CFG9346_LOCK     0x00
#define  CFG9346_UNLOCK   0xC0
#define RTL8139_MULT_INTR 0x5C /* Multiple Intrpt Select */
#define  MULT_INTR_CLEAR  0xF000

/* Interrupts */
#define PKT_RX            0x0001
#define TX_OK             0x0004

#endif /* NET_RTL8139_H_ */
