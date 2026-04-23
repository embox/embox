/**
 * @brief SJA1000 Stand-alone CAN controller
 *
 * @author Aleksey Zhmulin
 * @date 14.04.26
 */

#ifndef DRIVERS_CAN_SJA1000_H_
#define DRIVERS_CAN_SJA1000_H_

#include <stdint.h>

/*  Registers (PeliCan Mode) */
#define SJA_MOD   0x00 /* Mode Control Register */
#define SJA_CMR   0x01 /* Command Register */
#define SJA_SR    0x02 /* Status Register */
#define SJA_IR    0x03 /* Interrupt register */
#define SJA_IER   0x04 /* Interrupt Enable Register */
#define SJA_BTR0  0x06 /* Bus Timing Register 0 */
#define SJA_BTR1  0x07 /* Bus Timing Register 1 */
#define SJA_OCR   0x08 /* Output Control Register */
#define SJA_ALC   0x0b /* Arbitration Lost Capture Register */
#define SJA_ECC   0x0c /* Error Code Capture Register */
#define SJA_EWLR  0x0d /* Error Warning Limit Register */
#define SJA_RXERR 0x0e /* RX Error Counter Register */
#define SJA_TXERR 0x0f /* TX Error Counter Register */
#define SJA_RMC   0x1d /* RX Message Counter Register */
#define SJA_RBSA  0x1e /* Rx Buffer Start Addr Register */
#define SJA_CDR   0x1f /* Clock Divider Register */

/**
 * +-------------+-----------------------------+-----------------------------+
 * |   address   | Standard frame format (SFF) | Extended frame format (EFF) |
 * +-------------+-----------------------------+-----------------------------+
 * |                                                                         |
 * +------+------+-----------------------------+-----------------------------+
 * |   1  | 0x10 |      frame information      |      frame information      |
 * +------+------+-----------------------------+-----------------------------+
 * |   2  | 0x11 |        identifier 0         |        identifier 0         |
 * +------+------+-----------------------------+-----------------------------+
 * |   3  | 0x12 |        identifier 1         |        identifier 1         |
 * +------+------+-----------------------------+-----------------------------+
 * |   4  | 0x13 |         data byte 0         |        identifier 2         |
 * +------+------+-----------------------------+-----------------------------+
 * |   5  | 0x14 |         data byte 1         |        identifier 3         |
 * +------+------+-----------------------------+-----------------------------+
 * |   6  | 0x15 |         data byte 2         |         data byte 0         |
 * +------+------+-----------------------------+-----------------------------+
 * |   7  | 0x16 |         data byte 3         |         data byte 1         |
 * +------+------+-----------------------------+-----------------------------+
 * |   8  | 0x17 |         data byte 4         |         data byte 2         |
 * +------+------+-----------------------------+-----------------------------+
 * |   9  | 0x18 |         data byte 5         |         data byte 3         |
 * +------+------+-----------------------------+-----------------------------+
 * |  10  | 0x19 |         data byte 6         |         data byte 4         |
 * +------+------+-----------------------------+-----------------------------+
 * |  11  | 0x1a |         data byte 7         |         data byte 5         |
 * +------+------+-----------------------------+-----------------------------+
 * |  12  | 0x1b |           unused            |         data byte 6         |
 * +------+------+-----------------------------+-----------------------------+
 * |  13  | 0x1c |           unused            |         data byte 7         |
 * +------+------+-----------------------------+-----------------------------+
 */

/* Transmit / Receive Buffer (PeliCan Mode) */
#define SJA_FRM      0x10       /* Frame Information */
#define SJA_ID(n)    (0x11 + n) /* Identifier N */
#define SJA_SFDAT(n) (0x13 + n) /* Standard Frame Data Byte N */
#define SJA_EFDAT(n) (0x15 + n) /* Extended Frame Data Byte N */

/* SJA_MOD */
#define SJA_MOD_RM  (1 << 0) /* Reset Mode */
#define SJA_MOD_LOM (1 << 1) /* Listen Only Mode */
#define SJA_MOD_STM (1 << 2) /* Self Test Mode */
#define SJA_MOD_AFM (1 << 3) /* Acceptance Filter Mode */
#define SJA_MOD_SM  (1 << 4) /* Sleep Mode */

/* SJA_CMR */
#define SJA_CMR_TR  (1 << 0) /* Transmission Request */
#define SJA_CMR_AT  (1 << 1) /* Abort Transmission */
#define SJA_CMR_RRB (1 << 2) /* Release Receive Buffer */
#define SJA_CMR_CDO (1 << 3) /* Clear Data Overrun */
#define SJA_CMR_SRR (1 << 4) /* Self Reception Request */

/* SJA_SR */
#define SJA_SR_RBS (1 << 0) /* Receive Buffer Status */
#define SJA_SR_DOS (1 << 1) /* Data Overrun Status */
#define SJA_SR_TBS (1 << 2) /* Transmit Buffer Status */
#define SJA_SR_TCS (1 << 3) /* Transmission Complete Status */
#define SJA_SR_RS  (1 << 4) /* Receive Status */
#define SJA_SR_TS  (1 << 5) /* Transmit Status */
#define SJA_SR_ES  (1 << 6) /* Error Status */
#define SJA_SR_BS  (1 << 7) /* Bus Status */

/* SJA_IR */
#define SJA_IR_RI  (1 << 0) /* Receive Interrupt */
#define SJA_IR_TI  (1 << 1) /* Transmit Interrupt */
#define SJA_IR_EI  (1 << 2) /* Error Warning Interrupt */
#define SJA_IR_DOI (1 << 3) /* Data Overrun Interrupt */
#define SJA_IR_WUI (1 << 4) /* Wake-Up Interrupt */
#define SJA_IR_EPI (1 << 5) /* Error Passive Interrupt */
#define SJA_IR_ALI (1 << 6) /* Arbitration Lost Interrupt */
#define SJA_IR_BEI (1 << 7) /* Bus Error Interrupt */

/* SJA_IER */
#define SJA_IER_RIE  (1 << 0) /* Receive Interrupt Enable */
#define SJA_IER_TIE  (1 << 1) /* Transmit Interrupt Enable */
#define SJA_IER_EIE  (1 << 2) /* Error Warning Interrupt Enable */
#define SJA_IER_DOIE (1 << 3) /* Data Overrun Interrupt Enable */
#define SJA_IER_WUIE (1 << 4) /* Wake-Up Interrupt Enable */ _
#define SJA_IER_EPIE (1 << 5) /* Error Passive Interrupt Enable */
#define SJA_IER_ALIE (1 << 6) /* Arbitration Lost Interrupt Enable */
#define SJA_IER_BEIE (1 << 7) /* Bus Error Interrupt Enable */

/* SJA_BTR0 */
#define BTR0_BRP0 (1 << 0) /* Baud Rate Prescaler 0 */
#define BTR0_BRP1 (1 << 1) /* Baud Rate Prescaler 1 */
#define BTR0_BRP2 (1 << 2) /* Baud Rate Prescaler 2 */
#define BTR0_BRP3 (1 << 3) /* Baud Rate Prescaler 3 */
#define BTR0_BRP4 (1 << 4) /* Baud Rate Prescaler 4 */
#define BTR0_BRP5 (1 << 5) /* Baud Rate Prescaler 5 */
#define BTR0_SJW0 (1 << 6) /* Synchronization Jump Width 0 */
#define BTR0_SJW1 (1 << 7) /* Synchronization Jump Width 1 */

/* SJA_BTR1 */
#define BTR1_TSEG10 (1 << 0) /* Time Segment 1.0 */
#define BTR1_TSEG11 (1 << 1) /* Time Segment 1.1 */
#define BTR1_TSEG12 (1 << 2) /* Time Segment 1.2 */
#define BTR1_TSEG13 (1 << 3) /* Time Segment 1.3 */
#define BTR1_TSEG20 (1 << 4) /* Time Segment 2.0 */
#define BTR1_TSEG21 (1 << 5) /* Time Segment 2.1 */
#define BTR1_TSEG22 (1 << 6) /* Time Segment 2.2 */
#define BTR1_SAM    (1 << 7) /* Sampling */

/* SJA_CDR */
#define SJA_CDR_CD0     (1 << 0) /* CLKOUT pin Frequency 0 */
#define SJA_CDR_CD1     (1 << 1) /* CLKOUT pin Frequency 1 */
#define SJA_CDR_CD2     (1 << 2) /* CLKOUT pin Frequency 2 */
#define SJA_CDR_OFF     (1 << 3) /* CLKOUT pin Mode */
#define SJA_CDR_RXINTEN (1 << 5) /* TX1 pin Mode */
#define SJA_CDR_CBP     (1 << 6) /* Comparator Bypass */
#define SJA_CDR_MOD     (1 << 7) /* PeliCAN mode */

/* SJA_FRM */
#define SJA_FRM_DLC (1 << 0) /* Data Length Code Bit */
#define SJA_FRM_RTR (1 << 6) /* Remote Transmission Request */
#define SJA_FRM_FF  (1 << 7) /* Frame Format (EFF) */

#endif /* DRIVERS_CAN_SJA1000_H_ */
