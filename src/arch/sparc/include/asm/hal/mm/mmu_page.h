/**
 * @file
 *
 * @date 07.04.10
 * @author Anton Bondarev
 */

#ifndef SPARC_MMU_PAGE_H_
#define SPARC_MMU_PAGE_H_

#include <hal/mm/mmu_types.h>
#include <hal/mm/mmu_core.h>
#include <kernel/mm/virt_mem/mmu_core.h>

extern mmu_env_t *cur_env;

#define GET_PGD(ctx) \
	((mmu_pgd_t*)((unsigned long)(*(cur_env->ctx + ctx) << 4) & MMU_PTE_PMASK))

#define GET_PMD(ctx) \
	((mmu_pmd_t*)((unsigned long)((*GET_PGD(ctx)) << 4) & MMU_PTE_PMASK))

#define GET_PTE(ctx) \
	((mmu_pte_t*)((unsigned long)((*GET_PMD(ctx)) << 4) & MMU_PTE_PMASK))

#define GET_FLAGS(ctx, vaddr) \
	((mmu_page_flags_t*)(GET_PMD(ctx) + (vaddr >> MMU_MTABLE_MASK_OFFSET)))


static inline bool mmu_entry_is_pte(mmu_pte_t *pte) {
	return (0x2 == (0x3 & (uint32_t)(*pte))) ? true : false;
}

static inline bool mmu_entry_is_valid(mmu_pte_t *pte) {
	return (0x0 != (0x3 & (uint32_t)(*pte))) ? true : false;
}

static inline mmu_pte_t *mmu_get_next_level_pte(mmu_pte_t *ptp, int idx) {
	return (mmu_pte_t*)((uint32_t*)((((uint32_t)(*(ptp + idx)) << 4)) & (MMU_PTE_PMASK)));
}

static inline mmu_pte_t *mmu_page_get_entry(mmu_ctx_t ctx, vaddr_t vaddr) {
	mmu_pte_t *pte = GET_PGD(ctx);
	int level = 1;

	for (level = 1; level < 4; level++) {
		if (mmu_entry_is_pte(pte + ((vaddr & mmu_table_masks[level]) >>
			blog2(mmu_table_masks[level])))
			/*&& mmu_entry_is_valid(pte)*/) {
			return (pte + ((vaddr & mmu_table_masks[level]) >>
				blog2(mmu_table_masks[level])));
		}
		pte = mmu_get_next_level_pte(pte,
			(vaddr & mmu_table_masks[level]) >>
			blog2(mmu_table_masks[level]));
	}
	return NULL;
}

static inline void mmu_page_mark_writable(mmu_ctx_t ctx, vaddr_t vaddr) {
	mmu_pmd_t *m0 = GET_PMD(ctx);
	mmu_page_flags_t *flags;
	vaddr &= ~MMU_PAGE_MASK & MMU_MTABLE_MASK;
	flags = GET_FLAGS(ctx, vaddr);
	mmu_set_pte(m0 + (vaddr>> MMU_MTABLE_MASK_OFFSET),
			*flags | (MMU_PTE_RW << 2));
}

static inline void mmu_page_mark_executeable(mmu_ctx_t ctx, vaddr_t vaddr) {
	mmu_pmd_t *m0 = GET_PGD(ctx);
	mmu_page_flags_t *flags;
	vaddr &= ~MMU_PAGE_MASK & MMU_MTABLE_MASK;
	flags = GET_FLAGS(ctx, vaddr);
	mmu_set_pte(m0 + (vaddr >> MMU_MTABLE_MASK_OFFSET),
			*flags | (MMU_PTE_RE << 2));
}

static inline void mmu_page_mark_cacheable(mmu_ctx_t ctx, vaddr_t vaddr) {
	mmu_pmd_t *m0 = GET_PMD(ctx);
	mmu_page_flags_t *flags;
	vaddr &= ~MMU_PAGE_MASK & MMU_MTABLE_MASK;
	flags = GET_FLAGS(ctx, vaddr);
	mmu_set_pte(m0 + (vaddr >> MMU_MTABLE_MASK_OFFSET),
			*flags | MMU_PTE_CACHE);
}

static inline mmu_page_flags_t mmu_page_get_flags(mmu_ctx_t ctx, vaddr_t vaddr) {
	mmu_page_flags_t *flags;
	vaddr &= ~MMU_PAGE_MASK & MMU_MTABLE_MASK;
	flags = GET_FLAGS(ctx, vaddr);
	return *flags;
}
#include "asm/cache.h"

static inline void mmu_page_set_flags(mmu_ctx_t ctx, vaddr_t vaddr,
			mmu_page_flags_t flags) {
	mmu_pte_t pte_value;
	mmu_pte_t *pte = mmu_page_get_entry(ctx, vaddr);
	if (NULL == pte) {
		return;
	}

	pte_value = (*pte) & (~0x1C) ;

	mmu_set_pte(pte,
			pte_value | ((flags & 0x7) << 2));

/*	mmu_flush_cache_all(); */
	mmu_flush_tlb_all();

}

static inline void mmu_page_mark_valid(mmu_ctx_t ctx, vaddr_t vaddr) {
}

static inline void mmu_page_mark_invalide(mmu_ctx_t ctx, vaddr_t vaddr) {
}

#endif /* SPARC_MMU_PAGE_H_ */
