/**
 * @brief Kvaser PCI CAN bus interface board
 *
 * @author Aleksey Zhmulin
 * @date 14.04.26
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <asm/io.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <kernel/irq.h>
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

static inline int kvaser_xmit(struct net_device *dev, struct sk_buff *skb) {
	return 0;
}

static int kvaser_reset(struct net_device *dev) {
	/* Enable reset mode */
	sja_reg_store(dev, SJA_MOD, SJA_MOD_RM);

	/* Enable PeliCAN mode */
	sja_reg_orin(dev, SJA_CDR, SJA_CDR_MOD);

	/* Clear registers */
	sja_reg_store(dev, SJA_RMC, 0);
	sja_reg_store(dev, SJA_RXERR, 0);
	sja_reg_store(dev, SJA_TXERR, 0);

	return 0;
}

static irq_return_t kvaser_irq_handler(unsigned int irq_num, void *dev_id) {
	return IRQ_HANDLED;
}

static int kvaser_start(struct net_device *dev) {
	kvaser_reset(dev);

	return 0;
}

static int kvaser_stop(struct net_device *dev) {
	return 0;
}

static const struct net_driver kvaser_drv_ops = {
    .xmit = kvaser_xmit,
    .start = kvaser_start,
    .stop = kvaser_stop,
};

static int kvaser_init(struct pci_slot_dev *pci_dev) {
	struct net_device *netdev;
	int err;

	// TODO: canbus_alloc()
	netdev = etherdev_alloc(0);
	if (netdev == NULL) {
		return -ENOMEM;
	}

	netdev->base_addr = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;
	netdev->irq = pci_dev->irq;
	netdev->drv_ops = &kvaser_drv_ops;

	err = irq_attach(netdev->irq, kvaser_irq_handler, 0, NULL, "kvaser_pci");
	if (err < 0) {
		return err;
	}

	return 0;
}
