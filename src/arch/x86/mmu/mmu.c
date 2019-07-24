/**
 * @file
 * @brief
 *
 * @date 04.10.2012
 * @author Anton Bulychev
 */
#include <util/log.h>

#include <stdint.h>
#include <kernel/panic.h>

#include <asm/flags.h>
#include <asm/cr_regs.h>

#include <hal/mmu.h>
#include <mem/vmem.h>

#define MMU_PMD_FLAG  (MMU_PAGE_WRITABLE | MMU_PAGE_USERMODE)

static uintptr_t *ctx_table[0x100] __attribute__((aligned(MMU_PAGE_SIZE)));
static int ctx_counter = 0;


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

mmu_ctx_t mmu_create_context(uintptr_t *pgd) {
	mmu_ctx_t ctx = (mmu_ctx_t) (++ctx_counter);
	ctx_table[ctx] = pgd;
	return ctx;
}

uintptr_t *mmu_get_root(mmu_ctx_t ctx) {
	return ctx_table[ctx];
}

/* Present functions */
int mmu_present(int lvl, uintptr_t *entry) {
	switch (lvl) {
	case 0:
		return (((uint32_t)*entry) & MMU_PAGE_PRESENT);
	case 1:
		return (((uint32_t)*entry) & MMU_PAGE_PRESENT);
	default:
		log_error("Wrong lvl=%d for ARM small page", lvl);
		return 0;
	}
}

/* Set functions */
void mmu_set(int lvl, uintptr_t *entry, uintptr_t value) {
	switch (lvl) {
	case 0:
		*entry = ((((uint32_t) value) & (~MMU_PAGE_MASK))
				| MMU_PMD_FLAG | MMU_PAGE_PRESENT);
		break;
	case 1:
		*entry = ((((uint32_t) value) & (~MMU_PAGE_MASK)) | MMU_PAGE_PRESENT);
		break;
	default:
		log_error("Wrong lvl=%d for ARM small page", lvl);
		break;
	}
}

/* Value functions */

uintptr_t *mmu_get(int lvl, uintptr_t *entry) {
	switch (lvl) {
	case 0:
	case 1:
		return (uintptr_t *) (*entry & ~MMU_PAGE_MASK);
	default:
		log_error("Wrong lvl=%d for ARM small page", lvl);
		return 0;
	}
}

/* Unset functions */
void mmu_unset(int lvl, uintptr_t *entry) {
	*entry = 0;
}

uintptr_t mmu_pte_pack(uintptr_t addr, int prot) {
	int flags = 0;

	if (prot & PROT_WRITE) {
		flags |= MMU_PAGE_WRITABLE;
	}
	if (prot & PROT_NOCACHE) {
		flags |= MMU_PAGE_DISABLE_CACHE;
	}
	if (prot & VMEM_PAGE_USERMODE) {
		flags |= MMU_PAGE_USERMODE;
	}
	return addr | flags | MMU_PAGE_PRESENT;
}

int mmu_pte_set(uintptr_t *entry, uintptr_t value) {
	*entry = value;
	return 0;
}

uintptr_t mmu_pte_get(uintptr_t *entry) {
	return *entry & ~MMU_PAGE_MASK;
}

uintptr_t mmu_pte_unpack(uintptr_t pte, int *flags) {
	int prot = 0;

	if (pte & MMU_PAGE_WRITABLE) {
		prot |= PROT_WRITE;
	}
	if (pte & MMU_PAGE_DISABLE_CACHE) {
		prot |= PROT_NOCACHE;
	}
	if (pte & MMU_PAGE_USERMODE) {
		prot |= VMEM_PAGE_USERMODE;
	}
	*flags = prot;

	return pte & MMU_PAGE_MASK;
}
