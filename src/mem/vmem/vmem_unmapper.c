/**
 * @file
 * @brief
 *
 * @date 19.11.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <assert.h>

#include <util/binalign.h>
#include <mem/vmem.h>
#include <mem/vmem/vmem_alloc.h>

static inline int try_free_pte(mmu_pte_t *pte, mmu_pmd_t *pmd) {
	for (int pte_idx = 0 ; pte_idx < MMU_PTE_ENTRIES; pte_idx++) {
		if (mmu_pte_present(pte + pte_idx)) {
			return 0;
		}
	}

#if MMU_PTE_SHIFT != MMU_PMD_SHIFT
	mmu_pmd_unset(pmd);
	vmem_free_pte_table(pte);
#endif

	return 1;
}

static inline int try_free_pmd(mmu_pmd_t *pmd, mmu_pgd_t *pgd) {
	for (int pmd_idx = 0 ; pmd_idx < MMU_PMD_ENTRIES; pmd_idx++) {
		if (mmu_pmd_present(pmd + pmd_idx)) {
			return 0;
		}
	}

#if MMU_PMD_SHIFT != MMU_PGD_SHIFT
	mmu_pgd_unset(pgd);
	vmem_free_pmd_table(pmd);
#endif

	return 1;
}

static inline int try_free_pgd(mmu_pgd_t *pgd, mmu_ctx_t ctx) {
	for (int pgd_idx = 0 ; pgd_idx < MMU_PGD_ENTRIES; pgd_idx++) {
		if (mmu_pgd_present(pgd + pgd_idx)) {
			return 0;
		}
	}

	// Something missing
	vmem_free_pgd_table(pgd);

	return 1;
}

void vmem_unmap_region(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, size_t reg_size, int free_pages) {
	mmu_pgd_t *pgd;
	mmu_pmd_t *pmd;
	mmu_pte_t *pte;
	mmu_paddr_t v_end = virt_addr + reg_size;
	size_t pgd_idx, pmd_idx, pte_idx;
	void *addr;

	/* Considering that all boundaries are already aligned */
	assert(!(virt_addr & MMU_PAGE_MASK));
	assert(!(reg_size  & MMU_PAGE_MASK));

	pgd = mmu_get_root(ctx);

	vmem_get_idx_from_vaddr(virt_addr, &pgd_idx, &pmd_idx, &pte_idx);

	for ( ; pgd_idx < MMU_PGD_ENTRIES; pgd_idx++) {
		if (!mmu_pgd_present(pgd + pgd_idx)) {
			virt_addr = binalign_bound(virt_addr, MMU_PGD_SIZE);
			pte_idx = pmd_idx = 0;
			continue;
		}

		pmd = mmu_pgd_value(pgd + pgd_idx);

		for ( ; pmd_idx < MMU_PMD_ENTRIES; pmd_idx++) {
			if (!mmu_pmd_present(pmd + pmd_idx)) {
				virt_addr = binalign_bound(virt_addr, MMU_PMD_SIZE);
				pte_idx = 0;
				continue;
			}

			pte = mmu_pmd_value(pmd + pmd_idx);

			for ( ; pte_idx < MMU_PTE_ENTRIES; pte_idx++) {
				if (virt_addr >= v_end) {
					// Try to free pte, pmd, pgd
					if (try_free_pte(pte, pmd + pmd_idx) && try_free_pmd(pmd, pgd + pgd_idx)) {
						try_free_pgd(pgd, ctx);
					}

					return;
				}

				if (mmu_pte_present(pte + pte_idx)) {
					if (free_pages && mmu_pte_present(pte + pte_idx)) {
						addr = (void *) mmu_pte_value(pte + pte_idx);
						vmem_free_page(addr);
					}

					mmu_pte_unset(pte + pte_idx);
				}

				virt_addr += VMEM_PAGE_SIZE;
			}
			try_free_pte(pte, pmd + pmd_idx);
			pte_idx = 0;
		}

		try_free_pmd(pmd, pgd + pgd_idx);
		pmd_idx = 0;
	}

	try_free_pgd(pgd, ctx);
}

