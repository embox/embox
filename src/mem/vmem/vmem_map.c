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
#include <mem/vmem.h>
#include <mem/vmem/virtalloc.h>

static inline void vmem_set_pte_flags(mmu_pte_t *pte, vmem_page_flags_t flags) {
	mmu_pte_set_writable(pte, flags & VMEM_PAGE_WRITABLE);
	mmu_pte_set_cacheable(pte, flags & VMEM_PAGE_CACHEABLE);
	mmu_pte_set_usermode(pte, flags & VMEM_PAGE_USERMODE);
}

int vmem_map_region(mmu_ctx_t ctx, mmu_paddr_t phy_addr, mmu_vaddr_t virt_addr, size_t reg_size, vmem_page_flags_t flags) {
	mmu_pgd_t *pgd;
	mmu_pmd_t *pmd;
	mmu_pte_t *pte;
	mmu_paddr_t end_phy_addr = phy_addr + reg_size;
	size_t pgd_idx, pmd_idx, pte_idx;
	void *addr;

	virt_addr = virt_addr & (~MMU_PAGE_MASK);
	phy_addr = phy_addr & (~MMU_PAGE_MASK);

	pgd = mmu_get_root(ctx);

	// TODO: Implement another method of map
	while (phy_addr < end_phy_addr) {
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
		mmu_pte_set(pte + pte_idx, phy_addr);
		vmem_set_pte_flags(pte + pte_idx, flags);

		virt_addr += MMU_PAGE_SIZE;
		phy_addr += MMU_PAGE_SIZE;
	}

	return ENOERR;
}

int vmem_create_space(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, size_t reg_size, vmem_page_flags_t flags) {
	mmu_pgd_t *pgd;
	mmu_pmd_t *pmd;
	mmu_pte_t *pte;
	mmu_vaddr_t end_virt_addr = virt_addr + reg_size;
	size_t pgd_idx, pmd_idx, pte_idx;
	void *addr;

	virt_addr = virt_addr & (~MMU_PAGE_MASK);

	pgd = mmu_get_root(ctx);

	// TODO: Implement another method of creation
	while (virt_addr < end_virt_addr) {
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

		if (!mmu_pte_present(pte + pte_idx)) {
			addr = virt_alloc_page();
			if (!addr) {
				return -ENOMEM;
			}
			mmu_pte_set(pte + pte_idx, (mmu_paddr_t) addr);
			vmem_set_pte_flags(pte + pte_idx, flags);
		}

		virt_addr += MMU_PAGE_SIZE;
	}

	return ENOERR;
}

int vmem_page_set_flags(mmu_ctx_t ctx, mmu_vaddr_t virt_addr, vmem_page_flags_t flags) {
	size_t pgd_idx, pmd_idx, pte_idx;
	mmu_pgd_t *pgd;
	mmu_pmd_t *pmd;
	mmu_pte_t *pte;

	// Actually, this is unnecessary
	virt_addr = virt_addr & (~MMU_PAGE_MASK);

	pgd = mmu_get_root(ctx);

	pgd_idx = ((uint32_t) virt_addr & MMU_PGD_MASK) >> MMU_PGD_SHIFT;
	pmd_idx = ((uint32_t) virt_addr & MMU_PMD_MASK) >> MMU_PMD_SHIFT;
	pte_idx = ((uint32_t) virt_addr & MMU_PTE_MASK) >> MMU_PTE_SHIFT;

	if (!mmu_pgd_present(pgd + pgd_idx)) {
		return -ENOENT;
	}

	pmd = mmu_pgd_value(pgd + pgd_idx);

	if (!mmu_pmd_present(pmd + pmd_idx)) {
		return -ENOENT;
	}

	pte = mmu_pmd_value(pmd + pmd_idx);

	if (!mmu_pte_present(pte + pte_idx)) {
		return -ENOENT;
	}

	vmem_set_pte_flags(pte + pte_idx, flags);

	return ENOERR;
}
