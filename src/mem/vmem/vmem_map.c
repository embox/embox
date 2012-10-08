/**
 * @file
 * @brief
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 */

#include <hal/mmu.h>
#include <util/binalign.h>
#include <errno.h>
#include <mem/vmem/virtalloc.h>

// TODO: flags
int vmem_map_region(mmu_ctx_t ctx, mmu_paddr_t phy_addr, mmu_vaddr_t virt_addr, size_t reg_size, mmu_page_flags_t flags) {
	mmu_pgd_t *pgd;
	mmu_pmd_t *pmd;
	mmu_pte_t *pte;
	size_t pgd_idx, pmd_idx, pte_idx;
	void *addr;


	// TODO: Fix it
	reg_size = __binalign_mask(reg_size, MMU_PAGE_MASK);
	virt_addr = __binalign_mask(virt_addr, MMU_PAGE_MASK);
	phy_addr = __binalign_mask(phy_addr, MMU_PAGE_MASK);
	//

	pgd = mmu_get_root(ctx);

	// TODO: Implement another method of map
	while (reg_size > 0) {
		pgd_idx = ((uint32_t) virt_addr & MMU_PGD_MASK) >> MMU_PGD_SHIFT;
		pmd_idx = ((uint32_t) virt_addr & MMU_PMD_MASK) >> MMU_PMD_SHIFT;
		pte_idx = ((uint32_t) virt_addr & MMU_PTE_MASK) >> MMU_PTE_SHIFT;

		if (!mmu_pgd_present(pgd + pgd_idx)) {
			addr = virt_alloc_table();
			if (!addr) {
				return -ENOMEM;
			}
			mmu_pgd_set(pgd + pgd_idx, (mmu_pmd_t *) addr);
		}

		pmd = mmu_pgd_value(pgd + pgd_idx);

		if (!mmu_pmd_present(pmd + pmd_idx)) {
			addr = virt_alloc_table();
			if (!addr) {
				return -ENOMEM;
			}
			mmu_pmd_set(pmd + pmd_idx, (mmu_pmd_t *) addr);
		}

		pte = mmu_pmd_value(pmd + pmd_idx);

		// XXX: What if already presented???
		mmu_pte_set(pte + pte_idx, phy_addr, flags);

		virt_addr += MMU_PAGE_SIZE;
		phy_addr += MMU_PAGE_SIZE;
		reg_size -= MMU_PAGE_SIZE;
	}

	return ENOERR;
}
