/**
 * @file
 * @brief show list pci's devices
 *
 * @date 11.04.11
 * @author Gleb Efimov
 * @author Nikolay Korotkiy
 */

#include <embox/cmd.h>
#include <types.h>
#include <lib/list.h>
#include <drivers/pci.h>

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
	uint32_t devfn, l, bus, slot, func;
	uint16_t vendor, device;
	uint8_t hdr_type = 0, baseclass, subclass, rev;

	for (bus = 0; bus < PCI_BUS_QUANTITY; ++bus) {
		for (devfn = MIN_DEVFN; devfn < MAX_DEVFN; ++devfn) {
			func = devfn & 0x07;
			slot = (devfn >> 3) & 0x1f;
			if (func == 0) {
				pci_read_config8(bus, devfn, PCI_HEADER_TYPE, &hdr_type);
			} else if (!(hdr_type & 0x80)) {
				continue;
			}
			pci_read_config32(bus, devfn, PCI_VENDOR_ID, &l);
			if (l == 0xffffffff || l == 0x00000000) {
				hdr_type = 0;
				continue;
			}
			vendor = l & 0xffff;
			device = (l >> 16) & 0xffff;

			pci_read_config8(bus, devfn, PCI_BASECLASS_CODE, &baseclass);
			pci_read_config8(bus, devfn, PCI_SUBCLASS_CODE, &subclass);
			pci_read_config8(bus, devfn, PCI_REVISION_ID, &rev);
			printf("%02d:%02x.%d %s: %s %s (rev %02d)\n",
				bus, slot, func,
				find_class_name(baseclass, subclass),
				find_vendor_name(vendor),
				find_device_name(device), rev);
		}
	}
	return 0;
}
