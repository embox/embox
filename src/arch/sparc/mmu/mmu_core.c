/**
 * @file
 * @brief
 *
 * @date 05.10.2012
 * @author Anton Bulychev
 */

#include <embox/unit.h>

#include <hal/mmu.h>
#include <asm/asi.h>
#include <asm/mmu_consts.h>

static inline void mmu_set_mmureg(unsigned long addr_reg,
				unsigned long regval) {
	__asm__ __volatile__(
		"sta %0, [%1] %2\n\t"
		:
		: "r"(regval), "r"(addr_reg), "i"(ASI_M_MMUREGS)
		: "memory"
	);
}

static inline unsigned long mmu_get_mmureg(unsigned long addr_reg) {
	register int retval;
	__asm__ __volatile__(
		"lda [%1] %2, %0\n\t"
		: "=r" (retval)
		: "r" (addr_reg), "i" (ASI_M_MMUREGS)
	);
	return retval;
}

/** Set context table pointer */
static inline void mmu_set_ctable_ptr(unsigned long paddr) {
	paddr = ((paddr >> 4) & MMU_CTX_PMASK);
	mmu_set_mmureg(LEON_CNR_CTXP, paddr);
}

/** Get context table pointer */
static inline unsigned long mmu_get_ctable_ptr(void) {
	unsigned int retval;
	retval = mmu_get_mmureg(LEON_CNR_CTXP);
	return (retval & MMU_CTX_PMASK) << 4;
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

static inline void mmu_flush_tlb_all(void) {
	mmu_flush_cache_all();
	__asm__ __volatile__(
		"sta %%g0, [%0] %1\n\t"
		:
		: "r" (0x400), "i" (0x18) /* magic number detected */
		: "memory"
	);
}

static inline void mmu_on(void) {
	unsigned long val;

	/* Set up context table pointer */
	mmu_set_ctable_ptr((unsigned long) context_table);

	/* Turn on MMU */
	val = mmu_get_mmureg(LEON_CNR_CTRL);
	val |= 0x1;
	mmu_set_mmureg(LEON_CNR_CTRL, val);

	mmu_flush_cache_all();
}

/*
 * In general all page table modifications should use the V8 atomic
 * swap instruction.  This insures the mmu and the cpu are in sync
 * with respect to ref/mod bits in the page tables.
 */
void mmu_set_val(void *addr, unsigned long value) {
	__asm__ __volatile__(
		"swap [%2], %0"
		: "=&r" (value)
		: "0" (value), "r" (addr)
	);
}

void mmu_set_context(mmu_ctx_t ctx) {
	mmu_set_mmureg(LEON_CNR_CTX, ctx);
	//mmu_flush_tlb_all();
}

/*
static uint32_t boot_pgd[0x100] __attribute__((aligned(MMU_PAGE_SIZE)));

#define PAGE_16MB    0x1000000UL

void mmu_enable(void) {
	mmu_set_ptd_entry(&context_table[0], boot_pgd);

	for (unsigned int i = 0; i < 0x100; i++) {
		mmu_pte_set(&boot_pgd[i], i * PAGE_16MB);
		mmu_pte_set_writable(&boot_pgd[i], 1);
		mmu_pte_set_usermode(&boot_pgd[i], 0);
		mmu_pte_set_cacheable(&boot_pgd[i], 0);
	}

	mmu_on();
}
*/
