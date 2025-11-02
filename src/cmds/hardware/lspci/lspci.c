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

#include <lib/libds/array.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_repo.h>
#include <drivers/pci/pci_chip/pci_utils.h>

#include "lspci.h"

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

static inline uintptr_t pci_get_region_mask(uintptr_t region_reg) {
	if (region_reg & 0x1) {
		return region_reg & ~((1 << 2) - 1);
	} else {
		return region_reg & ~((1 << 4) - 1);
	}
}

static inline size_t pci_get_region_size(uint32_t region_reg) {
	if (0 == (region_reg & 0x1)) {
		return 1 << 24;
	} else {
		return 1 << 8;
	}
}

static inline int get_command(struct pci_slot_dev *pci_dev, uint16_t *val16) {
	int ret;

	ret = pci_read_config16(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func,
			PCI_COMMAND, val16);
	if (PCIUTILS_SUCCESS == ret) {
		return 0;
	}
	return -1;
}

static inline int get_status(struct pci_slot_dev *pci_dev, uint16_t *val16) {
	int ret;

	ret = pci_read_config16(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func,
			PCI_STATUS, val16);
	if (PCIUTILS_SUCCESS == ret) {
		return 0;
	}
	return -1;
}

static inline int get_cap_id(struct pci_slot_dev *pci_dev, uint32_t where, uint8_t *val8) {
	int ret;

	ret = pci_read_config8(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func,
			where + PCI_CAP_LIST_ID, val8);
	if (PCIUTILS_SUCCESS == ret) {
		return 0;
	}
	return -1;
}

static inline int get_cap_next(struct pci_slot_dev *pci_dev, uint32_t where, uint8_t *val8) {
	int ret;

	ret = pci_read_config8(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func,
			where + PCI_CAP_LIST_NEXT, val8);
	if (PCIUTILS_SUCCESS == ret) {
		*val8 &= ~0x3;
		return 0;
	}
	return -1;
}

static inline int get_cap_flags(struct pci_slot_dev *pci_dev, uint32_t where, uint16_t *val16) {
	int ret;

	ret = pci_read_config16(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func,
			where + PCI_CAP_FLAGS, val16);
	if (PCIUTILS_SUCCESS == ret) {
		return 0;
	}
	return -1;
}

static void cap_msi(struct pci_slot_dev *d, int where, int cap) {
	int is64;
	uint32_t t;
	uint16_t w;

	printf("MSI: Enable%c Count=%d/%d Maskable%c 64bit%c\n",
			FLAG(cap, PCI_MSI_FLAGS_ENABLE),
			1 << ((cap & PCI_MSI_FLAGS_QSIZE) >> 4),
			1 << ((cap & PCI_MSI_FLAGS_QMASK) >> 1),
			FLAG(cap, PCI_MSI_FLAGS_MASK_BIT), FLAG(cap, PCI_MSI_FLAGS_64BIT));

	is64 = cap & PCI_MSI_FLAGS_64BIT;

	printf("\t\tAddress: ");
	if (is64) {
		t = get_conf_long(d, where + PCI_MSI_ADDRESS_HI);
		w = get_conf_word(d, where + PCI_MSI_DATA_64);
		printf("%08" PRIx32, t);
	} else
		w = get_conf_word(d, where + PCI_MSI_DATA_32);
	t = get_conf_long(d, where + PCI_MSI_ADDRESS_LO);
	printf("%08" PRIx32 "  Data: %04" PRIx16 "\n", t, w);
	if (cap & PCI_MSI_FLAGS_MASK_BIT) {
		uint32_t mask, pending;

		if (is64) {

			mask = get_conf_long(d, where + PCI_MSI_MASK_BIT_64);
			pending = get_conf_long(d, where + PCI_MSI_PENDING_64);
		} else {

			mask = get_conf_long(d, where + PCI_MSI_MASK_BIT_32);
			pending = get_conf_long(d, where + PCI_MSI_PENDING_32);
		}
		printf("\t\tMasking: %08" PRIx32 "  Pending: %08" PRIx32 "\n", mask, pending);
	}
}

static void cap_msix(struct pci_slot_dev *d, int where, int cap) {
	uint32_t off;

	printf("MSI-X: Enable%c Count=%d Masked%c\n", FLAG(cap, PCI_MSIX_ENABLE),
			(cap & PCI_MSIX_TABSIZE) + 1, FLAG(cap, PCI_MSIX_MASK));

	off = get_conf_long(d, where + PCI_MSIX_TABLE);
	printf("\t\tVector table: BAR=%" PRId32 " offset=%08" PRIx32 "\n", off & PCI_MSIX_BIR,
			off & ~PCI_MSIX_BIR);
	off = get_conf_long(d, where + PCI_MSIX_PBA);
	printf("\t\tPBA: BAR=%" PRId32 " offset=%08" PRIx32 "\n", off & PCI_MSIX_BIR,
			off & ~PCI_MSIX_BIR);
}

static void cap_slotid(int cap) {
	int esr = cap & 0xff;
	int chs = cap >> 8;

	printf("Slot ID: %d slots, First%c, chassis %02x\n",
			esr & PCI_SID_ESR_NSLOTS, FLAG(esr, PCI_SID_ESR_FIC), chs);
}

static void cap_debug_port(int cap) {
	int bar = cap >> 13;
	int pos = cap & 0x1fff;

	printf("Debug port: BAR=%d offset=%04x\n", bar, pos);
}

static void show_capabilities(struct pci_slot_dev *pci_dev) {
	int can_have_ext_caps = 0;
	uint32_t where;
	uint8_t val8;


	pci_read_config8(pci_dev->busn,
			(pci_dev->slot << 3) | pci_dev->func,
			PCI_CAPABILITY_LIST, &val8);

	where = val8;

	while (where) {
		uint8_t id, next;
		uint16_t cap;
		printf("\tCapabilities: ");

		get_cap_id(pci_dev, where, &id);
		get_cap_next(pci_dev, where, &next);
		get_cap_flags(pci_dev, where, &cap);
		printf("[%02" PRIx32 "] ", where);

		if (id == 0xff) {
			printf("<chain broken>\n");
			break;
		}
		switch (id) {
		case PCI_CAP_ID_NULL:
			printf("Null\n");
			break;
		case PCI_CAP_ID_PM:
			//cap_pm(d, where, cap);
			printf("cap_pm\n");
			break;
		case PCI_CAP_ID_AGP:
			//cap_agp(d, where, cap);
			printf("cap_agp\n");
			break;
		case PCI_CAP_ID_VPD:
			//cap_vpd(d);
			printf("cap_vpd\n");
			break;
		case PCI_CAP_ID_SLOTID:
			cap_slotid(cap);
			break;
		case PCI_CAP_ID_MSI:
			cap_msi(pci_dev, where, cap);
			break;
		case PCI_CAP_ID_CHSWP:
			printf("CompactPCI hot-swap <?>\n");
			break;
		case PCI_CAP_ID_PCIX:
			//cap_pcix(d, where);
			printf("cap_pcix\n");
			can_have_ext_caps = 1;
			break;
		case PCI_CAP_ID_HT:
			//cap_ht(d, where, cap);
			printf("cap_ht\n");
			break;
		case PCI_CAP_ID_VNDR:
			//show_vendor_caps(d, where, cap);
			printf("show_vendor_caps\n");
			break;
		case PCI_CAP_ID_DBG:
			cap_debug_port(cap);
			break;
		case PCI_CAP_ID_CCRC:
			printf("CompactPCI central resource control <?>\n");
			break;
		case PCI_CAP_ID_HOTPLUG:
			printf("Hot-plug capable\n");
			break;
		case PCI_CAP_ID_SSVID:
			//cap_ssvid(d, where);
			printf("cap_ssvid\n");
			break;
		case PCI_CAP_ID_AGP3:
			printf("AGP3 <?>\n");
			break;
		case PCI_CAP_ID_SECURE:
			printf("Secure device <?>\n");
			break;
		case PCI_CAP_ID_EXP:
			//type = cap_express(d, where, cap);
			printf("cap_express\n");
			can_have_ext_caps = 1;
			break;
		case PCI_CAP_ID_MSIX:
			cap_msix(pci_dev, where, cap);
			break;
		case PCI_CAP_ID_SATA:
			//cap_sata_hba(d, where, cap);
			printf("cap_sata_hba\n");
			break;
		case PCI_CAP_ID_AF:
			//cap_af(d, where);
			printf("cap_af\n");
			break;
		case PCI_CAP_ID_EA:
			//cap_ea(d, where, cap);
			printf("cap_ea\n");
			break;
		default:
			printf("Capability ID %#02x [%04x]\n", id, cap);
		}
		where = next;
	}

	if (can_have_ext_caps) {
		printf("\tCapabilities: show_ext_caps\n");
	}
}

static void show_control(uint16_t val16) {
	printf("\t  Control: ");

	printf("I/O%c ", val16 & PCI_COMMAND_IO ? '+' : '-');
	printf("Mem%c ", val16 & PCI_COMMAND_MEMORY ? '+' : '-');
	printf("BusMaster%c ", val16 & PCI_COMMAND_MASTER ? '+' : '-');
	printf("SpecCycle%c ", val16 & PCI_COMMAND_SPECIAL ? '+' : '-');
	printf("VGASnoop%c ", val16 & PCI_COMMAND_VGA_PALETTE ? '+' : '-');
	printf("ParErr%c ", val16 & PCI_COMMAND_PARITY ? '+' : '-');
	printf("Stepping%c ", val16 & PCI_COMMAND_WAIT ? '+' : '-');
	printf("SERR%c ", val16 & PCI_COMMAND_SERR ? '+' : '-');
	printf("FastB2B%c ", val16 & PCI_COMMAND_FAST_BACK ? '+' : '-');
	printf("DisINTx%c\n", val16 & PCI_COMMAND_INTX_DISABLE ? '+' : '-');
}

static void show_status(uint16_t val16) {
	printf("\t  Status: ");

	printf("Cap%c ", val16 & PCI_STATUS_CAP_LIST ? '+' : '-');
	printf("66MHz%c ", val16 & PCI_STATUS_66MHZ ? '+' : '-');
	printf("UDF%c ", val16 & PCI_STATUS_UDF ? '+' : '-');
	printf("FastB2B%c ", val16 & PCI_STATUS_FAST_BACK ? '+' : '-');
	printf("ParErr%c ", val16 & PCI_STATUS_PARITY ? '+' : '-');
	printf("DEVSEL=");
	switch(val16 & PCI_STATUS_DEVSEL_MASK) {
	case PCI_STATUS_DEVSEL_FAST:
		printf("fast ");
		break;
	case PCI_STATUS_DEVSEL_MEDIUM:
		printf("med ");
		break;
	default:
		printf("slow ");
		break;
	}
	printf(">TAbort%c ", val16 & PCI_STATUS_SIG_TARGET_ABORT ? '+' : '-');
	printf("<TAbort%c ", val16 & PCI_STATUS_REC_TARGET_ABORT ? '+' : '-');
	printf("<MAbort%c ", val16 & PCI_STATUS_REC_MASTER_ABORT ? '+' : '-');
	printf(">SERR%c ", val16 & PCI_STATUS_SIG_SYSTEM_ERROR ? '+' : '-');
	printf("<PERR%c ", val16 & PCI_STATUS_DETECTED_PARITY ? '+' : '-');
	printf("INTx%c\n", val16 & PCI_STATUS_INTERRUPT ? '+' : '-');
}

static void show_regions(struct pci_slot_dev *pci_dev) {
	int bar_num;

	for (bar_num = 0; bar_num < ARRAY_SIZE(pci_dev->bar); bar_num ++) {
		uintptr_t base_addr = pci_dev->bar[bar_num];

		if (0 == base_addr) {
			continue;
		}
		printf("\t  Region (%s): Base: 0x%" PRIXPTR " [0x%" PRIXPTR "]\n",
				pci_get_region_type(base_addr),
				pci_get_region_mask(base_addr),
				pci_get_region_mask(base_addr) +
				(pci_get_region_size(base_addr) - 1));
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
		uint16_t status;
		uint16_t ctrl;
		int ret;

		ret = get_command(pci_dev, &ctrl);
		if (ret) {
			return;
		}

		ret = get_status(pci_dev, &status);
		if (ret) {
			return;
		}

		show_control(ctrl);

		show_status(status);

		printf("\t  IRQ number: %d\n", pci_dev->irq);

		show_regions(pci_dev);

		if (status & PCI_STATUS_CAP_LIST) {
			show_capabilities(pci_dev);
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
			printf("%x%x", val/16, val%16);
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
