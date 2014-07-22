/*
 * mmu_core_new.c
 *
 *  Created on: 2014 7 22
 *      Author: alexander
 */

#include <asm/asi.h>
#include <asm/mmu_consts.h>
#include <embox/unit.h>
#include <hal/mmu.h>
#include <kernel/printk.h>
#include <kernel/time/ktime.h>
#include <mem/page.h>
#include <string.h>
#include <util/binalign.h>

EMBOX_UNIT_INIT(mmu_enable);

#if 0
#define MMU_DEBUG(x) x
#else
#define MMU_DEBUG(x)
#endif

mmu_pgd_t *context_table[0x100]  __attribute__((aligned(MMU_PAGE_SIZE)));
static mmu_ctx_t ctx;
static int ctx_cnt;
static struct page_allocator *mmu_page_allocator;

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

static inline void mmu_flush_cache_all(void) {
	__asm__ __volatile__(
		"flush\n\t"
		"sta %%g0, [%%g0] %0\n\t"
		:
		: "i" (0x11) /* magic number detected */
		: "memory"
	);
}

static  unsigned long mmu_swap(unsigned long *addr, unsigned long value) {
	__asm__ __volatile__(
		"swap [%2], %0"
		: "=&r" (value)
		: "0" (value), "r" (addr)
	);
	return value;
}

mmu_pgd_t *mmu_get_root(mmu_ctx_t ctx) {
	return (mmu_pgd_t *) ((((unsigned long) context_table[ctx]) & MMU_PTD_PMASK) << 4);
}

/** Set page table entry with value */
void mmu_set_pte(mmu_pte_t *ptep, mmu_pte_t pteval) {
	mmu_swap((unsigned long *) ptep, pteval);
}

void mmu_pgd_set(mmu_pgd_t * pgdp, mmu_pmd_t * pmdp) {
	mmu_set_pte((mmu_pte_t *) pgdp,
		(MMU_ET_PTD | (((unsigned long) pmdp) >> 4)));
}

void mmu_pmd_set(mmu_pmd_t * pmdp, mmu_pte_t * ptep) {
	mmu_set_pte((mmu_pte_t *) pmdp,
		(MMU_ET_PTD | (((unsigned long) ptep) >> 4)));
}

mmu_pmd_t *mmu_pgd_get(mmu_pgd_t * pgdp) {
	return (mmu_pmd_t *) ((((unsigned long) *pgdp) & MMU_PTD_PMASK) << 4);
}

mmu_pte_t *mmu_pmd_get(mmu_pmd_t * pmdp) {
	return (mmu_pte_t *) ((((unsigned long) *pmdp) & MMU_PTD_PMASK) << 4);
}

mmu_pte_t mmu_pte_format(uint32_t addr, uint32_t flags) {
	return ((addr >> 4) & MMU_PTE_PMASK) | flags | MMU_PTE_ET;
}

void mmu_ctxd_set(mmu_ctx_t *ctxp, mmu_pgd_t *pgdp) {
	mmu_set_pte((mmu_pte_t *) ctxp,
		(MMU_ET_PTD | ((unsigned long) pgdp) >> 4));
}

static void *mmu_alloc_table(void) {
	void *addr = page_alloc(mmu_page_allocator, 1);
	assert (addr);
	if (addr) {
		memset(addr, 0 , MMU_PAGE_SIZE);
	}
	return addr;
}

static mmu_pte_t *mmu_alloc_pte_table(void) {
	return (mmu_pte_t *) mmu_alloc_table();
}

static void mmu_map_single_pte(mmu_ctx_t ctx, uint32_t pgd_idx, uint32_t ptd_idx, uint32_t pte_idx, uint32_t addr, uint32_t flags) {
	mmu_pgd_t *pgd_table;
	mmu_pmd_t *ptd_table;
	mmu_pte_t *pte_table;

	MMU_DEBUG(printk("pgd - %d, ptd - %d, pte - %d, addr - %x\n", pgd_idx, ptd_idx, pte_idx, addr));

	// TODO remove this!
	int fl = (0x3) << 2; // rwx

	pgd_table = mmu_get_root(ctx);

	MMU_DEBUG(printk("pgd_table=%p\n", pgd_table));

	/* Get ptd_table */
	if (0 == ((uint32_t) pgd_table[pgd_idx] & MMU_ET_PTD)) { /* we have been not allocated page for this region yet */
		ptd_table = mmu_alloc_pte_table();
		mmu_pgd_set(pgd_table + pgd_idx, ptd_table);
	} else {
		ptd_table = mmu_pgd_get(pgd_table + pgd_idx);
	}

	MMU_DEBUG(printk("ptd_table=%p\n", ptd_table));

	/* Get pte_table */
	if (0 == ((uint32_t) ptd_table[ptd_idx] & MMU_ET_PTD)) { /* we have been not allocated page for this region yet */
		pte_table = mmu_alloc_pte_table();
		mmu_pmd_set(ptd_table + ptd_idx, pte_table);
	} else {
		pte_table = mmu_pmd_get(ptd_table + ptd_idx);
	}

	MMU_DEBUG(printk("pte_table=%p\n", pte_table));

	pte_table[pte_idx] = mmu_pte_format(addr, fl);
}

int mmu_map_region(mmu_ctx_t ctx, uint32_t phy_addr, uint32_t virt_addr,
		size_t reg_size, uint32_t flags) {
	uint32_t pgd_idx, ptd_idx, pte_idx;

	if (0 == (reg_size = __binalign_mask(reg_size, MMU_PAGE_MASK))) {
		return 0;
	}

	virt_addr = __binalign_mask(virt_addr, MMU_PAGE_MASK);
	phy_addr = __binalign_mask(phy_addr, MMU_PAGE_MASK);

	while (reg_size > 0) {
		pgd_idx = (virt_addr & 0xFF000000) >> 24;
		ptd_idx = (virt_addr & 0x00FC0000) >> 18;
		pte_idx = (virt_addr & 0x0003F000) >> 12;

		mmu_map_single_pte(ctx, pgd_idx, ptd_idx, pte_idx, phy_addr, flags);

		virt_addr += MMU_PAGE_SIZE;
		phy_addr += MMU_PAGE_SIZE;
		reg_size -= MMU_PAGE_SIZE;
	}

	return 0;
}

int mmu_init_context(mmu_ctx_t *ctx) {
	*ctx = (mmu_ctx_t) (ctx_cnt++);

	mmu_ctxd_set((mmu_ctx_t *) (context_table + *ctx), mmu_alloc_table());

	return 0;
}

static int init_mmu_page_allocator(void) {
	char *pages_start;
	extern struct page_allocator *__phymem_allocator;

	if (!(pages_start = page_alloc(
			__phymem_allocator, 64))) {
		return -1;
	}

	mmu_page_allocator = page_allocator_init(pages_start,
			64 * 4096,
			MMU_PAGE_SIZE);

	return 0;
}

static int mmu_enable(void) {
	unsigned long val;

	if (0 > init_mmu_page_allocator()) {
		printk("ALLOCATOR INIT ERROR\n");
		return -1;
	}

	/* Set up context table pointer */
	mmu_set_ctable_ptr((unsigned long) context_table);

	mmu_init_context(&ctx);
	mmu_set_context(ctx);

	// FIXME
	mmu_map_region(ctx, 0x0C000000, 0x0C000000, 0x200000, 0);
	mmu_map_region(ctx, 0x40000000, 0x40000000, 0x200000, 0);
	mmu_map_region(ctx, 0x80000000, 0x80000000, 0x200000, 0);

	/* Turn on MMU */
	val = mmu_get_mmureg(LEON_CNR_CTRL);
	printk("LEON_CNR_CTRL = %x\n", (int)val);
	val |= 0x1; // set E
	mmu_set_mmureg(LEON_CNR_CTRL, val);

	mmu_flush_cache_all();

	printk("\nmmu enabled\n");

	return 0;
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
