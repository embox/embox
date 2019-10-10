/**
 * @file
 * @brief show list pci's devices
 *
 * @date 11.04.11
 * @author Gleb Efimov
 * @author Nikolay Korotkiy
 */

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <util/array.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_repo.h>
#include <drivers/pci/pci_chip/pci_utils.h>

struct pci_reg {
	uint8_t offset;
	int width;
	char name[100];
};

static struct pci_reg endpoint_regs[] = {
	{0x0, 2, "VENDOR ID"},
	{0x2, 2, "DEVICE ID"},
	{0x4, 2, "COMMAND"},
	{0x6, 2, "STATUS"},
	{0x8, 1, "REVISION"},
	{0x9, 1, "CLASS_PROG"},
	{0xa, 2, "CLASS_DEVICE"},
	{0xc, 1, "CACHE_LINE_SIZE"},
	{0xd, 1, "LATENCY_TIMER"},
	{0xe, 1, "HEADER_TYPE"},
	{0xf, 1, "BIST"},
	{0x10, 4, "BAR0"},
	{0x14, 4, "BAR1"},
	{0x18, 4, "BAR2"},
	{0x1c, 4, "BAR3"},
	{0x20, 4, "BAR4"},
	{0x24, 4, "BAR5"},
	{0x28, 4, "CARDBUS_CIS"},
	{0x2c, 2, "SUBSYSTEM_VENDOR_ID"},
	{0x2e, 2, "SUBSYSTEM_ID"},
	{0x30, 4, "ROM_ADDRESS"},
	{0x34, 1, "CAPABILITIES"},
	{0x3c, 1, "INTERRUPT_LINE"},
	{0x3d, 1, "INTERRUPT_PIN"},
	{0x3e, 1, "MIN_GNT"},
	{0x3f, 1, "MAX_LAT"},
	{0, 0, ""},
};

static struct pci_reg bridge_regs[] = {
	{0x0, 2, "VENDOR ID"},
	{0x2, 2, "DEVICE ID"},
	{0x4, 2, "COMMAND"},
	{0x6, 2, "STATUS"},
	{0x8, 1, "REVISION"},
	{0x9, 1, "CLASS_PROG"},
	{0xa, 2, "CLASS_DEVICE"},
	{0xc, 1, "CACHE_LINE_SIZE"},
	{0xd, 1, "LATENCY_TIMER"},
	{0xe, 1, "HEADER_TYPE"},
	{0xf, 1, "BIST"},
	{0x10, 4, "BAR0"},
	{0x14, 4, "BAR1"},
	{0x18, 1, "PRIMARY_BUS#"},
	{0x19, 1, "SECONDARY_BUS#"},
	{0x1a, 1, "SUBORDINATE_BUS#"},
	{0x1b, 1, "SECONDARY_LATENCY_TIMER"},
	{0x1c, 1, "IO_BASE"},
	{0x1d, 1, "IO_LIMIT"},
	{0x1e, 2, "SECONDARY_STATUS"},
	{0x20, 2, "MEMORY_BASE"},
	{0x22, 2, "MEMORY_LIMIT"},
	{0x24, 2, "PRFTCH_MEMORY_BASE"},
	{0x26, 2, "PRFTCH_MEMORY_LIMIT"},
	{0x28, 4, "PRFTCH_BASE_UPPER32"},
	{0x2c, 4, "PRFTCH_LIMIT_UPPER32"},
	{0x30, 2, "IO_BASE_UPPER16"},
	{0x32, 2, "IO_LIMIT_UPPER16"},
	{0x34, 1, "CAPABILITIES"},
	{0x3c, 1, "INTERRUPT_LINE"},
	{0x3d, 1, "INTERRUPT_PIN"},
	{0x3e, 2, "BRIDGE_CONTROL"},
	{0, 0, ""},
};

static void print_usage(void) {
	printf("Usage: lspci [-f] [-h] [-x] [-b bus] [-s slot] [-u func] [-n] [-l len] [-o off]\n");
	printf("\t[-f]      - full info\n");
	printf("\t[-x]      - hex dump of config space\n");
	printf("\t[-b bus]  - select bus\n");
	printf("\t[-s slot] - select slot\n");
	printf("\t[-u func] - select function\n");
	printf("\t[-n]      - human-readable dump of config space\n");
	printf("\t[-l len]  - length\n");
	printf("\t[-o off]  - offset\n");
	printf("\t[-t ndw]  - write/read ndw dwords\n");
	printf("\t[-h]      - print this help\n");
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

static void show_device(struct pci_slot_dev *pci_dev, int full) {
	printf("%02" PRId32 ":%2" PRIx8 ".%" PRId8 " (PCI dev %04X:%04X) [%d %d]\n"
				"\t %s: %s %s (rev %02d)\n",
				pci_dev->busn,
				pci_dev->slot,
				pci_dev->func,
				pci_dev->vendor,
				pci_dev->device,
				pci_dev->baseclass,
				pci_dev->subclass,
				find_class_name(pci_dev->baseclass, pci_dev->subclass),
				find_vendor_name(pci_dev->vendor),
				find_device_name(pci_dev->device),
				pci_dev->rev);
	if (full != 0) {
		int bar_num;
		printf("\t  IRQ number: %d\n", pci_dev->irq);

		for (bar_num = 0; bar_num < ARRAY_SIZE(pci_dev->bar); bar_num ++) {
			uintptr_t base_addr = pci_dev->bar[bar_num];
			if (0 == base_addr) {
				continue;
			}
			printf("\t  Region (%s): Base: 0x%" PRIXPTR " [0x%" PRIXPTR "]\n",
					pci_get_region_type(base_addr),
					base_addr & ~((1 << 4) - 1),
					(base_addr & ~((1 << 4) - 1)) +
					(pci_get_region_size(base_addr) - 1));
		}
	}
}

static void
dump_regs(struct pci_slot_dev *pci_dev, uint32_t offset, uint32_t length) {
	int i;
	uint8_t val;
	uint32_t ret;

	if (offset % 16) {
		printf("%" PRIx32 ": ", offset);
	}
	for (i = offset; i < offset+length; i++) {
		if (i % 16 == 0) {
			printf("%x: ", i);
		}
		ret = pci_read_config8(pci_dev->busn,
				(pci_dev->slot << 3) | pci_dev->func, i, &val);
		if (ret != PCIUTILS_SUCCESS) {
			printf("E%" PRIx32 "", ret);
		} else {
			printf("%" PRIx8 "%" PRIx8 "", val/16, val%16);
		}

		if (i % 16 == 15) {
			printf("\n");
		} else {
			printf(" ");
		}
	}
	if (i % 16) {
		printf("\n");
	}
}

static void dump_regs2(struct pci_slot_dev *pci_dev) {
	int i = 0;
	uint8_t val8;
	uint16_t val16;
	uint32_t val32, val, ret;

	struct pci_reg *regs = endpoint_regs;

	ret = pci_read_config8(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func, PCI_HEADER_TYPE, &val8);
	if (ret != 0) {
		printf("Unable to read device type: %" PRId32 "\n", ret);
		return;
	}
	if ((val8 & 0x7F) == 1) {
		regs = bridge_regs;
	}
	while (regs[i].width != 0) {
		switch (regs[i].width) {
		case 1:
			ret = pci_read_config8(pci_dev->busn,
					(pci_dev->slot << 3) | pci_dev->func,
					regs[i].offset, &val8);
			val = val8;
			break;
		case 2:
			ret = pci_read_config16(pci_dev->busn,
					(pci_dev->slot << 3) | pci_dev->func,
					regs[i].offset, &val16);
			val = val16;
			break;
		case 4:
			ret = pci_read_config32(pci_dev->busn,
					(pci_dev->slot << 3) | pci_dev->func,
					regs[i].offset, &val32);
			val = val32;
			break;
		default:
			printf("Bad register width: %d\n", regs[i].width);
			return;
		}

		if (ret == 0) {
			printf("%s (%" PRIx8 ", %d): %" PRIx32 "\n",
					regs[i].name, regs[i].offset, regs[i].width, val);
		} else {
			printf("%s (%" PRIx8 ", %d): ERROR %" PRIx32 "\n",
					regs[i].name, regs[i].offset, regs[i].width, ret);
		}

		i++;
	}
}


int main(int argc, char **argv) {
	int opt;
	struct pci_slot_dev *pci_dev;
	char *endptr;
	uint32_t offset = 0;
	uint32_t length = 64;

	int full = 0;
	int dump = 0;
	int names = 0;

	uint32_t busn = 0;
	int busn_set = 0;
	uint8_t  slot = 0;
	int slot_set = 0;
	uint8_t  func = 0;
	int func_set = 0;

	while (-1 != (opt = getopt(argc, argv, "hfxb:s:u:nl:o:r:w:"))) {
		switch (opt) {
		case 'f':
			full = 1;
			break;
		case 'x':
			dump = 1;
			break;
		case 'b':
			busn_set = 1;
			busn = strtoul(optarg, &endptr, 0);
			break;
		case 's':
			slot_set = 1;
			slot = strtoul(optarg, &endptr, 0);
			break;
		case 'u':
			func_set = 1;
			func = strtoul(optarg, &endptr, 0);
			break;
		case 'n':
			names = 1;
			break;
		case 'l':
			length = strtoul(optarg, &endptr, 0);
			break;
		case 'o':
			offset = strtoul(optarg, &endptr, 0);
			break;
		case '?':
		case 'h':
			print_usage();
			return 0;
		default:
			print_error();
			return 0;
		}
	}

	pci_foreach_dev(pci_dev) {
		if (busn_set && pci_dev->busn != busn) continue;
		if (slot_set && pci_dev->slot != slot) continue;
		if (func_set && pci_dev->func != func) continue;
		show_device(pci_dev, full);

		if (dump) {
			if (names)
				dump_regs2(pci_dev);
			else
				dump_regs(pci_dev, offset, length);
		}
	}

	return 0;
}
