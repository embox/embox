/**
 * @file mmu.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 2015-08-18
 */
#include <util/log.h>

#include <string.h>
#include <sys/mman.h>

#include <asm/hal/mmu.h>
#include <hal/mmu.h>
#include <mem/vmem.h>

extern void dcache_flush(const void *p, size_t size);

uintptr_t *mmu_get(int lvl, uintptr_t *entry) {
	switch (lvl) {
	case 0:
		return (uintptr_t *) (*entry & ~MMU_PAGE_MASK);
	default:
		log_error("Wrong lvl=%d for ARM small page", lvl);
		return 0;
	}
}

void mmu_set(int lvl, uintptr_t *entry, uintptr_t value) {
	switch (lvl) {
	case 0:
#if 0
	*pte = (mmu_pte_t) ((addr & ~MMU_PAGE_MASK)
			| ARM_MMU_TYPE_SECTION
			| ARM_MMU_SECTION_READ_ACC);
#endif
	*entry = ((value & ~MMU_PAGE_MASK)
			| 0x00C06); /* B=0, XN = 0, Dom=0; AP=11, TEX = 0, APX=0 */
	default:
		log_error("Wrong lvl=%d for ARM small page", lvl);
		return;
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
		return (*entry & ARM_MMU_TYPE_SECTION) == ARM_MMU_TYPE_SECTION;
	default:
		log_error("Wrong lvl=%d for ARM small page", lvl);
		return 0;
	}
}

uintptr_t mmu_pte_pack(uintptr_t addr, int prot) {
#if 0
	/* TODO not work now */
	int flags = ARM_MMU_PAGE_READ_ACC; /* TODO PROT_READ must be set implicitly */

	if (prot & PROT_WRITE) {
		flags |= ARM_MMU_PAGE_WRITE_ACC;
	}
	if (prot & PROT_READ) {
		flags |= ARM_MMU_PAGE_READ_ACC;
	}
	if (!(prot & PROT_NOCACHE)) {
		flags |= L1D_C;
	}

	if (prot & PROT_EXEC) {
		flags &= ~L1D_XN; /* ARM_MMU_SECTION_XN */
	} else {
		flags |= L1D_XN; /* ARM_MMU_SECTION_XN */
	}
	return (addr & ~MMU_PAGE_MASK) | flags | ARM_MMU_TYPE_SECTION;
#endif
	return ((addr & ~MMU_PAGE_MASK)
			| 0x00C06); /* B=0, XN = 0, Dom=0; AP=11, TEX = 0, APX=0 */
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

	return pte & ~MMU_PAGE_MASK;
}
