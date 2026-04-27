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

#include <asm/io.h>
#include <drivers/can/sja1000.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <net/can.h>

#define SINGLE_FILTER_MODE OPTION_GET(BOOLEAN, single_filter)

#define KVASER_VENDOR_ID 0x10e8
#define KVASER_DEVICE_ID 0x8406

/* s5920 32-Bit PCI Bus Target Interface */
#define S5920_INTCSR     0x38      /* Interrupt Control/Status Register */
#define S5920_INTCSR_AIE (1 << 13) /* Add-On Interrupt pin Enable */
#define S5920_INTCSR_AIS (1 << 22) /* Add-On Interrupt pin Status */
#define S5920_INTCSR_IA  (1 << 23) /* Interrupt Asserted */

static void kvaser_set_filter(struct can_controller *can, uint32_t code,
    uint32_t mask) {
	uintptr_t sja_base;
	int i;

	sja_base = (uintptr_t)can->priv;

	for (i = 0; i < 4; i++) {
		sja_reg_store(sja_base, SJA_ACR(i), ((code >> ((3 - i) * 8)) & 0xff));
		sja_reg_store(sja_base, SJA_AMR(i), ((mask >> ((3 - i) * 8)) & 0xff));
	}
}

int kvaser_start(struct can_controller *can) {
	uintptr_t sja_base;

	sja_base = (uintptr_t)can->priv;

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
	kvaser_set_filter(can, 0, 0xffffffff);

	/* Enable operating mode */
#if SINGLE_FILTER_MODE
	sja_reg_store(sja_base, SJA_MOD, SJA_MOD_AFM);
#else
	sja_reg_store(sja_base, SJA_MOD, 0);
#endif

	/* Enable RX interrupts */
	sja_reg_orin(sja_base, SJA_IER, SJA_IER_RIE);

	/* Release RX buffer */
	sja_reg_store(sja_base, SJA_CMR, SJA_CMR_RRB);

	return 0;
}

int kvaser_stop(struct can_controller *can) {
	uintptr_t sja_base;

	sja_base = (uintptr_t)can->priv;

	/* Disable RX interrupts */
	sja_reg_clear(sja_base, SJA_IER, SJA_IER_RIE);

	/* Enable sleep mode */
	sja_reg_orin(sja_base, SJA_MOD, SJA_MOD_SM);

	return 0;
}

void kvaser_eoi(struct can_controller *can) {
	uintptr_t sja_base;

	sja_base = (uintptr_t)can->priv;

	/* Clear pending interrupts */
	sja_reg_orin(sja_base, SJA_IR, 0);
}

int kvaser_hasrx(struct can_controller *can) {
	uintptr_t sja_base;

	sja_base = (uintptr_t)can->priv;

	/* Check if RX buffer is not empty */
	return (sja_reg_load(sja_base, SJA_SR) & SJA_SR_RBS);
}

int kvaser_send(struct can_controller *can, struct can_frame *frame) {
	uintptr_t sja_base;
	unsigned data_base;
	uint8_t frame_info;
	int i;

	sja_base = (uintptr_t)can->priv;

	/* Check if TX buffer is ready */
	for (i = 0; !(sja_reg_load(sja_base, SJA_SR) & SJA_SR_TBS); i++) {
		if (i >= 10000) {
			return -1;
		}
	}

	frame_info = frame->can_dlc;

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

	for (i = 0; i < frame->can_dlc; i++) {
		sja_reg_store(sja_base, data_base + i, frame->data[i]);
	}

	sja_reg_store(sja_base, SJA_CMR, SJA_CMR_TR);

	return 0;
}

int kvaser_receive(struct can_controller *can, struct can_frame *frame) {
	uintptr_t sja_base;
	unsigned data_base;
	uint8_t frame_info;
	int i;

	sja_base = (uintptr_t)can->priv;

	frame_info = sja_reg_load(sja_base, SJA_FRM);

	frame->can_dlc = frame_info & SJA_FRM_DLC;
	if (frame->can_dlc > 8) {
		frame->can_dlc = 8;
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

	for (i = 0; i < frame->can_dlc; i++) {
		frame->data[i] = sja_reg_load(sja_base, data_base + i);
	}

	/* Release RX buffer */
	sja_reg_store(sja_base, SJA_CMR, SJA_CMR_RRB);

	return 0;
}

static const struct can_ops kvaser_can_ops = {
    .start = kvaser_start,
    .stop = kvaser_stop,
    .eoi = kvaser_eoi,
    .hasrx = kvaser_hasrx,
    .send = kvaser_send,
    .receive = kvaser_receive,
};

static struct can_controller kvaser_can_controller;

CAN_CONTROLLER_REGISTER(kvaser_can_controller);

static int kvaser_pci_init(struct pci_slot_dev *pci_dev) {
	uintptr_t s5920_base;

	kvaser_can_controller.ops = &kvaser_can_ops;
	kvaser_can_controller.irq = pci_dev->irq;
	kvaser_can_controller.priv = (void *)(pci_dev->bar[1] & PCI_BASE_ADDR_IO_MASK);

	s5920_base = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;

	/* Enable card interrupts */
	out32(in32(s5920_base + S5920_INTCSR) | S5920_INTCSR_AIE,
	    s5920_base + S5920_INTCSR);

	return 0;
}

PCI_DRIVER("kvaser_pci", kvaser_pci_init, KVASER_VENDOR_ID, KVASER_DEVICE_ID);
