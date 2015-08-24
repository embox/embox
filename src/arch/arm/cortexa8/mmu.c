/**
 * @file mmu.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 2015-08-18
 */

#include <asm/regs.h>
#include <embox/unit.h>
#include <hal/mmu.h>
#include <string.h>

EMBOX_UNIT_INIT(mmu_init);

static uint8_t translation_table[16384] __attribute__((aligned(1 << 14)));

/**
 * @brief Fill translation table and so on
 * @note Assume MMU is off right now
 *
 * @return
 */
static int mmu_init(void) {
	memset(translation_table, 0xff, sizeof(translation_table));
	/* Setup physical address of the first level translation table */
	__asm__ __volatile__ (
		"ldr r0, =%[addr]\n\t"
		"mcr p15, 0, r0, c2, c0, 0\n\t"
		"mcr p15, 0, r0, c2, c0, 1"
		: : [addr] "X" (&translation_table[0])
	);
	return 0;
}

/**
* @brief Turn MMU on
*
* @note Set flag CR_M at c1, the control register
*/
void mmu_on(void) {
#ifndef NOMMU
	__asm__ __volatile__ (
		"mrc p15, 0, r0, c1, c0, 0\n\t"
		"orr r0, r0, %[flag]\n\t" /* enabling MMU */
		"mcr p15, 0, r0, c1, c0, 0"
		: : [flag] "I" (CR_M)
	);
#endif
}

/**
* @brief Turn MMU off
*
* @note Clear flag CR_M at c1, the control register
*/
void mmu_off(void) {
#ifndef NOMMU
	__asm__ __volatile__ (
		"mrc p15, 0, r0, c1, c0, 0\n\t"
		"bic r0, r0, %[flag]\n\t"
		"mcr p15, 0, r0, c1, c0, 0"
		: : [flag] "I" (CR_M)
	);
#endif
}

mmu_vaddr_t mmu_get_fault_address(void) {
	return 0;
}

mmu_ctx_t mmu_create_context(mmu_pgd_t *pgd) {
	return 0;
}

void mmu_set_context(mmu_ctx_t ctx) {
}

mmu_pgd_t *mmu_get_root(mmu_ctx_t ctx) {
	return 0;
}

mmu_pmd_t *mmu_pgd_value(mmu_pgd_t *pgd) {
	return 0;
}

mmu_pte_t *mmu_pmd_value(mmu_pmd_t *pmd) {
	return 0;
}

mmu_paddr_t mmu_pte_value(mmu_pte_t *pte) {
	return 0;
}

void mmu_pgd_set(mmu_pgd_t *pgd, mmu_pmd_t *pmd) {
}

void mmu_pmd_set(mmu_pgd_t *pmd, mmu_pmd_t *pte) {
}

void mmu_pte_set(mmu_pgd_t *pte, mmu_paddr_t addr) {
}

void mmu_pgd_unset(mmu_pgd_t *pgd) {
}
void mmu_pmd_unset(mmu_pgd_t *pmd) {
}
void mmu_pte_unset(mmu_pgd_t *pte) {
}

int mmu_pgd_present(mmu_pgd_t *pgd) {
	return 0;
}
int mmu_pmd_present(mmu_pmd_t *pmd) {
	return 0;
}
int mmu_pte_present(mmu_pte_t *pte) {
	return 0;
}

void mmu_pte_set_writable(mmu_pte_t *pte, int value) {
}
void mmu_pte_set_cacheable(mmu_pte_t *pte, int value) {
}
void mmu_pte_set_usermode(mmu_pte_t *pte, int value) {
}
void mmu_pte_set_executable(mmu_pte_t *pte, int val) {
}
