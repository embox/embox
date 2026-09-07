/**
 * @file
 * @brief DesignWare PCIe root complex backend.
 *
 * Generic host backend for the Synopsys DesignWare PCIe IP in root
 * complex mode. Per-controller coordinates (APB/DBI/config/memory
 * windows, flat bus number, legacy IRQ) are Mybuild options, so the
 * same driver covers every Rockchip SoC that integrates the DWC
 * controller by supplying different option values.
 *
 * Links are trained by the firmware: the board U-Boot preboot runs
 * `pci enum` before the kernel image is loaded, and the vendor
 * firmware leaves the PHYs, clocks and PERST# in their working state.
 * Per the firmware/OS division of labor this backend inherits that
 * state: it never asserts controller resets and never touches the
 * PHYs. It only checks that each link is up, programs its own iATU
 * windows and serves the flat embox PCI configuration space through
 * the DWC DBI/iATU register file.
 *
 * Configuration space of the root port lives in the DBI window.
 * Downstream configuration accesses are issued by retargeting a
 * single outbound iATU region per access, with the PCI TLP address
 * (bus << 24 | dev << 19 | func << 16) as the region target and the
 * controller local config window as the CPU base.
 *
 * Flat bus numbering: each controller owns two buses,
 *   bus bus_base     : root port
 *   bus bus_base + 1 : downstream devices
 * (bus_base is a per-controller option; e.g. the RK3568 has pcie2x1
 * on buses 0-1 and pcie3x2 on buses 2-3).
 */

#include <stdint.h>

#include <drivers/common/memory.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_chip/pci_utils.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/mem_barriers.h>
#include <hal/reg.h>
#include <util/log.h>

#define APB0_BASE OPTION_GET(NUMBER, apb0_base)
#define DBI0_BASE ((uint64_t)OPTION_GET(NUMBER, dbi0_base))
#define CFG0_BASE ((uint64_t)OPTION_GET(NUMBER, cfg0_base))
#define MEM0_BASE ((uint64_t)OPTION_GET(NUMBER, mem0_base))
#define MEM0_SIZE ((uint64_t)OPTION_GET(NUMBER, mem0_size))
#define BUS0_BASE OPTION_GET(NUMBER, bus0_base)
#define INTX0_IRQ OPTION_GET(NUMBER, intx0_irq)

#define APB1_BASE OPTION_GET(NUMBER, apb1_base)
#define DBI1_BASE ((uint64_t)OPTION_GET(NUMBER, dbi1_base))
#define CFG1_BASE ((uint64_t)OPTION_GET(NUMBER, cfg1_base))
#define MEM1_BASE ((uint64_t)OPTION_GET(NUMBER, mem1_base))
#define MEM1_SIZE ((uint64_t)OPTION_GET(NUMBER, mem1_size))
#define BUS1_BASE OPTION_GET(NUMBER, bus1_base)
#define INTX1_IRQ OPTION_GET(NUMBER, intx1_irq)

/* The ITS message-signalled interrupt doorbell (GITS_TRANSLATER). Every
 * controller routes its endpoints' MSI/MSI-X writes here, so the value is
 * shared across controllers: the GIC is one device in the SoC. */
#define DOORBELL_BASE ((uint64_t)OPTION_GET(NUMBER, doorbell_base))
#define DOORBELL_SIZE ((uint64_t)OPTION_GET(NUMBER, doorbell_size))

/* Rockchip client APB block */
#define PCIE_CLIENT_LTSSM_STATUS 0x300
#define LTSSM_SMLH_LINKUP        (1u << 16)
#define LTSSM_RDLH_LINKUP        (1u << 17)
#define LTSSM_LINKUP_MASK        (LTSSM_SMLH_LINKUP | LTSSM_RDLH_LINKUP)
#define LTSSM_STATE_MASK         0x3f
#define LTSSM_STATE_L0           0x11

/* Root port link capability/status inside the DBI register file */
#define DW_LINK_CAPABILITY      0x7c
#define DW_LINK_STATUS          0x80
#define LINK_STATUS_SPEED_SHIFT 16
#define LINK_STATUS_WIDTH_SHIFT 20

/* Unrolled outbound iATU register view: one block per region index */
#define ATU_REG_BLOCK(idx) (((uintptr_t)0x3 << 20) | ((idx) << 9))

/* Unrolled inbound iATU register view. The inbound regions live in the
 * same 0x300000 offset window, offset by one bit (0x100) from the outbound
 * regions, as in the DWC unroll iATU layout. */
#define ATU_REG_BLOCK_INB(idx) \
	(((uintptr_t)0x3 << 20) | ((idx) << 9) | ((uintptr_t)0x1 << 8))

#define ATU_CTRL1        0x00
#define ATU_CTRL2        0x04
#define ATU_BASE_LO      0x08
#define ATU_BASE_HI      0x0c
#define ATU_LIMIT        0x10
#define ATU_TGT_LO       0x14
#define ATU_TGT_HI       0x18
#define ATU_CTRL2_ENABLE (1u << 31)
#define ATU_TYPE_MEM     0x0
#define ATU_TYPE_CFG0    0x4

#define ATU_REGION_MEM 0
#define ATU_REGION_CFG 1

/* Inbound region carrying the MSI/MSI-X doorbell window. */
#define ATU_REGION_DOORBELL 0

/* The config window doubles as the transient CPU base of the CFG
 * iATU region, so it must be at least one 4K config space page and
 * comfortably below 4G (all TLP addresses here are 32 bit). */
#define CFG_WINDOW_SIZE 0x100000

#define ATU_ENABLE_ACK_RETRIES 1000

#define PCIE_DW_CTRL_N 2

struct pcie_dw_ctrl {
	uint64_t apb_base;
	uint64_t dbi_base;
	uint64_t cfg_base;
	uint64_t mem_base;
	uint64_t mem_size;
	uint8_t bus_base;
	uint8_t intx_irq;
	uint8_t link_up;
};

static struct pcie_dw_ctrl pcie_dw_ctrls[PCIE_DW_CTRL_N] = {
    {
        .apb_base = APB0_BASE,
        .dbi_base = DBI0_BASE,
        .cfg_base = CFG0_BASE,
        .mem_base = MEM0_BASE,
        .mem_size = MEM0_SIZE,
        .bus_base = BUS0_BASE,
        .intx_irq = INTX0_IRQ,
    },
    {
        .apb_base = APB1_BASE,
        .dbi_base = DBI1_BASE,
        .cfg_base = CFG1_BASE,
        .mem_base = MEM1_BASE,
        .mem_size = MEM1_SIZE,
        .bus_base = BUS1_BASE,
        .intx_irq = INTX1_IRQ,
    },
};

/* The per-controller MMIO areas are identity mapped by the
 * periph_memory layer. The DBI windows sit above 4G. */
PERIPH_MEMORY_DEFINE(pcie_dw0_apb, APB0_BASE, 0x1000);
PERIPH_MEMORY_DEFINE(pcie_dw0_dbi, DBI0_BASE, 0x400000);
PERIPH_MEMORY_DEFINE(pcie_dw0_cfg, CFG0_BASE, CFG_WINDOW_SIZE);
PERIPH_MEMORY_DEFINE(pcie_dw0_mem, MEM0_BASE, MEM0_SIZE);

PERIPH_MEMORY_DEFINE(pcie_dw1_apb, APB1_BASE, 0x1000);
PERIPH_MEMORY_DEFINE(pcie_dw1_dbi, DBI1_BASE, 0x400000);
PERIPH_MEMORY_DEFINE(pcie_dw1_cfg, CFG1_BASE, CFG_WINDOW_SIZE);
PERIPH_MEMORY_DEFINE(pcie_dw1_mem, MEM1_BASE, MEM1_SIZE);

static struct pcie_dw_ctrl *pcie_dw_ctrl_for_bus(uint32_t bus) {
	int i;

	for (i = 0; i < PCIE_DW_CTRL_N; i++) {
		struct pcie_dw_ctrl *c = &pcie_dw_ctrls[i];

		if (!c->link_up) {
			continue;
		}
		if (bus >= c->bus_base && bus <= c->bus_base + 1) {
			return c;
		}
	}
	return NULL;
}

static void pcie_dw_atu_map(struct pcie_dw_ctrl *c, int idx,
    uint32_t type, uint64_t cpu_base, uint64_t size, uint64_t pci_base) {
	uintptr_t reg = (uintptr_t)(c->dbi_base + ATU_REG_BLOCK(idx));
	int i;

	REG32_STORE(reg + ATU_CTRL2, 0);
	REG32_STORE(reg + ATU_CTRL1, type);
	REG32_STORE(reg + ATU_BASE_LO, (uint32_t)cpu_base);
	REG32_STORE(reg + ATU_BASE_HI, (uint32_t)(cpu_base >> 32));
	REG32_STORE(reg + ATU_LIMIT, (uint32_t)(cpu_base + size - 1));
	REG32_STORE(reg + ATU_TGT_LO, (uint32_t)pci_base);
	REG32_STORE(reg + ATU_TGT_HI, (uint32_t)(pci_base >> 32));
	REG32_STORE(reg + ATU_CTRL2, ATU_CTRL2_ENABLE);

	/* Commit the enable write before polling it back. The poll read
	 * must observe the store the controller already has, otherwise a
	 * stale CTRL2 would make us log a false failure. */
	dsb(st);

	/* Wait until the enable bit sticks (DBI write flush) */
	for (i = 0; i < ATU_ENABLE_ACK_RETRIES; i++) {
		if (REG32_LOAD(reg + ATU_CTRL2) & ATU_CTRL2_ENABLE) {
			return;
		}
	}
	log_error("pcie: iATU region %d enable did not stick", idx);
}

static void pcie_dw_setup_atu(struct pcie_dw_ctrl *c) {
	/* Identity mapped memory window covering the endpoint BARs */
	pcie_dw_atu_map(c, ATU_REGION_MEM, ATU_TYPE_MEM, c->mem_base,
	    c->mem_size, c->mem_base);
	/* Configuration window, retargeted per downstream access */
	pcie_dw_atu_map(c, ATU_REGION_CFG, ATU_TYPE_CFG0, c->cfg_base,
	    CFG_WINDOW_SIZE, c->bus_base + 1);
}

/* Route an endpoint's MSI/MSI-X write (a posted Memory Write TLP to the
 * ITS GITS_TRANSLATER) from the PCIe side into the system address space.
 *
 * For inbound regions the register roles swap relative to the outbound
 * regions above: BASE/LIMIT describe the PCI-side (TLP) address and
 * TARGET is the system-side (AXI) address the write is forwarded to.
 * The doorbell window is identity mapped, so both sides use the same
 * base. This window is what turns the endpoint's completion write into
 * an ITS translation; without it the write is dropped at the root port
 * and no LPI is ever produced. */
static void pcie_dw_inbound_map(struct pcie_dw_ctrl *c, int idx,
    uint64_t pci_base, uint64_t cpu_base, uint64_t size) {
	uintptr_t reg = (uintptr_t)(c->dbi_base + ATU_REG_BLOCK_INB(idx));
	int i;

	REG32_STORE(reg + ATU_CTRL2, 0);
	REG32_STORE(reg + ATU_CTRL1, ATU_TYPE_MEM);
	REG32_STORE(reg + ATU_BASE_LO, (uint32_t)pci_base);
	REG32_STORE(reg + ATU_BASE_HI, (uint32_t)(pci_base >> 32));
	REG32_STORE(reg + ATU_LIMIT, (uint32_t)(pci_base + size - 1));
	REG32_STORE(reg + ATU_TGT_LO, (uint32_t)cpu_base);
	REG32_STORE(reg + ATU_TGT_HI, (uint32_t)(cpu_base >> 32));
	REG32_STORE(reg + ATU_CTRL2, ATU_CTRL2_ENABLE);

	dsb(st);

	for (i = 0; i < ATU_ENABLE_ACK_RETRIES; i++) {
		if (REG32_LOAD(reg + ATU_CTRL2) & ATU_CTRL2_ENABLE) {
			return;
		}
	}
	log_error("pcie: inbound iATU region %d enable did not stick", idx);
}

static int pcie_dw_cfg_map(struct pcie_dw_ctrl *c, uint32_t bus,
    uint32_t devfn, uint32_t where, uintptr_t *va) {
	uint32_t tlp;

	if (where >= CFG_WINDOW_SIZE) {
		return PCIUTILS_INVALID;
	}

	if (bus == c->bus_base) {
		/* Root port: the DBI register file is its config space.
		 * A DWC root complex has no devices past slot 0. */
		if (PCI_SLOT(devfn) != 0) {
			return PCIUTILS_TIMEOUT;
		}
		*va = (uintptr_t)c->dbi_base + (where & ~0x3u);
		return PCIUTILS_SUCCESS;
	}

	/* Only slot 0 exists below a root port in PCIe. Probing other
	 * slots is not just wasted time: single-device links tend to
	 * answer config TLPs for any slot number, which plants phantom
	 * devices into the scan, and some endpoints never complete such
	 * TLPs at all, wedging the CPU on the config read. */
	if (PCI_SLOT(devfn) != 0) {
		return PCIUTILS_TIMEOUT;
	}

	tlp = (bus << 24) | (PCI_SLOT(devfn) << 19) | (PCI_FUNC(devfn) << 16);
	pcie_dw_atu_map(c, ATU_REGION_CFG, ATU_TYPE_CFG0, c->cfg_base,
	    CFG_WINDOW_SIZE, tlp);
	*va = (uintptr_t)c->cfg_base + (where & ~0x3u);
	return PCIUTILS_SUCCESS;
}

static uint32_t pcie_dw_config_read(uint32_t bus, uint32_t dev_fn,
    uint32_t where, void *value, size_t size) {
	struct pcie_dw_ctrl *c;
	uintptr_t va;
	uint32_t tmp;
	int ret;

	c = pcie_dw_ctrl_for_bus(bus);
	if (c == NULL) {
		tmp = 0xffffffff;
		ret = PCIUTILS_TIMEOUT;
	}
	else if ((ret = pcie_dw_cfg_map(c, bus, dev_fn, where, &va))
	         != PCIUTILS_SUCCESS) {
		tmp = 0xffffffff;
	}
	else {
		tmp = REG32_LOAD(va);
		ret = PCIUTILS_SUCCESS;
	}

	switch (size) {
	case 1:
		*(uint8_t *)value = tmp >> (8 * (where & 3));
		break;
	case 2:
		*(uint16_t *)value = tmp >> (8 * (where & 3));
		break;
	default:
		*(uint32_t *)value = tmp;
		break;
	}

	return ret;
}

static uint32_t pcie_dw_config_write(uint32_t bus, uint32_t dev_fn,
    uint32_t where, uint32_t value, size_t size) {
	struct pcie_dw_ctrl *c;
	uintptr_t va;
	uint32_t tmp;
	uint32_t shift;
	int ret;

	c = pcie_dw_ctrl_for_bus(bus);
	if (c == NULL) {
		return PCIUTILS_TIMEOUT;
	}
	ret = pcie_dw_cfg_map(c, bus, dev_fn, where, &va);
	if (ret != PCIUTILS_SUCCESS) {
		return ret;
	}

	/* The iATU config window only accepts DWORD accesses: merge
	 * sub-dword writes into a read-modify-write of the DWORD. */
	shift = 8 * (where & 3);
	tmp = REG32_LOAD(va);
	switch (size) {
	case 1:
		tmp = (tmp & ~(0xffu << shift)) | ((value & 0xff) << shift);
		break;
	case 2:
		tmp = (tmp & ~(0xffffu << shift)) | ((value & 0xffff) << shift);
		break;
	default:
		tmp = value;
		break;
	}
	REG32_STORE(va, tmp);

	return PCIUTILS_SUCCESS;
}

uint32_t pci_read_config8(uint32_t bus, uint32_t dev_fn, uint32_t where,
    uint8_t *value) {
	return pcie_dw_config_read(bus, dev_fn, where, value, 1);
}

uint32_t pci_read_config16(uint32_t bus, uint32_t dev_fn, uint32_t where,
    uint16_t *value) {
	return pcie_dw_config_read(bus, dev_fn, where, value, 2);
}

uint32_t pci_read_config32(uint32_t bus, uint32_t dev_fn, uint32_t where,
    uint32_t *value) {
	return pcie_dw_config_read(bus, dev_fn, where, value, 4);
}

uint32_t pci_write_config8(uint32_t bus, uint32_t dev_fn, uint32_t where,
    uint8_t value) {
	return pcie_dw_config_write(bus, dev_fn, where, value, 1);
}

uint32_t pci_write_config16(uint32_t bus, uint32_t dev_fn, uint32_t where,
    uint16_t value) {
	return pcie_dw_config_write(bus, dev_fn, where, value, 2);
}

uint32_t pci_write_config32(uint32_t bus, uint32_t dev_fn, uint32_t where,
    uint32_t value) {
	return pcie_dw_config_write(bus, dev_fn, where, value, 4);
}

unsigned int pci_irq_number(struct pci_slot_dev *dev) {
	struct pcie_dw_ctrl *c = pcie_dw_ctrl_for_bus(dev->busn);

	/* INTx INTID of the controller legacy interrupt. Informational
	 * only: the current drivers do not use PCIe interrupts. */
	return c ? c->intx_irq : 0;
}

static int pcie_dw_init(void) {
	int i;
	int up = 0;

	for (i = 0; i < PCIE_DW_CTRL_N; i++) {
		struct pcie_dw_ctrl *c = &pcie_dw_ctrls[i];
		uint32_t status;
		uint32_t link;
		uint32_t dbi;

		if (c->apb_base == 0) {
			continue;
		}

		/* Inherit the firmware trained link; never reset. */
		status = REG32_LOAD((uintptr_t)(c->apb_base + PCIE_CLIENT_LTSSM_STATUS));
		if ((status & LTSSM_LINKUP_MASK) != LTSSM_LINKUP_MASK
		    || (status & LTSSM_STATE_MASK) != LTSSM_STATE_L0) {
			log_error("pcie%d: link is not up (LTSSM 0x%08x), skipping;"
			          " run U-Boot 'pci enum'",
			    i, status);
			continue;
		}

		pcie_dw_setup_atu(c);
		pcie_dw_inbound_map(c, ATU_REGION_DOORBELL, DOORBELL_BASE,
		    DOORBELL_BASE, DOORBELL_SIZE);

		link = REG32_LOAD((uintptr_t)(c->dbi_base + DW_LINK_STATUS));
		dbi = REG32_LOAD((uintptr_t)(c->dbi_base + DW_LINK_CAPABILITY));
		log_info("pcie%d: firmware link inherited (bus %d-%d),"
		         " link Gen%x x%d (max Gen%x x%d)",
		    i, c->bus_base, c->bus_base + 1,
		    (link >> LINK_STATUS_SPEED_SHIFT) & 0xf,
		    (link >> LINK_STATUS_WIDTH_SHIFT) & 0x1f, dbi & 0xf,
		    (dbi >> 4) & 0x3f);

		c->link_up = 1;
		up++;
	}

	if (up == 0) {
		log_error("pcie: no link inherited from firmware,"
		          " PCI bus will be empty");
	}

	return 0;
}

EMBOX_UNIT_INIT(pcie_dw_init);
