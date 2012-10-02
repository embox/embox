/**
 * @file
 * @brief Implements MMU core interface @link #mmu_core.h @endllink
 *        for SPARC architecture.
 *
 * @date 18.04.10
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Anton Kozlov
 */

#include <types.h>
#include <string.h>

#include <hal/mm/mmu_types.h>
#include <hal/mm/mmu_core.h>
#include <asm/hal/mm/mmu_page.h>

#include <asm/asi.h>
#include <embox/unit.h>
#include <asm/hal/mm/table_alloc.h>
#include <mem/pagealloc/opallocator.h>
#include <mem/vmem/virtalloc.h>

#include <err.h>

#include <prom/prom_printf.h>


static mmu_pgd_t *context_table[0x100]  __attribute__((aligned(MMU_PAGE_SIZE)));
static int ctx_counter = 0;

/* Setup module starting function */
EMBOX_UNIT_INIT(mmu_init);

void mmu_on(void) {
	unsigned long val;
	mmu_set_context((unsigned long) 0);

	val = mmu_get_mmureg(LEON_CNR_CTRL);
	val |= 0x1;

	prom_printf("CTXP: 0x%08x\n", (unsigned int) mmu_get_mmureg(LEON_CNR_CTXP));

	mmu_set_mmureg(LEON_CNR_CTRL, val);

	mmu_flush_cache_all();
}

void mmu_off(void) {
	unsigned long val;
	mmu_flush_cache_all();

	val = mmu_get_mmureg(LEON_CNR_CTRL);
	val &= ~0x1;
	mmu_set_mmureg(LEON_CNR_CTRL, val);
}

mmu_pgd_t *mmu_get_root(mmu_ctx_t ctx) {
	return context_table[ctx];
}

int mmu_valid_entry(mmu_pte_t pte) {
        return (((unsigned int) mmu_is_pte) & MMU_PTE_ET) |
                            (((unsigned int) pte) & MMU_ET_PTD);
}

mmu_ctx_t mmu_create_context(void) {
	mmu_ctx_t ctx = (mmu_ctx_t) (ctx_counter++);

	context_table[ctx] = (mmu_pgd_t *) alloc_pte_table();

	return ctx;
}

void switch_mm(mmu_ctx_t prev, mmu_ctx_t next) {
	mmu_set_context(next);
	mmu_flush_tlb_all();
}

static int mmu_init(void) {
	unsigned long val = ((unsigned long) context_table);
	mmu_set_mmureg(LEON_CNR_CTXP, val >> 4);

	return 0;
}
