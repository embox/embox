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

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>

#include <net/l2/ethernet.h>
#include "ne2k.h"

#include <net/netdevice.h>
#include <net/inetdevice.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(ne2k_isa_init);

#define BASE_ADDR    OPTION_GET(NUMBER, base_addr)
#define IRQ_NUM      OPTION_GET(NUMBER, irq_num)

static int ne2k_isa_init(void) {
	struct net_device *nic;

	nic = etherdev_alloc(0);
	if (nic == NULL) {
		return -ENOMEM;
	}

	nic->irq = IRQ_NUM;
	nic->base_addr = BASE_ADDR;

	return ne2k_dev_init(nic);
}
