/**
 * @file
 * @brief
 *
 * @date 13.12.12
 * @author Ilia Vaprol
 */

#include <asm/psr.h>
#include <asm/regs.h>
#include <hal/mmu.h>
#include <stdint.h>

#define MMU_PAGE_PRESENT        (1UL)
#define MMU_PAGE_WRITABLE       (1UL << 11)
#define MMU_PAGE_USERMODE       (1UL << 9)
#define MMU_PAGE_CACHEABLE      (1UL << 10)
#define MMU_PAGE_4MB            (1UL << 3)
#define MMU_PMD_FLAG            (MMU_PAGE_WRITABLE | MMU_PAGE_USERMODE)

void mmu_on(void) {
	__set_msr(__get_msr() | MSR_IS | MSR_DS);
}

void mmu_set_context(mmu_ctx_t ctx) {
}

mmu_ctx_t mmu_create_context(mmu_pgd_t *pgd) {
	return 0;
#if 0
	mmu_ctx_t ctx = (mmu_ctx_t) (++ctx_counter);
	ctx_table[ctx] = pgd;
	return ctx;
#endif
}

mmu_pgd_t *mmu_get_root(mmu_ctx_t ctx) {
	return 0; /*ctx_table[ctx];*/
}

/**
 * Present functions
 */
int mmu_pgd_present(mmu_pgd_t *pgd) {
	return 1;
}

int mmu_pmd_present(mmu_pmd_t *pmd) {
	return (uint32_t)*pmd & MMU_PAGE_PRESENT;
}

int mmu_pte_present(mmu_pte_t *pte) {
	return (uint32_t)*pte & MMU_PAGE_PRESENT;
}

/**
 * Set functions
 */
void mmu_pgd_set(mmu_pgd_t *pgd, mmu_pmd_t *pmd) {
}

void mmu_pmd_set(mmu_pmd_t *pmd, mmu_pmd_t *pte) {
	*pmd = (mmu_pmd_t)(((uint32_t)pte & ~MMU_PAGE_MASK)
			| MMU_PMD_FLAG | MMU_PAGE_PRESENT);
}

void mmu_pte_set(mmu_pgd_t *pte, mmu_paddr_t addr) {
	*pte = (mmu_pte_t)(((uint32_t)addr & (~MMU_PAGE_MASK)
			| MMU_PAGE_PRESENT);
}

/**
 * Value functions
 */
mmu_pmd_t * mmu_pgd_value(mmu_pgd_t *pgd) {
	return (mmu_pmd_t *)pgd;
}

mmu_pte_t * mmu_pmd_value(mmu_pmd_t *pmd) {
	return (mmu_pte_t *)(*pmd & ~MMU_PAGE_MASK);
}

mmu_paddr_t mmu_pte_value(mmu_pte_t *pte) {
	return (mmu_paddr_t)(*pte & ~MMU_PAGE_MASK);
}

/**
 * Unset functions
 */
void mmu_pgd_unset(mmu_pgd_t *pgd) {
}

void mmu_pmd_unset(mmu_pmd_t *pmd) {
	*pmd = 0;
}

void mmu_pte_unset(mmu_pgd_t *pte) {
	*pte = 0;
}

/**
 * Page Table flags
 */
void mmu_pte_set_writable(mmu_pte_t *pte, int val) {
	*pte = val ? *pte | MMU_PAGE_WRITABLE : *pte & ~MMU_PAGE_WRITABLE;
}

void mmu_pte_set_usermode(mmu_pte_t *pte, int val) {
	*pte = val ? *pte | MMU_PAGE_USERMODE : *pte & ~MMU_PAGE_USERMODE;
}

void mmu_pte_set_cacheable(mmu_pte_t *pte, int val) {
	*pte = val ? *pte | MMU_PAGE_CACHEABLE : *pte & ~MMU_PAGE_CACHEABLE;
}

void mmu_pte_set_executable(mmu_pte_t *pte, int val) {

}
