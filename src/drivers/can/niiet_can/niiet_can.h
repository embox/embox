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


/* hardware */
#define NIIET_CAN_NODE_MAX        2
#define NIIET_CAN_MSG_MAX       128

/* software */
#define NIIET_MO_RX_MAX          16
#define NIIET_MO_TX_MAX          16

#define CAN_NODE_LIST_ID(node)   (node + 1)

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

#define CAN_NODE_NCR_INIT   (1 << CAN_NODE_NCR_INIT_Pos)
#define CAN_NODE_NCR_TRIE   (1 << CAN_NODE_NCR_TRIE_Pos)
#define CAN_NODE_NCR_LECIE  (1 << CAN_NODE_NCR_LECIE_Pos)
#define CAN_NODE_NCR_ALIE   (1 << CAN_NODE_NCR_ALIE_Pos)
#define CAN_NODE_NCR_CANDIS (1 << CAN_NODE_NCR_CANDIS_Pos)
#define CAN_NODE_NCR_CCE    (1 << CAN_NODE_NCR_CCE_Pos)
#define CAN_NODE_NCR_CALM   (1 << CAN_NODE_NCR_CALM_Pos)

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
#define CAN_NODE_NPCR_LBM_Pos 8

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
	struct niiet_can_node_regs Node[NIIET_CAN_NODE_MAX];
};

/* PANCTR */
#define CAN_PANCTR_PANCMD_Pos 0
#define CAN_PANCTR_BUSY_Pos   8
#define CAN_PANCTR_RBUSY_Pos  9
#define CAN_PANCTR_PANAR1_Pos 16
#define CAN_PANCTR_PANAR2_Pos 24

#define CAN_PANCTR_BUSY       (1 << CAN_PANCTR_BUSY_Pos)
#define CAN_PANCTR_RBUSY      (1 << CAN_PANCTR_RBUSY_Pos)

#define CAN_PANCTR_PANCMD_INIT       0x01
#define CAN_PANCTR_PANCMD_STAT_INC   0x02
#define CAN_PANCTR_PANCMD_DYN_INC    0x03
#define CAN_PANCTR_PANCMD_MOV_UP     0x04
#define CAN_PANCTR_PANCMD_INS_UP     0x05
#define CAN_PANCTR_PANCMD_MOV_DOWN   0x06
#define CAN_PANCTR_PANCMD_INS_DOWN   0x07

struct niiet_canmsg_msg_regs {
	volatile uint32_t MOFCR;
	volatile uint32_t MOFGPR;
	volatile uint32_t MOIPR;
	volatile uint32_t MOAMR;
	volatile uint32_t MODATAL;
	volatile uint32_t MODATAH;
	volatile uint32_t MOAR;
	volatile uint32_t MOCTR; /* MOSTAT*/
};

struct niiet_canmsg_regs {
	struct niiet_canmsg_msg_regs Msg[NIIET_CAN_MSG_MAX];
};

#define CANMSG_MOCTR_RESRXPND_Pos  0
#define CANMSG_MOCTR_RESTXPND_Pos  1
#define CANMSG_MOCTR_RESRXUPD_Pos  2
#define CANMSG_MOCTR_RESNEWDAT_Pos 3
#define CANMSG_MOCTR_RESMSGLST_Pos 4
#define CANMSG_MOCTR_RESMSGVAL_Pos 5
#define CANMSG_MOCTR_RESRTSEL_Pos  6
#define CANMSG_MOCTR_RESRXEN_Pos   7
#define CANMSG_MOCTR_RESTXRQ_Pos   8
#define CANMSG_MOCTR_RESTXEN0_Pos  9
#define CANMSG_MOCTR_RESTXEN1_Pos  10
#define CANMSG_MOCTR_RESDIR_Pos    11
#define CANMSG_MOCTR_SETRXPND_Pos  16
#define CANMSG_MOCTR_SETTXPND_Pos  17
#define CANMSG_MOCTR_SETRXUPD_Pos  18
#define CANMSG_MOCTR_SETNEWDAT_Pos 19
#define CANMSG_MOCTR_SETMSGLST_Pos 20
#define CANMSG_MOCTR_SETMSGVAL_Pos 21
#define CANMSG_MOCTR_SETRTSEL_Pos  22
#define CANMSG_MOCTR_SETRXEN_Pos   23
#define CANMSG_MOCTR_SETTXRQ_Pos   24
#define CANMSG_MOCTR_SETTXEN0_Pos  25
#define CANMSG_MOCTR_SETTXEN1_Pos  26
#define CANMSG_MOCTR_SETDIR_Pos    27

#define CANMSG_MOCTR_RESRXPND    (1 << CANMSG_MOCTR_RESRXPND_Pos )
#define CANMSG_MOCTR_RESTXPND    (1 << CANMSG_MOCTR_RESTXPND_Pos )
#define CANMSG_MOCTR_RESRXUPD    (1 << CANMSG_MOCTR_RESRXUPD_Pos )
#define CANMSG_MOCTR_RESNEWDAT   (1 << CANMSG_MOCTR_RESNEWDAT_Pos)
#define CANMSG_MOCTR_RESMSGLST   (1 << CANMSG_MOCTR_RESMSGLST_Pos)
#define CANMSG_MOCTR_RESMSGVAL   (1 << CANMSG_MOCTR_RESMSGVAL_Pos)
#define CANMSG_MOCTR_RESRTSEL    (1 << CANMSG_MOCTR_RESRTSEL_Pos )
#define CANMSG_MOCTR_RESRXEN     (1 << CANMSG_MOCTR_RESRXEN_Pos  )
#define CANMSG_MOCTR_RESTXRQ     (1 << CANMSG_MOCTR_RESTXRQ_Pos  )
#define CANMSG_MOCTR_RESTXEN0    (1 << CANMSG_MOCTR_RESTXEN0_Pos )
#define CANMSG_MOCTR_RESTXEN1    (1 << CANMSG_MOCTR_RESTXEN1_Pos )
#define CANMSG_MOCTR_RESDIR      (1 << CANMSG_MOCTR_RESDIR_Pos   )
#define CANMSG_MOCTR_SETRXPND    (1 << CANMSG_MOCTR_SETRXPND_Pos )
#define CANMSG_MOCTR_SETTXPND    (1 << CANMSG_MOCTR_SETTXPND_Pos )
#define CANMSG_MOCTR_SETRXUPD    (1 << CANMSG_MOCTR_SETRXUPD_Pos )
#define CANMSG_MOCTR_SETNEWDAT   (1 << CANMSG_MOCTR_SETNEWDAT_Pos)
#define CANMSG_MOCTR_SETMSGLST   (1 << CANMSG_MOCTR_SETMSGLST_Pos)
#define CANMSG_MOCTR_SETMSGVAL   (1 << CANMSG_MOCTR_SETMSGVAL_Pos)
#define CANMSG_MOCTR_SETRTSEL    (1 << CANMSG_MOCTR_SETRTSEL_Pos )
#define CANMSG_MOCTR_SETRXEN     (1 << CANMSG_MOCTR_SETRXEN_Pos  )
#define CANMSG_MOCTR_SETTXRQ     (1 << CANMSG_MOCTR_SETTXRQ_Pos  )
#define CANMSG_MOCTR_SETTXEN0    (1 << CANMSG_MOCTR_SETTXEN0_Pos )
#define CANMSG_MOCTR_SETTXEN1    (1 << CANMSG_MOCTR_SETTXEN1_Pos )
#define CANMSG_MOCTR_SETDIR      (1 << CANMSG_MOCTR_SETDIR_Pos   )

#define CANMSG_MOFCR_MMC_Pos   0
#define CANMSG_MOFCR_GDFS_Pos  8
#define CANMSG_MOFCR_IDC_Pos   9
#define CANMSG_MOFCR_DLCC_Pos  10
#define CANMSG_MOFCR_DATC_Pos  11
#define CANMSG_MOFCR_RXIE_Pos  16
#define CANMSG_MOFCR_TXIE_Pos  17
#define CANMSG_MOFCR_OVIE_Pos  18
#define CANMSG_MOFCR_FRREN_Pos 20
#define CANMSG_MOFCR_RMM_Pos   21
#define CANMSG_MOFCR_SDT_Pos   22
#define CANMSG_MOFCR_STT_Pos   23
#define CANMSG_MOFCR_DLC_Pos   24

#define CANMSG_MOFCR_MMC(mmc) (mmc << CANMSG_MOFCR_MMC_Pos)
#define CANMSG_MOFCR_GDFS     (1 << CANMSG_MOFCR_GDFS_Pos)
#define CANMSG_MOFCR_IDC      (1 << CANMSG_MOFCR_IDC_Pos)
#define CANMSG_MOFCR_DLCC     (1 << CANMSG_MOFCR_DLCC_Pos)
#define CANMSG_MOFCR_DATC     (1 << CANMSG_MOFCR_DATC_Pos)
#define CANMSG_MOFCR_RXIE     (1 << CANMSG_MOFCR_RXIE_Pos)
#define CANMSG_MOFCR_TXIE     (1 << CANMSG_MOFCR_TXIE_Pos)
#define CANMSG_MOFCR_OVIE     (1 << CANMSG_MOFCR_OVIE_Pos)
#define CANMSG_MOFCR_FRREN    (1 << CANMSG_MOFCR_FRREN_Pos)
#define CANMSG_MOFCR_RMM      (1 << CANMSG_MOFCR_RMM_Pos)
#define CANMSG_MOFCR_SDT      (1 << CANMSG_MOFCR_SDT_Pos)
#define CANMSG_MOFCR_STT      (1 << CANMSG_MOFCR_STT_Pos)
#define CANMSG_MOFCR_DLC(dlc) (dlc << CANMSG_MOFCR_DLC_Pos)

#define CANMSG_MOAR_ID_EFF_Pos  0
#define CANMSG_MOAR_ID_SFF_Pos  18
#define CANMSG_MOAR_IDE_Pos     29
#define CANMSG_MOAR_PRI_Pos     30

#define CANMSG_MOAR_ID_EFF(id) (id << CANMSG_MOAR_ID_EFF_Pos)
#define CANMSG_MOAR_ID_SFF(id) (id << CANMSG_MOAR_ID_SFF_Pos)
#define CANMSG_MOAR_IDE        (1 << CANMSG_MOAR_IDE_Pos)
#define CANMSG_MOAR_PRI(pri)   (pri << CANMSG_MOAR_PRI_Pos)

#define NIIET_MO_DESC_INITED   (1 << 0)
#define NIIET_MO_DESC_USED     (1 << 1)

struct niiet_mo_desc {
	int num;
	uint32_t flags;
};

struct niiet_node_desc {
	struct niiet_mo_desc rx_mo_array[NIIET_MO_RX_MAX];
	struct niiet_mo_desc tx_mo_array[NIIET_MO_TX_MAX];
};

struct niiet_can_dev_priv {
	struct can_dev *can_dev;
	struct niiet_can_regs *regs;
	struct niiet_canmsg_regs *canmsg_regs;
	int can_node_max;

	struct niiet_node_desc node_desc[NIIET_CAN_NODE_MAX];
};

extern void niiet_can_bconf_init(struct niiet_can_dev_priv *niiet_can);

#endif /* DRIVERS_CAN_NIIET_CAN_H_ */
