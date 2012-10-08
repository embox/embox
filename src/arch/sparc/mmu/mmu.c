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

/* TODO: Replace it somewhere. */
#define LEON_CNR_CTRL        0x00000000 /** Control Register */
#define LEON_CNR_CTXP        0x00000100 /** Context Table Pointer Register */
#define LEON_CNR_CTX         0x00000200 /** Context Register */
#define LEON_CNR_F           0x00000300 /** Fault Status Register */
#define LEON_CNR_FADDR       0x00000400 /** Fault Address Register */

#define MMU_CTX_PMASK      0xfffffff0
#define MMU_PTD_PMASK      0xfffffff0
#define MMU_PTE_PMASK      0xffffff00

#define MMU_ET_PTD           0x1
#define MMU_ET_PTE           0x2

#define MMU_ET_PRESENT       0x3

#define MMU_PAGE_WRITABLE    ((1UL << 0) << 2)
#define MMU_PAGE_SOMEFLAG    ((1UL << 1) << 2)
#define MMU_PAGE_SUPERVISOR  ((1UL << 2) << 2)

#define MMU_PAGE_CACHEABLE   (1UL << 7)

#define __nocache_pa(VADDR)  VADDR

EMBOX_UNIT_INIT(mmu_init);

static mmu_pgd_t *context_table[0x100]  __attribute__((aligned(MMU_PAGE_SIZE)));
static int ctx_counter = 0;


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

#define mmu_set_context(context) mmu_set_mmureg(LEON_CNR_CTX, context)
#define mmu_get_context() mmu_get_mmureg(LEON_CNR_CTX)

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
 * In general all page table modifications should use the V8 atomic
 * swap instruction.  This insures the mmu and the cpu are in sync
 * with respect to ref/mod bits in the page tables.
 */
static  unsigned long mmu_swap(unsigned long *addr, unsigned long value) {
	__asm__ __volatile__(
		"swap [%2], %0"
		: "=&r" (value)
		: "0" (value), "r" (addr)
	);
	return value;
}

/** Set page table entry with value */
static void mmu_set_pte(mmu_pte_t *ptep, mmu_pte_t pteval) {
	mmu_swap((unsigned long *) ptep, pteval);
}

/* XXX should we hyper_flush_whole_icache here - Anton */
static inline void mmu_ctxd_set(mmu_ctx_t *ctxp, mmu_pgd_t *pgdp) {
	mmu_set_pte((mmu_pte_t *) ctxp,
		(MMU_ET_PTD | (__nocache_pa((unsigned long) pgdp) >> 4)));
}

void mmu_pgd_set(mmu_pgd_t *pgd, mmu_pmd_t *pmd) {
	mmu_set_pte((mmu_pte_t *) pgd,
		(MMU_ET_PTD | (__nocache_pa((unsigned long) pmd) >> 4)));
}

void mmu_pmd_set(mmu_pmd_t *pmd, mmu_pte_t *pte) {
	mmu_set_pte((mmu_pte_t *) pmd,
		(MMU_ET_PTD | (__nocache_pa((unsigned long) pte) >> 4)));
}

mmu_pmd_t *mmu_pgd_value(mmu_pgd_t *pgd) {
	return (mmu_pmd_t *) ((((unsigned long) *pgd) & MMU_PTD_PMASK) << 4);
}

mmu_pte_t *mmu_pmd_value(mmu_pmd_t * pmd) {
	return (mmu_pte_t *) ((((unsigned long) *pmd) & MMU_PTD_PMASK) << 4);
}

mmu_pte_t mmu_pte_format(mmu_paddr_t addr, unsigned int flags) {
	return ((addr >> 4) & MMU_PTE_PMASK) | flags | MMU_ET_PTE;
}

void mmu_pte_set(mmu_pte_t *pte, mmu_paddr_t addr) {
	mmu_set_pte(pte, (unsigned int) mmu_pte_format(addr, MMU_PAGE_SOMEFLAG));
}

mmu_paddr_t mmu_pte_value(mmu_pte_t *pte) {
	return ((unsigned int) *pte & MMU_PTE_PMASK) << 4;
}

int mmu_pgd_present(mmu_pgd_t *pgd) {
	return (unsigned int)*pgd & MMU_ET_PRESENT;
}

int mmu_pmd_present(mmu_pmd_t *pmd) {
	return (unsigned int)*pmd & MMU_ET_PRESENT;
}

int mmu_pte_present(mmu_pte_t *pte) {
	return (unsigned int)*pte & MMU_ET_PRESENT;
}

void mmu_on(void) {
	unsigned long val;
	mmu_set_context((unsigned long) 0);

	val = mmu_get_mmureg(LEON_CNR_CTRL);
	val |= 0x1;
	mmu_set_mmureg(LEON_CNR_CTRL, val);

	mmu_flush_cache_all();
}

void mmu_off(void) {
	unsigned long val;

	mmu_flush_cache_all();

	val = mmu_get_mmureg(LEON_CNR_CTRL);
	val &= ~0x1;
	mmu_set_mmureg(LEON_CNR_CTRL, val);
}

mmu_pgd_t *mmu_get_root(mmu_ctx_t ctx) {
	return (mmu_pgd_t *) ((((unsigned long) context_table[ctx]) & MMU_PTD_PMASK) << 4);
}

mmu_ctx_t mmu_create_context(mmu_pgd_t *pgd) {
	mmu_ctx_t ctx = (mmu_ctx_t) (ctx_counter++);

	mmu_ctxd_set((mmu_ctx_t *) (context_table + ctx), pgd);

	return ctx;
}

void switch_mm(mmu_ctx_t prev, mmu_ctx_t next) {
	mmu_set_context(next);
	mmu_flush_tlb_all();
}

static int mmu_init(void) {
	mmu_set_ctable_ptr((unsigned long) context_table);

	return 0;
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
