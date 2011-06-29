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
#include <drivers/pci_repo.h>
#include <drivers/pci_utils.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: lspci [-f] [-h]\n\t[-f] - full info\n");
}

static void print_error(void) {
	printf("Wrong parameters\n");
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

static int exec(int argc, char **argv) {
	int opt;
	struct pci_dev *pci_dev;

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


	pci_foreach_dev(pci_dev) {
		printf("%02d:%2x.%d (PCI dev %04X:%04X) \n"
					"\t %s: %s %s (rev %02d)\n",
					pci_dev->busn,
					pci_dev->slot,
					pci_dev->func,
					pci_dev->vendor,
					pci_dev->device,
					find_class_name(pci_dev->baseclass, pci_dev->subclass),
					find_vendor_name(pci_dev->vendor),
					find_device_name(pci_dev->device),
					pci_dev->rev);
		if (full == 0) {
			int bar_num;
			printf("\t  IRQ number: 0x%d\n", pci_dev->irq);

			for(bar_num = 0; bar_num < ARRAY_SIZE(pci_dev->bar); bar_num ++) {
				uint32_t base_addr = pci_dev->bar[bar_num];
				if (0 == base_addr) {
					continue;
				}
				printf("\t  Region (%s): Base: 0x%X [0x%X]\n",
						pci_get_region_type(base_addr),
						base_addr & ~((1 << 4) - 1),
						(base_addr & ~((1 << 4) - 1)) +
						(pci_get_region_size(base_addr) - 1));
			}
		}

	}

	return 0;
}
