/**
 * @file
 * @brief OMAP3 General Purpose Memory Controller
 *
 * @date 30.10.13
 * @author Alexander Kalmuk
 */

#include <util/log.h>

#include <errno.h>
#include <sys/mman.h>

#include <drivers/common/memory.h>

#include <embox/unit.h>
#include <hal/reg.h>
#ifndef NOMMU
#include <mem/vmem.h>
#endif

#include <drivers/omap_gpmc.h>
#include <drivers/gpmc.h>

#define GPMC_MEMORY_START  0x00000000 /*0x1000000*/
#define GPMC_MEMORY_END    0x3fffffff
#define GPMC_CS_MEMORY_MAX 0x01000000

#define GPMC_BASE_ADDRESS 0x6E000000

EMBOX_UNIT_INIT(gpmc_init);

uint32_t gpmc_reg_read(int offset) {
	return REG_LOAD(GPMC_BASE_ADDRESS + offset);
}

void gpmc_reg_write(int offset, uint32_t val) {
	REG_STORE(GPMC_BASE_ADDRESS + offset, val);
}

uint32_t gpmc_cs_reg_read(int cs, int offset) {
	unsigned long reg_addr;
	reg_addr = GPMC_BASE_ADDRESS + offset + (cs * GPMC_CS_SIZE);
	return REG_LOAD(reg_addr);
}

void gpmc_cs_reg_write(int cs, int offset, uint32_t val) {
	unsigned long reg_addr;
	reg_addr = GPMC_BASE_ADDRESS + offset + (cs * GPMC_CS_SIZE);
	REG_STORE(reg_addr, val);
}

int gpmc_cs_enabled(int cs) {
	uint32_t l;
	l = gpmc_cs_reg_read(cs, GPMC_CS_CONFIG7);
	return l & GPMC_CONFIG7_CSVALID;
}

static int gpmc_cs_enable_mem(int cs, uint32_t base, uint32_t size) {
	uint32_t l;
	uint32_t mask;

	mask = (1 << GPMC_SECTION_SHIFT) - size;
	l = gpmc_cs_reg_read(cs, GPMC_CS_CONFIG7);
	l &= ~0x3f;
	l = (base >> GPMC_CHUNK_SHIFT) & 0x3f;
	l &= ~(0x0f << 8);
	l |= ((mask >> GPMC_CHUNK_SHIFT) & 0x0f) << 8;
	l |= GPMC_CONFIG7_CSVALID;
	gpmc_cs_reg_write(cs, GPMC_CS_CONFIG7, l);

#ifndef NOMMU
	/* TODO use mmap instead of vmem_map_region */
	vmem_map_region(vmem_current_context(), base, base, size, PROT_WRITE | PROT_READ | PROT_NOCACHE);
#endif

	return 0;
}

int gpmc_cs_init(int cs, uint32_t *base, uint32_t size) {
	*base = GPMC_MEMORY_START + GPMC_CS_MEMORY_MAX * cs;

	return gpmc_cs_enable_mem(cs, *base, size);
}

static int gpmc_init(void) {
	uint32_t l;

	l = gpmc_reg_read(GPMC_REVISION);

	log_info("GPMC rev (major = %d minor = %d)", GPMC_REVISION_MAJOR(l), GPMC_REVISION_MINOR(l));

	return 0;
}

PERIPH_MEMORY_DEFINE(omap_gpmc, GPMC_BASE_ADDRESS, 0x200);
