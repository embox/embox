/**
 * @file
 * @brief
 *
 * @date 04.10.2012
 * @author Anton Bulychev
 */

#include <stdint.h>
#include <kernel/panic.h>

#include <asm/flags.h>

#include <hal/mmu.h>
#include <mem/vmem.h>

#define MMU_PMD_FLAG  (MMU_PAGE_WRITABLE | MMU_PAGE_USERMODE)

static mmu_pgd_t *ctx_table[0x100] __attribute__((aligned(MMU_PAGE_SIZE)));
static int ctx_counter = 0;

/*
 * Work with registers
 */

static inline void set_cr3(unsigned int pagedir) {
	asm ("mov %0, %%cr3": :"r" (pagedir));
}

static inline unsigned int get_cr3(void) {
	unsigned int _cr3;
	asm ("mov %%cr3, %0":"=r" (_cr3));
	return _cr3;
}

static inline void set_cr0(unsigned int val) {
	asm ("mov %0, %%cr0" : :"r" (val));
}

static inline unsigned int get_cr0(void) {
	unsigned int _cr0;
	asm ("mov %%cr0, %0":"=r" (_cr0));
	return _cr0;
}

static inline void set_cr4(unsigned int val) {
	asm ("mov %0, %%cr4" : :"r" (val));
}

static inline unsigned int get_cr4(void) {
	unsigned int _cr4;
	asm ("mov %%cr2, %0":"=r" (_cr4):);
	return _cr4;
}


static inline unsigned int get_cr2(void) {
	unsigned int _cr2;

	asm ("mov %%cr2, %0":"=r" (_cr2):);
	return _cr2;
}

/*
#define PAGE_4MB         0x400000UL
#define DEFAULT_FLAGS    (MMU_PAGE_PRESENT | MMU_PAGE_WRITABLE \
						 | MMU_PAGE_DISABLE_CACHE | MMU_PAGE_4MB)

static uint32_t boot_page_dir[0x400] __attribute__ ((section(".data"), aligned(0x1000)));

void mmu_enable(void) {
	for (unsigned int i = 0; i < 0x400; i++) {
		boot_page_dir[i] = DEFAULT_FLAGS | (PAGE_4MB * i);
	}

	set_cr3((uint32_t) boot_page_dir); // Set boot page dir
	set_cr4(get_cr4() | X86_CR4_PSE);  // Set 4MB paging
	set_cr0(get_cr0() | X86_CR0_PG);   // Enable MMU
}*/

void mmu_on(void) {
	set_cr0(get_cr0() | X86_CR0_PG | X86_CR0_WP);
}

void mmu_off(void) {
	set_cr0(get_cr0() & ~X86_CR0_PG);
}

void mmu_flush_tlb_single(unsigned long addr) {
	asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

void mmu_flush_tlb(void) {
	set_cr3(get_cr3());
}

mmu_vaddr_t mmu_get_fault_address(void) {
	return get_cr2() & ~(VMEM_PAGE_SIZE - 1);
}

void mmu_set_context(mmu_ctx_t ctx) {
	set_cr3((uint32_t) mmu_get_root(ctx));
}

mmu_ctx_t mmu_create_context(mmu_pgd_t *pgd) {
	mmu_ctx_t ctx = (mmu_ctx_t) (++ctx_counter);
	ctx_table[ctx] = pgd;
	return ctx;
}

mmu_pgd_t *mmu_get_root(mmu_ctx_t ctx) {
	return ctx_table[ctx];
}

/* Present functions */

int mmu_pgd_present(mmu_pgd_t *pgd) {
	return 1;
}

int mmu_pmd_present(mmu_pmd_t *pmd) {
	return ((uint32_t) *pmd & MMU_PAGE_PRESENT);
}

int mmu_pte_present(mmu_pte_t *pte) {
	return ((uint32_t) *pte & MMU_PAGE_PRESENT);
}

/* Set functions */

void mmu_pgd_set(mmu_pgd_t *pgd, mmu_pmd_t *pmd) {
	panic("%s\n", __func__);
}

void mmu_pmd_set(mmu_pmd_t *pmd, mmu_pmd_t *pte) {
	*pmd = (mmu_pmd_t) ((((uint32_t) pte) & (~MMU_PAGE_MASK))
			| MMU_PMD_FLAG | MMU_PAGE_PRESENT);
}

void mmu_pte_set(mmu_pgd_t *pte, mmu_paddr_t addr) {
	*pte = (mmu_pte_t) ((((uint32_t) addr) & (~MMU_PAGE_MASK))
			| MMU_PAGE_PRESENT);
}

/* Value functions */

mmu_pmd_t *mmu_pgd_value(mmu_pgd_t *pgd) {
	return (mmu_pmd_t *) pgd;
}

mmu_pte_t *mmu_pmd_value(mmu_pmd_t *pmd) {
	return (mmu_pte_t *) ((*pmd) & (~MMU_PAGE_MASK));
}

mmu_paddr_t mmu_pte_value(mmu_pte_t *pte) {
	return (mmu_paddr_t) ((*pte) & (~MMU_PAGE_MASK));
}

/* Unset functions */

void mmu_pgd_unset(mmu_pgd_t *pgd) {
	panic("%s\n", __func__);
}

void mmu_pmd_unset(mmu_pmd_t *pmd) {
	*pmd = 0;
}

void mmu_pte_unset(mmu_pgd_t *pte) {
	*pte = 0;
}

/* Page Table flags */

void mmu_pte_set_writable(mmu_pte_t *pte, int val) {
	if (val) {
		*pte = *pte | MMU_PAGE_WRITABLE;
	} else {
		*pte = *pte & (~MMU_PAGE_WRITABLE);
	}
}

void mmu_pte_set_usermode(mmu_pte_t *pte, int val) {
	if (val) {
		*pte = *pte | MMU_PAGE_USERMODE;
	} else {
		*pte = *pte & (~MMU_PAGE_USERMODE);
	}
}

void mmu_pte_set_cacheable(mmu_pte_t *pte, int val) {
	if (val) {
		*pte = *pte & (~MMU_PAGE_DISABLE_CACHE);
	} else {
		*pte = *pte | MMU_PAGE_DISABLE_CACHE;
	}
}

void mmu_pte_set_executable(mmu_pte_t *pte, int val) {

}
