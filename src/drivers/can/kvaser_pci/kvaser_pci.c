/**
 * @brief Kvaser PCI CAN bus interface board
 *
 * @author Aleksey Zhmulin
 * @date 14.04.26
 */

#include <errno.h>
#include <linux/can.h>
#include <stddef.h>
#include <stdint.h>

#include <asm/io.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <kernel/irq.h>
#include <net/can_netdevice.h>
#include <net/inetdevice.h>
#include <net/l2/ethernet.h>
#include <net/netdevice.h>
#include <net/skbuff.h>

#include "sja1000.h"

PCI_DRIVER("kvaser_pci", kvaser_init, 0x10e8, 0x8406);

static void sja_reg_store(struct net_device *dev, unsigned int reg, uint8_t val) {
	out8(val, dev->base_addr + reg);
}

static uint8_t sja_reg_load(struct net_device *dev, unsigned int reg) {
	return in8(dev->base_addr + reg);
}

static inline void sja_reg_orin(struct net_device *dev, unsigned int reg,
    uint8_t mask) {
	return sja_reg_store(dev, reg, sja_reg_load(dev, reg) | mask);
}

static inline void sja_reg_andin(struct net_device *dev, unsigned int reg,
    uint8_t mask) {
	return sja_reg_store(dev, reg, sja_reg_load(dev, reg) & mask);
}

static inline void sja_reg_clear(struct net_device *dev, unsigned int reg,
    uint8_t mask) {
	return sja_reg_store(dev, reg, sja_reg_load(dev, reg) & ~mask);
}

static int kvaser_reset(struct net_device *dev) {
	/* Enable reset mode */
	sja_reg_store(dev, SJA_MOD, SJA_MOD_RM);

	/* Enable PeliCAN mode */
	sja_reg_orin(dev, SJA_CDR, SJA_CDR_MOD);

	/* Clear registers */
	sja_reg_store(dev, SJA_RXERR, 0);
	sja_reg_store(dev, SJA_TXERR, 0);
	sja_reg_store(dev, SJA_RMC, 0);
	sja_reg_store(dev, SJA_FRM, 0);
	sja_reg_store(dev, SJA_BTR0, 0);
	sja_reg_store(dev, SJA_BTR1, 0);
	sja_reg_store(dev, SJA_OCR, 0);
	sja_reg_store(dev, SJA_IER, 0);

	/* Enable operating mode and dual filter mode */
	sja_reg_store(dev, SJA_MOD, 0);

	return 0;
}

static int kvaser_start(struct net_device *dev) {
	kvaser_reset(dev);

	/* Enable interrupts */
	sja_reg_orin(dev, SJA_IER, SJA_IER_RIE);
	sja_reg_orin(dev, SJA_IER, SJA_IER_TIE);

	return 0;
}

static int kvaser_stop(struct net_device *dev) {
	/* Disable interrupts */
	sja_reg_clear(dev, SJA_IER, SJA_IER_RIE);
	sja_reg_clear(dev, SJA_IER, SJA_IER_TIE);

	/* Enable sleep mode */
	sja_reg_orin(dev, SJA_MOD, SJA_MOD_SM);

	return 0;
}

static int kvaser_xmit(struct net_device *dev, struct sk_buff *skb) {
	struct can_frame *frame;
	uint8_t frame_info;
	unsigned int data_base;
	int i;

	/* Check if TX buffer is ready */
	if (!(sja_reg_load(dev, SJA_SR) & SJA_SR_TBS)) {
		return -EBUSY;
	}

	frame = skb_data_cast_in(skb->data);
	// len = skb->len;

	frame_info = frame->can_dlc;

	if (frame->can_id & CAN_RTR_FLAG) {
		frame_info |= SJA_FRM_RTR;
	}

	if (frame->can_id & CAN_EFF_FLAG) {
		frame_info |= SJA_FRM_FF;

		data_base = SJA_EFDAT(0);

		sja_reg_store(dev, SJA_ID(0), (frame->can_id & 0x1fe00000) >> 21);
		sja_reg_store(dev, SJA_ID(1), (frame->can_id & 0x001fe000) >> 13);
		sja_reg_store(dev, SJA_ID(2), (frame->can_id & 0x00001fe0) >> 5);
		sja_reg_store(dev, SJA_ID(3), (frame->can_id & 0x0000001f) << 3);
	}
	else {
		data_base = SJA_SFDAT(0);

		sja_reg_store(dev, SJA_ID(0), (frame->can_id & 0x07f8) >> 3);
		sja_reg_store(dev, SJA_ID(1), (frame->can_id & 0x0007) << 5);
	}

	sja_reg_store(dev, SJA_FRM, frame_info);

	for (i = 0; i < frame->can_dlc; i++) {
		sja_reg_store(dev, data_base + i, frame->data[i]);
	}

	sja_reg_store(dev, SJA_CMR, SJA_CMR_TR);

	skb_free(skb);

	return 0;
}

static const struct net_driver kvaser_drv_ops = {
    .xmit = kvaser_xmit,
    .start = kvaser_start,
    .stop = kvaser_stop,
};

// static irq_return_t kvaser_irq_handler(unsigned int irq_num, void *dev_id) {
// 	return IRQ_HANDLED;
// }

static int kvaser_init(struct pci_slot_dev *pci_dev) {
	struct net_device *netdev;
	// int err;

	netdev = etherdev_alloc(0);
	if (netdev == NULL) {
		return -ENOMEM;
	}

	netdev->base_addr = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;
	netdev->irq = pci_dev->irq;
	netdev->drv_ops = &kvaser_drv_ops;

	// err = irq_attach(netdev->irq, kvaser_irq_handler, 0, NULL, "kvaser_pci");
	// if (err < 0) {
	// 	return err;
	// }

	return cannetdev_register(netdev);
}
