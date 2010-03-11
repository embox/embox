/**
 * \file mmu_probe.c
 * \date Jun 9, 2009
 * \author anton
 * \details
 */
#include "common.h"
#include "asm/leon.h"
#include "asm/mmu.h"
#include "asm/srmmu_probe.h"
#include "embox/test.h"

EMBOX_TEST(run);

#define TLBNUM 4

static int run() {
	unsigned long pteval;
	unsigned long j, i, val;

	functype func = mmu_func1;

	//alloc mem for pages
	__asm__(
			".section .data\n\t"
			".align %0\n\t"
			"page0: .skip %0\n\t"
			"page1: .skip %0\n\t"
			"page2: .skip %4\n\t"
			".text\n"
			: : "i" (PAGE_SIZE),
			"i"(SRMMU_PGD_TABLE_SIZE) ,
			"i"(SRMMU_PMD_TABLE_SIZE) ,
			"i"(SRMMU_PTE_TABLE_SIZE) ,
			"i"((3)*PAGE_SIZE) );

	mmu_probe_init();
	mmu_flush_cache_all ();
	mmu_flush_tlb_all ();

	/* one-on-one mapping for context 0 */
	mmu_probe_map_region(0, 0, 0x1000000, MMU_PRIV);
	//  mmu_probe_map_region(0x20000000, 0x20000000, 0x1000000, MMU_PRIV);
	mmu_probe_map_region(0x40000000, 0x40000000, 0x1000000, MMU_PRIV);
	mmu_probe_map_region(0x80000000, 0x80000000, 0x1000000, MMU_PRIV);

	/* testarea:
	 *  map 0x40000000  at f0080000 [vaddr:(0) (240)(2)(-)] as pmd
	 *  map page0       at f0041000 [vaddr:(0) (240)(1)(1)] as page MMU_PRIV_RDONLY
	 *  map mmu_func1() at f0042000 [vaddr:(0) (240)(1)(2)] as page
	 *  map f0043000 - f007f000 [vaddr:(0) (240)(1)(3)] - [vaddr:(0) (240)(1)(63)] as page
	 * page fault test:
	 *  missing pgd at f1000000 [vaddr:(0) (241)(-)(-)]
	 */
	mmu_probe_map_region(0x40000000, 0xf0080000, 0x40000, MMU_PRIV);
	//not alloc here (alloc after page_fault)
	mmu_probe_map_region(0x0, 0xf0042000, 0x1000, 0);
	//first read only during trap clear this flag
	mmu_probe_map_region((unsigned long) &page0, 0xf0041000, 0x1000,
			MMU_PRIV_RDONLY);
	for (i = 3; i < TLBNUM + 3; i++) {
		mmu_probe_map_region((((unsigned long) &page2) + (((i - 3) % 3)
				* PAGE_SIZE)), (0xf0040000 + (i * PAGE_SIZE)), 0x1000, MMU_PRIV);
	}
	mmu_probe_repair_table_init((unsigned long) &page0);

	//for translation test
	((unsigned long *) &page0)[0] = 0;
	((unsigned long *) &page0)[1] = 0x12345678;

	/* close your eyes and pray ... */
	mmu_probe_start();

	/* do tests*/
	// ==page translation test== page0 in 0xf0041000 addr 0x40000000 0xf0080000
	if ((*((unsigned long *) 0xf0041000)) != 0
			|| (*((unsigned long *) 0xf0041004)) != 0x12345678) {
		MMU_RETURN(-1);
	}

	if ((*((unsigned long *) 0xf0080000)) != (*((unsigned long *) 0x40000000))) {
		MMU_RETURN(-1);
	}

	/* ==page faults tests==*/

	val = *((volatile unsigned long *) 0xf1000000);
	/* write protection fault */
	//  /* repair info for write protection fault (0xf0041000) */
	*((volatile unsigned long *) 0xf0041004) = 0x87654321;
	if ((*((volatile unsigned long *) 0xf0041004)) != 0x87654321) {
		MMU_RETURN(-1);
	}

	//==test several page for modify flags==
	for (j = 0xf0043000, i = 3; i < TLBNUM + 3; i++, j += 0x1000) {
		*((unsigned long *) j) = j;
		if (*((unsigned long*) (((unsigned long) &page2) + (((i - 3) % 3)
				* PAGE_SIZE))) != j) {
			MMU_RETURN (-1);
		}
	}
	flush_data_cache();

	for (j = 0, i = 3; i < TLBNUM + 3; i++) {
		unsigned int page_addr = (((unsigned long) &page2) + (((i - 3) % 3)
				* PAGE_SIZE));
		unsigned int page_desc = mmu_get_page_desc(page_addr);
		pteval = ((page_addr >> 4) | SRMMU_ET_PTE | SRMMU_PRIV);
		if (mmu_get_page_desc(page_addr) & (SRMMU_DIRTY | SRMMU_REF))
			j++;

		if ((page_desc & ~(SRMMU_DIRTY | SRMMU_REF)) != (pteval & ~(SRMMU_DIRTY
				| SRMMU_REF))) {
			MMU_RETURN (-1);
		}
	}

	//at least one entry has to have been flushed
	if (j == 0) {
		MMU_RETURN (-1);
	}

	/* ==instruction page fault== */
	func = (functype) 0xf0042000;
	func();

	MMU_RETURN (0);
}
