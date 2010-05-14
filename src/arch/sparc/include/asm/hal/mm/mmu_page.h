/**
 * @file
 *
 * @date 07.04.2010
 * @author Anton Bondarev
 */

#ifndef SPARC_MMU_PAGE_H_
#define SPARC_MMU_PAGE_H_

#include <hal/mm/mmu_core.h>

extern mmu_env_t *cur_env;

#define GET_PGD(ctx) \
	((pgd_t*)((unsigned long)(*(cur_env->ctx + ctx) << 4) & MMU_PTE_PMASK))

#define GET_PMD(ctx) \
	((pmd_t*)((unsigned long)((*GET_PGD(ctx)) << 4) & MMU_PTE_PMASK))

#define GET_PTE(ctx) \
	((pte_t*)((unsigned long)((*GET_PMD(ctx)) << 4) & MMU_PTE_PMASK))

#define GET_FLAGS(ctx, vaddr) \
	((mmu_page_flags_t*)(GET_PMD(ctx) + (vaddr >> MMU_MTABLE_MASK_OFFSET)))


inline static bool mmu_entry_is_pte(pte_t *pte) {
	return !(MMU_ET_PTD & (uint32_t)(*pte));
}

inline static bool mmu_entry_is_valid(pte_t *pte) {
	return (*pte) ? true : false;/*if pte is NULL pte not valid*/
}

inline static pte_t *mmu_get_next_level_pte(pte_t *ptp, int idx) {

	if (mmu_entry_is_pte(ptp) || !mmu_entry_is_valid(ptp))
		return NULL;

	return (pte_t*)((uint32_t*)((((uint32_t)ptp << 4)) & (~MMU_PTE_PMASK)))[idx];
}

static unsigned long masks[] = {
	0xffffffff,
	MMU_GTABLE_MASK,
	MMU_MTABLE_MASK,
	MMU_PTABLE_MASK,
	MMU_PAGE_MASK
};

inline static pte_t *mmu_page_get_entry(mmu_ctx_t ctx, vaddr_t vaddr) {
	//unsigned long *context[] = {NULL, NULL, NULL, NULL, NULL };
	pte_t *pte = GET_PGD(ctx);
	int level = 1;
	//uint32_t addr_mask = masks[level];
	for(level; level < 4; level ++) {
		if (mmu_entry_is_pte(pte) && mmu_entry_is_valid(pte)) {
			return NULL;
		}
		pte = mmu_get_next_level_pte(pte, vaddr & masks[level]);
	}
	return NULL;
}

inline static void mmu_page_mark_writable(mmu_ctx_t ctx, vaddr_t vaddr) {
	pmd_t *m0 = GET_PMD(ctx);
	mmu_page_flags_t *flags;
	vaddr &= ~MMU_PAGE_MASK & MMU_MTABLE_MASK;
	flags = GET_FLAGS(ctx, vaddr);
	mmu_set_pte(m0 + (vaddr>> MMU_MTABLE_MASK_OFFSET),
			*flags | (MMU_PTE_RW << 2));
}

inline static void mmu_page_mark_executeable(mmu_ctx_t ctx, vaddr_t vaddr) {
	pmd_t *m0 = GET_PGD(ctx);
	mmu_page_flags_t *flags;
	vaddr &= ~MMU_PAGE_MASK & MMU_MTABLE_MASK;
	flags = GET_FLAGS(ctx, vaddr);
	mmu_set_pte(m0 + (vaddr >> MMU_MTABLE_MASK_OFFSET),
			*flags | (MMU_PTE_RE << 2));
}

inline static void mmu_page_mark_cacheable(mmu_ctx_t ctx, vaddr_t vaddr) {
	pmd_t *m0 = GET_PMD(ctx);
	mmu_page_flags_t *flags;
	vaddr &= ~MMU_PAGE_MASK & MMU_MTABLE_MASK;
	flags = GET_FLAGS(ctx, vaddr);
	mmu_set_pte(m0 + (vaddr >> MMU_MTABLE_MASK_OFFSET),
			*flags | MMU_PTE_CACHE);
}

inline static mmu_page_flags_t mmu_page_get_flags(mmu_ctx_t ctx, vaddr_t vaddr) {
	mmu_page_flags_t *flags;
	vaddr &= ~MMU_PAGE_MASK & MMU_MTABLE_MASK;
	flags = GET_FLAGS(ctx, vaddr);
	return *flags;
}
#if 0
inline static void mmu_page_set_flags(mmu_ctx_t ctx, vaddr_t vaddr,
			mmu_page_flags_t flags) {
	pmd_t *m0 = GET_PMD(ctx);
	mmu_page_flags_t *tmp;
	flags = flags << 1;
	if(flags & (MMU_PAGE_CACHEABLE << 1)) {
		flags |= MMU_PTE_CACHE;
	}
	vaddr &= ~MMU_PAGE_MASK & MMU_MTABLE_MASK;
	tmp = GET_FLAGS(ctx, vaddr);
	mmu_set_pte(m0 + (vaddr >> MMU_MTABLE_MASK_OFFSET),
			(*tmp & 0xFFFFFF03) | flags);
}
#endif

inline static void mmu_page_set_flags(mmu_ctx_t ctx, vaddr_t vaddr,
			mmu_page_flags_t flags) {
	pte_t *pte = mmu_page_get_entry(ctx, vaddr);
	pte_t pte_value = (*pte) & (~0xFF);

	mmu_set_pte(pte,
			pte_value | flags);
}

inline static void mmu_page_mark_valid(mmu_ctx_t ctx, vaddr_t vaddr) {

}

inline static void mmu_page_mark_invalide(mmu_ctx_t ctx, vaddr_t vaddr) {

}

#endif /* SPARC_MMU_PAGE_H_ */
