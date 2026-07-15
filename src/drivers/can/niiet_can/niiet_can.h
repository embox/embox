/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.07.26
 */

#ifndef DRIVERS_CAN_NIIET_CAN_H_
#define DRIVERS_CAN_NIIET_CAN_H_

/* Global Module Registers */
#define CAN_CLC     0x00  /* Clock Control Register */
#define CAN_ID      0x08  /* Module Identification Register */
#define CAN_FDR     0x0c  /* Frequency Divider Register */
#define CAN_LIST    0x100 /* List Register 0 */
#define CAN_MSPND   0x140 /* Message Pending Register 0 */
#define CAN_MSID    0x180 /* Message Index Register 0 */
#define CAN_MSIMASK 0x1c0 /* Message Index Mask Register 0 */
#define CAN_PANCTR  0x1c4 /* Panel Control Register */
#define CAN_MCR     0x1c8 /* Module Control Register */
#define CAN_MITR    0x1cc /* Module Interrupt Trigger Register */

/* CAN Node Registers */
#define CAN_NCR   0x00 /* Node Control Register */
#define CAN_NSR   0x04 /* Node Status Register */
#define CAN_NIPR  0x08 /* Node Interrupt Pointer Register */
#define CAN_NPCR  0x0c /* Node Port Control Register */
#define CAN_NBTR  0x10 /* Node Bit Timing Register */
#define CAN_NECNT 0x14 /* Node Error Counter Register */
#define CAN_NFCR  0x18 /* Node Frame Counter Register */

/* Message Object Registers */
#define CAN_MOFCR   0x00 /* Message Object Function Control Register */
#define CAN_MOFGPR  0x04 /* Message Object FIFO/Gateway Pointer Register */
#define CAN_MOIPR   0x08 /* Message Object Interrupt Pointer Register */
#define CAN_MOAMR   0x0c /* Message Object Acceptance Mask Register */
#define CAN_MODATAL 0x10 /* Message Object Data Low Register */
#define CAN_MODATAH 0x14 /* Message Object Data High Register */
#define CAN_MOAR    0x18 /* Message Object Arbitration Register Register */
#define CAN_MOCTR   0x1c /* Message Object Control Register */
#define CAN_MOSTAT  0x1c /* Message Object Status Register */

/* CAN_CLC */
#define CAN_CLC_DISR (1 << 0) /* Module Disable Request */
#define CAN_CLC_DISS (1 << 1) /* Module Disable Status */

/* CAN_FDR */
#define CAN_FDR_STEP       /* Step Value */
#define CAN_FDR_STEP_MASK  0x3ff
#define CAN_FDR_STEP_SHIFT 0

#define CAN_FDR_DM       /* Divider Mode */
#define CAN_FDR_DM_MASK  0x3
#define CAN_FDR_DM_SHIFT 14
#define CAN_FDR_DM_NORM  0b01 /* Normal Divider Mode */
#define CAN_FDR_DM_FRACT 0b10 /* Fractional Divider Mode */

/* CAN_PANCTR */
#define CAN_PANCTR_PANCMD       /* Panel Command */
#define CAN_PANCTR_PANCMD_MASK  0xff
#define CAN_PANCTR_PANCMD_SHIFT 14

#endif /* DRIVERS_CAN_NIIET_CAN_H_ */
