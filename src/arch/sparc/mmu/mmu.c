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

static mmu_pgd_t *context_table[0x100]  __attribute__((aligned(MMU_PAGE_SIZE)));
static int ctx_counter = 0;

/* ------------------ Managing MMU registers ----------------*/
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

/*
 * 1. Sets up context table pointer.
 * 2. Sets up context number.
 * 3. Turns on MMU.
 */
static inline void mmu_on(void) {
	unsigned long val;

	mmu_set_ctable_ptr((unsigned long) context_table);

	mmu_set_context(0);

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
static inline unsigned long mmu_set_val(void *addr, unsigned long value) {
	__asm__ __volatile__(
		"swap [%2], %0"
		: "=&r" (value)
		: "0" (value), "r" (addr)
	);
	return value;
}

/* ------------------------ Useful defines ----------------------- */

#define mmu_set_ptd_entry(addr,ptr,flags)                                \
	mmu_set_val((void *)(addr),                                          \
			    ((((unsigned long)(ptr))>>4) & MMU_PTD_PMASK) | (flags))

#define mmu_set_pte_entry(addr,ptr,flags)                                \
	mmu_set_val((void *)(addr),                                          \
			    ((((unsigned long)(ptr))>>4) & MMU_PTE_PMASK) | (flags))

#define mmu_get_ptd_ptr(entry) \
	((((unsigned long)(entry)) & MMU_PTD_PMASK) << 4)

#define mmu_get_pte_ptr(entry) \
	((((unsigned long)(entry)) & MMU_PTE_PMASK) << 4)


/* ------------------ Functions which are external -------------- */

mmu_ctx_t mmu_create_context(mmu_pgd_t *pgd) {
	mmu_ctx_t ctx = (mmu_ctx_t) (++ctx_counter);
	mmu_set_ptd_entry(&context_table[ctx], pgd, MMU_ET_PTD);
	return ctx;
}

void mmu_set_context(mmu_ctx_t ctx) {
	mmu_set_mmureg(LEON_CNR_CTX, ctx);
	//mmu_flush_tlb_all();
}

mmu_pgd_t *mmu_get_root(mmu_ctx_t ctx) {
	return (mmu_pgd_t *) mmu_get_ptd_ptr(context_table[ctx]);
}

void mmu_pgd_set(mmu_pgd_t *pgd, mmu_pmd_t *pmd) {
	mmu_set_ptd_entry(pgd, pmd, MMU_ET_PTD);
}

mmu_pmd_t *mmu_pgd_value(mmu_pgd_t *pgd) {
	return (mmu_pmd_t *) mmu_get_ptd_ptr(*pgd);
}

int mmu_pgd_present(mmu_pgd_t *pgd) {
	return ((unsigned int) *pgd) & MMU_ET_PRESENT;
}

void mmu_pmd_set(mmu_pmd_t *pmd, mmu_pte_t *pte) {
	mmu_set_ptd_entry(pmd, pte, MMU_ET_PTD);
}

mmu_pte_t *mmu_pmd_value(mmu_pmd_t * pmd) {
	return (mmu_pte_t *) mmu_get_ptd_ptr(*pmd);
}

int mmu_pmd_present(mmu_pmd_t *pmd) {
	return ((unsigned int) *pmd) & MMU_ET_PRESENT;
}

void mmu_pte_set(mmu_pte_t *pte, mmu_paddr_t addr) {
	mmu_set_pte_entry(pte, addr, MMU_PAGE_SOMEFLAG | MMU_ET_PTE);
}

mmu_paddr_t mmu_pte_value(mmu_pte_t *pte) {
	return (mmu_paddr_t) mmu_get_pte_ptr(*pte);
}

int mmu_pte_present(mmu_pte_t *pte) {
	return ((unsigned int) *pte) & MMU_ET_PRESENT;
}

static uint32_t boot_pgd[0x100] __attribute__((aligned(0x1000)));

#define PAGE_16MB    0x1000000UL

void mmu_enable(void) {
	mmu_set_ptd_entry(&context_table[0], boot_pgd, MMU_ET_PTD);

	for (int i=0; i<0x100; i++) {
		mmu_pte_set(&boot_pgd[i], i * PAGE_16MB);
		mmu_pte_set_writable(&boot_pgd[i], 1);
		mmu_pte_set_usermode(&boot_pgd[i], 0);
		mmu_pte_set_cacheable(&boot_pgd[i], 0);
	}

	mmu_on();
}

/*
 * Page Table flags
 */
void mmu_pte_set_writable(mmu_pte_t *pte, int val) {
	if (val) {
		*pte = *pte | MMU_PAGE_WRITABLE;
	} else {
		*pte = *pte & (~MMU_PAGE_WRITABLE);
	}
}

void mmu_pte_set_usermode(mmu_pte_t *pte, int val) {
	if (val) {
		*pte = *pte & (~MMU_PAGE_SUPERVISOR);
	} else {
		*pte = *pte | MMU_PAGE_SUPERVISOR;
	}
}

void mmu_pte_set_cacheable(mmu_pte_t *pte, int val) {
	if (val) {
		*pte = *pte | MMU_PAGE_CACHEABLE;
	} else {
		*pte = *pte & (~MMU_PAGE_CACHEABLE);
	}
}
