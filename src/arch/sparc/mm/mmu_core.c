/**
 * @file
 * @brief Implements MMU core interface @link #mmu_core.h @endllink
 *        for SPARC architecture.
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 * @author Nikolay Korotky
 */
#include <types.h>
#include <string.h>
#include <hal/mm/mmu_core.h>
#include <asm/asi.h>
#include <embox/unit.h>

/* main system MMU environment*/
static mmu_env_t system_env;
static mmu_env_t *cur_env;

extern pgd_t sys_pg0;
extern pmd_t sys_pm0;
extern pte_t sys_pt0;
extern ctxd_t sys_ctx;

/* Setup module starting function */
EMBOX_UNIT_INIT(mmu_init)
;

void mmu_save_status(uint32_t *status) {

}

void mmu_restore_status(uint32_t *status) {

}

void mmu_save_table(uint32_t *status) {

}

void mmu_restore_table(uint32_t *status) {

}

void mmu_on(void) {
	unsigned long val;
	val = mmu_get_mmureg(LEON_CNR_CTRL);
	val |= 0x1;
	mmu_set_context(0);
	mmu_set_mmureg(LEON_CNR_CTRL, val);
	mmu_flush_cache_all();
}

void mmu_off(void) {
	unsigned long val;
	val = mmu_get_mmureg(LEON_CNR_CTRL);
	val &= ~0x1;
	mmu_set_mmureg(LEON_CNR_CTRL, val);
}

int mmu_map_region(mmu_ctx_t ctx, paddr_t phy_addr, vaddr_t virt_addr,
		size_t reg_size, uint32_t flags) {
	flags = flags << 2;
	pgd_t *g0 = (pgd_t *) cur_env->pg0;
	pmd_t *m0 = (pmd_t *) cur_env->pm0;
	pte_t *p0 = (pte_t *) cur_env->pt0;
	/* align on page size */
	reg_size &= ~MMU_PAGE_MASK;
	phy_addr &= ~MMU_PAGE_MASK;
	virt_addr &= ~MMU_PAGE_MASK;
	if (flags)
		flags |= MMU_PTE_ET;
	if (0 == (reg_size & (~MMU_GTABLE_MASK))) {
		/* alloc 16Mb */
		/* g0 must set in mmu_probe_init */
		phy_addr &= MMU_GTABLE_MASK;
		virt_addr &= MMU_GTABLE_MASK;
		mmu_set_pte(g0 + (virt_addr >> MMU_GTABLE_MASK_OFFSET),
				(phy_addr >> 4) | flags);
		return 0;
	}
	if (0 == (reg_size & (~(MMU_GTABLE_MASK | MMU_MTABLE_MASK)))) {
		/* alloc 256kb */
		if (0 == *(g0 + ((virt_addr & MMU_GTABLE_MASK)
					>> (MMU_GTABLE_MASK_OFFSET )))) {
			/* if middle table not set */
			mmu_pgd_set(g0 + ((virt_addr & MMU_GTABLE_MASK)
					>> MMU_GTABLE_MASK_OFFSET), m0);
		}
		//paddr &= MMU_MTABLE_MASK;
		virt_addr &= MMU_MTABLE_MASK;
		mmu_set_pte(m0 + (virt_addr >> MMU_MTABLE_MASK_OFFSET),
				(phy_addr >> 4) | flags);
	}
	return -1;
}

void mmu_restore_env(mmu_env_t *env) {
	/* disable virtual mode*/
	mmu_off();

	/* change cur_env pointer */
	cur_env = env;

	mmu_set_ctable_ptr(env->ctx);
	mmu_ctxd_set(env->ctx, env->pg0);
	mmu_flush_tlb_all();

	/* restore MMU mode */
	env->status ? mmu_on(): mmu_off();
}

void mmu_save_env(mmu_env_t *env) {
	mmu_save_status(&(cur_env->status));

	/* disable virtual mode*/
	mmu_off();

	/* save cur_env pointer */
	memcpy(env, cur_env, sizeof(mmu_env_t));

//	mmu_save_table(env->pg0);
//	mmu_save_table(env->pm0);
//	mmu_save_table(env->pt0);
}

void mmu_set_env(mmu_env_t *env) {
	mmu_off();

	/* change cur_env pointer */
	cur_env = env;

	mmu_set_ctable_ptr((unsigned long)env->ctx);
	mmu_ctxd_set(env->ctx, env->pg0);
	mmu_flush_tlb_all();

//	mmu_restore_table(env->pg0);
//	mmu_restore_table(env->pm0);
//	mmu_restore_table(env->pt0);

	/* restore MMU mode */
	env->status ? mmu_on() : mmu_off();
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
	(&system_env)->pg0 = &sys_pg0;
	(&system_env)->pm0 = &sys_pm0;
	(&system_env)->pt0 = &sys_pt0;
	(&system_env)->ctx = &sys_ctx;

	cur_env = &system_env;

	return 0;
}
