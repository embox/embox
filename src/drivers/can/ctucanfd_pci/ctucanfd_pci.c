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

#include <drivers/can_dev.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <kernel/irq.h>

#include "ctucanfd_pci.h"

#define CAN_DEV_ID OPTION_GET(NUMBER, dev_id)

#define CTUCANFD_VENDOR_ID 0x1760
#define CTUCANFD_DEVICE_ID 0xff00

#define CTUCAN_REG_STORE(reg, val)  REG32_STORE(ctucan_base + reg, val)
#define CTUCAN_REG_LOAD(reg)        REG32_LOAD(ctucan_base + reg)
#define CTUCAN_REG_ORIN(reg, mask)  REG32_ORIN(ctucan_base + reg, mask)
#define CTUCAN_REG_ANDIN(reg, mask) REG32_ANDIN(ctucan_base + reg, mask)
#define CTUCAN_REG_CLEAR(reg, mask) REG32_CLEAR(ctucan_base + reg, mask)

static uintptr_t ctucan_base;

void ctucan_reset(void) {
	CTUCAN_REG_STORE(CTUCANFD_SET_MODE, CTUCANFD_MODE_RST);
}

int ctucan_open(struct can_dev *dev) {
	return 0;
}

void ctucan_close(struct can_dev *dev) {
}

void ctucan_eoi(struct can_dev *dev) {
}

void ctucan_irq_en(struct can_dev *dev) {
}

void ctucan_irq_dis(struct can_dev *dev) {
}

int ctucan_hasrx(struct can_dev *dev) {
	return 0;
}

int ctucan_send(struct can_dev *dev, can_frame_t *frame) {
	return 0;
}

int ctucan_receive(struct can_dev *dev, can_frame_t *frame) {
	return 0;
}

static const struct can_ops ctucan_ops = {
    .open = ctucan_open,
    .close = ctucan_close,
    .eoi = ctucan_eoi,
    .irq_en = ctucan_irq_en,
    .irq_dis = ctucan_irq_dis,
    .hasrx = ctucan_hasrx,
    .send = ctucan_send,
    .receive = ctucan_receive,
};

CAN_DEVICE_DEF(ctucan_dev, &ctucan_ops, NULL, CAN_DEV_ID);

static int ctucan_pci_init(struct pci_slot_dev *pci_dev) {
	int irq;
	int err;

	ctucan_base = pci_dev->bar[1] & PCI_BASE_ADDR_IO_MASK;
	irq = pci_dev->irq;

	err = irq_attach(irq, can_dev_irq_handler, IF_SHARESUP, &ctucan_dev, NULL);
	if (err) {
		return err;
	}

	return 0;
}

PCI_DRIVER("ctucan_pci", ctucan_pci_init, CTUCANFD_VENDOR_ID, CTUCANFD_DEVICE_ID);
