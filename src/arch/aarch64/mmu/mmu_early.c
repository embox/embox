/**
 * @file
 * @brief The MMU, on before anything can take a spinlock.
 *
 * With SCTLR.M clear every data access at EL1 is Device, and on a Cortex-A72
 * an exclusive to such memory raises an SError -- so the first spinlock of
 * the boot aborts. vmem_init() cannot help: it needs the kernel task, which
 * is initialised before it and locks.
 *
 * So this maps 1 GiB blocks of its own, RAM Normal and everything else
 * Device, and turns translation on from kernel_start(). It is coarse on
 * purpose: whatever the system wants afterwards replaces it.
 */
#include <stdint.h>

#include <framework/mod/options.h>
#include <hal/cache.h>
#include <hal/mmu.h>
#include <util/log.h>

#include <hal/reg.h>

#include "mmu.h"

#define EARLY_ON OPTION_GET(NUMBER, early_on)

#if EARLY_ON

#define GIB (1024ULL * 1024 * 1024)

/* 4 KiB granule, 48-bit VA: a level 0 entry spans 512 GiB, a level 1 block
 * 1 GiB, and one table of each covers what a board in this tree addresses */
#define L1_BLOCKS 512

/* Descriptors, and the attribute bits that matter here */
#define DESC_BLOCK   0x1ULL
#define DESC_TABLE   0x3ULL
#define DESC_ATTR(i) ((uint64_t)(i) << 2)
#define DESC_AP_RW   (0x0ULL << 6)  /* EL1 read/write, EL0 none */
#define DESC_SH_IS   (0x3ULL << 8)  /* Inner Shareable */
#define DESC_AF      (0x1ULL << 10) /* accessed, so no fault on first touch */
#define DESC_UXN     (0x1ULL << 54)

/* MAIR index 0 is Device-nGnRnE and 1 is Normal, as mmu_init() writes them */
#define ATTR_DEVICE 0
#define ATTR_NORMAL 1

static uint64_t early_l0[512] __attribute__((aligned(4096)));
static uint64_t early_l1[L1_BLOCKS] __attribute__((aligned(4096)));

extern char _ram_base[];
extern char _ram_size[];

void arch_mmu_early_on(void) {
	uint64_t ram_start, ram_end;
	unsigned i;

	/* The DRAM the kernel was linked into, rounded out to whole blocks */
	ram_start = (uint64_t)(uintptr_t)_ram_base & ~(GIB - 1);
	ram_end = ((uint64_t)(uintptr_t)_ram_base + (uint64_t)(uintptr_t)_ram_size
	              + GIB - 1)
	          & ~(GIB - 1);

	for (i = 0; i < L1_BLOCKS; i++) {
		uint64_t base = (uint64_t)i * GIB;
		uint64_t desc = base | DESC_BLOCK | DESC_AP_RW | DESC_AF;

		if (base >= ram_start && base + GIB <= ram_end) {
			desc |= DESC_ATTR(ATTR_NORMAL) | DESC_SH_IS;
		}
		else {
			/* Execute-never keeps a speculative fetch off a peripheral */
			desc |= DESC_ATTR(ATTR_DEVICE) | DESC_UXN;
		}
		early_l1[i] = desc;
	}

	early_l0[0] = (uint64_t)(uintptr_t)early_l1 | DESC_TABLE;
	for (i = 1; i < 512; i++) {
		early_l0[i] = 0;
	}

	/* Written with translation off, so push them out to where a walk that
	 * reads through the caches will not find a stale line instead */
	dcache_flush(early_l0, sizeof(early_l0));
	dcache_flush(early_l1, sizeof(early_l1));

	ARCH_REG_STORE(TTBR0_EL1, (uint64_t)(uintptr_t)early_l0);

	/* Sets TCR and MAIR, flushes the TLB, and enables M together with C and
	 * I, which is what makes an exclusive legal */
	mmu_on();

	log_debug("early mmu on: RAM %#llx..%#llx normal, the rest device",
	    (unsigned long long)ram_start, (unsigned long long)ram_end);
}

#else /* !EARLY_ON */

/* The board did not ask, so vmem_init() decides when translation goes on */
void arch_mmu_early_on(void) {
}

#endif /* EARLY_ON */
