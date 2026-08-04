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

#include "niiet_can_node0.h"

#define CAN_DEV_ID OPTION_GET(NUMBER, dev_id)

#define CAN_BASE_ADDR OPTION_GET(NUMBER, base_addr)
#define CAN_IRQ_NUM   OPTION_GET(NUMBER, irq_num)

#define CAN_REG_STORE(reg, val)  REG32_STORE(CAN_BASE_ADDR + reg, val)
#define CAN_REG_LOAD(reg)        REG32_LOAD(CAN_BASE_ADDR + reg)
#define CAN_REG_ORIN(reg, mask)  REG32_ORIN(CAN_BASE_ADDR + reg, mask)
#define CAN_REG_ANDIN(reg, mask) REG32_ANDIN(CAN_BASE_ADDR + reg, mask)
#define CAN_REG_CLEAR(reg, mask) REG32_CLEAR(CAN_BASE_ADDR + reg, mask)

#define CAN_INTLINE_0 0
#define CAN_INTLINE_1 1

#define CAN_LIST_0 0
#define CAN_LIST_1 1
#define CAN_LIST_2 2

#define CAN_MSG_TX 0
#define CAN_MSG_RX 1
#define CAN_MSG_NR 2

#define CAN_MOCTR_TX_MASK CAN_MOCTR_SET_TXEN0 | CAN_MOCTR_SET_TXEN1 | CAN_MOCTR_SET_MSGVAL | CAN_MOCTR_CLR_RXEN | CAN_MOCTR_CLR_RTSEL
;
static int niiet_can_ctrl_init(void) {
	uint32_t reg;

	/* Enable CAN module */
	CAN_REG_STORE(CAN_CLC, 0);
	while (CAN_REG_LOAD(CAN_CLC) & CAN_CLC_DISS) {}

	/* Setup frequency divider */
	reg = CAN_REG_LOAD(CAN_FDR);
	reg = FIELD_SET(reg, CAN_FDR_STEP, 0x3ff);
	reg = FIELD_SET(reg, CAN_FDR_DM, CAN_FDR_DM_NORM);
	CAN_REG_STORE(CAN_FDR, reg);

	return 0;
}

static void niiet_can_pancmd(uint8_t pancmd, uint8_t arg1, uint8_t arg2) {
	/* Wait if panel operation is in progress */
	while (CAN_REG_LOAD(CAN_PANCTR) & (CAN_PANCTR_BUSY | CAN_PANCTR_RBUSY)) {}

	CAN_REG_STORE(CAN_PANCTR, FIELD(CAN_PANCTR_PANCMD, pancmd)
	                              | FIELD(CAN_PANCTR_PANAR1, arg1)
	                              | FIELD(CAN_PANCTR_PANAR2, arg2));
}

static void niiet_can_config(struct can_dev *can) {
	uint32_t reg;
	int i;

	/* Disable CAN node */
	CAN_REG_ORIN(CAN_NCR(0), CAN_NCR_INIT);

	/* Enable node configuration */
	CAN_REG_ORIN(CAN_NCR(0), CAN_NCR_CCE);

	/* Set bit timing */
	CAN_REG_STORE(CAN_NBTR(0),
	    FIELD(CAN_NBTR_TSEG1, 7) | FIELD(CAN_NBTR_TSEG2, 0)
	        | FIELD(CAN_NBTR_SJW, 1) | FIELD(CAN_NBTR_BRP, 4));

	/* Set interrupt line 0 for transfer interrupt */
	CAN_REG_STORE(CAN_NIPR(0), FIELD(CAN_NIPR_TRINP, CAN_INTLINE_0));

	/* Enable/disable loopback mode */
	reg = can->conf.loopback ? CAN_NPCR_LBM : 0;
	CAN_REG_STORE(CAN_NPCR(0), reg);

	/* Allocate message objects */
	for (i = 0; i < CAN_MSG_NR; i++) {
		reg = CAN_REG_LOAD(CAN_MOSTAT(i));
		if (FIELD_GET(reg, CAN_MOSTAT_LIST) != CAN_LIST_1) {
			niiet_can_pancmd(CAN_PANCTR_PANCMD_ALLOC, i, CAN_LIST_1);
		}
	}

	/* Disable node configuration */
	CAN_REG_CLEAR(CAN_NCR(0), CAN_NCR_CCE);

#if 1 /* XXX */
	/* Disable CAN node */
	CAN_REG_ORIN(CAN_NCR(1), CAN_NCR_INIT);

	/* Enable node configuration */
	CAN_REG_ORIN(CAN_NCR(1), CAN_NCR_CCE);

	/* Set bit timing */
	CAN_REG_STORE(CAN_NBTR(1),
	    FIELD(CAN_NBTR_TSEG1, 7) | FIELD(CAN_NBTR_TSEG2, 0)
	        | FIELD(CAN_NBTR_SJW, 1) | FIELD(CAN_NBTR_BRP, 4));

	/* Set interrupt line 0 for transfer interrupt */
	CAN_REG_STORE(CAN_NIPR(1), FIELD(CAN_NIPR_TRINP, CAN_INTLINE_1));

	/* Enable/disable loopback mode */
	CAN_REG_STORE(CAN_NPCR(1), CAN_NPCR_LBM);

	/* Allocate message objects */
	for (i = 2; i < 4; i++) {
		reg = CAN_REG_LOAD(CAN_MOSTAT(i));
		if (FIELD_GET(reg, CAN_MOSTAT_LIST) != CAN_LIST_2) {
			niiet_can_pancmd(CAN_PANCTR_PANCMD_ALLOC, i, CAN_LIST_2);
		}
	}

	/* Disable node configuration */
	CAN_REG_CLEAR(CAN_NCR(1), CAN_NCR_CCE);
#endif
}

static int niiet_can_open(struct can_dev *can) {
	/* Enable CAN node */
	CAN_REG_CLEAR(CAN_NCR(0), CAN_NCR_INIT);

	/* Enable transfer interrupt */
	CAN_REG_ORIN(CAN_NCR(0), CAN_NCR_TRIE);

#if 1 /* XXX */
	CAN_REG_CLEAR(CAN_NCR(1), CAN_NCR_INIT);
#endif

	return 0;
}

static void niiet_can_close(struct can_dev *can) {
#if 1 /* XXX */
	CAN_REG_ORIN(CAN_NCR(1), CAN_NCR_INIT);
#endif

	/* Disable transfer interrupt */
	CAN_REG_CLEAR(CAN_NCR(0), CAN_NCR_TRIE);

	/* Disable CAN node */
	CAN_REG_ORIN(CAN_NCR(0), CAN_NCR_INIT);
}

static int niiet_can_send(struct can_dev *can, const void *data) {
	struct can_frame *frame;
	uint32_t reg;

	frame = (struct can_frame *)data;

	reg = CAN_REG_LOAD(CAN_NSR(0));
	(void)reg;

	reg = CAN_REG_LOAD(CAN_MOSTAT(CAN_MSG_TX));
	(void)reg;

	/* Wait if transmission is ongoing */
	while (CAN_REG_LOAD(CAN_MOSTAT(CAN_MSG_TX)) & CAN_MOSTAT_TXRQ) {}

	/* Disable transmission */
	CAN_REG_STORE(CAN_MOCTR(CAN_MSG_TX), CAN_MOCTR_CLR_MSGVAL);

	reg = FIELD(CAN_MOAR_ID, frame->can_id) | FIELD(CAN_MOAR_PRI, 1);
	if (frame->can_id & CAN_EFF_FLAG) {
		reg |= CAN_MOAR_IDE;
	}
	CAN_REG_STORE(CAN_MOAR(CAN_MSG_TX), reg);

	reg = FIELD(CAN_MOFCR_MMC, 0) | CAN_MOFCR_TXIE | CAN_MOFCR_STT;
	reg = FIELD_SET(reg, CAN_MOFCR_DLC, frame->len);
	CAN_REG_STORE(CAN_MOFCR(CAN_MSG_TX), reg);

	if (frame->can_id & CAN_RTR_FLAG) {
		reg = CAN_MOCTR_TX_MASK | CAN_MOCTR_CLR_DIR;
	}
	else {
		reg = CAN_MOCTR_TX_MASK | CAN_MOCTR_SET_DIR;
		CAN_REG_STORE(CAN_MODATAL(CAN_MSG_TX), *(uint32_t *)&frame->data[0]);
		CAN_REG_STORE(CAN_MODATAH(CAN_MSG_TX), *(uint32_t *)&frame->data[4]);
	}
	CAN_REG_STORE(CAN_MOCTR(CAN_MSG_TX), reg);

	/* Enable transmission */
	CAN_REG_STORE(CAN_MOCTR(CAN_MSG_TX), CAN_MOCTR_SET_TXRQ);

	return 0;
}

static const struct can_dev_ops niiet_can_ops = {
    .cdo_config = niiet_can_config,
    .cdo_open = niiet_can_open,
    .cdo_close = niiet_can_close,
    .cdo_send = niiet_can_send,
};

CAN_DEVICE_DEF(niiet_can_node0_dev, &niiet_can_ops, NULL, CAN_DEV_ID);

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

extern void niiet_can_bconf_init(void);

EMBOX_UNIT_INIT(niiet_can_init);

static int niiet_can_init(void) {
	struct can_dev *can;

	can = &niiet_can_node0_dev;

	niiet_can_bconf_init();
	niiet_can_ctrl_init();

	return irq_attach(CAN_IRQ_NUM, niiet_can_irq_handler, 0, can, NULL);
}
