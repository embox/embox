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
#include <embox/unit.h>
#include <hal/mmu.h>
#include <mem/vmem.h>

#include <framework/mod/options.h>
#include <kernel/printk.h>

EMBOX_UNIT_INIT(mmu_init);

#define DOMAIN_ACCESS OPTION_GET(NUMBER, domain_access)

static uint8_t translation_table[16384] __attribute__((aligned(1 << 14)));

/**
 * @brief Fill translation table and so on
 * @note Assume MMU is off right now
 *
 * @return
 */
static int mmu_init(void) {
	memset(translation_table, 0, sizeof(translation_table));

	__asm__ __volatile__ (
		/* setup c3, Domain Access Control Register */
#if DOMAIN_ACCESS == 1
		"mov r0, #0x55\n\t" /* Client for all domains */
#elif DOMAIN_ACCESS == 3
		"mov r0, #0xff\n\t" /* Manager for all domains */
#else
#error Given domain access level is not supported
#endif
		"orr r0, r0, lsl #8\n\t"
		"orr r0, r0, lsl #16\n\t"
		"mcr p15, 0, r0, c3, c0, 0\n\t"
		: :
	);

	/* Setup physical address of the first level translation table */
	__asm__ __volatile__ (
		"ldr r0, =translation_table\n\t"
		"mcr p15, 0, r0, c2, c0, 0\n\t"
		"mcr p15, 0, r0, c2, c0, 1" : :
	);

	return 0;
}

/**
* @brief Turn MMU on
*
* @note Set flag CR_M at c1, the control register
*/
void mmu_on(void) {
	while(1);
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

void mmu_flush_tlb(void) {
	/* NIY */
}

mmu_vaddr_t mmu_get_fault_address(void) {
	mmu_vaddr_t val;

	__asm__ __volatile__ (
		"mrc p15, 0, %[out], c6, c0, 0" : [out] "=r" (val) :
	);

	return val;
}

mmu_ctx_t mmu_create_context(mmu_pgd_t *pgd) {
	return 0;
}

void mmu_set_context(mmu_ctx_t ctx) {
}

/**
 * @brief Get address of first translation level
 * @note XXX We have the same ctx for all tasks
 *
 * @param ctx
 *
 * @return Pointer to translation table
 */
mmu_pgd_t *mmu_get_root(mmu_ctx_t ctx) {
	return (mmu_pgd_t *) translation_table;
}

/**
 * This two functions could seems strange, but
 * these return values are for supporting secion paging mode
 */
mmu_pmd_t *mmu_pgd_value(mmu_pgd_t *pgd) {
	return pgd; //(mmu_pmd_t *) (((uint8_t *) pgd - translation_table) * 0x4 + translation_table);
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
