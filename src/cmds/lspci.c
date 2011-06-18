/**
 * @file
 * @brief show list pci's devices
 *
 * @date 11.04.11
 * @author Gleb Efimov
 * @author Nikolay Korotkiy
 */
#include <types.h>
#include <embox/cmd.h>
#include <lib/list.h>
#include <getopt.h>
#include <stdio.h>

#include <drivers/pci.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: arp [-i if] [-s|d] [-a host] [-m hwaddr] [-h]\n");
}

static void print_error(void) {
	printf("Wrong parameters\n");
}

static void show_short_info(uint32_t bus, uint32_t devfn, uint32_t vendor_reg) {
	uint16_t vendor, device;
	uint8_t baseclass, subclass, rev;
	uint32_t slot, func;

	func = devfn & 0x07;
	slot = (devfn >> 3) & 0x1f;

	vendor = vendor_reg & 0xffff;
	device = (vendor_reg >> 16) & 0xffff;

	pci_read_config8(bus, devfn, PCI_BASECLASS_CODE, &baseclass);
	pci_read_config8(bus, devfn, PCI_SUBCLASS_CODE, &subclass);
	pci_read_config8(bus, devfn, PCI_REVISION_ID, &rev);
	printf("%02d:%02x.%d (PCI dev %04X:%04X) \n"
			"\t %s: %s  %s (rev %02d)\n",
		bus, slot, func, vendor, device,
		find_class_name(baseclass, subclass),
		find_vendor_name(vendor),
		find_device_name(device), rev);
}

static inline char *pci_get_region_type(uint32_t region_reg) {
	if (region_reg & 0x1) {
		return "I/O";
	} else {
		return "Mem";
	}
}
static inline size_t pci_get_region_size(uint32_t region_reg) {
	if (0 == (region_reg & 0x1)) {
		return 1 << 24;
	} else {
		return 1 << 8;
	}
}
static void show_full_info(uint32_t bus, uint32_t devfn, uint32_t vendor_reg) {
	uint16_t vendor, device;
	uint8_t baseclass, subclass, rev, irq;
	uint32_t slot, func, base_addr;
	uint8_t bar_num;

	func = devfn & 0x07;
	slot = (devfn >> 3) & 0x1f;

	vendor = vendor_reg & 0xffff;
	device = (vendor_reg >> 16) & 0xffff;

	pci_read_config8(bus, devfn, PCI_BASECLASS_CODE, &baseclass);
	pci_read_config8(bus, devfn, PCI_SUBCLASS_CODE, &subclass);
	pci_read_config8(bus, devfn, PCI_REVISION_ID, &rev);
	printf("%02d:%02x.%d (dev %04X:%04X) \n"
			"\t%s: %s %s (rev %02d)\n",
		bus, slot, func, vendor, device,
		find_class_name(baseclass, subclass),
		find_vendor_name(vendor),
		find_device_name(device), rev);

	pci_read_config8(bus, devfn, PCI_INTERRUPT_LINE, &irq);
	printf("\t  IRQ number: 0x%d\n", irq);

	for(bar_num = 0; bar_num < 6; bar_num ++) {
		pci_read_config32(bus, devfn, PCI_BASE_ADDR_REG_0 + (bar_num << 2),
				&base_addr);
		if (0 == base_addr) {
			continue;
		}
		printf("\t  Region (%s): Base: 0x%X [0x%X]\n",
				pci_get_region_type(base_addr), base_addr,
				(base_addr & ~((1 << 4) - 1)) + (pci_get_region_size(base_addr) - 1));
	}

}

static int exec(int argc, char **argv) {
	int opt;
	uint32_t devfn, bus, vendor, func, slot;
	uint8_t hdr_type = 0;

	int full = 0;

	if (argc == 1) {
		full = 0;
	} else {
		getopt_init();
		while (-1 != (opt = getopt(argc, argv, "hf"))) {
			switch (opt) {
			case 'f':
				full = 1;
				break;
			case '?':
			case 'h':
				print_usage();
				return 0;
			default:
				print_error();
				return 0;
			}
		};
	}/*else have some parameters*/


	for (bus = 0; bus < PCI_BUS_QUANTITY; ++bus) {
		for (devfn = MIN_DEVFN; devfn < MAX_DEVFN; ++devfn) {
			func = devfn & 0x07;
			slot = (devfn >> 3) & 0x1f;
			if (func == 0) {
				pci_read_config8(bus, devfn, PCI_HEADER_TYPE, &hdr_type);
			} else if (!(hdr_type & 0x80)) {
				continue;
			}
			pci_read_config32(bus, devfn, PCI_VENDOR_ID, &vendor);
			if (vendor == 0xffffffff || vendor == 0x00000000) {
				hdr_type = 0;
				continue;
			}

			if (full == 0) {
				show_short_info(bus, devfn, vendor);
			} else {
				show_full_info(bus, devfn, vendor);
			}
		}
	}
	return 0;
}
