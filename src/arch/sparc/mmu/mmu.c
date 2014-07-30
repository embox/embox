/**
 * @file
 * @brief
 *
 * @date 20.07.2014
 * @author Alexander Kalmuk
 */

#include <asm/asi.h>
#include <asm/mmu_consts.h>
#include <hal/mmu.h>
#include <hal/test/traps_core.h>
#include <kernel/printk.h>
#include <kernel/time/ktime.h>
#include <mem/page.h>
#include <string.h>
#include <util/binalign.h>

#include <mem/vmem.h> // for mmu_handle_page_fault()

//#define MMU_DEBUG

#ifdef MMU_DEBUG
#define MMU_DEBUG_PRINT(x) x
#else
#define MMU_DEBUG_PRINT(x)
#endif

mmu_pgd_t *context_table[0x100]  __attribute__((aligned(MMU_PAGE_SIZE)));
static int ctx_cnt;

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

static inline void mmu_set_val(void *addr, unsigned long value) {
	__asm__ __volatile__(
		"swap [%2], %0"
		: "=&r" (value)
		: "0" (value), "r" (addr)
	);
}

static int mmu_handle_page_fault(uint32_t trap_nr, void *data) {
	uint32_t far;
#ifdef MMU_DEBUG
	uint32_t fsr;

	fsr = mmu_get_mmureg(LEON_CNR_F);
#endif
	far = mmu_get_mmureg(LEON_CNR_FADDR);

	// TODO handle types of FT (invalid address, protection error, etc.)
	MMU_DEBUG_PRINT(printk("\nfsr - 0x%x, far - 0x%x\n", fsr, far));
	vmem_handle_page_fault((mmu_vaddr_t) far);

	return 0;
}

void mmu_on(void) {
	uint32_t val;

	/* Set up context table pointer */
	mmu_set_ctable_ptr((unsigned long) context_table);

	/* Set up page fault handler */
	testtraps_set_handler(TRAP_TYPE_HARDTRAP, 0x9, mmu_handle_page_fault);

	/* Turn on MMU */
	val = mmu_get_mmureg(LEON_CNR_CTRL);
	val |= 0x1; // set E
	mmu_set_mmureg(LEON_CNR_CTRL, val);

	mmu_flush_cache_all();

	MMU_DEBUG_PRINT(printk("\nmmu enabled\n"));
}

void mmu_off(void) {
	uint32_t val;

	val = mmu_get_mmureg(LEON_CNR_CTRL);
	val &= ~0x1; // set E
	mmu_set_mmureg(LEON_CNR_CTRL, val);
}

static void mmu_ctxd_set(mmu_ctx_t *ctxp, mmu_pgd_t *pgdp) {
	mmu_set_val((unsigned long *) ctxp,
		(MMU_ET_PTD | (((unsigned long) pgdp) >> 4)));
}

mmu_vaddr_t mmu_get_fault_address(void) {
	return mmu_get_mmureg(LEON_CNR_FADDR);
}

mmu_ctx_t mmu_create_context(mmu_pgd_t *pgd) {
	mmu_ctx_t ctx = (mmu_ctx_t) (++ctx_cnt);
	mmu_ctxd_set((mmu_ctx_t *) (context_table + ctx), pgd);
	MMU_DEBUG_PRINT(printk("created context - 0x%x\n", ctx));
	return ctx;
}

void mmu_set_context(mmu_ctx_t ctx) {
	mmu_set_mmureg(LEON_CNR_CTX, ctx);
}

mmu_pgd_t *mmu_get_root(mmu_ctx_t ctx) {
	return (mmu_pgd_t *) ((((unsigned long) context_table[ctx]) & MMU_PTD_PMASK) << 4);
}

mmu_pmd_t *mmu_pgd_value(mmu_pgd_t *pgd) {
	return (mmu_pmd_t *) ((((unsigned long) *pgd) & MMU_PTD_PMASK) << 4);
}

mmu_pte_t *mmu_pmd_value(mmu_pmd_t *pmd) {
	return (mmu_pte_t *) ((((unsigned long) *pmd) & MMU_PTD_PMASK) << 4);
}

mmu_paddr_t mmu_pte_value(mmu_pte_t *pte) {
	return (((unsigned long) *pte) & MMU_PTE_PMASK) << 4;
}

void mmu_pte_set(mmu_pte_t *ptep, mmu_pte_t addr) {
	mmu_set_val((unsigned long *) ptep, ((addr >> 4) & MMU_PTE_PMASK) | MMU_ET_PTE);
}

void mmu_pgd_set(mmu_pgd_t *pgdp, mmu_pmd_t *pmdp) {
	mmu_set_val((unsigned long *) pgdp,
		(MMU_ET_PTD | (((unsigned long) pmdp) >> 4)));
}

void mmu_pmd_set(mmu_pmd_t *pmdp, mmu_pte_t *ptep) {
	mmu_set_val((unsigned long *) pmdp,
		(MMU_ET_PTD | (((unsigned long) ptep) >> 4)));
}

void mmu_pgd_unset(mmu_pgd_t *pgd) {
	return ;
}

void mmu_pmd_unset(mmu_pmd_t *pmd) {
	*pmd = 0;
}

void mmu_pte_unset(mmu_pgd_t *pte) {
	*pte = 0;
}

int mmu_pgd_present(mmu_pgd_t *pgd) {
	return ((uint32_t) *pgd & MMU_ET_PTD);
}

int mmu_pmd_present(mmu_pmd_t *pmd) {
	return ((uint32_t) *pmd & MMU_ET_PTD);
}

int mmu_pte_present(mmu_pte_t *pte) {
	return ((uint32_t) *pte & MMU_ET_PTE);
}

void mmu_pte_set_writable(mmu_pte_t *pte, int value){
	if (value) {
		*pte = *pte | MMU_PAGE_WRITABLE;
	} else {
		*pte = *pte & (~MMU_PAGE_WRITABLE);
	}
}

void mmu_pte_set_cacheable(mmu_pte_t *pte, int value) {
	if (value) {
		*pte = *pte | MMU_PAGE_CACHEABLE;
	} else {
		*pte = *pte & (~MMU_PAGE_CACHEABLE);
	}
}

void mmu_pte_set_usermode(mmu_pte_t *pte, int value) {
	//MMU_DEBUG_PRINT(printk("\nmmu_pte_set_usermode does not implemented!\n"));
}

void mmu_pte_set_executable(mmu_pte_t *pte, int value) {
	if (value) {
		*pte = *pte | MMU_PAGE_EXECUTABLE;
	} else {
		*pte = *pte & (~MMU_PAGE_EXECUTABLE);
	}
}
