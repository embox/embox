/**
 * @file
 * @brief
 *
 * @date 20.07.2014
 * @author Alexander Kalmuk
 */
#include <util/log.h>

#include <stdint.h>
#include <string.h>
#include <util/binalign.h>

#include <asm/asi.h>
#include <asm/mmu_consts.h>
#include <hal/mmu.h>
#include <hal/test/traps_core.h>

#include <mem/page.h>

#include <mem/vmem.h> // for mmu_handle_page_fault()

static uintptr_t *context_table[0x100]  __attribute__((aligned(MMU_PAGE_SIZE)));
static int ctx_cnt;

/** Set context table pointer */
static inline void mmu_set_ctable_ptr(unsigned long paddr) {
	paddr = ((paddr >> 4) & MMU_CTX_PMASK);
	mmu_set_mmureg(LEON_CNR_CTXP, paddr);
}

static inline void mmu_flush_cache_all(void) {
	__asm__ __volatile__(
		"flush\n\t"
		"sta %%g0, [%%g0] %0\n\t"
		:
		: "i" (0x11) /* magic number detected */
		: "memory"
	);
}

static inline void mmu_set_val(void *addr, unsigned long value) {
	__asm__ __volatile__(
		"swap [%2], %0"
		: "=&r" (value)
		: "0" (value), "r" (addr)
	);
}

void mmu_on(void) {
	uint32_t val;

	/* Set up context table pointer */
	mmu_set_ctable_ptr((unsigned long) context_table);

	/* Turn on MMU */
	val = mmu_get_mmureg(LEON_CNR_CTRL);
	val |= 0x1; // set E
	mmu_set_mmureg(LEON_CNR_CTRL, val);

	mmu_flush_cache_all();

	log_debug("mmu enabled");
}

void mmu_off(void) {
	uint32_t val;

	val = mmu_get_mmureg(LEON_CNR_CTRL);
	val &= ~0x1; // set E
	mmu_set_mmureg(LEON_CNR_CTRL, val);
}

static void mmu_ctxd_set(mmu_ctx_t *ctxp, uintptr_t *pgdp) {
	mmu_set_val((unsigned long *) ctxp,
		(MMU_ET_PTD | (((unsigned long) pgdp) >> 4)));
}

mmu_vaddr_t mmu_get_fault_address(void) {
	return mmu_get_mmureg(LEON_CNR_FADDR);
}

mmu_ctx_t mmu_create_context(uintptr_t *pgd) {
	mmu_ctx_t ctx = (mmu_ctx_t) (++ctx_cnt);
	mmu_ctxd_set((mmu_ctx_t *) (context_table + ctx), pgd);
	log_debug("created context - 0x%x", ctx);
	return ctx;
}

void mmu_set_context(mmu_ctx_t ctx) {
	mmu_set_mmureg(LEON_CNR_CTX, ctx);
}

uintptr_t *mmu_get_root(mmu_ctx_t ctx) {
	return (uintptr_t *) ((((unsigned long) context_table[ctx]) & MMU_PTD_PMASK) << 4);
}

uintptr_t *mmu_get(int lvl, uintptr_t *entry) {
	switch (lvl) {
	case 0:
	case 1:
		return (uintptr_t *) ((*entry & MMU_PTD_PMASK) << 4);
	case 2:
		return (uintptr_t *)((*entry & MMU_PTE_PMASK) << 4);
	}
	return 0;
}

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

void mmu_unset(int lvl, uintptr_t *entry)  {
	switch(lvl) {
	case 1:
	case 2:
		*entry = 0;
		break;
	}
}

int mmu_present(int lvl, uintptr_t *entry)  {
	switch(lvl) {
	case 0:
	case 1:
		return *entry & MMU_ET_PTD;
	case 2:
		return *entry & MMU_ET_PTE;
	}
	return 0;
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
	return ((addr >> 4) & MMU_PTE_PMASK) | flags | MMU_ET_PTE;
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
void mmu_pte_set_writable(uintptr_t *pte, int value){
	if (value & PROT_WRITE) {
		*pte = *pte | MMU_PAGE_WRITABLE;
	} else {
		*pte = *pte & (~MMU_PAGE_WRITABLE);
	}
}

void mmu_pte_set_cacheable(uintptr_t *pte, int value) {
	if (value & PROT_NOCACHE) {
		*pte = *pte & (~MMU_PAGE_CACHEABLE);
	} else {
		*pte = *pte | MMU_PAGE_CACHEABLE;
	}
}

void mmu_pte_set_usermode(uintptr_t *pte, int value) {
}

void mmu_pte_set_executable(uintptr_t *pte, int value) {
	if (value) {
		*pte = *pte | MMU_PAGE_EXECUTABLE;
	} else {
		*pte = *pte & (~MMU_PAGE_EXECUTABLE);
	}
}
#endif

static inline void mmu_flush_tlb_all(void) {
	mmu_flush_cache_all();
	__asm__ __volatile__(
		"sta %%g0, [%0] %1\n\t"
		:
		: "r" (0x400), "i" (0x18) /* magic number detected */
		: "memory"
	);
}

void mmu_flush_tlb(void) {
	mmu_flush_tlb_all();
}
