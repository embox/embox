/**
 * @file
 * @brief
 *
 * @date 02.04.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <stddef.h>

#include <drivers/pci/pci.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <util/indexator.h>
#include <util/log.h>

#define PCI_SPACE_BASE  OPTION_GET(NUMBER, pci_space_base)
#define PCI_SPACE_SIZE  OPTION_GET(NUMBER, pci_space_size)
#define PCI_WINDOW_SIZE OPTION_GET(NUMBER, pci_window_size)

EMBOX_UNIT_INIT(pci_bios_init);

#include <util/binalign.h>

struct space_allocator {
	void *space_base;
	size_t space_size;
};

static struct space_allocator pci_allocator = {
    .space_base = (void *)PCI_SPACE_BASE,
    .space_size = PCI_SPACE_SIZE,
};

void *space_alloc(struct space_allocator *allocator, size_t win, size_t align) {
	void *ret;
	allocator->space_base =
	    (void *)binalign_bound((size_t)allocator->space_base, align);

	ret = allocator->space_base;
	allocator->space_base = (void *)((size_t)(allocator->space_base) + win);

	return ret;
}

void *space_current(struct space_allocator *allocator) {
	return allocator->space_base;
}

static void pci_bus_configure(uint32_t busn);

static int pci_slot_configure(uint32_t busn, uint32_t devfn) {
	int bar_num;
	uint32_t length;
	uint32_t bar;
	void *window;

	for (bar_num = 0; bar_num < 6; bar_num++) {
		/* Write all '1' */
		pci_write_config32(busn, devfn, PCI_BASE_ADDR_REG_0 + (bar_num << 2),
		    0xFFFFFFFF);
		/* Read back size */
		pci_read_config32(busn, devfn, PCI_BASE_ADDR_REG_0 + (bar_num << 2),
		    &bar);
		/* if no bar available */
		if (bar == 0) {
			continue;
		}
		length = 1 + ~(bar & 0xFFFFFFF0);

		window = space_alloc(&pci_allocator, length, length);
		pci_write_config32(busn, devfn, PCI_BASE_ADDR_REG_0 + (bar_num << 2),
		    (uint32_t)(uintptr_t)window);
		log_debug("pci bus %d fn = %d bar_num %d "
		          "bar = 0x%X win = 0x%X len = 0x%X\n",
		    busn, devfn, bar_num, bar, (uint32_t)(uintptr_t)window,
		    (uint32_t)(uintptr_t)length);
	}
	return 0;
}

INDEX_DEF(bus_indexator, 0, 32);
static int pci_bridge_configure(int busn, int devfn) {
	int subord;
	size_t newbusn;
	uint32_t memconf;
	void *space_base, *space_end;

	/* align space at 1Mb */
	space_base = space_alloc(&pci_allocator, 0, PCI_WINDOW_SIZE);
	newbusn = index_alloc(&bus_indexator, INDEX_MIN);
	assert(newbusn != INDEX_NONE);
	/* enable new bus with all subordinate
	 * primary = busn
	 * secondary = newbusn
	 * subordinate = 0xFF
	 */

	pci_write_config32(busn, devfn, PCI_PRIMARY_BUS,
	    (busn) | (newbusn << 8) | (0xFF << 16));

	log_debug("bridge start configure busn %d newbus %d\n*******", busn,
	    newbusn);

	pci_bus_configure(newbusn);
	subord = index_find(&bus_indexator, INDEX_MIN) - 1;
	pci_write_config32(busn, devfn, PCI_PRIMARY_BUS,
	    (busn) | (newbusn << 8) | (subord << 16));

	log_debug("bridge start configure subordinate %d\n*******", subord);

	/* align space at 1Mb and check the difference */
	space_end = space_alloc(&pci_allocator, 0, PCI_WINDOW_SIZE);
	if (space_base < space_end) {
		memconf = ((uintptr_t)(space_base) >> 16) & 0xFFF0;
		memconf |= ((uintptr_t)(space_end)-1) & 0xFFF00000;

		pci_write_config32(busn, devfn, 0x20, memconf);
	}

	return 0;
}
extern uint32_t pci_get_vendor_id(uint32_t bus, uint32_t devfn);
static void pci_bus_configure(uint32_t busn) {
	uint32_t devfn, vendor_reg;
	uint8_t hdr_type, is_multi = 0;

	for (devfn = MIN_DEVFN; devfn < MAX_DEVFN; ++devfn) {
		/* Devices are required to implement function 0, so if
		 * it's missing then there is no device here. */
		if (PCI_FUNC(devfn) && !is_multi) {
			/* Not a multiple function device */
			continue;
		}
		if (-1 == (vendor_reg = pci_get_vendor_id(busn, devfn))) {
			continue;
		}

		pci_read_config8(busn, devfn, PCI_HEADER_TYPE, &hdr_type);
		if (!PCI_FUNC(devfn)) {
			/* If bit 7 of this register is set, the device
			 * has multiple functions;
			 * otherwise, it is a single function device */
			is_multi = hdr_type & (1 << 7);
		}

		/*The header type is divided into two sections.
		 * Bits 6..0 comprise the header type. Bit 7 is the single/multi
		 * function device flag (0=single 1=multi). The header type specifies
		 * the format of bytes 0x10 to 0x3f. The two defined types are 0x00,
		 * the standard header type (pictured above), and 0x01,
		 * PCI-PCI bridge.*/
		if ((hdr_type & 0x7F) == 1) { /* bridge */
			/*		    new_dev->baseclass == PCI_BASE_CLASS_BRIDGE &&
		    new_dev->subclass == PCI_CLASS_BRIDGE_PCI)
		     */
			pci_bridge_configure(busn, devfn);
		}
		else { /* not bridge */
			pci_slot_configure(busn, devfn);
		}
		/* Enable bus mastering and memory requests processing */
		pci_write_config32(busn, devfn, PCI_COMMAND,
		    PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);
	}
}

static int pci_bios_init(void) {
	size_t busn;
	busn = index_alloc(&bus_indexator, INDEX_MIN);
	if (busn == INDEX_NONE) {
		return -ENOMEM;
	}
	pci_bus_configure(busn);

	return 0;
}
