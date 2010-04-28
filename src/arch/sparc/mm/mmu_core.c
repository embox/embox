/**
 * @file
 * @brief Implements MMU core interface @link #mmu_core.h @endllink
 *        for SPARC architecture.
 *
 * @date 18.04.2010
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Anton Kozlov
 */
#include <types.h>
#include <string.h>
#include <hal/mm/mmu_core.h>
#include <asm/hal/mm/mmu_page.h>
#include <asm/asi.h>
#include <embox/unit.h>
#include <lib/page_alloc.h>

/* main system MMU environment*/
static mmu_env_t system_env;
/*static*/ mmu_env_t *cur_env;

extern pgd_t sys_pg0;
extern pmd_t sys_pm0;
extern pte_t sys_pt0;
extern ctxd_t sys_ctx;

/* Setup module starting function */
EMBOX_UNIT_INIT(mmu_init)
;

void mmu_save_status(uint32_t *status) {
	__asm__ __volatile__("sta %0, [%1] %2\n\t"
		:
		: "r"(status), "r"(LEON_CNR_CTRL), "i"(ASI_M_MMUREGS)
		: "memory"
	);
}

void mmu_restore_status(uint32_t *status) {
	__asm__ __volatile__("lda [%1] %2, %0\n\t"
		: "=r" (status)
		: "r" (LEON_CNR_CTRL), "i" (ASI_M_MMUREGS)
	);
}

void mmu_save_table(uint32_t *status) {

}

void mmu_restore_table(uint32_t *status) {

}

void mmu_on(void) {
	unsigned long val;
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

uint8_t page_pool[PAGE_SIZE * 100];
uint8_t *page_pool_ptr = NULL;

//TODO replace this with default page alloc
static pmark_t *mypage_alloc(void) {
	if (page_pool_ptr == NULL) {
		page_pool_ptr = page_pool;
	}
	page_pool_ptr += PAGE_SIZE;
	return (pmark_t *) (((unsigned long) page_pool_ptr) & ~0xfff);
}
static void mypage_free(pmark_t *page) {
#ifdef DEBUG
	printf("---need to free page %8x\n", (unsigned long) page);
#endif
	return;
}

static pmark_t *clear_page_alloc(void) {
	uint32_t i;
	pmark_t *t = mypage_alloc();
	for (i = 0; i < PAGE_SIZE >> 2; i++) {
		*(((uint32_t *) t) + i) = 0;
	}
	return t;
}

//#define DEBUG

/* it should be computed but seems too complicated (must be aligned
 * for _all_ possible requests for space -- this the main feature of
 * table_alloc) */
#define PAGE_HEADER_SIZE 1024
typedef struct {
	uint32_t free;
} page_header_t;
uint8_t *cur_page = NULL;
size_t cur_rest = 0;

unsigned long *table_alloc(size_t size) {
	uint8_t *t, *page;
	if (cur_rest < size) {
		cur_page = (uint8_t *) clear_page_alloc();
#ifdef DEBUG
		printf("---requesting new page %x\n",cur_page);
#endif
		cur_rest = PAGE_SIZE - PAGE_HEADER_SIZE;
		((page_header_t *) cur_page)->free = cur_rest;
		cur_page += PAGE_HEADER_SIZE;
	}
	t = cur_page;
	cur_page += size;
	/* The page table pointed to by a PTP must be
	* aligned on a boundary equal to the size of the page table.
	*   -- SRMMU.*/
	cur_rest -= size;
	page = (uint8_t *) (((unsigned long) cur_page) & ~MMU_PAGE_MASK);
	if (cur_rest == 0) {
		page -= PAGE_SIZE;
	}
#ifdef DEBUG
	printf("page %x; page->free %x; cur_rest %x; size %x, return %x\n", page, ((page_header_t *) page)->free, cur_rest, size, t);
#endif
	((page_header_t *) page)->free -= size;
	return (pmd_t *) t;
}

static unsigned long sizes[] = {
	LEON_CNR_CTX_NCTX,
	MMU_PGD_TABLE_SIZE,
	MMU_PMD_TABLE_SIZE,
	MMU_PTE_TABLE_SIZE
};

void table_free(unsigned long *table, int level) {
	uint8_t *page = (uint8_t *) (((unsigned long) table) & ~MMU_PAGE_MASK);
	int i;
	int size = sizes[level];
#ifdef DEBUG
	printf("table %x; level %x; page %x; size %x; is free %x\n", (unsigned long) table, level,
			(unsigned long) page, size, ((page_header_t *) page)->free + size);
#endif
	for (i = 0; i < sizes[level-1]; i++ ) {
		unsigned long t = *(table + i);
		if (t & MMU_ET_PTD != 0) {
#ifdef DEBUG
			printf("on %x to %x\n", table + i, (t & MMU_PTD_PMASK) << 4);
#endif
			table_free(((unsigned long *) ((t & MMU_PTD_PMASK) << 4)), level + 1);
		}
	}
	((page_header_t *) page)->free += size;
	if (((page_header_t *) page)->free == PAGE_SIZE - PAGE_HEADER_SIZE && page != cur_page) {
		mypage_free(page);
	}
}


static unsigned long levels[] = {
	0xffffffff,
	MMU_MTABLE_MASK + (1<<MMU_MTABLE_MASK_OFFSET),
	MMU_PTABLE_MASK + (1<<MMU_PTABLE_MASK_OFFSET),
	MMU_PAGE_MASK + 1,
	1
};

static unsigned long offsets[] = {
	32,
	MMU_GTABLE_MASK_OFFSET,
	MMU_MTABLE_MASK_OFFSET,
	MMU_PTABLE_MASK_OFFSET,
	0
};

static unsigned long masks[] = {
	0xffffffff,
	MMU_GTABLE_MASK,
	MMU_MTABLE_MASK,
	MMU_PTABLE_MASK,
	MMU_PAGE_MASK
};

static unsigned long *context[] = {NULL, NULL, NULL, NULL, NULL };

typedef void (*setter)(unsigned long ptd, unsigned long pte);

static setter setters[] = {
	NULL,
	&mmu_pgd_set,
	&mmu_pmd_set,
	&mmu_set_pte,
	NULL
};

void flags_translate(uint32_t *flags) {
	uint32_t fl = *flags;
	uint32_t ret = 0;
	if (fl & MMU_PAGE_CACHEABLE) {
		ret |= 1 << 7;
	}
	if (fl & MMU_PAGE_EXECUTEABLE) {
		ret |= MMU_PTE_RE << 2;
	}
	if (fl & MMU_PAGE_WRITEABLE) {
		ret |= MMU_PTE_RW << 2;
	}
	*flags = ret;
}

int mmu_map_region(mmu_ctx_t ctx, paddr_t phy_addr, vaddr_t virt_addr,
		size_t reg_size, uint32_t flags) {
	uint8_t cur_level = 1; /* pointer in levels array -- techincal thing*/
	uint32_t cur_offset;
	signed long treg_size;
	unsigned long pte;
	context[1] = (unsigned long *) (((*(((unsigned long *) cur_env->ctx + ctx)) & MMU_CTX_PMASK) << 4));

	/* assuming addresses aligned to page size */
	phy_addr &= ~MMU_PAGE_MASK;
	virt_addr &= ~MMU_PAGE_MASK;
	reg_size &= ~MMU_PAGE_MASK;
	treg_size = reg_size;
	/* translate general flags to sparc-specified */
	flags_translate(&flags);
	/* will map the best fitting area on each step
	 * will choose smaller area, while not found the best fitting */
	while ( treg_size > 0) {
		for (cur_level = 1; cur_level < 4; cur_level++) {
			cur_offset = ((virt_addr & masks[cur_level]) >> offsets[cur_level]);
#ifdef DEBUG
			printf("level %d; vaddr 0x%8x; paddr 0x%8x; context 0x%8x\n",
				cur_level, virt_addr, phy_addr, context[cur_level]);
#endif
			/* if mapping vaddr is alligned and if required size is pretty fit */
			if ((virt_addr % levels[cur_level] == 0) &&
			       (levels[cur_level] <= treg_size)) {
			    break;
			}
			/* else route - will do more fragmentation */
			if (*(context[cur_level] + cur_offset) == 0) {
				/* there is no middle page - creating */
				pmark_t *table = table_alloc(MMU_PMD_TABLE_SIZE);
				if (table == NULL) {
					return -1;
				}
				/* setting it's pointer to a prev level page */
				(*setters[cur_level])(context[cur_level] + cur_offset,
					(void *) table);
			}
			/* going to the next level map */
			pte = *((unsigned long *) context[cur_level] + cur_offset);
			context[cur_level + 1] = (unsigned long *) ((pte & MMU_PTD_PMASK) << 4);
		}
		/* we are on the best fitting level - creating mapping */
		mmu_set_pte(context[cur_level] + cur_offset,
				(((phy_addr >> 4) ) & MMU_PTE_PMASK) | flags | MMU_PTE_ET);
		pte = *((uint32_t *) context[cur_level] + cur_offset);
		/* reducing mapping area */
		treg_size -= levels[cur_level];
		phy_addr += levels[cur_level];
		virt_addr += levels[cur_level];
	}
	return 0;
}

void mmu_restore_env(mmu_env_t *env) {
	/* disable virtual mode*/
	mmu_off();

	/* change cur_env pointer */
	cur_env = env;

	mmu_set_ctable_ptr((unsigned long)env->ctx);
	mmu_set_context(cur_env->cur_ctx);
	mmu_flush_tlb_all();
	mmu_restore_status(&(cur_env->status));

	/* restore MMU mode */
	env->status ? mmu_on(): mmu_off();
}

void mmu_save_env(mmu_env_t *env) {
	mmu_save_status(&(cur_env->status));

	/* disable virtual mode*/
	mmu_off();

	/* save cur_env pointer */
	memcpy(env, cur_env, sizeof(mmu_env_t));
}

void mmu_set_env(mmu_env_t *env) {
	mmu_off();

	/* change cur_env pointer */
	cur_env = env;

	mmu_set_ctable_ptr((unsigned long)env->ctx);
	mmu_set_context(env->cur_ctx);
	mmu_flush_tlb_all();

	/* restore MMU mode */
	env->status ? mmu_on() : mmu_off();
}

uint8_t used_context[LEON_CNR_CTX_NCTX];

mmu_ctx_t mmu_create_context(void) {
	int i;
	for (i = 0; i < LEON_CNR_CTX_NCTX; i++) {
		if (!used_context[i])
			break;
	}
#ifdef DEBUG
	printf("create %d\n", i);
#endif
	if (i >= LEON_CNR_CTX_NCTX)
		return -1;
	mmu_ctxd_set(cur_env->ctx + i, table_alloc(MMU_PGD_TABLE_SIZE));
	used_context[i] = 1;
	return i;
}

void mmu_delete_context(mmu_ctx_t ctx) {
   used_context[ctx] = 0;
#ifdef DEBUG
   printf("delete %d\n",ctx);
#endif
   table_free(((unsigned long) *( cur_env->ctx + ctx) & MMU_CTX_PMASK) << 4, 1);
}

void switch_mm(mmu_ctx_t prev, mmu_ctx_t next) {
	cur_env->cur_ctx = next;
	mmu_set_context(next);
	mmu_flush_tlb_all();
}

/**
 * Module initializing function.
 * Setups system environment, but neither switch on virtual mode.
 */
static int mmu_init(void) {
	//TODO: set srmmu_fault trap handler
	/* alloc mem for tables ctx context table */
	__asm__(
		".section .data\n\t"
		".align %0\n\t"
		"sys_ctx: .skip %1\n\t"
		".align %1\n\t"
		"sys_pg0: .skip %1\n\t"
		".align %2\n\t"
		"sys_pm0: .skip %2\n\t"
		".align %3\n\t"
		"sys_pt0: .skip %3\n\t"
		".text\n"
		: : "i" (PAGE_SIZE),
		"i"(MMU_PGD_TABLE_SIZE) ,
		"i"(MMU_PMD_TABLE_SIZE) ,
		"i"(MMU_PTE_TABLE_SIZE)
	);
	(&system_env)->status = 0;
	(&system_env)->fault_addr = 0;
	(&system_env)->inst_fault_cnt = (&system_env)->data_fault_cnt = 0;
	(&system_env)->ctx = &sys_ctx;
	(&system_env)->cur_ctx = 0;
	mmu_ctxd_set((&system_env)->ctx, &sys_pg0);
	mmu_pgd_set(&sys_pg0, &sys_pm0);
	mmu_pmd_set(&sys_pm0, &sys_pt0);

	cur_env = &system_env;
	used_context[0] = 1;

	return 0;
}
