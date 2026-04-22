/**
 * @brief SJA1000 Stand-alone CAN controller
 *
 * @author Aleksey Zhmulin
 * @date 14.04.26
 */

#ifndef DRIVERS_CAN_SJA1000_H_
#define DRIVERS_CAN_SJA1000_H_

#include <stdint.h>

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
#define SJA_RXERR 0x0e /* RX/TX Error Counter Register */
#define SJA_TXERR 0x0f /* TX Error Counter Register */
#define SJA_BUF   0x10 /* RX/TX Buffer Register */
#define SJA_RMC   0x1d /* RX Message Counter Register */
#define SJA_RBSA  0x1e /* Rx Buffer Start Addr Register */
#define SJA_CDR   0x1f /* Clock Divider Register */

// #define SJA_ID0  0x11 /* ID for standard and extended frames */
// #define SJA_ID1  0x12
// #define SJA_ID2  0x13 /* ID cont. for extended frames */
// #define SJA_ID3  0x14
// #define SJA_DATS 0x13 /* Data start standard frame */
// #define SJA_DATE 0x15 /* Data start extended frame */
// #define SJA_ACR0 0x10 /* Acceptance Code (4 bytes) in RESET mode */
// #define SJA_AMR0 0x14 /* Acceptance Mask (4 bytes) in RESET mode */

#endif /* DRIVERS_CAN_SJA1000_H_ */
