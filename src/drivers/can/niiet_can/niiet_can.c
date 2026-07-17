/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.07.26
 */

#include <errno.h>
#include <linux/can.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#include <drivers/can_dev.h>

#include <embox/unit.h>
#include <framework/mod/options.h>

#include <hal/reg.h>
#include <kernel/irq.h>
#include <util/field.h>

#include <config/board_config.h>

#include "niiet_can.h"


EMBOX_UNIT_INIT(niiet_can_init);

#define CAN_DEV_ID    OPTION_GET(NUMBER, dev_id)
// FIXME use from bconf
#define CAN_BASE_ADDR OPTION_GET(NUMBER, base_addr)

#define CAN_REG_STORE(reg, val)  REG32_STORE(CAN_BASE_ADDR + reg, val)
#define CAN_REG_LOAD(reg)        REG32_LOAD(CAN_BASE_ADDR + reg)
#define CAN_REG_ORIN(reg, mask)  REG32_ORIN(CAN_BASE_ADDR + reg, mask)
#define CAN_REG_ANDIN(reg, mask) REG32_ANDIN(CAN_BASE_ADDR + reg, mask)
#define CAN_REG_CLEAR(reg, mask) REG32_CLEAR(CAN_BASE_ADDR + reg, mask)

static inline void niiet_can_mo_move_to_list(struct niiet_can_regs *regs,
    							int idx, int list) {
	regs->PANCTR = (CAN_PANCTR_PANCMD_STAT_INC << CAN_PANCTR_PANCMD_Pos)
	               | (idx << CAN_PANCTR_PANAR1_Pos)
	               | (list << CAN_PANCTR_PANAR2_Pos);

	while (regs->PANCTR & (CAN_PANCTR_BUSY | CAN_PANCTR_RBUSY)) {};
}

static inline int niiet_can_get_free_mo(struct niiet_can_dev_priv *priv, int node) {
	struct niiet_node_desc *node_desc;
	int i;

	node_desc = &priv->node_desc[node];
	for (i = 0; i < NIIET_MO_TX_MAX; i++) {
		if (0 != (node_desc->tx_mo_array[i].flags & NIIET_MO_DESC_USED)) {
			return node_desc->tx_mo_array[i].num;
		}
	}

	return -1;
}

static inline void niiet_can_mo_conf_tx(struct niiet_canmsg_regs *canmsg_regs,
    			int obj_num, struct can_frame *frame) {
	uint32_t flags = 0;

	if (frame->can_id & CAN_RTR_FLAG) {
		flags = CANMSG_MOCTR_RESDIR;
	}
	else {
		flags = CANMSG_MOCTR_SETDIR;
	}
	flags |= CANMSG_MOCTR_SETTXEN0 | CANMSG_MOCTR_SETTXEN1;

	canmsg_regs->Msg[obj_num].MOCTR = flags;
	canmsg_regs->Msg[obj_num].MOFCR = CANMSG_MOFCR_DLC(frame->len); /* CANMSG_MOFCR_TXIE */

	if (frame->can_id & CAN_EFF_FLAG) {
		flags = CANMSG_MOAR_ID_EFF(frame->can_id & CAN_EFF_MASK);
		flags |= CANMSG_MOAR_IDE;
	} else {
		flags = CANMSG_MOAR_ID_SFF(frame->can_id & CAN_SFF_MASK);

	}
	flags |= CANMSG_MOAR_PRI(0x2); /* by identifier + IDE + DIR */
	canmsg_regs->Msg[obj_num].MOAR = flags;

	/* for FIFO canmsg_regs->Msg[obj_num].MOFGPR */
	/* with IRQ canmsg_regs->Msg[obj_num].MOIPR */

	flags  = frame->data[0];
	flags |= frame->data[1] << 8;
	flags |= frame->data[2] << 16;
	flags |= frame->data[3] << 24;
	canmsg_regs->Msg[obj_num].MODATAL = flags;

	flags  = frame->data[4];
	flags |= frame->data[5] << 8;
	flags |= frame->data[6] << 16;
	flags |= frame->data[7] << 24;
	canmsg_regs->Msg[obj_num].MODATAH = flags;

}

static inline void niiet_can_mo_conf_rx(struct niiet_canmsg_regs *canmsg_regs,
					int obj_num, struct can_frame *frame) {
	uint32_t flags = 0;

	if (frame->can_id & CAN_RTR_FLAG) {
		flags = CANMSG_MOCTR_SETDIR;
	}
	else {
		flags = CANMSG_MOCTR_RESDIR;
	}
	flags |= CANMSG_MOCTR_SETRXEN;

	canmsg_regs->Msg[obj_num].MOCTR = flags;
	/* canmsg_regs->Msg[obj_num].MOAMR */
}


static void niiet_can_mo_send(struct niiet_canmsg_regs *canmsg_regs, int obj_num) {
    canmsg_regs->Msg[obj_num].MOCTR = CANMSG_MOCTR_SETTXRQ | CANMSG_MOCTR_SETMSGVAL;
}

#if 0
void CAN_Object_Receive(uint32_t obj_num)
{
    CANMSG->Msg[obj_num].MOCTR = CANMSG_Msg_MOCTR_SETMSGVAL_Msk;
}
#endif

static void niiet_can_config(struct can_dev *can) {
	uint32_t reg;

	CAN_REG_STORE(CAN_CLC, 0);

	while (CAN_REG_LOAD(CAN_CLC) & CAN_CLC_DISS) {}	

	reg = CAN_REG_LOAD(CAN_FDR);
	reg = FIELD_SET(reg, CAN_FDR_STEP, 0x3ff);
	reg = FIELD_SET(reg, CAN_FDR_DM, CAN_FDR_DM_NORM);
	CAN_REG_STORE(CAN_FDR, reg);
}

static inline int niiet_can_node_init(struct can_dev *can, int node_num) {
	struct niiet_can_dev_priv *priv;
	struct niiet_can_regs *regs;
	struct niiet_can_node_regs *node;
	struct niiet_node_desc *node_desc;
	struct niiet_mo_desc *mo_desc;
	int i;

	priv = can->priv;
	regs = priv->regs;
	node = &regs->Node[node_num];

	node->NCR = CAN_NODE_NCR_CCE | CAN_NODE_NCR_INIT;
	/* write regs NBTRx & NPCRx */
	node->NBTR = 2305;/* Tbit = 1 / (1 Мбит/с) */

	node->NIPR = (1 << CAN_NODE_NIPR_TRINP_Pos);

	//for loopback mode
	//node->NPCR = (1 << CAN_NODE_NPCR_LBM_Pos);

	/* lock writing & IRQ EN*/
	node->NCR = CAN_NODE_NCR_TRIE;

	/* first MO set as RX (0) than as TX (0) than  set node 1 */
	node_desc = &priv->node_desc[0];

	for (i = 0; i < (NIIET_MO_RX_MAX); i++) {
		mo_desc = &node_desc->rx_mo_array[i];
		mo_desc->flags = NIIET_MO_DESC_INITED;
		mo_desc->num   = i; 
		niiet_can_mo_move_to_list(regs, i, CAN_NODE_LIST_ID(0));
	}

	for (i = 0; i < (NIIET_MO_RX_MAX + NIIET_MO_TX_MAX) ; i++) {
		mo_desc = &node_desc->tx_mo_array[i];
		mo_desc->flags = NIIET_MO_DESC_INITED;
		mo_desc->num   = i; 
		niiet_can_mo_move_to_list(regs, i, CAN_NODE_LIST_ID(0));
	}

	return 0;
}

static int niiet_can_open(struct can_dev *can) {
	int node_num;

	node_num = 0;
	niiet_can_node_init(can, node_num);

	return 0;
}

static void niiet_can_close(struct can_dev *can) {
}

static int niiet_can_send(struct can_dev *can, const void *data) {
	struct can_frame *frame;
	struct niiet_can_dev_priv *priv;
	int mo_idx;
	uint8_t frame_len;

	priv = can->priv;
	frame = (struct can_frame *)data;

	frame_len = frame->len;
	if (frame_len > 8) {
		frame_len = 8;
	}

	mo_idx = niiet_can_get_free_mo(can->priv, 0);
	if (mo_idx == -1) {
		log_error("couldn't get free MO");
		return -1;
	}

	niiet_can_mo_conf_tx(priv->canmsg_regs, mo_idx, frame);

	niiet_can_mo_send(priv->canmsg_regs, mo_idx);

	return 0;
}	

static const struct can_dev_ops niiet_can_ops = {
    .cdo_config = niiet_can_config,
    .cdo_open = niiet_can_open,
    .cdo_close = niiet_can_close,
    .cdo_send = niiet_can_send,
};

struct niiet_can_dev_priv niiet_can_dev_priv;
CAN_DEVICE_DEF(niiet_can_dev, &niiet_can_ops, &niiet_can_dev_priv, CAN_DEV_ID);

static inline void niiet_can_receive(struct can_dev *can) {
	struct can_frame frame;

	can_dev_receive(can, &frame);
}

static irq_return_t niiet_can_irq_handler(unsigned int irq_num, void *data) {
	struct can_dev *can;

	can = (struct can_dev *)data;

	/* Clear pending interrupts */
	// sja_reg_orin(sja_base, SJA_IR, 0);

	/* Check if RX buffer is not empty */
	// while (sja_reg_load(sja_base, SJA_SR) & SJA_SR_RBS) {
	// 	niiet_can_receive(can);
	// }

	can_dev_notify(can);

	return IRQ_HANDLED;
}

static int niiet_can_init(void) {
	int res;

	niiet_can_dev_priv.can_dev = &niiet_can_dev;
	niiet_can_dev_priv.regs = (struct niiet_can_regs *)(uintptr_t)CAN_BASE_ADDR;

#if defined (CONF_CAN_IRQ_NODE0)
	res = irq_attach(CONF_CAN_IRQ_NODE0, niiet_can_irq_handler, 0,
						(void *)&niiet_can_dev, "can_node0");
	if (res < 0) {
		return res;
	}
#endif /* defined (CONF_CAN_IRQ_NODE0) */

#if defined (CONF_CAN_IRQ_NODE1)
	res = irq_attach(CONF_CAN_IRQ_NODE1, niiet_can_irq_handler, 0,
						(void *)&niiet_can_dev, "can_node0");
	if (res < 0) {
		return res;
	}
#endif /* defined (CONF_CAN_IRQ_NODE0) */

	niiet_can_bconf_init(&niiet_can_dev_priv);

	return 0;
}
