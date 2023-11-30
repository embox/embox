/**
 * @file mmu.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 2015-08-25
 */

#include <inttypes.h>
#include <string.h>
#include <sys/mman.h>

#include <asm/hal/mmu.h>
#include <hal/cache.h>
#include <hal/mmu.h>
#include <hal/reg.h>
#include <mem/vmem.h>
#include <util/log.h>

#define L1_ADDR_MASK 0xFFFFFC00
#define L2_ADDR_MASK 0xFFFFF000

uintptr_t *mmu_get(int lvl, uintptr_t *entry) {
	switch (lvl) {
	case 0:
		return (uintptr_t *)(*entry & L1_ADDR_MASK);
	case 1:
		return (uintptr_t *)(*entry & L2_ADDR_MASK);
	default:
		log_error("Wrong lvl=%d for ARM small page", lvl);
		return 0;
	}
}

void mmu_set(int lvl, uintptr_t *entry, uintptr_t value) {
	switch (lvl) {
	case 0:
		*entry = (value & L1_ADDR_MASK) | ARM_MMU_TYPE_PAGE;
		break;
	case 1:
		*entry = (value & L2_ADDR_MASK) | L1D_TYPE_SD | ARM_MMU_PAGE_READ_ACC;
		break;
	default:
		log_error("Wrong lvl=%d for ARM small page", lvl);
		break;
	}

	dcache_flush(entry, sizeof(*entry));
}

void mmu_unset(int lvl, uintptr_t *entry) {
	*entry = 0;
	dcache_flush(entry, sizeof(*entry));
}

int mmu_present(int lvl, uintptr_t *entry) {
	switch (lvl) {
	case 0:
		return (((uint32_t)*entry) & ARM_MMU_TYPE_PAGE) == ARM_MMU_TYPE_PAGE;
	case 1:
		return (((uint32_t)*entry) & ARM_MMU_SMALL_PAGE) == ARM_MMU_SMALL_PAGE;
	default:
		log_error("Wrong lvl=%d for ARM small page", lvl);
		return 0;
	}
}

uintptr_t mmu_pte_pack(uintptr_t addr, int prot) {
	int flags; /* TODO PROT_READ must be set implicitly */

	flags = ARM_MMU_PAGE_READ_ACC;

	if (prot & PROT_WRITE) {
		flags |= ARM_MMU_PAGE_WRITE_ACC;
	}
	if (prot & PROT_READ) {
		flags |= ARM_MMU_PAGE_READ_ACC;
	}
	if (!(prot & PROT_NOCACHE)) {
#ifndef V5_FORMAT
		flags |= L1D_TEX_USE; /* Outer strongly-ordered memory */
#endif
		flags |= L1D_C | L1D_B; /* Inner write-through cached memory */
	}
	else {
		flags |= L1D_B; /* Shareable device memory */
	}

	if (prot & PROT_EXEC) {
		flags &= ~L1D_XN; /* ARM_MMU_SECTION_XN */
	}
	else {
		flags |= L1D_XN; /* ARM_MMU_SECTION_XN */
	}
	return (addr & L2_ADDR_MASK) | L1D_TYPE_SD | flags;
}

int mmu_pte_set(uintptr_t *entry, uintptr_t value) {
	*entry = value;
	dcache_flush(entry, sizeof(*entry));
	return 0;
}

uintptr_t mmu_pte_get(uintptr_t *entry) {
	return *entry;
}

uintptr_t mmu_pte_unpack(uintptr_t pte, int *flags) {
	int prot = 0;

	if (pte & ARM_MMU_PAGE_WRITE_ACC) {
		prot |= PROT_WRITE;
	}
	if (pte & ARM_MMU_PAGE_READ_ACC) {
		prot |= PROT_READ;
	}
	if (!(pte & L1D_C)) {
		prot |= PROT_NOCACHE;
	}
	if (!(pte & L1D_XN)) {
		prot |= PROT_EXEC;
	}
	*flags = prot;

	return pte & L2_ADDR_MASK;
}

/* vmem_info */
#include <kernel/printk.h>

static uint32_t *get_l1_desc(uint32_t virt_addr) {
	uint32_t *_translation_table = (void *)(ARCH_REG_LOAD(TTBR0)
	                                        & TTBR0_ADDR_MASK);
	int l1_idx = virt_addr >> 20;
	return &_translation_table[l1_idx];
}

static uint32_t *get_l2_desc(uint32_t virt_addr, uint32_t l1_desc) {
	uint32_t *l2_tbl = (uint32_t *)(l1_desc & L1_ADDR_MASK);
	int l2_idx = (virt_addr >> 12) & 0xFF;
	return &l2_tbl[l2_idx];
}

uint32_t vmem_info(uint32_t vaddr) {
	uint32_t *l1_desc = get_l1_desc(vaddr);
	printk("l1 desc %p\n", (void *)*l1_desc);
	uint32_t *l2_desc;

	vaddr &= ~0xFFF;

	printk("vmem info: base %p; vaddr %p",
	    (void *)(ARCH_REG_LOAD(TTBR0) & TTBR0_ADDR_MASK), (void *)vaddr);

	if (*l1_desc == 0x0) {
		printk(" unmapped l1_desc = 0x%" PRIx32 "\n", *l1_desc);
		return 0x0;
	}

	l2_desc = get_l2_desc(vaddr, *l1_desc);
	printk("->%p desc(%p);\n", (void *)((*l2_desc) & L2_ADDR_MASK),
	    (void *)(*l2_desc));
	printk("l1 desc %p\n", (void *)*l1_desc);
	return ((*l2_desc) & L2_ADDR_MASK);
}
