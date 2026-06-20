/**
 * @brief Kvaser PCI CAN bus interface board
 *
 * @author Aleksey Zhmulin
 * @date 14.04.26
 */

#include <errno.h>
#include <linux/can.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include <asm/io.h>
#include <drivers/can/sja1000.h>
#include <drivers/can_dev.h>
#include <drivers/char_dev.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>

#define CAN_DEV_ID         OPTION_GET(NUMBER, dev_id)
#define SINGLE_FILTER_MODE OPTION_GET(BOOLEAN, single_filter)

#define KVASER_VENDOR_ID 0x10e8
#define KVASER_DEVICE_ID 0x8406

/* s5920 32-Bit PCI Bus Target Interface */
#define S5920_INTCSR     0x38      /* Interrupt Control/Status Register */
#define S5920_INTCSR_AIE (1 << 13) /* Add-On Interrupt pin Enable */
#define S5920_INTCSR_AIS (1 << 22) /* Add-On Interrupt pin Status */
#define S5920_INTCSR_IA  (1 << 23) /* Interrupt Asserted */

static uintptr_t sja_base;
static uintptr_t s5920_base;

static void kvaser_set_filter(uint32_t code, uint32_t mask) {
	int i;

	for (i = 0; i < 4; i++) {
		sja_reg_store(sja_base, SJA_ACR(i), ((code >> ((3 - i) * 8)) & 0xff));
		sja_reg_store(sja_base, SJA_AMR(i), ((mask >> ((3 - i) * 8)) & 0xff));
	}
}

static void kvaser_reset(struct can_dev *can) {
	/* Enable card interrupts */
	out32(in32(s5920_base + S5920_INTCSR) | S5920_INTCSR_AIE,
	    s5920_base + S5920_INTCSR);

	/* Enable reset mode */
	sja_reg_store(sja_base, SJA_MOD, SJA_MOD_RM);

	/* Enable PeliCAN mode */
	sja_reg_orin(sja_base, SJA_CDR, SJA_CDR_MOD);

	/* Clear registers */
	sja_reg_store(sja_base, SJA_RXERR, 0);
	sja_reg_store(sja_base, SJA_TXERR, 0);
	sja_reg_store(sja_base, SJA_RMC, 0);
	sja_reg_store(sja_base, SJA_BTR0, 0);
	sja_reg_store(sja_base, SJA_BTR1, 0);
	sja_reg_store(sja_base, SJA_OCR, 0);
	sja_reg_store(sja_base, SJA_IER, 0);
	sja_reg_store(sja_base, SJA_FRM, 0);

	/* Set filter to receive all CAN frames */
	kvaser_set_filter(0, 0xffffffff);
}

static int kvaser_open(struct can_dev *can) {
	/* Enable operating mode */
#if SINGLE_FILTER_MODE
	sja_reg_store(sja_base, SJA_MOD, SJA_MOD_AFM);
#else
	sja_reg_store(sja_base, SJA_MOD, 0);
#endif

	/* Release RX buffer */
	sja_reg_store(sja_base, SJA_CMR, SJA_CMR_RRB);

	return 0;
}

static void kvaser_close(struct can_dev *can) {
	/* Enable sleep mode */
	sja_reg_orin(sja_base, SJA_MOD, SJA_MOD_SM);
}

static void kvaser_rxint(struct can_dev *can, int enable) {
	if (enable) {
		/* Enable RX interrupt */
		sja_reg_orin(sja_base, SJA_IER, SJA_IER_RIE);
		sja_reg_orin(sja_base, SJA_IR, 0);
	}
	else {
		/* Disable RX interrupt */
		sja_reg_clear(sja_base, SJA_IER, SJA_IER_RIE);
	}
}

static int kvaser_send(struct can_dev *can, const void *data) {
	const struct can_frame *frame;
	unsigned data_base;
	uint8_t frame_info;
	int i;

	frame = (const struct can_frame *)data;

	/* Check if TX buffer is ready */
	if (!(sja_reg_load(sja_base, SJA_SR) & SJA_SR_TBS)) {
		return -EBUSY;
	}

	frame_info = frame->len;

	if (frame->can_id & CAN_RTR_FLAG) {
		frame_info |= SJA_FRM_RTR;
	}

	if (frame->can_id & CAN_EFF_FLAG) {
		frame_info |= SJA_FRM_FF;

		data_base = SJA_EFDAT(0);

		sja_reg_store(sja_base, SJA_ID(0), (frame->can_id & 0x1fe00000) >> 21);
		sja_reg_store(sja_base, SJA_ID(1), (frame->can_id & 0x001fe000) >> 13);
		sja_reg_store(sja_base, SJA_ID(2), (frame->can_id & 0x00001fe0) >> 5);
		sja_reg_store(sja_base, SJA_ID(3), (frame->can_id & 0x0000001f) << 3);
	}
	else {
		data_base = SJA_SFDAT(0);

		sja_reg_store(sja_base, SJA_ID(0), (frame->can_id & 0x07f8) >> 3);
		sja_reg_store(sja_base, SJA_ID(1), (frame->can_id & 0x0007) << 5);
	}

	sja_reg_store(sja_base, SJA_FRM, frame_info);

	for (i = 0; i < frame->len; i++) {
		sja_reg_store(sja_base, data_base + i, frame->data[i]);
	}

	sja_reg_store(sja_base, SJA_CMR, SJA_CMR_TR);

	return 0;
}

static const struct can_ops kvaser_can_ops = {
    .co_reset = kvaser_reset,
    .co_open = kvaser_open,
    .co_close = kvaser_close,
    .co_rxint = kvaser_rxint,
    .co_send = kvaser_send,
};

CAN_DEVICE_DEF(kvaser_can_dev, &kvaser_can_ops, NULL, CAN_DEV_ID);

static void kvaser_receive(struct can_frame *frame) {
	unsigned data_base;
	uint8_t frame_info;
	int i;

	frame_info = sja_reg_load(sja_base, SJA_FRM);

	frame->len = frame_info & SJA_FRM_DLC;
	if (frame->len > 8) {
		frame->len = 8;
	}

	frame->can_id = 0;

	if (frame_info & SJA_FRM_RTR) {
		frame->can_id |= CAN_RTR_FLAG;
	}

	if (frame_info & SJA_FRM_FF) {
		frame->can_id |= CAN_EFF_FLAG;
		frame->can_id |= (sja_reg_load(sja_base, SJA_ID(0)) << 21);
		frame->can_id |= (sja_reg_load(sja_base, SJA_ID(1)) << 13);
		frame->can_id |= (sja_reg_load(sja_base, SJA_ID(2)) << 5);
		frame->can_id |= (sja_reg_load(sja_base, SJA_ID(3)) >> 3);

		data_base = SJA_EFDAT(0);
	}
	else {
		frame->can_id = (sja_reg_load(sja_base, SJA_ID(0)) << 3);
		frame->can_id |= (sja_reg_load(sja_base, SJA_ID(1)) >> 5);

		data_base = SJA_SFDAT(0);
	}

	for (i = 0; i < frame->len; i++) {
		frame->data[i] = sja_reg_load(sja_base, data_base + i);
	}

	/* Release RX buffer */
	sja_reg_store(sja_base, SJA_CMR, SJA_CMR_RRB);
}

static irq_return_t kvaser_irq_handler(unsigned int irq_num, void *data) {
	struct can_dev *can;
	struct can_msg *msg;

	can = (struct can_dev *)data;

	/* Clear pending interrupts */
	sja_reg_orin(sja_base, SJA_IR, 0);

	/* Check if RX buffer is not empty */
	while (sja_reg_load(sja_base, SJA_SR) & SJA_SR_RBS) {
		msg = can_msg_alloc(can);
		if (!msg) {
			can_rx_stop(can);
			break;
		}
		kvaser_receive(&msg->frame);
		can_msg_queue_push(can, msg);
	}

	can_rx_notify(can);

	return IRQ_HANDLED;
}

static int kvaser_pci_init(struct pci_slot_dev *pci_dev) {
	s5920_base = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;
	sja_base = pci_dev->bar[1] & PCI_BASE_ADDR_IO_MASK;

	kvaser_reset(&kvaser_can_dev);

	return irq_attach(pci_dev->irq, kvaser_irq_handler, IF_SHARESUP,
	    &kvaser_can_dev, NULL);
}

PCI_DRIVER("kvaser_pci", kvaser_pci_init, KVASER_VENDOR_ID, KVASER_DEVICE_ID);
