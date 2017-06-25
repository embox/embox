/**
 * @file
 * @brief Device driver for PCI NE2000 PCI clones (e.g. RealTek RTL-8029).
 * @details Driver to fool Qemu into sending and receiving
 *          packets for us via it's model=ne2k_pci emulation
 *          This driver is unlikely to work with real hardware
 *          without substantial modifications and is purely for
 *          helping with the development of network stacks.
 *          Interrupts are not supported.
 *
 * @date 11.01.11
 * @author Biff
 *         - Initial implementation
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 *         - Adaptation for Embox
 */

#include <stdint.h>
#include <string.h>
#include <errno.h>
//#include <arpa/inet.h>
//#include <net/l0/net_entry.h>

//#include <asm/io.h>


#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
//#include <kernel/irq.h>
#include <net/l2/ethernet.h>
#include "ne2k.h"

#include <net/netdevice.h>
#include <net/inetdevice.h>
//#include <net/skbuff.h>

//#include <embox/unit.h>


PCI_DRIVER("ne2k_pci", ne2k_init, PCI_VENDOR_ID_REALTEK, PCI_DEV_ID_REALTEK_8029);


static int ne2k_init(struct pci_slot_dev *pci_dev) {

	uint32_t nic_base;
	struct net_device *nic;

	assert(pci_dev != NULL);

	nic_base = pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK;

	nic = etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}

	nic->irq = pci_dev->irq;
	nic->base_addr = nic_base;

	return ne2k_dev_init(nic);
}
