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


static uintptr_t *ctx_table[0x100] __attribute__((aligned(MMU_PAGE_SIZE)));
static int ctx_counter = 0;

void mmu_on(void) {
	/* TODO PowerPC MMU hasn't been implemented yet
	__set_msr(__get_msr() | MSR_IS | MSR_DS);
	*/
}

void mmu_off(void) {
	__set_msr(__get_msr() & ~(MSR_IS | MSR_DS));
}

void mmu_set_context(mmu_ctx_t ctx) {
}

mmu_ctx_t mmu_create_context(uintptr_t *pgd) {
	//return 0;
#if 1
	mmu_ctx_t ctx = (mmu_ctx_t) (++ctx_counter);
	ctx_table[ctx] = pgd;
	return ctx;
#endif
}

uintptr_t *mmu_get_root(mmu_ctx_t ctx) {
	return ctx_table[ctx];
}

/* Present functions */
int mmu_present(int lvl, uintptr_t *entry)  {
	switch(lvl) {
	case 0:
		return (uint32_t)*entry & MMU_PAGE_PRESENT;
	case 1:
		return (uint32_t)*entry & MMU_PAGE_PRESENT;
	case 2:
		return (uint32_t)*entry & MMU_PAGE_PRESENT;
	}
	return 0;
}
#if 0
int mmu_pgd_present(uintptr_t *pgd) {
	return 1;
}

int mmu_pmd_present(uintptr_t *pmd) {
	return (uint32_t)*pmd & MMU_PAGE_PRESENT;
}

int mmu_pte_present(uintptr_t *pte) {
	return (uint32_t)*pte & MMU_PAGE_PRESENT;
}
#endif
/* Set functions */
void mmu_set(int lvl, uintptr_t *entry, uintptr_t value) {
	switch (lvl) {
	case 0:
		*entry = (uintptr_t)(((uint32_t)value & ~MMU_PAGE_MASK)
				| MMU_PMD_FLAG | MMU_PAGE_PRESENT);
	case 1:
		if ((uintptr_t)entry == 0x72aa4) {
			lvl = 1;
		}
		*entry = (uintptr_t)(((uint32_t)value & ~MMU_PAGE_MASK)
				| MMU_PAGE_PRESENT);

		return;
	case 2:
		*entry = (uintptr_t)(((uint32_t)value & ~MMU_PAGE_MASK)
				| MMU_PAGE_PRESENT);
		return;
	}
}
#if 0
void mmu_pgd_set(uintptr_t *pgd, uintptr_t *pmd) {
}

void mmu_pmd_set(uintptr_t *pmd, uintptr_t *pte) {
	*pmd = (uintptr_t)(((uint32_t)pte & ~MMU_PAGE_MASK)
			| MMU_PMD_FLAG | MMU_PAGE_PRESENT);
}

void mmu_pte_set(uintptr_t *pte, mmu_paddr_t addr) {
	*pte = (uintptr_t)(((uint32_t)addr & (~MMU_PAGE_MASK)
			| MMU_PAGE_PRESENT);
}
#endif

/* Value functions */
uintptr_t *mmu_get(int lvl, uintptr_t *entry) {
	switch (lvl) {
	case 0:
		return (uintptr_t *)(*entry & ~MMU_PAGE_MASK);
	case 1:
		return (uintptr_t *)(*entry & ~MMU_PAGE_MASK);
	case 2:
		return (uintptr_t *)(*entry & ~MMU_PAGE_MASK);
	}
	return 0;
}
#if 0
uintptr_t * mmu_pgd_value(uintptr_t *pgd) {
	return (uintptr_t *)pgd;
}

uintptr_t * mmu_pmd_value(uintptr_t *pmd) {
	return (uintptr_t *)(*pmd & ~MMU_PAGE_MASK);
}

mmu_paddr_t mmu_pte_value(uintptr_t *pte) {
	return (mmu_paddr_t)(*pte & ~MMU_PAGE_MASK);
}
#endif

/* Unset functions */
void mmu_unset(int lvl, uintptr_t *entry)  {
	switch(lvl) {
	case 1:
	case 2:
		*entry = 0;
		break;
	}
}
#if 0
void mmu_pgd_unset(uintptr_t *pgd) {
}

void mmu_pmd_unset(uintptr_t *pmd) {
	*pmd = 0;
}

void mmu_pte_unset(uintptr_t *pte) {
	*pte = 0;
}
#endif
/**
 * Page Table flags
 */
void mmu_pte_set_writable(uintptr_t *pte, int val) {
	*pte = val ? *pte | MMU_PAGE_WRITABLE : *pte & ~MMU_PAGE_WRITABLE;
}

void mmu_pte_set_usermode(uintptr_t *pte, int val) {
	*pte = val ? *pte | MMU_PAGE_USERMODE : *pte & ~MMU_PAGE_USERMODE;
}

void mmu_pte_set_cacheable(uintptr_t *pte, int val) {
	*pte = val ? *pte | MMU_PAGE_CACHEABLE : *pte & ~MMU_PAGE_CACHEABLE;
}

void mmu_pte_set_executable(uintptr_t *pte, int val) {

}

void mmu_flush_tlb(void) {
}
