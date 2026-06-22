/**
 * @brief Stand-Alone CAN Controller With SPI Interface
 *
 * @author Aleksey Zhmulin
 * @date 08.06.26
 */

#ifndef DRIVERS_CAN_MCP2515_H_
#define DRIVERS_CAN_MCP2515_H_

/**
 * Commands
 */
#define MCP2515_RESET       0xC0
#define MCP2515_READ        0x03
#define MCP2515_READ_RX0    0x90
#define MCP2515_READ_RX1    0x94
#define MCP2515_WRITE       0x02
#define MCP2515_LOAD_TX0    0x40
#define MCP2515_LOAD_TX1    0x42
#define MCP2515_LOAD_TX2    0x44
#define MCP2515_RTS_TX0     0x81
#define MCP2515_RTS_TX1     0x82
#define MCP2515_RTS_TX2     0x84
#define MCP2515_RTS_ALL     0x87
#define MCP2515_READ_STATUS 0xA0
#define MCP2515_RX_STATUS   0xB0
#define MCP2515_BITMOD      0x05

/**
 * Registers
 */
#define MCP2515_RXF0SIDH  0x00
#define MCP2515_RXF0SIDL  0x01
#define MCP2515_RXF0EID8  0x02
#define MCP2515_RXF0EID0  0x03
#define MCP2515_RXF1SIDH  0x04
#define MCP2515_RXF1SIDL  0x05
#define MCP2515_RXF1EID8  0x06
#define MCP2515_RXF1EID0  0x07
#define MCP2515_RXF2SIDH  0x08
#define MCP2515_RXF2SIDL  0x09
#define MCP2515_RXF2EID8  0x0a
#define MCP2515_RXF2EID0  0x0b
#define MCP2515_BFPCTRL   0x0c
#define MCP2515_TXRTSCTRL 0x0d
#define MCP2515_CANSTAT   0x0e
#define MCP2515_CANCTRL   0x0f
#define MCP2515_RXF3SIDH  0x10
#define MCP2515_RXF3SIDL  0x11
#define MCP2515_RXF3EID8  0x12
#define MCP2515_RXF3EID0  0x13
#define MCP2515_RXF4SIDH  0x14
#define MCP2515_RXF4SIDL  0x15
#define MCP2515_RXF4EID8  0x16
#define MCP2515_RXF4EID0  0x17
#define MCP2515_RXF5SIDH  0x18
#define MCP2515_RXF5SIDL  0x19
#define MCP2515_RXF5EID8  0x1a
#define MCP2515_RXF5EID0  0x1b
#define MCP2515_TEC       0x1c
#define MCP2515_REC       0x1d
#define MCP2515_RXM0SIDH  0x20
#define MCP2515_RXM0SIDL  0x21
#define MCP2515_RXM0EID8  0x22
#define MCP2515_RXM0EID0  0x23
#define MCP2515_RXM1SIDH  0x24
#define MCP2515_RXM1SIDL  0x25
#define MCP2515_RXM1EID8  0x26
#define MCP2515_RXM1EID0  0x27
#define MCP2515_CNF3      0x28
#define MCP2515_CNF2      0x29
#define MCP2515_CNF1      0x2a
#define MCP2515_CANINTE   0x2b
#define MCP2515_CANINTF   0x2c
#define MCP2515_EFLG      0x2d
#define MCP2515_TXB0CTRL  0x30
#define MCP2515_TXB0SIDH  0x31
#define MCP2515_TXB0SIDL  0x32
#define MCP2515_TXB0EID8  0x33
#define MCP2515_TXB0EID0  0x34
#define MCP2515_TXB0DLC   0x35
#define MCP2515_TXB0D0    0x36
#define MCP2515_RXB0CTRL  0x60
#define MCP2515_RXB0SIDH  0x61
#define MCP2515_RXB0SIDL  0x62
#define MCP2515_RXB0EID8  0x63
#define MCP2515_RXB0EID0  0x64
#define MCP2515_RXB0DLC   0x65
#define MCP2515_RXB0D0    0x66
#define MCP2515_RXB1CTRL  0x70
#define MCP2515_RXB1SIDH  0x71
#define MCP2515_RXB1SIDL  0x72
#define MCP2515_RXB1EID8  0x73
#define MCP2515_RXB1EID0  0x74
#define MCP2515_RXB1DLC   0x75
#define MCP2515_RXB1D0    0x76

/* MCP2515_CANSTAT */
#define MCP2515_CANSTAT_MOD       /* Operation Mode */
#define MCP2515_CANSTAT_MOD_MASK  0x7
#define MCP2515_CANSTAT_MOD_SHIFT 5
#define MCP2515_CANSTAT_MOD_NORM  0b000 /* Normal Mode */
#define MCP2515_CANSTAT_MOD_SLEEP 0b001 /* Sleep Mode */
#define MCP2515_CANSTAT_MOD_LOOP  0b010 /* Loopback Mode` */
#define MCP2515_CANSTAT_MOD_LISTN 0b011 /* Listen-only Mode */
#define MCP2515_CANSTAT_MOD_CONF  0b100 /* Configuration Mode */

/* MCP2515_CANCTRL */
#define MCP2515_CANCTRL_CLKEN (1 << 2) /* CLKOUT Pin Enable */
#define MCP2515_CANCTRL_OSM   (1 << 3) /* One-Shot Mode */
#define MCP2515_CANCTRL_ABAT  (1 << 4) /* Abort All Pending TX */

/* MCP2515_CANINTF */
#define MCP2515_CANINTF_RX0  (1 << 0) /* RX Buffer 0 Full Interrupt */
#define MCP2515_CANINTF_RX1  (1 << 1) /* RX Buffer 1 Full Interrupt */
#define MCP2515_CANINTF_TX0  (1 << 2) /* TX Buffer 0 Empty Interrupt */
#define MCP2515_CANINTF_TX1  (1 << 3) /* TX Buffer 1 Empty Interrupt */
#define MCP2515_CANINTF_TX2  (1 << 4) /* TX Buffer 2 Empty Interrupt */
#define MCP2515_CANINTF_ERR  (1 << 5) /* Error Interrupt */
#define MCP2515_CANINTF_WAK  (1 << 6) /* Wakeup Interrupt */
#define MCP2515_CANINTF_MERR (1 << 7) /* Message Error Interrupt */

/* MCP2515_CNF1 */
#define MCP2515_CNF1_BRP       /* Baud Rate Prescaler */
#define MCP2515_CNF1_BRP_MASK  0x3f
#define MCP2515_CNF1_BRP_SHIFT 0

#define MCP2515_CNF1_SJW       /* Synchronization Jump Width */
#define MCP2515_CNF1_SJW_MASK  0x3
#define MCP2515_CNF1_SJW_SHIFT 6

/* MCP2515_CNF2 */
#define MCP2515_CNF2_SAM     (1 << 6) /* Sample Point Configuration */
#define MCP2515_CNF2_BTLMODE (1 << 7) /* Phase Segment 2 Length Mode */

#define MCP2515_CNF2_PROPSEG       /* Propagation Segment Length */
#define MCP2515_CNF2_PROPSEG_MASK  0x7
#define MCP2515_CNF2_PROPSEG_SHIFT 0

#define MCP2515_CNF2_PHSEG1       /* Phase Segment 1 Length */
#define MCP2515_CNF2_PHSEG1_MASK  0x7
#define MCP2515_CNF2_PHSEG1_SHIFT 3

/* MCP2515_CNF3 */
#define MCP2515_CNF3_WAKFIL (1 << 6) /* Wake-up Filter */
#define MCP2515_CNF3_SOF    (1 << 7) /* Start-of-Frame Signal */

#define MCP2515_CNF3_PHSEG2       /* Phase Segment 2 Length */
#define MCP2515_CNF3_PHSEG2_MASK  0x7
#define MCP2515_CNF3_PHSEG2_SHIFT 0

/**
 * Transmit Buffer
 */
#define MCP2515_TXB_CTRL 0x0
#define MCP2515_TXB_SIDH 0x1
#define MCP2515_TXB_SIDL 0x2
#define MCP2515_TXB_EID8 0x3
#define MCP2515_TXB_EID0 0x4
#define MCP2515_TXB_DLC  0x5
#define MCP2515_TXB_DATA 0x6

#define MCP2515_TXB_SIZE 14

/* MCP2515_TXB_CTRL */
#define MCP2515_TXB_CTRL_TXREQ (1 << 3) /* Message Transmit Request */
#define MCP2515_TXB_CTRL_TXERR (1 << 4) /* Transmission Error Detected */
#define MCP2515_TXB_CTRL_MLOA  (1 << 5) /* Message Lost Arbitration */
#define MCP2515_TXB_CTRL_ABTF  (1 << 6) /* Message Aborted Flag */

#define MCP2515_TXB_CTRL_TXP       /* Transmit Buffer Priority */
#define MCP2515_TXB_CTRL_TXP_MASK  0x3
#define MCP2515_TXB_CTRL_TXP_SHIFT 0

/* MCP2515_TXB_SIDL */
#define MCP2515_TXB_SIDL_EXIDE (1 << 3) /* Extended Identifier Enable */

/* MCP2515_TXB_DLC */
#define MCP2515_TXB_DLC_RTR (1 << 6)

/**
 * Receive Buffer
 */
#define MCP2515_RXB_CTRL 0x0
#define MCP2515_RXB_SIDH 0x1
#define MCP2515_RXB_SIDL 0x2
#define MCP2515_RXB_EID8 0x3
#define MCP2515_RXB_EID0 0x4
#define MCP2515_RXB_DLC  0x5
#define MCP2515_RXB_DATA 0x6

#define MCP2515_RXB_SIZE 14

/* MCP2515_RXB_CTRL */
#define MCP2515_RXB_CTRL_FILHIT (1 << 0) /* Filter Hit */
#define MCP2515_RXB_CTRL_BUKT1  (1 << 1) /* Read-only Copy of BUKT */
#define MCP2515_RXB_CTRL_BUKT   (1 << 2) /* Rollover Enable */
#define MCP2515_RXB_CTRL_RXRTR  (1 << 3) /* Received Remote Transfer Request */

#define MCP2515_RXB_CTRL_RXM       /* RX Mode */
#define MCP2515_RXB_CTRL_RXM_MASK  0x3
#define MCP2515_RXB_CTRL_RXM_SHIFT 5
#define MCP2515_RXB_CTRL_RXM_ALL   0b11 /* Receive All Messages */
#define MCP2515_RXB_CTRL_RXM_VALID 0b00 /* Receive All Valid Messages */

/* MCP2515_RXB_SIDL */
#define MCP2515_RXB_SIDL_EXIDE (1 << 3) /* Extended Identifier Enable */

#define MCP2515_RXB_SIDL_EID       /* Extended Identifier Filter */
#define MCP2515_RXB_SIDL_EID_MASK  0x3
#define MCP2515_RXB_SIDL_EID_SHIFT 0

#define MCP2515_RXB_SIDL_SID       /* Standard Identifier Filter */
#define MCP2515_RXB_SIDL_SID_MASK  0x7
#define MCP2515_RXB_SIDL_SID_SHIFT 5

/* MCP2515_RXB_DLC */
#define MCP2515_RXB_DLC_RTR (1 << 6)

#endif /* DRIVERS_CAN_MCP2515_H_ */
