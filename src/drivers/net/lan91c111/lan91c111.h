/**
 * @file
 *
 * @date Apr 8, 2020
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_NET_LAN91C111_LAN91C111_H_
#define SRC_DRIVERS_NET_LAN91C111_LAN91C111_H_


/* Bank 0 */
#define BANK_TCR        (BANK_BASE_ADDR + 0x0)
#define BANK_EPH_STATUS (BANK_BASE_ADDR + 0x2)
#define BANK_RCR        (BANK_BASE_ADDR + 0x4)
#define BANK_COUNTER    (BANK_BASE_ADDR + 0x6)
#define BANK_MIR        (BANK_BASE_ADDR + 0x8)
#define BANK_RPCR       (BANK_BASE_ADDR + 0xA)
/*      BANK_RESERVED   (BANK_BASE_ADDR + 0xC) */
#define BANK_BANK       (BANK_BASE_ADDR + 0xE) /* Common for all banks */

/* Bank 1 */
#define BANK_CONFIG          (BANK_BASE_ADDR + 0x0)
#define BANK_BASE            (BANK_BASE_ADDR + 0x2)
#define BANK_IA01            (BANK_BASE_ADDR + 0x4)
#define BANK_IA23            (BANK_BASE_ADDR + 0x6)
#define BANK_IA45            (BANK_BASE_ADDR + 0x8)
#define BANK_GENERAL_PURPOSE (BANK_BASE_ADDR + 0xA)
#define BANK_CONTROL         (BANK_BASE_ADDR + 0xC)
/*      BANK_BANK            (BANK_BASE_ADDR + 0xE) -- already defined above */

/* Bank 2 */
#define BANK_MMU_CMD           (BANK_BASE_ADDR + 0x0)
#define BANK_PNR               (BANK_BASE_ADDR + 0x2)
#define BANK_TX_ALLOC          (BANK_BASE_ADDR + 0x3)
#define BANK_FIFO_PORTS        (BANK_BASE_ADDR + 0x4)
#define BANK_POINTER           (BANK_BASE_ADDR + 0x6)
#define BANK_DATA              (BANK_BASE_ADDR + 0x8)
/*      BANK_DATA              (BANK_BASE_ADDR + 0xA) -- another 2 bytes */
#define BANK_INTERRUPT_STATUS  (BANK_BASE_ADDR + 0xC)
#define BANK_INTERRUPT_MASK    (BANK_BASE_ADDR + 0xD)
/*      BANK_BANK       (BANK_BASE_ADDR + 0xE) -- already defined above */

/* Bank 3 */
#define BANK_MT01     (BANK_BASE_ADDR + 0x0)
#define BANK_MT23     (BANK_BASE_ADDR + 0x2)
#define BANK_MT45     (BANK_BASE_ADDR + 0x4)
#define BANK_MT67     (BANK_BASE_ADDR + 0x6)
#define BANK_MGMT     (BANK_BASE_ADDR + 0x8)
#define BANK_REVISION (BANK_BASE_ADDR + 0xA)
#define BANK_RCV      (BANK_BASE_ADDR + 0xC)
/*      BANK_BANK     (BANK_BASE_ADDR + 0xE) -- already defined above */

/* Commands */
#define CMD_NOP                0
#define CMD_TX_ALLOC           1
#define CMD_RESET_MMU          2
#define CMD_RX_POP             3
#define CMD_RX_POP_AND_RELEASE 4
#define CMD_PACKET_FREE        5
#define CMD_TX_ENQUEUE         6
#define CMD_TX_FIFO_RESET      7

/* Various flags */
#define MMU_BUSY 0x0001
#define RX_EN    0x0100
#define TX_EN    0x0001

#define RX_INT    0x01
#define TX_INT    0x02
#define ALLOC_INT 0x08

#define FIFO_EMPTY 0x80

#define ALLOC_MASK  0x08
#define TX_MASK     0x02
#define RX_MASK     0x01

#define CRC_CONTROL   0x10
#define ODD_CONTROL   0x20

#define LAN91C111_FRAME_SIZE_MAX 2048
#define LAN91C111_IRQ            27

#define PNUM_MASK 0x3F
#define PLEN_MASK 0x7FF

#define RX_FIFO_PACKET 0x8000
#define AUTO_INCR      0x4000

#define LAN91C111_STATUS_BYTES  2
#define LAN91C111_CTL_BYTES     2

#endif /* SRC_DRIVERS_NET_LAN91C111_LAN91C111_H_ */
