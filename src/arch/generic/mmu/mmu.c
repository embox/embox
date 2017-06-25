/**
 * @file
 * @brief
 *
 * @date 03.12.12
 * @author Anton Bulychev
 */

#include <hal/mmu.h>

mmu_pgd_t *context_table[0x100]  __attribute__((aligned(MMU_PAGE_SIZE)));
int ctx_counter = 0;

mmu_ctx_t mmu_create_context(mmu_pgd_t *pgd) {
	mmu_ctx_t ctx = (mmu_ctx_t) (++ctx_counter);
	mmu_set_ptd_entry(&context_table[ctx], pgd);
	return ctx;
}

mmu_pgd_t *mmu_get_root(mmu_ctx_t ctx) {
	return (mmu_pgd_t *) mmu_get_ptd_ptr(context_table[ctx]);
}

/* Set functions */

void mmu_pgd_set(mmu_pgd_t *pgd, mmu_pmd_t *pmd) {
	mmu_set_ptd_entry(pgd, pmd);
}

void mmu_pmd_set(mmu_pmd_t *pmd, mmu_pte_t *pte) {
	mmu_set_ptd_entry(pmd, pte);
}

void mmu_pte_set(mmu_pte_t *pte, mmu_paddr_t addr) {
	mmu_set_pte_entry(pte, addr, MMU_PAGE_SOMEFLAG);
}

/* Value functions */

mmu_pmd_t *mmu_pgd_value(mmu_pgd_t *pgd) {
	return (mmu_pmd_t *) mmu_get_ptd_ptr(*pgd);
}

mmu_pte_t *mmu_pmd_value(mmu_pmd_t * pmd) {
	return (mmu_pte_t *) mmu_get_ptd_ptr(*pmd);
}

mmu_paddr_t mmu_pte_value(mmu_pte_t *pte) {
	return (mmu_paddr_t) mmu_get_pte_ptr(*pte);
}

/* Present functions */

int mmu_pgd_present(mmu_pgd_t *pgd) {
	return ((unsigned int) *pgd) & MMU_ET_PRESENT;
}

int mmu_pmd_present(mmu_pmd_t *pmd) {
	return ((unsigned int) *pmd) & MMU_ET_PRESENT;
}

int mmu_pte_present(mmu_pte_t *pte) {
	return ((unsigned int) *pte) & MMU_ET_PRESENT;
}

/* Unset functions */

void mmu_pgd_unset(mmu_pgd_t *pgd) {
	mmu_set_val(pgd, 0);
}

void mmu_pmd_unset(mmu_pmd_t *pmd) {
	mmu_set_val(pmd, 0);
}

void mmu_pte_unset(mmu_pte_t *pte) {
	mmu_set_val(pte, 0);
}

/* Page Table flags */
void mmu_pte_set_writable(mmu_pte_t *pte, int val) {
	if (val) {
		mmu_set_val(pte, *pte | MMU_PAGE_WRITABLE);
	} else {
		mmu_set_val(pte, *pte & (~MMU_PAGE_WRITABLE));
	}
}

void mmu_pte_set_usermode(mmu_pte_t *pte, int val) {
	if (val) {
		mmu_set_val(pte, *pte & (~MMU_PAGE_SUPERVISOR));
	} else {
		mmu_set_val(pte, *pte | MMU_PAGE_SUPERVISOR);
	}
}

void mmu_pte_set_cacheable(mmu_pte_t *pte, int val) {
	if (val) {
		mmu_set_val(pte, *pte | MMU_PAGE_CACHEABLE);
	} else {
		mmu_set_val(pte, *pte & (~MMU_PAGE_CACHEABLE));
	}
}

void mmu_pte_set_executable(mmu_pte_t *pte, int val) {

}
