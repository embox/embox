/**
 * @file
 * @brief
 *
 * @date 03.12.12
 * @author Anton Bulychev
 */
#include <stdint.h>
#include <sys/mman.h>

#include <mem/vmem.h>
#include <hal/mmu.h>

static uintptr_t *context_table[0x100]  __attribute__((aligned(MMU_PAGE_SIZE)));
static int ctx_counter = 0;

mmu_ctx_t mmu_create_context(uintptr_t *pgd) {
	mmu_ctx_t ctx = (mmu_ctx_t) (++ctx_counter);
	mmu_set_ptd_entry(&context_table[ctx], pgd);
	return ctx;
}

uintptr_t *mmu_get_root(mmu_ctx_t ctx) {
	return (uintptr_t *) mmu_get_ptd_ptr(context_table[ctx]);
}

/* Set functions */
void mmu_set(int lvl, uintptr_t *entry, uintptr_t value) {
	switch (lvl) {
	case 0:
	case 1:
		mmu_set_val((unsigned long *) entry,
			(MMU_ET_PTD | (((unsigned long) value) >> 4)));
		return;
	case 2:
		mmu_set_val((unsigned long *) entry, ((value >> 4) & MMU_PTE_PMASK) | MMU_ET_PTE);
		return;
	}
}

/* Value functions */
uintptr_t *mmu_get(int lvl, uintptr_t *entry) {
	switch (lvl) {
	case 0:
	case 1:
		return (uintptr_t *) ((((unsigned long) *entry) & MMU_PTD_PMASK) << 4);
	case 2:
		return (uintptr_t *)(((*entry) & MMU_PTE_PMASK) << 4);
	}
	return 0;
}

/* Present functions */
int mmu_present(int lvl, uintptr_t *entry) {
	switch(lvl) {
	case 0:
	case 1:
		return *entry & MMU_ET_PTD;
	case 2:
		return *entry & MMU_ET_PTE;
	}
	return 0;
}

/* Unset functions */
void mmu_unset(int lvl, uintptr_t *entry) {
	switch(lvl) {
	case 1:
	case 2:
		*entry = 0;
		break;
	}
}

uintptr_t mmu_pte_pack(uintptr_t addr, int prot) {
	int flags = 0;

	if (prot & PROT_WRITE) {
		flags |= MMU_PAGE_WRITABLE;
	}
	if (!(prot & PROT_NOCACHE)) {
		flags |= MMU_PAGE_CACHEABLE;
	}
	if (prot & PROT_EXEC) {
		flags |= MMU_PAGE_EXECUTABLE;
	}
	return ((addr >> 4) & MMU_PTE_PMASK) | MMU_ET_PTE;
}

int mmu_pte_set(uintptr_t *entry, uintptr_t value) {
	mmu_set_val(entry, value);
	return 0;
}

uintptr_t mmu_pte_get(uintptr_t *entry) {
	return *entry;
}

uintptr_t mmu_pte_unpack(uintptr_t pte, int *flags) {
	int prot = 0;

	if (pte & MMU_PAGE_WRITABLE) {
		prot |= PROT_WRITE;
	}
	if (!(pte & MMU_PAGE_CACHEABLE)) {
		prot |= PROT_NOCACHE;
	}
	if (pte & MMU_PAGE_EXECUTABLE) {
		prot |= PROT_EXEC;
	}
	*flags = prot;

	return (pte & MMU_PTE_PMASK) << 4;
}

#if 0
/* Page Table flags */
void mmu_pte_set_writable(uintptr_t *pte, int val) {
	if (val) {
		mmu_set_val(pte, *pte | MMU_PAGE_WRITABLE);
	} else {
		mmu_set_val(pte, *pte & (~MMU_PAGE_WRITABLE));
	}
}

void mmu_pte_set_usermode(uintptr_t *pte, int val) {
	if (val) {
		mmu_set_val(pte, *pte & (~MMU_PAGE_SUPERVISOR));
	} else {
		mmu_set_val(pte, *pte | MMU_PAGE_SUPERVISOR);
	}
}

void mmu_pte_set_cacheable(uintptr_t *pte, int val) {
	if (val & PROT_NOCACHE) {
		mmu_set_val(pte, *pte & (~MMU_PAGE_CACHEABLE));
	} else {
		mmu_set_val(pte, *pte | MMU_PAGE_CACHEABLE);
	}
}

void mmu_pte_set_executable(uintptr_t *pte, int val) {
}
#endif
