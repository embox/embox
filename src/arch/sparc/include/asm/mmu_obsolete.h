/**
 * @file
 *
 * @date 18.06.10
 * @author Anton Bondarev
 */
#ifndef _MMU_H
#define _MMU_H

#include <asm/srmmu_probe_obsolete.h>

#define mmu_flush_cache_all()   leon_flush_tlb_all()
#define mmu_flush_tlb_all()     leon_flush_tlb_all()

/* page table param */
#define PAGE_SHIFT   12
#define PAGE_SIZE   (1<<PAGE_SHIFT)

/* PMD_SHIFT determines the size of the area a second-level page table can map */
#define SRMMU_PMD_SHIFT         18
#define SRMMU_PMD_SIZE          (1UL << SRMMU_PMD_SHIFT)
#define SRMMU_PMD_MASK          (~(SRMMU_PMD_SIZE-1))
#define SRMMU_PMD_ALIGN(addr)   (((addr)+SRMMU_PMD_SIZE-1)&SRMMU_PMD_MASK)

/* PGDIR_SHIFT determines what a third-level page table entry can map */
#define SRMMU_PGDIR_SHIFT       24
#define SRMMU_PGDIR_SIZE        (1UL << SRMMU_PGDIR_SHIFT)
#define SRMMU_PGDIR_MASK        (~(SRMMU_PGDIR_SIZE-1))
#define SRMMU_PGDIR_ALIGN(addr) (((addr)+SRMMU_PGDIR_SIZE-1)&SRMMU_PGDIR_MASK)

#define SRMMU_PTRS_PER_PTE      64
#define SRMMU_PTRS_PER_PMD      64
#define SRMMU_PTRS_PER_PGD      256
#define SRMMU_PTRS_PER_CTX      256

#define SRMMU_PTE_TABLE_SIZE    0x100 /* 64 entries, 4 bytes a piece */
#define SRMMU_PMD_TABLE_SIZE    0x100 /* 64 entries, 4 bytes a piece */
#define SRMMU_PGD_TABLE_SIZE    0x400 /* 256 entries, 4 bytes a piece */

/* Definition of the values in the ET field of PTD's and PTE's */
#define SRMMU_ET_MASK         0x3
#define SRMMU_ET_INVALID      0x0
#define SRMMU_ET_PTD          0x1
#define SRMMU_ET_PTE          0x2
#define SRMMU_ET_REPTE        0x3 /* AIEEE, SuperSparc II reverse endian page! */

/* Physical page extraction from PTP's and PTE's. */
#define SRMMU_CTX_PMASK    0xfffffff0
#define SRMMU_PTD_PMASK    0xfffffff0
#define SRMMU_PTE_PMASK    0xffffff00

/* The pte non-page bits.  Some notes:
 * 1) cache, dirty, valid, and ref are frobbable
 *    for both supervisor and user pages.
 * 2) exec and write will only give the desired effect
 *    on user pages
 * 3) use priv and priv_readonly for changing the
 *    characteristics of supervisor ptes
 */
#define SRMMU_CACHE        0x80
#define SRMMU_DIRTY        0x40
#define SRMMU_REF          0x20
#define SRMMU_EXEC         0x08
#define SRMMU_WRITE        0x04
#define SRMMU_VALID        0x02 /* SRMMU_ET_PTE */
#define SRMMU_PRIV         0x1c
#define SRMMU_PRIV_RDONLY  0x18

#define SRMMU_CHG_MASK    (0xffffff00 | SRMMU_REF | SRMMU_DIRTY)

/* Some day I will implement true fine grained access bits for
 * user pages because the SRMMU gives us the capabilities to
 * enforce all the protection levels that vma's can have.
 * XXX But for now...
 */
#define SRMMU_PAGE_NONE    __pgprot(SRMMU_VALID | SRMMU_CACHE | \
				SRMMU_PRIV | SRMMU_REF)
#define SRMMU_PAGE_SHARED  __pgprot(SRMMU_VALID | SRMMU_CACHE | \
				SRMMU_EXEC | SRMMU_WRITE | SRMMU_REF)
#define SRMMU_PAGE_COPY    __pgprot(SRMMU_VALID | SRMMU_CACHE | \
				SRMMU_EXEC | SRMMU_REF)
#define SRMMU_PAGE_RDONLY  __pgprot(SRMMU_VALID | SRMMU_CACHE | \
				SRMMU_EXEC | SRMMU_REF)
#define SRMMU_PAGE_KERNEL  __pgprot(SRMMU_VALID | SRMMU_CACHE | SRMMU_PRIV | \
				SRMMU_DIRTY | SRMMU_REF)

/* mmu asi spaces*/
#define ASI_M_FLUSH_PROBE  0x18
#define ASI_M_MMUREGS      0x19

#define SRMMU_CTRL_REG           0x00000000
#define SRMMU_CTXTBL_PTR         0x00000100
#define SRMMU_CTX_REG            0x00000200
#define SRMMU_FAULT_STATUS       0x00000300
#define SRMMU_FAULT_ADDR         0x00000400
/*
constant MMCTRL_E� : integer := 0;
constant MMCTRL_NF : integer := 1;
constant MMCTRL_PSO : integer := 7;
constant MMCTRL_SC_U : integer := 23;
constant MMCTRL_SC_D : integer := 8;
constant MMCTRL_VER_U : integer := 27;
constant MMCTRL_VER_D : integer := 24;
constant MMCTRL_IMPL_U : integer := 31;
constant MMCTRL_IMPL_D : integer := 28;
constant MMCTRL_TLBDIS : integer := 31;
*/

#ifndef __ASSEMBLER__

static inline unsigned long get_sp_reg(void) {
	register int retval;
	__asm__ __volatile__("mov %%sp, %0\n\t"
		: "=r" (retval)
	);
	return retval;
}

static inline void srmmu_set_mmureg(unsigned long regval) {
	__asm__ __volatile__("sta %0, [%%g0] %1\n\t"
		:
		: "r" (regval), "i" (ASI_M_MMUREGS)
		: "memory"
	);
}

static inline unsigned long srmmu_get_mmureg(unsigned long addr_reg) {
	register int retval;
	__asm__ __volatile__("lda [%1] %2, %0\n\t"
		: "=r" (retval)
		: "r" (addr_reg), "i" (ASI_M_MMUREGS)
	);
	return retval;
}

static inline void srmmu_set_ctable_ptr(unsigned long paddr) {
	paddr = ((paddr >> 4) & SRMMU_CTX_PMASK);
	__asm__ __volatile__("sta %0, [%1] %2\n\t"
		:
		: "r" (paddr), "r" (SRMMU_CTXTBL_PTR), "i" (ASI_M_MMUREGS)
		: "memory"
	);
}

static inline unsigned long srmmu_get_ctable_ptr(void) {
	unsigned int retval;
	__asm__ __volatile__("lda [%1] %2, %0\n\t"
		: "=r" (retval)
		: "r" (SRMMU_CTXTBL_PTR), "i" (ASI_M_MMUREGS)
	);
	return (retval & SRMMU_CTX_PMASK) << 4;
}

static inline void srmmu_set_context(int context) {
	__asm__ __volatile__("sta %0, [%1] %2\n\t"
		:
		: "r" (context), "r" (SRMMU_CTX_REG), "i" (ASI_M_MMUREGS)
		: "memory"
	);
}

static inline int srmmu_get_context(void) {
	register int retval;
	__asm__ __volatile__("lda [%1] %2, %0\n\t"
		: "=r" (retval)
		: "r" (SRMMU_CTX_REG), "i" (ASI_M_MMUREGS)
	);
	return retval;
}

static inline void srmmu_flush_whole_tlb(void) {
	__asm__ __volatile__("sta %%g0, [%0] %1\n\t"
		:
		: "r" (0x400) /* Flush entire TLB!! */, "i" (ASI_M_FLUSH_PROBE)
		: "memory"
	);
}

typedef unsigned long pte_t;
typedef unsigned long iopte_t;
typedef unsigned long pmd_t;
typedef unsigned long pgd_t;
typedef unsigned long ctxd_t;
typedef unsigned long pgprot_t;
typedef unsigned long iopgprot_t;

#define pte_val(x)      (x)
#define iopte_val(x)    (x)
#define pmd_val(x)      (x)
#define pgd_val(x)      (x)
#define ctxd_val(x)     (x)
#define pgprot_val(x)   (x)
#define iopgprot_val(x) (x)

#define __pte(x)      (x)
#define __iopte(x)    (x)
#define __pmd(x)      (x)
#define __pgd(x)      (x)
#define __ctxd(x)     (x)
#define __pgprot(x)   (x)
#define __iopgprot(x) (x)

/*
 * In general all page table modifications should use the V8 atomic
 * swap instruction.  This insures the mmu and the cpu are in sync
 * with respect to ref/mod bits in the page tables.
 */
#if 1
static  unsigned long srmmu_swap(unsigned long *addr, unsigned long value) {
#ifndef IMAGE_CREATE
	__asm__ __volatile__("swap [%2], %0"
		: "=&r" (value)
		: "0" (value), "r" (addr)
	);
#else
	unsigned long old;
	*addr = value;
	value = old;
#endif
	return value;
}

static  void srmmu_set_pte(pte_t *ptep, pte_t pteval) {
	srmmu_swap((unsigned long *)ptep, pte_val(pteval));
}

/* The very generic SRMMU page table operations. */
static  int srmmu_device_memory(unsigned long x) {
	return ((x & 0xF0000000) != 0);
}
#endif
//int srmmu_cache_pagetables;

/* XXX Make this dynamic based on ram size - Anton */
#define SRMMU_NOCACHE_BITMAP_SIZE (SRMMU_NOCACHE_NPAGES * 16)
#define SRMMU_NOCACHE_BITMAP_SHIFT (PAGE_SHIFT - 4)

#if 0
void *srmmu_nocache_pool;
void *srmmu_nocache_bitmap;
int srmmu_nocache_low;
int srmmu_nocache_used;
spinlock_t srmmu_nocache_spinlock;
#endif

/* This makes sense. Honest it does - Anton */
#define __nocache_pa(VADDR) VADDR //(((unsigned long)VADDR) - SRMMU_NOCACHE_VADDR + __pa((unsigned long)srmmu_nocache_pool))
#define __nocache_va(PADDR) PADDR // (__va((unsigned long)PADDR) - (unsigned long)srmmu_nocache_pool + SRMMU_NOCACHE_VADDR)
#define __nocache_fix(VADDR) VADDR // __va(__nocache_pa(VADDR))

#if 1
static  unsigned long srmmu_pgd_page(pgd_t pgd) {
	return srmmu_device_memory(pgd_val(pgd)) ? ~0 :
		(unsigned long)__nocache_va((pgd_val(pgd) & SRMMU_PTD_PMASK) << 4);
}

static  unsigned long srmmu_pmd_page(pmd_t pmd) {
	return srmmu_device_memory(pmd_val(pmd)) ? ~0 :
		(unsigned long)__nocache_va((pmd_val(pmd) & SRMMU_PTD_PMASK) << 4);
}

#if 0
static  struct page *srmmu_pte_page(pte_t pte) {
	return (mem_map + (unsigned long)(srmmu_device_memory(pte_val(pte)) ? ~0 :
		(((pte_val(pte) & SRMMU_PTE_PMASK) << 4) >> PAGE_SHIFT)));
}
#endif
static  int srmmu_pte_none(pte_t pte) {
	return !(pte_val(pte) & 0xFFFFFFF);
}

static  int srmmu_pte_present(pte_t pte) {
	return ((pte_val(pte) & SRMMU_ET_MASK) == SRMMU_ET_PTE);
}

static  void srmmu_pte_clear(pte_t *ptep) {
	srmmu_set_pte(ptep, __pte(0));
}

static  int srmmu_pmd_none(pmd_t pmd) {
	return !(pmd_val(pmd) & 0xFFFFFFF);
}

static  int srmmu_pmd_bad(pmd_t pmd) {
	return (pmd_val(pmd) & SRMMU_ET_MASK) != SRMMU_ET_PTD;
}

static  int srmmu_pmd_present(pmd_t pmd) {
	return ((pmd_val(pmd) & SRMMU_ET_MASK) == SRMMU_ET_PTD);
}

static  void srmmu_pmd_clear(pmd_t *pmdp) {
	srmmu_set_pte((pte_t *)pmdp, __pte(0));
}

static  int srmmu_pgd_none(pgd_t pgd) {
	return !(pgd_val(pgd) & 0xFFFFFFF);
}

static  int srmmu_pgd_bad(pgd_t pgd) {
	return (pgd_val(pgd) & SRMMU_ET_MASK) != SRMMU_ET_PTD;
}

static int srmmu_pgd_present(pgd_t pgd) {
	return ((pgd_val(pgd) & SRMMU_ET_MASK) == SRMMU_ET_PTD);
}

static void srmmu_pgd_clear(pgd_t * pgdp) {
	srmmu_set_pte((pte_t *)pgdp, __pte(0));
}

static int srmmu_pte_write(pte_t pte) {
	return pte_val(pte) & SRMMU_WRITE;
}

static int srmmu_pte_dirty(pte_t pte) {
	return pte_val(pte) & SRMMU_DIRTY;
}

static int srmmu_pte_young(pte_t pte) {
	return pte_val(pte) & SRMMU_REF;
}

static pte_t srmmu_pte_wrprotect(pte_t pte) {
	return __pte(pte_val(pte) & ~SRMMU_WRITE);
}

static pte_t srmmu_pte_mkclean(pte_t pte) {
	return __pte(pte_val(pte) & ~SRMMU_DIRTY);
}

static pte_t srmmu_pte_mkold(pte_t pte) {
	return __pte(pte_val(pte) & ~SRMMU_REF);
}

static pte_t srmmu_pte_mkwrite(pte_t pte) {
	return __pte(pte_val(pte) | SRMMU_WRITE);
}

static pte_t srmmu_pte_mkdirty(pte_t pte) {
	return __pte(pte_val(pte) | SRMMU_DIRTY);
}

static pte_t srmmu_pte_mkyoung(pte_t pte) {
	return __pte(pte_val(pte) | SRMMU_REF);
}

/*
 * Conversion functions: convert a page and protection to a page entry,
 * and a page entry and page directory to the page they refer to.
 */
#if 0
static pte_t srmmu_mk_pte(struct page *page, pgprot_t pgprot) {
	return __pte((((page - mem_map) << PAGE_SHIFT) >> 4) | pgprot_val(pgprot));
}
#endif

static pte_t srmmu_mk_pte_phys(unsigned long page, pgprot_t pgprot) {
	return __pte(((page) >> 4) | pgprot_val(pgprot));
}

static pte_t srmmu_mk_pte_io(unsigned long page, pgprot_t pgprot, int space) {
	return __pte(((page) >> 4) | (space << 28) | pgprot_val(pgprot));
}

/* XXX should we hyper_flush_whole_icache here - Anton */
static void srmmu_ctxd_set(ctxd_t *ctxp, pgd_t *pgdp) {
	srmmu_set_pte((pte_t *)ctxp,
		(SRMMU_ET_PTD | (__nocache_pa((unsigned long) pgdp) >> 4)));
}

static void srmmu_pgd_set(pgd_t * pgdp, pmd_t * pmdp) {
	srmmu_set_pte((pte_t *)pgdp,
		(SRMMU_ET_PTD | (__nocache_pa((unsigned long) pmdp) >> 4)));
}

static void srmmu_pmd_set(pmd_t * pmdp, pte_t * ptep) {
	srmmu_set_pte((pte_t *)pmdp,
		(SRMMU_ET_PTD | (__nocache_pa((unsigned long) ptep) >> 4)));
}

static pte_t srmmu_pte_modify(pte_t pte, pgprot_t newprot) {
	return __pte((pte_val(pte) & SRMMU_CHG_MASK) | pgprot_val(newprot));
}
#if 0
/* to find an entry in a top-level page table... */
extern pgd_t *srmmu_pgd_offset(struct mm_struct * mm, unsigned long address) {
	return mm->pgd + (address >> SRMMU_PGDIR_SHIFT);
}
#endif
/* Find an entry in the second-level page table.. */
static pmd_t *srmmu_pmd_offset(pgd_t * dir, unsigned long address) {
	return (pmd_t *) srmmu_pgd_page(*dir) +
		((address >> SRMMU_PMD_SHIFT) & (SRMMU_PTRS_PER_PMD - 1));
}

/* Find an entry in the third-level page table.. */
static pte_t *srmmu_pte_offset(pmd_t * dir, unsigned long address) {
	return (pte_t *) srmmu_pmd_page(*dir) +
		((address >> PAGE_SHIFT) & (SRMMU_PTRS_PER_PTE - 1));
}
#endif

#endif /*__ASSEMBLER__*/

#endif /* _MMU_H */
