/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.07.26
 */

#ifndef DRIVERS_CAN_NIIET_CAN_H_
#define DRIVERS_CAN_NIIET_CAN_H_

#include <stdint.h>

#include <config/board_config.h>

/* Global Module Registers */
#define CAN_CLC     0x000 /* Clock Control Register */
#define CAN_ID      0x008 /* Module Identification Register */
#define CAN_FDR     0x00c /* Frequency Divider Register */
#define CAN_LIST    0x100 /* List Register 0 */
#define CAN_MSPND   0x140 /* Message Pending Register 0 */
#define CAN_MSID    0x180 /* Message Index Register 0 */
#define CAN_MSIMASK 0x1c0 /* Message Index Mask Register 0 */
#define CAN_PANCTR  0x1c4 /* Panel Control Register */
#define CAN_MCR     0x1c8 /* Module Control Register */
#define CAN_MITR    0x1cc /* Module Interrupt Trigger Register */

#define CAN_NODE0_BASE   0x200
#define CAN_NODE1_BASE   0x300

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

struct niiet_can_node_regs {
	volatile uint32_t NCR;
	volatile uint32_t NSR;
	volatile uint32_t NIPR;
	volatile uint32_t NPCR;
	volatile uint32_t NBTR;
	volatile uint32_t NECNT;
	volatile uint32_t NFCR;
	volatile uint32_t Reserved0[57];
};

/* NCR */
#define CAN_NODE_NCR_INIT_Pos   0
#define CAN_NODE_NCR_TRIE_Pos   1
#define CAN_NODE_NCR_LECIE_Pos  2
#define CAN_NODE_NCR_ALIE_Pos   3
#define CAN_NODE_NCR_CANDIS_Pos 4
#define CAN_NODE_NCR_CCE_Pos    6
#define CAN_NODE_NCR_CALM_Pos   7

#define CAN_NODE_NCR_INIT       (1 << CAN_NODE_NCR_INIT_Pos)
#define CAN_NODE_NCR_TRIE       (1 << CAN_NODE_NCR_TRIE_Pos)
#define CAN_NODE_NCR_LECIE      (1 << CAN_NODE_NCR_LECIE_Pos)
#define CAN_NODE_NCR_ALIE       (1 << CAN_NODE_NCR_ALIE_Pos)
#define CAN_NODE_NCR_CANDIS     (1 << CAN_NODE_NCR_CANDIS_Pos)
#define CAN_NODE_NCR_CCE        (1 << CAN_NODE_NCR_CCE_Pos)
#define CAN_NODE_NCR_CALM       (1 << CAN_NODE_NCR_CALM_Pos)

/* NBTR  */
#define CAN_NODE_NBTR_BRP_Pos   0  /*!< Prescaler rate */
#define CAN_NODE_NBTR_SJW_Pos   6  /*!< Transition width resynchronization */
#define CAN_NODE_NBTR_TSEG1_Pos 8  /*!< Parameter 1 */
#define CAN_NODE_NBTR_TSEG2_Pos 12 /*!< Parameter 2 */
#define CAN_NODE_NBTR_DIV8_Pos  15 /*!< Frequency divider by 8 */

/* NIPR */
#define CAN_NODE_NIPR_ALINP_Pos  0
#define CAN_NODE_NIPR_LECINP_Pos 4
#define CAN_NODE_NIPR_TRINP_Pos  8
#define CAN_NODE_NIPR_CFCINP_Pos 12

/* Bit field positions: */
#define CAN_NODE_NPCR_LBM_Pos    8

struct niiet_can_regs {
	volatile uint32_t CLC;
	volatile uint32_t Reserved0;
	volatile uint32_t ID;
	volatile uint32_t FDR;
	volatile uint32_t Reserved1[60];
	volatile uint32_t LIST[8];
	volatile uint32_t Reserved2[8];
	volatile uint32_t MSPND[8];
	volatile uint32_t Reserved3[8];
	volatile uint32_t MSID[8];
	volatile uint32_t Reserved4[8];
	volatile uint32_t MSIMASK;
	volatile uint32_t PANCTR;
	volatile uint32_t MCR;
	volatile uint32_t MITR;
	volatile uint32_t Reserved5[12];
	struct niiet_can_node_regs Node[2];
};

struct niiet_canmsg_regs {
	volatile uint32_t MOFCR;
	volatile uint32_t MOFGPR;
	volatile uint32_t MOIPR;
	volatile uint32_t MOAMR;
	volatile uint32_t MODATAL;
	volatile uint32_t MODATAH;
	volatile uint32_t MOAR;
	volatile uint32_t MOCTR; /* MOSTAT*/
};

struct niiet_can_dev_priv {
    struct can_dev *can_dev;
    struct niiet_can_regs *regs;
    struct niiet_canmsg_regs *canmsg_regs;
    int can_node_max;
};

extern void niiet_can_bconf_init(struct niiet_can_dev_priv *niiet_can);

#endif /* DRIVERS_CAN_NIIET_CAN_H_ */
