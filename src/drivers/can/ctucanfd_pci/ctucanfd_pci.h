/**
 * @brief CTU CAN FD Controller
 *
 * @author Aleksey Zhmulin
 * @date 29.05.26
 */

#ifndef DRIVERS_CAN_CTUCANFD_PCI_H_
#define DRIVERS_CAN_CTUCANFD_PCI_H_

/* Control registers */
#define CTUCANFD_VER   0x00 /* Version Register */
#define CTUCANFD_DEVID 0x00 /* Device ID Register */
#define CTUCANFD_MODE  0x04 /* Mode Register */
#define CTUCANFD_STAT  0x08 /* Status Register */
#define CTUCANFD_CMD   0x0c /* Command Register */
#define CTUCANFD_ISR   0x10 /* Interrupt Status Register */
#define CTUCANFD_IES   0x14 /* Interrupt Enable Set Register */
#define CTUCANFD_IEC   0x18 /* Interrupt Enable Clear Register */
#define CTUCANFD_IMS   0x1c /* Interrupt Mask Set Register */
#define CTUCANFD_IMC   0x20 /* Interrupt Mask Clear Register */
#define CTUCANFD_BTR   0x24 /* Bit Timing Register */
#define CTUCANFD_BTRFD 0x28 /* Bit Timing FD Register */
#define CTUCANFD_ELR   0x2c /* Error Limit Register */
#define CTUCANFD_FAULT 0x2c /* Fault State Register */
#define CTUCANFD_REC   0x30 /* RX Error Counter Register */
#define CTUCANFD_TEC   0x30 /* TX Error Counter Register */
#define CTUCANFD_ERR   0x34 /* Error Counter Register */
#define CTUCANFD_CPR   0x38 /* Counter Preset Register */
#define CTUCANFD_FAM   0x3c /* Filter A Mask Register */
#define CTUCANFD_FAV   0x40 /* Filter A Value Register */
#define CTUCANFD_FBM   0x44 /* Filter B Mask Register */
#define CTUCANFD_FBV   0x48 /* Filter B Value Register */
#define CTUCANFD_FCM   0x4c /* Filter C Mask Register */
#define CTUCANFD_FCV   0x50 /* Filter C Value Register */
#define CTUCANFD_FRL   0x54 /* Filter Range Low Register */
#define CTUCANFD_FRH   0x58 /* Filter Range High Register */
#define CTUCANFD_FCR   0x5c /* Filter Control Register */
#define CTUCANFD_FSR   0x5c /* Filter Status Register */
#define CTUCANFD_RXMI  0x60 /* RX Memory Info Register  */
#define CTUCANFD_RXPT  0x64 /* RX Pointers Register */
#define CTUCANFD_RXS   0x68 /* TX Status/Settings Register */
#define CTUCANFD_RXD   0x6c /* RX Data Register */
#define CTUCANFD_TXS   0x70 /* TX Status Register */
#define CTUCANFD_TXC   0x74 /* TX Command Register */
#define CTUCANFD_TXI   0x74 /* TX Info Register */
#define CTUCANFD_TXP   0x78 /* TX Priority Register */
#define CTUCANFD_ECC   0x7c /* Error Code Capture Register */
#define CTUCANFD_TDR   0x80 /* Transmitter delay register */
#define CTUCANFD_SSPC  0x80 /* Secondary Sampling Point Config Register */
#define CTUCANFD_TSL   0x94 /* Timestamp Low Register */
#define CTUCANFD_TSH   0x98 /* Timestamp High Register */

/* TXT Buﬀer 1 */
#define CTUCANFD_TXB_FMT 0x100 /* Frame format */
#define CTUCANFD_TXB_ID  0x104 /* CAN Identifier */
#define CTUCANFD_TXB_TSL 0x108 /* Timestamp (Lower 32 bits) */
#define CTUCANFD_TXB_TSU 0x10c /* Timestamp (Upper 32 bits) */
#define CTUCANFD_TXB_DAT 0x110 /* Data (64 bytes) */

/* CTUCANFD_MODE */
#define CTUCANFD_MODE_RST    (1 << 0)  /* Soft Reset */
#define CTUCANFD_MODE_BMM    (1 << 1)  /* Bus Monitoring Mode */
#define CTUCANFD_MODE_STM    (1 << 2)  /* Self Test Mode */
#define CTUCANFD_MODE_AFM    (1 << 3)  /* Acceptance Filters Mode */
#define CTUCANFD_MODE_FDE    (1 << 4)  /* Flexible Data Rate Mode */
#define CTUCANFD_MODE_TTTM   (1 << 5)  /* Time Triggered Transmission Mode */
#define CTUCANFD_MODE_ROM    (1 << 6)  /* Restricted Mperation Mode */
#define CTUCANFD_MODE_ACF    (1 << 7)  /* Acknowledge Forbidden Mode */
#define CTUCANFD_MODE_TSTM   (1 << 8)  /* Test Mode */
#define CTUCANFD_MODE_RXBAM  (1 << 9)  /* TX Buffer Automatic Mode */
#define CTUCANFD_MODE_TXBBM  (1 << 10) /* TXT Buffer Backup Mode */
#define CTUCANFD_MODE_SAM    (1 << 11) /* Self-Acknowledge Mode */
#define CTUCANFD_MODE_ERFM   (1 << 12) /* Error Frame Receive Mode */
#define CTUCANFD_MODE_RTRLE  (1 << 16) /* Retransmit Limit Enable */
#define CTUCANFD_MODE_ILBP   (1 << 21) /* Internal Loop Back Mode */
#define CTUCANFD_MODE_ENA    (1 << 22) /* Main Enable of CTU CAN FD */
#define CTUCANFD_MODE_NISOFD (1 << 23) /* Non ISO FD */
#define CTUCANFD_MODE_PEX    (1 << 24) /* Protocol Exception Handling */
#define CTUCANFD_MODE_TBFBO  (1 << 25) /* TX Failed when Bus-Off */
#define CTUCANFD_MODE_FDRF   (1 << 26) /* Frame Filter Drop Remote Frames */
#define CTUCANFD_MODE_PCHKE  (1 << 27) /* Enable Parity Checks */

/* CTUCANFD_CMD */
#define CTUCANFD_CMD_RXRPMV  (1 << 1)  /* RX buffer read pointer move */
#define CTUCANFD_CMD_RRB     (1 << 2)  /* Release RX buffer */
#define CTUCANFD_CMD_CDO     (1 << 3)  /* Clear Data Overrun */
#define CTUCANFD_CMD_ERCRST  (1 << 4)  /* Error counters reset */
#define CTUCANFD_CMD_RXFCRST (1 << 5)  /* Clear RX bus traffic counter */
#define CTUCANFD_CMD_TXFCRST (1 << 6)  /* Clear TX bus traffic counter */
#define CTUCANFD_CMD_CPEXS   (1 << 7)  /* Clear protocol exception status */
#define CTUCANFD_CMD_CRXPE   (1 << 8)  /* Clear STATUS[RXPE] flag */
#define CTUCANFD_CMD_CTXPE   (1 << 9)  /* Clear STATUS[TXPE] flag */
#define CTUCANFD_CMD_CTXDPE  (1 << 10) /* Clear STATUS[TXDPE] flag */

/* CTUCANFD_ISR */
#define CTUCANFD_ISR_RXI    (1 << 0)  /* Frame received interrupt */
#define CTUCANFD_ISR_TXI    (1 << 1)  /* Frame transmitted interrupt */
#define CTUCANFD_ISR_EWLI   (1 << 2)  /* Error warning limit interrupt */
#define CTUCANFD_ISR_DOI    (1 << 3)  /* Data overrun interrupt */
#define CTUCANFD_ISR_FCSI   (1 << 4)  /* Fault state changed interrupt */
#define CTUCANFD_ISR_ALI    (1 << 5)  /* Arbitration lost interrupt */
#define CTUCANFD_ISR_BEI    (1 << 6)  /* Bus error interrupt */
#define CTUCANFD_ISR_OFI    (1 << 7)  /* Overload frame interrupt */
#define CTUCANFD_ISR_RXFI   (1 << 8)  /* RX buffer full interrupt */
#define CTUCANFD_ISR_BSI    (1 << 9)  /* Bit rate shifted interrupt */
#define CTUCANFD_ISR_RBNEI  (1 << 10) /* RX buffer not empty interrupt */
#define CTUCANFD_ISR_TXBHCI (1 << 11) /* TXT buffer HW command interrupt */

/* CTUCANFD_RXS */
#define CTUCANFD_RXS_RXE   (1 << 0) /* RX buffer is empty */
#define CTUCANFD_RXS_RXF   (1 << 1) /* RX buffer is full */
#define CTUCANFD_RXS_RXMOF (1 << 2) /* RX buffer middle of frame */

#define CTUCANFD_RXS_RXFRC       /* RX buffer frame count */
#define CTUCANFD_RXS_RXFRC_MASK  0x3ff
#define CTUCANFD_RXS_RXFRC_SHIFT 4

/* CTUCANFD_TXS */
#define CTUCANFD_TXS_MASK     0xf /* TX buffer 1 status */
#define CTUCANFD_TXS_NOTEXIST 0   /* TXT buffer doesn't exist */
#define CTUCANFD_TXS_RDY      1   /* "Ready" state */
#define CTUCANFD_TXS_TRAN     2   /* "TX in progress" state */
#define CTUCANFD_TXS_ABTP     3   /* "Abort in progress" state */
#define CTUCANFD_TXS_TOK      4   /* "TX OK" state */
#define CTUCANFD_TXS_ERR      6   /* "Failed" state */
#define CTUCANFD_TXS_ABT      7   /* "Aborted" state */
#define CTUCANFD_TXS_ETY      8   /* "Empty" state */
#define CTUCANFD_TXS_PER      9   /* "Parity error" state */

/* CTUCANFD_TXC */
#define CTUCANFD_TXC_TXCE (1 << 0)  /* TX Command Empty */
#define CTUCANFD_TXC_TXCR (1 << 1)  /* TX Command Ready */
#define CTUCANFD_TXC_TXCA (1 << 2)  /* TX Command Abort */
#define CTUCANFD_TXC_TXB1 (1 << 8)  /* TXT buffer 1 */
#define CTUCANFD_TXC_TXB2 (1 << 9)  /* TXT buffer 2 */
#define CTUCANFD_TXC_TXB3 (1 << 10) /* TXT buffer 3 */
#define CTUCANFD_TXC_TXB4 (1 << 11) /* TXT buffer 4 */
#define CTUCANFD_TXC_TXB5 (1 << 12) /* TXT buffer 5 */
#define CTUCANFD_TXC_TXB6 (1 << 13) /* TXT buffer 6 */
#define CTUCANFD_TXC_TXB7 (1 << 14) /* TXT buffer 7 */
#define CTUCANFD_TXC_TXB8 (1 << 15) /* TXT buffer 8 */

/* CTUCANFD_TXB_FMT */
#define CTUCANFD_TXB_FMT_RTR (1 << 5)  /* Remote Transmission Request */
#define CTUCANFD_TXB_FMT_IDE (1 << 6)  /* Extended Format */
#define CTUCANFD_TXB_FMT_FDF (1 << 7)  /* FD Format */
#define CTUCANFD_TXB_FMT_BRS (1 << 9)  /* Bit Rate Shift */
#define CTUCANFD_TXB_FMT_ESI (1 << 10) /* Error State Indicator */

#define CTUCANFD_TXB_FMT_DLC       /* Data Length Code */
#define CTUCANFD_TXB_FMT_DLC_MASK  0xf
#define CTUCANFD_TXB_FMT_DLC_SHIFT 0

#define CTUCANFD_TXB_FMT_RWCNT       /* RX Buﬀer Size without FMT */
#define CTUCANFD_TXB_FMT_RWCNT_MASK  0x1f
#define CTUCANFD_TXB_FMT_RWCNT_SHIFT 11

/* CTUCANFD_TXB_ID */
#define CTUCANFD_TXB_ID_EXT       /* Extended Frame ID */
#define CTUCANFD_TXB_ID_EXT_MASK  0x1fffffff
#define CTUCANFD_TXB_ID_EXT_SHIFT 0

#define CTUCANFD_TXB_ID_STD       /* Standard Frame ID */
#define CTUCANFD_TXB_ID_STD_MASK  0x7ff
#define CTUCANFD_TXB_ID_STD_SHIFT 18

#endif /* DRIVERS_CAN_CTUCANFD_PCI_H_ */
