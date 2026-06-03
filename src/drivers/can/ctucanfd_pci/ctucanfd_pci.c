/**
 * @brief CTU CAN FD Controller
 *
 * @author Aleksey Zhmulin
 * @date 29.05.26
 */

#include <errno.h>
#include <linux/can.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

#include <drivers/can_dev.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <mem/vmem_device_memory.h>
#include <util/field.h>

#include "ctucanfd_pci.h"

#define CAN_DEV_ID OPTION_GET(NUMBER, dev_id)

#define CTUCANFD_VENDOR_ID 0x1760
#define CTUCANFD_DEVICE_ID 0xff00

#define CTUCANFD_REG_STORE(reg, val)  REG32_STORE(ctucanfd_base + reg, val)
#define CTUCANFD_REG_LOAD(reg)        REG32_LOAD(ctucanfd_base + reg)
#define CTUCANFD_REG_ORIN(reg, mask)  REG32_ORIN(ctucanfd_base + reg, mask)
#define CTUCANFD_REG_ANDIN(reg, mask) REG32_ANDIN(ctucanfd_base + reg, mask)
#define CTUCANFD_REG_CLEAR(reg, mask) REG32_CLEAR(ctucanfd_base + reg, mask)

#define CTUCANFD_DATA_STORE(reg, val) REG8_STORE(ctucanfd_base + reg, val)
#define CTUCANFD_DATA_LOAD(reg)       REG8_LOAD(ctucanfd_base + reg)

static uintptr_t ctucanfd_base;

void ctucanfd_reset(void) {
	CTUCANFD_REG_STORE(CTUCANFD_MODE, CTUCANFD_MODE_RST);

	CTUCANFD_REG_STORE(CTUCANFD_BTR, 0);
	CTUCANFD_REG_STORE(CTUCANFD_BTRFD, 0);

	/* Clear all interrupts */
	CTUCANFD_REG_STORE(CTUCANFD_IEC, 0xffff);

	/* Configure TXT Buﬀer 1 priority */
	CTUCANFD_REG_STORE(CTUCANFD_TXP, 0x1);
}

int ctucanfd_open(struct can_dev *dev) {
	/* Enable CAN device */
	CTUCANFD_REG_STORE(CTUCANFD_MODE, CTUCANFD_MODE_ENA | CTUCANFD_MODE_RXBAM);

	return 0;
}

void ctucanfd_close(struct can_dev *dev) {
	/* Disable CAN device */
	CTUCANFD_REG_STORE(CTUCANFD_MODE, 0);
}

void ctucanfd_eoi(struct can_dev *dev) {
	/* Clear pending RX interrupt */
	CTUCANFD_REG_STORE(CTUCANFD_ISR, CTUCANFD_ISR_RBNEI);
}

void ctucanfd_irq_en(struct can_dev *dev) {
	/* Enable RX interrupt */
	CTUCANFD_REG_STORE(CTUCANFD_IES, CTUCANFD_ISR_RBNEI);
}

void ctucanfd_irq_dis(struct can_dev *dev) {
	/* Disable RX interrupt */
	CTUCANFD_REG_STORE(CTUCANFD_IEC, CTUCANFD_ISR_RBNEI);
}

int ctucanfd_hasrx(struct can_dev *dev) {
	return !(CTUCANFD_REG_LOAD(CTUCANFD_RXS) & CTUCANFD_RXS_RXE);
}

int ctucanfd_send(struct can_dev *dev, can_frame_t *frame) {
	uint32_t fmt;
	uint32_t id;
	int i;

	if ((CTUCANFD_REG_LOAD(CTUCANFD_TXS) & CTUCANFD_TXS_MASK) != CTUCANFD_TXS_ETY) {
		// log_warning("TX buffer not available");
	}

	fmt = FIELD(CTUCANFD_TXB_FMT_DLC, frame->len);
	fmt |= CTUCANFD_TXB_FMT_FDF;

	if (frame->can_id & CAN_RTR_FLAG) {
		fmt |= CTUCANFD_TXB_FMT_RTR;
	}

	if (frame->can_id & CAN_EFF_FLAG) {
		fmt |= CTUCANFD_TXB_FMT_IDE;
		id = FIELD(CTUCANFD_TXB_ID_EXT, frame->can_id);
	}
	else {
		id = FIELD(CTUCANFD_TXB_ID_STD, frame->can_id);
	}

	CTUCANFD_REG_STORE(CTUCANFD_TXB_FMT, fmt);
	CTUCANFD_REG_STORE(CTUCANFD_TXB_ID, id);
	CTUCANFD_REG_STORE(CTUCANFD_TXB_TSL, 0);
	CTUCANFD_REG_STORE(CTUCANFD_TXB_TSU, 0);

	for (i = 0; i < frame->len; i++) {
		CTUCANFD_DATA_STORE(CTUCANFD_TXB_DAT + i, frame->data[i]);
	}

	CTUCANFD_REG_STORE(CTUCANFD_TXC, CTUCANFD_TXC_TXCR | CTUCANFD_TXC_TXB1);

	return 0;
}

int ctucanfd_receive(struct can_dev *dev, can_frame_t *frame) {
	uint32_t rwcnt;
	uint32_t tmp;
	uint32_t fmt;
	uint32_t id;
	int i;

	// rxfrc = FIELD_GET(CTUCANFD_REG_LOAD(CTUCANFD_RXS), CTUCANFD_RXS_RXFRC);

	// for (; rxfrc > 0; rxfrc--) {
	// 	fmt = CTUCANFD_REG_LOAD(CTUCANFD_RXD);
	// }

	fmt = CTUCANFD_REG_LOAD(CTUCANFD_RXD);
	id = CTUCANFD_REG_LOAD(CTUCANFD_RXD);

	/* Skip timestamp */
	tmp = CTUCANFD_REG_LOAD(CTUCANFD_RXD);
	tmp = CTUCANFD_REG_LOAD(CTUCANFD_RXD);
	(void)tmp;

	rwcnt = FIELD_GET(fmt, CTUCANFD_TXB_FMT_RWCNT) - 3;

	frame->len = FIELD_GET(fmt, CTUCANFD_TXB_FMT_DLC);

	if (fmt & CTUCANFD_TXB_FMT_IDE) {
		frame->can_id = FIELD_GET(id, CTUCANFD_TXB_ID_EXT);
		frame->can_id = CAN_EFF_FLAG;
	}
	else {
		frame->can_id = FIELD_GET(id, CTUCANFD_TXB_ID_STD);
	}

	for (i = 0; i < rwcnt; i++) {
		*((uint32_t *)(&frame->data[i * 4])) = CTUCANFD_REG_LOAD(CTUCANFD_RXD);
	}

	return 0;
}

static const struct can_ops ctucanfd_ops = {
    .open = ctucanfd_open,
    .close = ctucanfd_close,
    .eoi = ctucanfd_eoi,
    .irq_en = ctucanfd_irq_en,
    .irq_dis = ctucanfd_irq_dis,
    .hasrx = ctucanfd_hasrx,
    .send = ctucanfd_send,
    .receive = ctucanfd_receive,
};

CAN_DEVICE_DEF(ctucanfd_dev, &ctucanfd_ops, NULL, CAN_DEV_ID);

static int ctucanfd_pci_init(struct pci_slot_dev *pci_dev) {
	int irq;
	int err;

	ctucanfd_base = pci_dev->bar[1];
	irq = pci_dev->irq;

	if (mmap_device_memory((void *)ctucanfd_base, 0x10000,
	        PROT_WRITE | PROT_READ | PROT_NOCACHE, MAP_FIXED, ctucanfd_base)
	    != (void *)ctucanfd_base) {
		return -1;
	}

	err = irq_attach(irq, can_dev_irq_handler, IF_SHARESUP, &ctucanfd_dev, NULL);
	if (err) {
		return err;
	}

	ctucanfd_reset();

	return 0;
}

PCI_DRIVER("ctucanfd_pci", ctucanfd_pci_init, CTUCANFD_VENDOR_ID,
    CTUCANFD_DEVICE_ID);
