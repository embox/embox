/**
 * @file mmu.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 2015-08-25
 */

#include <string.h>
#include <asm/hal/mmu.h>
#include <asm/regs.h>
#include <hal/mmu.h>
#include <mem/vmem.h>

mmu_pmd_t *mmu_pgd_value(mmu_pgd_t *pgd) {
	return (mmu_pmd_t*) (((uint32_t)*pgd) & ~MMU_PAGE_MASK);
}

mmu_pte_t *mmu_pmd_value(mmu_pmd_t *pmd) {
	return pmd;
}

mmu_paddr_t mmu_pte_value(mmu_pte_t *pte) {
	return 0;
}

void mmu_pgd_set(mmu_pgd_t *pgd, mmu_pmd_t *pmd) {
	*pgd = (mmu_pgd_t) ((((uint32_t)pmd) & ~MMU_PAGE_MASK)
		| ARM_MMU_TYPE_PAGE);

}

void mmu_pmd_set(mmu_pgd_t *pmd, mmu_pmd_t *pte) {
}

void mmu_pte_set(mmu_pte_t *pte, mmu_paddr_t addr) {
#if 0
	*pte = (mmu_pte_t) ((((uint32_t)addr) & ~MMU_PAGE_MASK)
			| ARM_MMU_PAGE_READ_ACC
			| ARM_MMU_SMALL_PAGE);
#endif
	*pte = (mmu_pte_t) ((((uint32_t)addr) & ~MMU_PAGE_MASK)
			| 0x036);
}

void mmu_pgd_unset(mmu_pgd_t *pgd) {
	*pgd = 0x0;
}

void mmu_pmd_unset(mmu_pgd_t *pmd) {
	*pmd = 0x0;
}

void mmu_pte_unset(mmu_pgd_t *pte) {
	*pte = 0x0;
}

int mmu_pgd_present(mmu_pgd_t *pgd) {
	return (((uint32_t)*pgd) & ARM_MMU_TYPE_PAGE) == ARM_MMU_TYPE_PAGE;
}

int mmu_pmd_present(mmu_pmd_t *pmd) {
	return (((uint32_t)*pmd) & ARM_MMU_SMALL_PAGE) == ARM_MMU_SMALL_PAGE;
}

int mmu_pte_present(mmu_pte_t *pte) {
	return 0;
}

void mmu_pte_set_writable(mmu_pte_t *pte, int value) {
#if 0
	if (value & VMEM_PAGE_WRITABLE) {
		*pte |= ARM_MMU_PAGE_WRITE_ACC;
	} else {
		*pte &= ~ARM_MMU_PAGE_WRITE_ACC;
	}
#endif
}

void mmu_pte_set_cacheable(mmu_pte_t *pte, int value) {
	if (value & VMEM_PAGE_CACHEABLE) {
		*pte |= L1D_C;
	} else {
		*pte &= ~L1D_C;
	}
}

void mmu_pte_set_usermode(mmu_pte_t *pte, int value) {
}

void mmu_pte_set_executable(mmu_pte_t *pte, int value) {
#if 0
	if (!(value & VMEM_PAGE_EXECUTABLE)) {
		*pte |= L1D_XN;
	} else {
		*pte &= ~L1D_XN;
	}
#endif
}
