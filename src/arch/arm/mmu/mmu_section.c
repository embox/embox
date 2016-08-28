/**
 * @file mmu.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 2015-08-18
 */

#include <string.h>
#include <asm/hal/mmu.h>
#include <asm/regs.h>
#include <hal/mmu.h>
#include <mem/vmem.h>

mmu_pmd_t *mmu_pgd_value(mmu_pgd_t *pgd) {
	return pgd;
}

mmu_pte_t *mmu_pmd_value(mmu_pmd_t *pmd) {
	return pmd;
}

mmu_paddr_t mmu_pte_value(mmu_pte_t *pte) {
	return 0;
}

void mmu_pgd_set(mmu_pgd_t *pgd, mmu_pmd_t *pmd) {
}

void mmu_pmd_set(mmu_pgd_t *pmd, mmu_pmd_t *pte) {
}

void mmu_pte_set(mmu_pte_t *pte, mmu_paddr_t addr) {
	*pte = (mmu_pte_t) ((addr & ~MMU_PAGE_MASK)
			| ARM_MMU_TYPE_SECTION
			| ARM_MMU_SECTION_READ_ACC);
}

void mmu_pgd_unset(mmu_pgd_t *pgd) {
}

void mmu_pmd_unset(mmu_pgd_t *pmd) {
}

void mmu_pte_unset(mmu_pgd_t *pte) {
	*pte = 0x0;
}

int mmu_pgd_present(mmu_pgd_t *pgd) {
	return 1;
}
int mmu_pmd_present(mmu_pmd_t *pmd) {
	return 1;
}
int mmu_pte_present(mmu_pte_t *pte) {
	return (*pte & ARM_MMU_TYPE_SECTION) == ARM_MMU_TYPE_SECTION;
}

void mmu_pte_set_writable(mmu_pte_t *pte, int value) {
	if (value & VMEM_PAGE_WRITABLE)
		*pte |= ARM_MMU_SECTION_WRITE_ACC;
}

void mmu_pte_set_cacheable(mmu_pte_t *pte, int value) {
	if (value & VMEM_PAGE_CACHEABLE)
		*pte |= L1D_C;
}

void mmu_pte_set_usermode(mmu_pte_t *pte, int value) {
}

void mmu_pte_set_executable(mmu_pte_t *pte, int value) {
	if (!(value & VMEM_PAGE_EXECUTABLE))
		*pte |= L1D_XN;
}
