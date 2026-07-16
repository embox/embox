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

static void niiet_can_reset(struct can_dev *can) {
	uint32_t reg;

	CAN_REG_STORE(CAN_CLC, 0);

	while (CAN_REG_LOAD(CAN_CLC) & CAN_CLC_DISS) {}

	reg = CAN_REG_LOAD(CAN_FDR);
	reg = FIELD_SET(reg, CAN_FDR_STEP, 0x3ff);
	reg = FIELD_SET(reg, CAN_FDR_DM, CAN_FDR_DM_NORM);
	CAN_REG_STORE(CAN_FDR, reg);
}

static int niiet_can_open(struct can_dev *can) {
	return 0;
}

static void niiet_can_close(struct can_dev *can) {
}

static int niiet_can_send(struct can_dev *can, const void *data) {
	return 0;
}

static const struct can_dev_ops niiet_can_ops = {
    .cdo_reset = niiet_can_reset,
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
	struct can_dev *can;
	int res;

	niiet_can_dev_priv.can_dev = &niiet_can_dev;
	niiet_can_dev_priv.regs = (struct niiet_can_regs *)(uintptr_t)CAN_BASE_ADDR;

	can = &niiet_can_dev;

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

	niiet_can_reset(can);

	return 0;
}
