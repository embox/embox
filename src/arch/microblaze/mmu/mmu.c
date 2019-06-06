/**
 * @file
 * @brief
 *
 * @date 03.12.12
 * @author Anton Bulychev
 */
#include <stdint.h>

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
uintptr_t *mmu_value(int lvl, uintptr_t *entry) {
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
	if (val) {
		mmu_set_val(pte, *pte | MMU_PAGE_CACHEABLE);
	} else {
		mmu_set_val(pte, *pte & (~MMU_PAGE_CACHEABLE));
	}
}

void mmu_pte_set_executable(uintptr_t *pte, int val) {
}
