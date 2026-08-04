/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.07.26
 */

#ifndef DRIVERS_CAN_NIIET_CAN_H_
#define DRIVERS_CAN_NIIET_CAN_H_

#include <stdint.h>

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

/* CAN Node Registers */
struct niiet_can_node {
	volatile uint32_t NCR;   /* Node Control Register */
	volatile uint32_t NSR;   /* Node Status Register */
	volatile uint32_t NIPR;  /* Node Interrupt Pointer Register */
	volatile uint32_t NPCR;  /* Node Port Control Register */
	volatile uint32_t NBTR;  /* Node Bit Timing Register */
	volatile uint32_t NECNT; /* Node Error Counter Register */
	volatile uint32_t NFCR;  /* Node Frame Counter Register */
};

/* Message Object Registers */
struct niiet_can_mo {
	volatile uint32_t MOFCR;   /* MO Function Control Register */
	volatile uint32_t MOFGPR;  /* MO FIFO/Gateway Pointer Register */
	volatile uint32_t MOIPR;   /* MO Interrupt Pointer Register */
	volatile uint32_t MOAMR;   /* MO Acceptance Mask Register */
	volatile uint32_t MODATAL; /* MO Data Low Register */
	volatile uint32_t MODATAH; /* MO Data High Register */
	volatile uint32_t MOAR;    /* MO Arbitration Register */
	union {
		volatile const uint32_t MOSTAT; /* MO Control Register */
		volatile uint32_t MOCTR;        /* MO Status Register */
	};
}

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
#define CAN_PANCTR_BUSY  (1 << 8) /* Panel Busy Flag */
#define CAN_PANCTR_RBUSY (1 << 9) /* Result Busy Flag */

#define CAN_PANCTR_PANCMD       /* Panel Command */
#define CAN_PANCTR_PANCMD_MASK  0xff
#define CAN_PANCTR_PANCMD_SHIFT 0
#define CAN_PANCTR_PANCMD_INIT  0x1 /* Initialize Lists */
#define CAN_PANCTR_PANCMD_ALLOC 0x2 /* Allocate Message Object to a List */

#define CAN_PANCTR_PANAR1       /* Panel Argument 1 */
#define CAN_PANCTR_PANAR1_MASK  0xff
#define CAN_PANCTR_PANAR1_SHIFT 16

#define CAN_PANCTR_PANAR2       /* Panel Argument 2 */
#define CAN_PANCTR_PANAR2_MASK  0xff
#define CAN_PANCTR_PANAR2_SHIFT 24

/* CAN_NCR */
#define CAN_NCR_INIT   (1 << 0) /* Node Initialization */
#define CAN_NCR_TRIE   (1 << 1) /* Transfer Interrupt Enable */
#define CAN_NCR_LECIE  (1 << 2) /* Last Error Code Interrupt Enable */
#define CAN_NCR_ALIE   (1 << 3) /* Alert Interrupt Enable */
#define CAN_NCR_CANDIS (1 << 4) /* CAN Disable */
#define CAN_NCR_TXDIS  (1 << 5) /* Transmit Disable */
#define CAN_NCR_CCE    (1 << 6) /* Configuration Change Enable */
#define CAN_NCR_CALM   (1 << 7) /* CAN Analyzer Mode */

/* CAN_NIPR */
#define CAN_NIPR_ALINP       /* Alert Interrupt Node Pointer */
#define CAN_NIPR_ALINP_MASK  0xf
#define CAN_NIPR_ALINP_SHIFT 0

#define CAN_NIPR_LECINP       /* Last Error Code Interrupt Node Pointer */
#define CAN_NIPR_LECINP_MASK  0xf
#define CAN_NIPR_LECINP_SHIFT 4

#define CAN_NIPR_TRINP       /* Transfer Interrupt Node Pointer */
#define CAN_NIPR_TRINP_MASK  0xf
#define CAN_NIPR_TRINP_SHIFT 8

/* CAN_NBTR */
#define CAN_NBTR_DIV8 (1 << 15) /* Divide Prescaler Clock by 8 */

#define CAN_NBTR_BRP       /* Baud Rate Prescaler */
#define CAN_NBTR_BRP_MASK  0x3f
#define CAN_NBTR_BRP_SHIFT 0

#define CAN_NBTR_SJW       /* Synchronization Jump Width */
#define CAN_NBTR_SJW_MASK  0x3
#define CAN_NBTR_SJW_SHIFT 6

#define CAN_NBTR_TSEG1       /* Time Segment 1 */
#define CAN_NBTR_TSEG1_MASK  0xf
#define CAN_NBTR_TSEG1_SHIFT 8

#define CAN_NBTR_TSEG2       /* Time Segment 2 */
#define CAN_NBTR_TSEG2_MASK  0x7
#define CAN_NBTR_TSEG2_SHIFT 12

/* CAN_NPCR */
#define CAN_NPCR_LBM (1 << 8) /* Loop-Back Mode */

/* CAN_MOFCR */
#define CAN_MOFCR_GDFS  (1 << 8)  /* Gateway Data Frame Send */
#define CAN_MOFCR_IDC   (1 << 9)  /* Identifier Copy */
#define CAN_MOFCR_DLCC  (1 << 10) /* Data Length Code Copy */
#define CAN_MOFCR_DATC  (1 << 11) /* Data Copy */
#define CAN_MOFCR_RXIE  (1 << 16) /* Receive Interrupt Enable */
#define CAN_MOFCR_TXIE  (1 << 17) /* Transmit Interrupt Enable */
#define CAN_MOFCR_OVIE  (1 << 18) /* Overflow Interrupt Enable */
#define CAN_MOFCR_FRREN (1 << 20) /* Foreign Remote Request Enable */
#define CAN_MOFCR_RMM   (1 << 21) /* Transmit Object Remote Monitoring */
#define CAN_MOFCR_SDT   (1 << 22) /* Single Data Transfer */
#define CAN_MOFCR_STT   (1 << 23) /* Single Transmit Trial */

#define CAN_MOFCR_MMC       /* Message Mode Control */
#define CAN_MOFCR_MMC_MASK  0xf
#define CAN_MOFCR_MMC_SHIFT 0

#define CAN_MOFCR_DLC       /* Data Length Code */
#define CAN_MOFCR_DLC_MASK  0xf
#define CAN_MOFCR_DLC_SHIFT 24

/* CAN_MOAR */
#define CAN_MOAR_IDE (1 << 29) /* Identifier Extension */

#define CAN_MOAR_ID       /* CAN Identifier */
#define CAN_MOAR_ID_MASK  0x1fffffff
#define CAN_MOAR_ID_SHIFT 0

#define CAN_MOAR_PRI       /* Priority Class */
#define CAN_MOAR_PRI_MASK  0x3
#define CAN_MOAR_PRI_SHIFT 30

/* CAN_MOCTR */
#define CAN_MOCTR_CLR_RXPND  (1 << 0)  /* Clear Receive Pending */
#define CAN_MOCTR_CLR_TXPND  (1 << 1)  /* Clear Transmit Pending */
#define CAN_MOCTR_CLR_RXUPD  (1 << 2)  /* Clear Receive Updating */
#define CAN_MOCTR_CLR_NEWDAT (1 << 3)  /* Clear New Data */
#define CAN_MOCTR_CLR_MSGLST (1 << 4)  /* Clear Message Lost */
#define CAN_MOCTR_CLR_MSGVAL (1 << 5)  /* Clear Message Valid */
#define CAN_MOCTR_CLR_RTSEL  (1 << 6)  /* Clear RX/TX Selected */
#define CAN_MOCTR_CLR_RXEN   (1 << 7)  /* Clear Receive Enable */
#define CAN_MOCTR_CLR_TXRQ   (1 << 8)  /* Clear Transmit Request */
#define CAN_MOCTR_CLR_TXEN0  (1 << 9)  /* Clear Transmit Enable 0 */
#define CAN_MOCTR_CLR_TXEN1  (1 << 10) /* Clear Transmit Enable 1 */
#define CAN_MOCTR_CLR_DIR    (1 << 11) /* Clear Message Direction */
#define CAN_MOCTR_SET_RXPND  (1 << 16) /* Set Receive Pending */
#define CAN_MOCTR_SET_TXPND  (1 << 17) /* Set Transmit Pending */
#define CAN_MOCTR_SET_RXUPD  (1 << 18) /* Set Receive Updating */
#define CAN_MOCTR_SET_NEWDAT (1 << 19) /* Set New Data */
#define CAN_MOCTR_SET_MSGLST (1 << 20) /* Set Message Lost */
#define CAN_MOCTR_SET_MSGVAL (1 << 21) /* Set Message Valid */
#define CAN_MOCTR_SET_RTSEL  (1 << 22) /* Set RX/TX Selected */
#define CAN_MOCTR_SET_RXEN   (1 << 23) /* Set Receive Enable */
#define CAN_MOCTR_SET_TXRQ   (1 << 24) /* Set Transmit Request */
#define CAN_MOCTR_SET_TXEN0  (1 << 25) /* Set Transmit Enable 0 */
#define CAN_MOCTR_SET_TXEN1  (1 << 26) /* Set Transmit Enable 1 */
#define CAN_MOCTR_SET_DIR    (1 << 27) /* Set Message Direction */

/* CAN_MOSTAT */
#define CAN_MOSTAT_RXPND  (1 << 0)  /* Receive Pending */
#define CAN_MOSTAT_TXPND  (1 << 1)  /* Transmit Pending */
#define CAN_MOSTAT_RXUPD  (1 << 2)  /* Receive Updating */
#define CAN_MOSTAT_NEWDAT (1 << 3)  /* New Data */
#define CAN_MOSTAT_MSGLST (1 << 4)  /* Message Lost */
#define CAN_MOSTAT_MSGVAL (1 << 5)  /* Message Valid */
#define CAN_MOSTAT_RTSEL  (1 << 6)  /* RX/TX Selected */
#define CAN_MOSTAT_RXEN   (1 << 7)  /* Receive Enable */
#define CAN_MOSTAT_TXRQ   (1 << 8)  /* Transmit Request */
#define CAN_MOSTAT_TXEN0  (1 << 9)  /* Transmit Enable 0 */
#define CAN_MOSTAT_TXEN1  (1 << 10) /* Transmit Enable 1 */
#define CAN_MOSTAT_DIR    (1 << 11) /* Message Direction */

#define CAN_MOSTAT_LIST       /* List Allocation */
#define CAN_MOSTAT_LIST_MASK  0xf
#define CAN_MOSTAT_LIST_SHIFT 12

/* CAN Node Registers */
#define CAN_NCR(n)   (0x200 + 0x100 * n) /* Node Control Register */
#define CAN_NSR(n)   (0x204 + 0x100 * n) /* Node Status Register */
#define CAN_NIPR(n)  (0x208 + 0x100 * n) /* Node Interrupt Pointer Register */
#define CAN_NPCR(n)  (0x20c + 0x100 * n) /* Node Port Control Register */
#define CAN_NBTR(n)  (0x210 + 0x100 * n) /* Node Bit Timing Register */
#define CAN_NECNT(n) (0x214 + 0x100 * n) /* Node Error Counter Register */
#define CAN_NFCR(n)  (0x218 + 0x100 * n) /* Node Frame Counter Register */

/* Message Object Registers */
#define CAN_MOFCR(n)   (0x1000 + 0x20 * n) /* MO Function Control Register */
#define CAN_MOFGPR(n)  (0x1004 + 0x20 * n) /* MO FIFO/Gateway Pointer Register */
#define CAN_MOIPR(n)   (0x1008 + 0x20 * n) /* MO Interrupt Pointer Register */
#define CAN_MOAMR(n)   (0x100c + 0x20 * n) /* MO Acceptance Mask Register */
#define CAN_MODATAL(n) (0x1010 + 0x20 * n) /* MO Data Low Register */
#define CAN_MODATAH(n) (0x1014 + 0x20 * n) /* MO Data High Register */
#define CAN_MOAR(n)    (0x1018 + 0x20 * n) /* MO Arbitration Register */
#define CAN_MOCTR(n)   (0x101c + 0x20 * n) /* MO Control Register */
#define CAN_MOSTAT(n)  (0x101c + 0x20 * n) /* MO Status Register */

#endif /* DRIVERS_CAN_NIIET_CAN_H_ */
