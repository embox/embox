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

inline static void mmu_page_mark_valid(mmu_ctx_t ctx, vaddr_t vaddr) {

}

inline static void mmu_page_mark_invalide(mmu_ctx_t ctx, vaddr_t vaddr) {

}

#endif /* SPARC_MMU_PAGE_H_ */
