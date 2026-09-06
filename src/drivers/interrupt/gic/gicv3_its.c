/**
 * @file
 * @brief GICv3 Interrupt Translation Service driver.
 *
 * @author zhugengyu
 * @date 05.09.2026
 *
 * Delivers message-signalled interrupts as LPIs through the ITS of a
 * GIC-600. The driver owns the kernel IRQ window above
 * GIC_LPI_IRQ_BASE (see gic_lpi.h) and translates between those IRQ
 * numbers and the raw LPI INTIDs the CPU interface acknowledges.
 *
 * The firmware leaves the ITS quiescent and without valid state, so
 * the probe builds every table from scratch: the LPI property and
 * pending tables in the redistributor, then the device and collection
 * tables plus the command queue in the ITS proper. Command execution
 * is synchronous: every command is flushed to memory, published via
 * GITS_CWRITER and waited on in GITS_CREADR, which keeps the
 * programming order deterministic at the cost of a bounded spin per
 * command.
 *
 * The ITS fetches all tables over a non-coherent port, so every table
 * write is followed by an explicit dcache flush before the hardware
 * may observe it -- the same discipline the on-board GMAC and NVMe
 * drivers apply to their DMA rings.
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include <drivers/common/memory.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/cache.h>
#include <hal/mem_barriers.h>
#include <hal/reg.h>
#include <util/log.h>

#include "gic_base.h"
#include "gic_lpi.h"
#include "gicv3.h"
#include "gicv3_its.h"

#define ITS_BASE       OPTION_GET(NUMBER, its_base)
#define ITS_LPI_TABLE_BASE OPTION_GET(NUMBER, lpi_table_base)
#define ITS_GICD_TYPER (GICD_BASE + 0x0008)

EMBOX_UNIT_INIT(its_init);

/* Bounded poll loops, the pattern used by the other on-board drivers:
 * a wedged ITS must disable itself instead of hanging the boot. */
#define ITS_QUIESCENT_TRIES 1000000
#define ITS_CMD_TRIES       10000000
#define ITS_RWP_TRIES       1000000

struct its_dev {
	uint32_t devid;
	uint8_t used;
	uint8_t itt[ITS_ITT_ENTRIES * 8] __attribute__((aligned(256)));
};

static struct {
	int ready;
	unsigned int pta;       /* GITS_TYPER.PTA: RDBase encoding */
	uint64_t rd_target;     /* encoded redistributor for MAPC/SYNC */
	unsigned int cmd_tail;  /* next free command queue entry */
	unsigned int lpi_count; /* high-water mark of the LPI window */
	unsigned int dev_count;
	struct its_dev devs[ITS_MAX_DEVICES];
	struct {
		uint32_t devid;
		uint32_t eventid;
		uint8_t used;
	} lpi_map[GIC_LPI_QUANTITY];
} its;

/* Driver-owned tables. The pending table is the only one with a
 * hardware alignment requirement: GICR_PENDBASER stores its address
 * shifted by 16 bits, so it must live 64K aligned. The ITS fetches
 * them over a non-coherent port and the redistributor updates the
 * pending table in place, so they live in an uncached identity
 * mapping instead of cached memory like the command queue. */
static uint64_t its_cmdq[ITS_CMDQ_ENTRIES * 4] __attribute__((aligned(4096)));
static uint64_t its_dev_table[ITS_DEV_TABLE_ENTRIES]
    __attribute__((aligned(4096)));
static uint64_t its_coll_table[ITS_COLL_TABLE_ENTRIES]
    __attribute__((aligned(4096)));

#define its_lpi_prop ((uint8_t *)ITS_LPI_TABLE_BASE)
#define its_lpi_pend ((uint8_t *)ITS_LPI_TABLE_BASE + ITS_PROP_SIZE)

PERIPH_MEMORY_DEFINE(its_regs, ITS_BASE, 0x20000);
PERIPH_MEMORY_DEFINE(its_lpi_tables, ITS_LPI_TABLE_BASE,
    ITS_PROP_SIZE + ITS_PEND_SIZE);

static int its_wait_reg32(uintptr_t reg, uint32_t mask, uint32_t expect,
    unsigned int tries) {
	while (tries-- != 0) {
		if ((REG32_LOAD(reg) & mask) == expect) {
			return 0;
		}
	}
	return -ETIMEDOUT;
}

static int its_wait_rwp(void) {
	return its_wait_reg32(GICR_CTLR, GICR_CTLR_RWP, 0, ITS_RWP_TRIES);
}

static int its_quiescent(void) {
	uint32_t ctlr = REG32_LOAD(ITS_BASE + GITS_CTLR);

	if (ctlr & GITS_CTLR_ENABLED) {
		REG32_STORE(ITS_BASE + GITS_CTLR, ctlr & ~GITS_CTLR_ENABLED);
	}

	/* A wedged queue never reports quiescent; bail out instead of
	 * hanging the boot. */
	return its_wait_reg32(ITS_BASE + GITS_CTLR, GITS_CTLR_QUIESCENT,
	    GITS_CTLR_QUIESCENT, ITS_QUIESCENT_TRIES);
}

static int its_cmd_post(const uint64_t cmd[4]) {
	uint64_t *entry = &its_cmdq[its.cmd_tail * 4];
	uint32_t offset = its.cmd_tail * 32;
	int ret;

	memcpy(entry, cmd, 4 * sizeof(uint64_t));
	/* The ITS reads commands over its non-coherent port: the entry
	 * must reach memory before the writer pointer publishes it. */
	dcache_flush(entry, 4 * sizeof(uint64_t));
	dsb(st);

	its.cmd_tail = (its.cmd_tail + 1) % ITS_CMDQ_ENTRIES;
	REG32_STORE(ITS_BASE + GITS_CWRITER, offset);
	dsb(st);

	/* The queue drains strictly in order: CREADR catching up with the
	 * offset just written means the command has been consumed. */
	ret = its_wait_reg32(ITS_BASE + GITS_CREADR, 0xffffffff, offset, ITS_CMD_TRIES);
	if (ret != 0) {
		log_error("its: command %#llx timed out", (unsigned long long)cmd[0]);
	}

	return ret;
}

static int its_cmd_sync(void) {
	/* SYNC targets a redistributor: with GITS_TYPER.PTA clear the
	 * RDbase field carries the processor number from GICR_TYPER.
	 * Unlike MAPD/MAPC there is no Valid bit in a SYNC. */
	const uint64_t cmd[4] = {ITS_CMD_SYNC, 0,
	    its.rd_target << ITS_CMD_RDBASE_SHIFT, 0};

	return its_cmd_post(cmd);
}

static int its_cmd_mapd(struct its_dev *dev) {
	/* Size encodes log2 of the ITT entry count minus one. */
	const uint64_t cmd[4] = {ITS_CMD_MAPD
	                             | ((uint64_t)dev->devid << ITS_CMD_DEVID_SHIFT),
	    ITS_ITT_ENTRIES == 64 ? 5 : 0,
	    ((uint64_t)(uintptr_t)dev->itt) | ITS_CMD_VALID, 0};
	int ret;

	memset(dev->itt, 0, sizeof(dev->itt));
	dcache_flush(dev->itt, sizeof(dev->itt));
	dsb(st);

	ret = its_cmd_post(cmd);
	if (ret == 0) {
		ret = its_cmd_sync();
	}

	return ret;
}

static int its_cmd_mapc(void) {
	/* word2: collection id (0), redistributor reference, Valid. */
	const uint64_t cmd[4] = {ITS_CMD_MAPC, 0,
	    (its.rd_target << ITS_CMD_RDBASE_SHIFT) | ITS_CMD_VALID, 0};

	return its_cmd_post(cmd);
}

static int its_cmd_mapti(uint32_t devid, uint32_t eventid, unsigned int lpi_slot) {
	const uint64_t cmd[4] = {ITS_CMD_MAPTI
	                             | ((uint64_t)devid << ITS_CMD_DEVID_SHIFT),
	    eventid | ((uint64_t)(GIC_LPI_INTID_BASE + lpi_slot) << ITS_CMD_PINTID_SHIFT),
	    0, /* collection id 0 */
	    0};
	int ret;

	ret = its_cmd_post(cmd);
	if (ret == 0) {
		ret = its_cmd_sync();
	}

	return ret;
}

static int its_cmd_inv(uint32_t devid, uint32_t eventid) {
	const uint64_t cmd[4] = {ITS_CMD_INV | ((uint64_t)devid << ITS_CMD_DEVID_SHIFT),
	    eventid, 0, 0};
	int ret;

	ret = its_cmd_post(cmd);
	if (ret == 0) {
		ret = its_cmd_sync();
	}

	return ret;
}

static int its_lpi_tables_setup(void) {
	uint64_t reg;
	unsigned int id_bits;
	int ret;

	/* Program the redistributor LPI accounting for this CPU. The
	 * pending table is zero-filled through the PTZ bit and delivery
	 * starts disabled; individual LPIs are armed via the property
	 * table once they are mapped. */
	ret = its_wait_rwp();
	if (ret == 0) {
		REG32_CLEAR(GICR_CTLR, ITS_GICR_CTLR_ENABLE_LPIS);
		ret = its_wait_rwp();
	}
	if (ret != 0) {
		return ret;
	}

	/* The tables live in the uncached identity mapping: stores
	 * already reach memory the ITS and redistributor observe. */
	memset(its_lpi_prop, 0, ITS_PROP_SIZE);
	memset(its_lpi_pend, 0, ITS_PEND_SIZE);
	dsb(st);

	/* PROPBASER.IDbits carries (number of LPI INTID bits - 1). The
	 * redistributor sizes its view of the property and pending
	 * tables from this field: this board reports a useless value in
	 * GICD_TYPER.IDbits, so like the reference firmware setup, program
	 * the field for the 64K LPIs the tables are sized for. */
	id_bits = ITS_LPI_IDBITS_MAX - 1;
	log_info("its: PROPBASER.IDbits %u", id_bits);

	reg = ((uint64_t)(uintptr_t)its_lpi_prop)
	      | ((uint64_t)id_bits << ITS_PROP_IDBITS_SHIFT)
	      | ((uint64_t)ITS_CACHE_RAWAWB << ITS_PROP_INNER_SHIFT)
	      | ((uint64_t)ITS_CACHE_INNER_SHAREABLE << ITS_PROP_SHARE_SHIFT)
	      | ((uint64_t)ITS_CACHE_RAWAWB << ITS_PROP_OUTER_SHIFT);
	REG64_STORE(GICR_PROPBASER, reg);

	reg = ((uint64_t)(uintptr_t)its_lpi_pend)
	      | ((uint64_t)ITS_CACHE_RAWAWB << ITS_PEND_INNER_SHIFT)
	      | ((uint64_t)ITS_CACHE_INNER_SHAREABLE << ITS_PEND_SHARE_SHIFT)
	      | ((uint64_t)ITS_CACHE_RAWAWB << ITS_PEND_OUTER_SHIFT) | ITS_PEND_PTZ;
	REG64_STORE(GICR_PENDBASER, reg);

	ret = its_wait_rwp();
	if (ret != 0) {
		return ret;
	}

	REG32_ORIN(GICR_CTLR, ITS_GICR_CTLR_ENABLE_LPIS);

	return its_wait_rwp();
}

static uint64_t its_baser_reg(const void *table, size_t size, unsigned int type) {
	uint64_t pages = (size + 0xfff) / 0x1000;

	return (pages - 1) << ITS_BASER_SIZE_SHIFT
	       | (uint64_t)ITS_CACHE_RAWAWB << ITS_BASER_INNER_SHIFT
	       | (uint64_t)type << ITS_BASER_TYPE_SHIFT
	       | (uint64_t)ITS_CACHE_RAWAWB << ITS_BASER_OUTER_SHIFT
	       | (uint64_t)ITS_CACHE_INNER_SHAREABLE << ITS_BASER_SHARE_SHIFT
	       | ((uint64_t)(uintptr_t)table) | ITS_BASER_VALID;
}

static int its_baser_setup(void) {
	uint64_t reg;

	dcache_flush(its_dev_table, sizeof(its_dev_table));
	dcache_flush(its_coll_table, sizeof(its_coll_table));
	dsb(st);

	reg = its_baser_reg(its_dev_table, sizeof(its_dev_table),
	    ITS_BASER_TYPE_DEVICE);
	REG64_STORE(ITS_BASE + GITS_BASER(0), reg);
	log_info("its: BASER0 devices %016llx -> %016llx", (unsigned long long)reg,
	    (unsigned long long)REG64_LOAD(ITS_BASE + GITS_BASER(0)));

	/* This ITS binds the collection table to BASER1: reset reports
	 * the collection type there and programming any other slot is
	 * silently dropped, which wedges the command queue. */
	reg = its_baser_reg(its_coll_table, sizeof(its_coll_table),
	    ITS_BASER_TYPE_COLLECTION);
	REG64_STORE(ITS_BASE + GITS_BASER(1), reg);
	log_info("its: BASER1 collections %016llx -> %016llx", (unsigned long long)reg,
	    (unsigned long long)REG64_LOAD(ITS_BASE + GITS_BASER(1)));

	return 0;
}

static int its_cmd_queue_setup(void) {
	uint64_t reg;

	/* Size encodes the queue region in 4K pages minus one. */
	reg = ((ITS_CMDQ_ENTRIES * 32) / 0x1000 - 1) << ITS_CBASER_SIZE_SHIFT
	      | (uint64_t)ITS_CACHE_RAWAWB << ITS_CBASER_INNER_SHIFT
	      | (uint64_t)ITS_CACHE_RAWAWB << ITS_CBASER_OUTER_SHIFT
	      | (uint64_t)ITS_CACHE_INNER_SHAREABLE << ITS_CBASER_SHARE_SHIFT
	      | ((uint64_t)(uintptr_t)its_cmdq) | ITS_CBASER_VALID;
	REG64_STORE(ITS_BASE + GITS_CBASER, reg);

	its.cmd_tail = 0;
	REG32_STORE(ITS_BASE + GITS_CWRITER, 0);

	return its_wait_reg32(ITS_BASE + GITS_CREADR, 0xffffffff, 0, ITS_CMD_TRIES);
}

uint64_t gic_its_trans_addr(void) {
	return (uint64_t)(ITS_BASE + GITS_TRANSLATER);
}

int gic_its_device_attach(uint32_t devid) {
	struct its_dev *dev = NULL;
	int i;
	int ret;

	if (!its.ready) {
		return -ENODEV;
	}

	for (i = 0; i < its.dev_count; i++) {
		if (its.devs[i].devid == devid) {
			return 0; /* already mapped */
		}
	}
	if (its.dev_count == ITS_MAX_DEVICES) {
		log_error("its: no device slot left for %#x", devid);
		return -ENOSPC;
	}

	dev = &its.devs[its.dev_count];
	memset(dev, 0, sizeof(*dev));
	dev->devid = devid;

	ret = its_cmd_mapd(dev);
	if (ret == 0) {
		its.dev_count++;
	}

	return ret;
}

int gic_its_event_map(uint32_t devid, uint32_t eventid) {
	unsigned int slot;

	if (!its.ready) {
		return -ENODEV;
	}

	/* Reuse the first freed slot, otherwise take the next one from
	 * the high-water mark. */
	for (slot = 0; slot < its.lpi_count; slot++) {
		if (!its.lpi_map[slot].used) {
			break;
		}
	}
	if (slot == GIC_LPI_QUANTITY) {
		log_error("its: LPI window exhausted");
		return -ENOSPC;
	}
	if (slot == its.lpi_count) {
		its.lpi_count++;
	}

	if (its_cmd_mapti(devid, eventid, slot) != 0) {
		return -EIO;
	}

	its.lpi_map[slot].devid = devid;
	its.lpi_map[slot].eventid = eventid;
	its.lpi_map[slot].used = 1;

	return GIC_LPI_IRQ_BASE + slot;
}

void gic_its_event_unmap(uint32_t devid, uint32_t eventid) {
	const uint64_t cmd[4] = {ITS_CMD_DISCARD
	                             | ((uint64_t)devid << ITS_CMD_DEVID_SHIFT),
	    eventid, 0, 0};
	unsigned int slot;

	for (slot = 0; slot < its.lpi_count; slot++) {
		if (its.lpi_map[slot].used && its.lpi_map[slot].devid == devid
		    && its.lpi_map[slot].eventid == eventid) {
			its.lpi_map[slot].devid = 0;
			its.lpi_map[slot].eventid = 0;
			its.lpi_map[slot].used = 0;
			break;
		}
	}

	its_cmd_post(cmd);
}

int gic_its_send_int(uint32_t devid, uint32_t eventid) {
	const uint64_t cmd[4] = {ITS_CMD_INT | ((uint64_t)devid << ITS_CMD_DEVID_SHIFT),
	    eventid, 0, 0};

	/* Consuming the INT through CREADR is enough: the LPI write to
	 * the redistributor happens as the command executes. */
	return its_cmd_post(cmd);
}

int gic_its_send_clear(uint32_t devid, uint32_t eventid) {
	const uint64_t cmd[4] = {ITS_CMD_CLEAR
	                             | ((uint64_t)devid << ITS_CMD_DEVID_SHIFT),
	    eventid, 0, 0};

	return its_cmd_post(cmd);
}

/* Kernel IRQ number plumbing for the GICv3 driver, see gic_lpi.h. */

int gic_lpi_intid_to_irq(unsigned int intid) {
	if (!its.ready || intid >= GIC_LPI_INTID_BASE + its.lpi_count) {
		return -1;
	}

	return GIC_LPI_IRQ_BASE + (intid - GIC_LPI_INTID_BASE);
}

unsigned int gic_lpi_irq_to_intid(unsigned int irq) {
	return GIC_LPI_INTID_BASE + (irq - GIC_LPI_IRQ_BASE);
}

void gic_lpi_set_state(unsigned int irq, int enable) {
	unsigned int slot = irq - GIC_LPI_IRQ_BASE;
	uint8_t *prop = &its_lpi_prop[slot];

	if (!its.ready || slot >= its.lpi_count) {
		return;
	}

	*prop = enable ? (ITS_LPI_PROP_ENABLE | ITS_LPI_PROP_PRIO) : 0;
	dsb(st);

	/* The ITS caches property entries: an INV is required before a
	 * property change takes effect. */
	its_cmd_inv(its.lpi_map[slot].devid, its.lpi_map[slot].eventid);
}

static int its_init(void) {
	uint64_t typer;
	int ret;

	typer = REG64_LOAD(ITS_BASE + GITS_TYPER);
	its.pta = (unsigned int)((typer & GITS_TYPER_PTA) != 0);

	/* Encode the redistributor reference once: with PTA clear the
	 * commands want the processor number from GICR_TYPER bits
	 * [23:8], otherwise the 64K-aligned frame address. */
	if (its.pta != 0) {
		its.rd_target = (uint64_t)GICR_BASE;
	}
	else {
		its.rd_target = (REG32_LOAD(GICR_TYPER) >> 8) & 0xffff;
	}

	log_info("its: base %#x typer %016llx pta %u rd_target %llx", ITS_BASE,
	    (unsigned long long)typer, its.pta, (unsigned long long)its.rd_target);

	ret = its_quiescent();
	if (ret == 0) {
		ret = its_lpi_tables_setup();
	}
	if (ret == 0) {
		ret = its_baser_setup();
	}
	if (ret == 0) {
		ret = its_cmd_queue_setup();
	}
	if (ret != 0) {
		log_error("its: init failed (%d), LPI delivery stays disabled", ret);
		return 0; /* keep the boot alive, MSI allocation will fail */
	}

	REG32_ORIN(ITS_BASE + GITS_CTLR, GITS_CTLR_ENABLED);

	ret = its_cmd_mapc();
	if (ret != 0) {
		log_error("its: MAPC failed, LPI delivery stays disabled");
		return 0;
	}

	its.ready = 1;
	log_info("its: ready, irq window %u..%u, doorbell %#llx", GIC_LPI_IRQ_BASE,
	    GIC_LPI_IRQ_BASE + GIC_LPI_QUANTITY - 1,
	    (unsigned long long)gic_its_trans_addr());

	return 0;
}
