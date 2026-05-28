/**
 *
 * @author Anton Bondarev
 * @date 28.05.26
 */

#include <errno.h>
#include <linux/can.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/can_dev.h>

#include <bsp/stm32cube_hal.h>

#include <framework/mod/options.h>

#include "stm32cube_can_priv.h"

static int stm32cube_open(struct can_dev  *can) {
	return 0;
}

static void stm32cube_close(struct can_dev  *can) {
//	return 0;
}

static void stm32cube_eoi(struct can_dev  *can) {
}


static void stm32cube_irq_en(struct can_dev *dev) {
}

static void stm32cube_irq_dis(struct can_dev *dev) {
}

static int stm32cube_hasrx(struct can_dev  *can) {
	return 0;
}

static int stm32cube_send(struct can_dev  *can, struct can_frame *frame) {
	return 0;
}

static int stm32cube_receive(struct can_dev  *can, struct can_frame *frame) {
	return 0;
}

const struct can_ops stm32cube_can_ops = {
    .open = stm32cube_open,
    .close = stm32cube_close,
    .eoi = stm32cube_eoi,
	.irq_en = stm32cube_irq_en,
    .irq_dis = stm32cube_irq_dis,
    .hasrx = stm32cube_hasrx,
    .send = stm32cube_send,
    .receive = stm32cube_receive,
};


