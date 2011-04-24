/**
 * @file
 * @brief Defines specific features for MMU in SPARC architecture.
 *
 * @date 15.03.2010
 * @author Anton Bondarev
 * @author Alexander Batyukov
 * @author Nikolay Korotky
 */

#ifndef SPARC_MMU_CORE_H_
#define SPARC_MMU_CORE_H_

#include <types.h>
#include <asm/asi.h>
#include <hal/mm/mmu_types.h>
/**
 * MMU registers.
 * Note: MMU mapped via an Alternate Address Space of the CPU (ASI_M_MMUREGS).
 */

#define LEON_CNR_CTRL        0x00000000 /** Control Register */
#define LEON_CNR_CTXP        0x00000100 /** Context Table Pointer Register */
#define LEON_CNR_CTX         0x00000200 /** Context Register */
#define LEON_CNR_F           0x00000300 /** Fault Status Register */
#define LEON_CNR_FADDR       0x00000400 /** Fault Address Register */

#define LEON_CNR_CTX_NCTX    256        /** number of MMU ctx */
//#define LEON_CNR_CTRL_TLBDIS 0x80000000

/** Control Register's fields */

/** specific implementation of the MMU */
#define MMU_CTRL_IMPL        0xF0000000
/** particular version of this MMU implementation */
#define MMU_CTRL_VER         0x0F000000
/** The System Control bits */
#define MMU_CTRL_SC          0x00FFFF00
/**
 * The PSO bit controls whether the memory model seen by the processor
 * is Partial Store Ordering (PSO=1) or Total Store Ordering (PSO=0).
 */
#define MMU_CTRL_PSO         0x00000080
/**
 * “No Fault” bit.
 */
#define MMU_CTRL_NF          0x00000002
/**
 * The Enable bit enables or disables the MMU.
 */
#define MMU_CTRL_E           0x00000001

/** Fault Status Register's fields */

/**
 * External Bus Error
 */
#define MMU_F_EBE            0x0003FC00
/**
 * The Level field is set to the page table
 * level of the entry which caused the fault.
 */
#define MMU_F_L              0x00000300
/**
 * Access Type:
 * 0 Load from User Data Space
 * 1 Load from Supervisor Data Space
 * 2 Load/Execute from User Instruction Space
 * 3 Load/Execute from Supervisor Instruction Space
 * 4 Store to User Data Space
 * 5 Store to Supervisor Data Space
 * 6 Store to User Instruction Space
 * 7 Store to Supervisor Instruction Space
 */
#define MMU_F_AT             0x000000E0
/**
 * Fault Type:
 * 0 None
 * 1 Invalid address error
 * 2 Protection error
 * 3 Privilege violation error
 * 4 Translation error
 * 5 Access bus error
 * 6 Internal error
 */
#define MMU_F_FT             0x0000001C
/**
 * Fault Address Valid bit
 */
#define MMU_F_FAV            0x00000002
/**
 * Overwrite bit
 */
#define MMU_F_OW             0x00000001

/** MMU three-level mapping */

/** Level-3 Table:64 entries, 4 bytes a piece */
#define __MMU_PTABLE_SIZE   0x40UL
/** Level-2 Table:64 entries, 4 bytes a piece */
#define __MMU_MTABLE_SIZE   0x40UL
/** Level-1 Table: 256 entries, 4 bytes a piece */
#define __MMU_GTABLE_SIZE   0x100UL

/* size of table entry */
#define __MMU_ENTRY_SIZE    4

/* must be aligned for sizes of all tables */
#define PAGE_HEADER_SIZE 0x400

/** 4K-byte pages */
#define __MMU_PAGE_SIZE            (1<<12)

/* The PTE non-page bits.  Some notes:
 * 1) cache, modified, valid, and ref are frobbable
 *    for both supervisor and user pages.
 * 2) exec and write will only give the desired effect
 *    on user pages
 * 3) use priv and priv_readonly for changing the
 *    characteristics of supervisor ptes
 */
#define MMU_PTE_CACHE        0x80
#define MMU_PTE_MODIFIED     0x40
#define MMU_PTE_REF          0x20
#define MMU_PTE_EXEC         0x08
#define MMU_PTE_WRITE        0x04
//#define MMU_PTE_PRIV         0x1c
//#define MMU_PTE_PRIV_RDONLY  0x18
#define MMU_PTE_ET           0x2

/** Accesses Allowed:
 *     |User access        |Supervisor access
 * ACC |(ASI = 0x8 or 0xA) |(ASI = 0x9 or 0xB)
 * 0   |Read Only          |Read Only
 * 1   |Read/Write         |Read/Write
 * 2   |Read/Execute       |Read/Execute
 * 3   |Read/Write/Execute |Read/Write/Execute
 * 4   |Execute Only       |Execute Only
 * 5   |Read Only          |Read/Write
 * 6   |No Access          |Read/Execute
 * 7   |No Access          |Read/Write/Execute
 */
#define MMU_PTE_RO         0x0
#define MMU_PTE_RW         0x1
#define MMU_PTE_RE         0x2
#define MMU_PTE_EO         0x4

#define __MMU_PAGE_CACHEABLE   MMU_PTE_CACHE
#define __MMU_PAGE_WRITEABLE   MMU_PTE_WRITE
#define __MMU_PAGE_EXECUTEABLE MMU_PTE_EXEC


/* Physical page extraction from PTP's and PTE's. */
#define MMU_CTX_PMASK      0xfffffff0
#define MMU_PTD_PMASK      0xfffffff0
#define MMU_PTE_PMASK      0xffffff00

/**
 * Entry Type — This field differentiates a PTD from a PTE. For a PTD,
 * it must contain the value 1.
 */
#define MMU_ET_PTD           0x1
#if 0
typedef unsigned long pte_t;
typedef unsigned long pmd_t;
typedef unsigned long pld_t;
typedef unsigned long pgd_t;
typedef unsigned long ctxd_t;
#endif

#define pte_val(x)    (x)
#define pmd_val(x)    (x)
#define pgd_val(x)    (x)
#define ctxd_val(x)   (x)

#define __pte(x)      (x)
#define __pmd(x)      (x)
#define __pgd(x)      (x)
#define __ctxd(x)     (x)

/* This makes sense. Honest it does - Anton */
#define __nocache_pa(VADDR)  VADDR
#define __nocache_va(PADDR)  PADDR
#define __nocache_fix(VADDR) VADDR
#if 1
#define __MMU_GTABLE_MASK         0xFF000000
#define __MMU_GTABLE_MASK_OFFSET  24
#define __MMU_MTABLE_MASK         0x00FC0000
#define __MMU_MTABLE_MASK_OFFSET  18
#define __MMU_PTABLE_MASK         0x0003F000
#define __MMU_PTABLE_MASK_OFFSET  12
#define __MMU_PAGE_MASK           0xFFF
#endif

#define MMU_GTABLE_MASK_OFFSET __MMU_GTABLE_MASK_OFFSET
#define MMU_MTABLE_MASK_OFFSET __MMU_MTABLE_MASK_OFFSET
#define MMU_PTABLE_MASK_OFFSET __MMU_PTABLE_MASK_OFFSET
/** Set MMU reg */
static inline void mmu_set_mmureg(unsigned long addr_reg,
				unsigned long regval) {
	__asm__ __volatile__(
		"sta %0, [%1] %2\n\t"
		:
		: "r"(regval), "r"(addr_reg), "i"(ASI_M_MMUREGS)
		: "memory"
	);
}

/** Get MMU reg */
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
	mmu_swap((unsigned long *) ptep, pte_val(pteval));
}

/* XXX should we hyper_flush_whole_icache here - Anton */
static inline void mmu_ctxd_set(mmu_ctx_t *ctxp, mmu_pgd_t *pgdp) {
	mmu_set_pte((mmu_pte_t *) ctxp,
		(MMU_ET_PTD | (__nocache_pa((unsigned long) pgdp) >> 4)));
}

static inline void mmu_pgd_set(mmu_pgd_t * pgdp, mmu_pmd_t * pmdp) {
	mmu_set_pte((mmu_pte_t *) pgdp,
		(MMU_ET_PTD | (__nocache_pa((unsigned long) pmdp) >> 4)));
}

static inline void mmu_pmd_set(mmu_pmd_t * pmdp, mmu_pte_t * ptep) {
	mmu_set_pte((mmu_pte_t *) pmdp,
		(MMU_ET_PTD | (__nocache_pa((unsigned long) ptep) >> 4)));
}

static inline mmu_pmd_t *mmu_pgd_get(mmu_pgd_t * pgdp) {
	return (mmu_pmd_t *) ((((unsigned long) *pgdp) & MMU_PTD_PMASK) << 4);
}

static inline mmu_pte_t *mmu_pmd_get(mmu_pmd_t * pmdp) {
	return (mmu_pte_t *) ((((unsigned long) *pmdp) & MMU_PTD_PMASK) << 4);
}

static inline mmu_pte_t mmu_pte_format(paddr_t addr, mmu_page_flags_t flags) {
	return ((addr >> 4) & MMU_PTE_PMASK) | flags | MMU_PTE_ET;
}

static inline paddr_t mmu_pte_extract(mmu_pte_t pte) {
	return (pte & MMU_PTE_PMASK) << 4;
}

static inline mmu_page_flags_t mmu_flags_extract(mmu_pte_t pte) {
	return (pte & 0xff);
}

static inline int mmu_is_pte(mmu_pte_t pte) {
	return pte & MMU_PTE_ET;
}

#endif /* SPARC_MMU_CORE_H_ */
