/**
 * @file
 * @brief Implements MMU core interface @link #mmu_core.h @endllink
 *        for SPARC architecture.
 *
 * @date 18.04.10
 * @author Anton Bondarev
 * @author Nikolay Korotky
 * @author Anton Kozlov
 */

#include <types.h>
#include <string.h>

#include <hal/mm/mmu_types.h>
#include <hal/mm/mmu_core.h>
#include <asm/hal/mm/mmu_page.h>

#include <asm/asi.h>
#include <embox/unit.h>
#include <kernel/mm/virt_mem/table_alloc.h>
#include <kernel/mm/pagealloc/opallocator.h>

/* main system MMU environment*/
static mmu_env_t system_env;
/*static*/ mmu_env_t *cur_env;

extern mmu_pgd_t sys_pg0;
extern mmu_pmd_t sys_pm0;
extern mmu_pte_t sys_pt0;
extern mmu_ctx_t sys_ctx;

/* Setup module starting function */
EMBOX_UNIT_INIT(mmu_init);

void mmu_save_status(uint32_t *status) {
	__asm__ __volatile__(
		"sta %0, [%1] %2\n\t"
		:
		: "r"(status), "r"(LEON_CNR_CTRL), "i"(ASI_M_MMUREGS)
		: "memory"
	);
}

void mmu_restore_status(uint32_t *status) {
	__asm__ __volatile__(
		"lda [%1] %2, %0\n\t"
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
	mmu_set_ctable_ptr((unsigned long)cur_env->ctx);
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

mmu_pgd_t * mmu_get_root(mmu_ctx_t ctx) {
	return (mmu_pgd_t *) (((*(((unsigned long *) cur_env->ctx + ctx)) &
					    MMU_CTX_PMASK) << 4));
}

int mmu_valid_entry(mmu_pte_t pte) {
	return (((unsigned int) mmu_is_pte) & MMU_PTE_ET) |
			    (((unsigned int) pte) & MMU_ET_PTD);
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

static uint8_t used_context[LEON_CNR_CTX_NCTX];

mmu_ctx_t mmu_create_context(void) {
	int i;
	for (i = 0; i < LEON_CNR_CTX_NCTX; i++) {
		if (!used_context[i])
			break;
	}
	LOG_DEBUG("create %d\n", i);
	if (i >= LEON_CNR_CTX_NCTX)
		return -1;
	mmu_ctxd_set(cur_env->ctx + i,
			(mmu_pgd_t *) mmu_table_alloc(MMU_GTABLE_SIZE));
	used_context[i] = 1;
	return i;
}

void mmu_delete_context(mmu_ctx_t ctx) {
	used_context[ctx] = 0;
	LOG_DEBUG("delete %d\n",ctx);
	mmu_table_free((unsigned long *) (((unsigned long) (*(cur_env->ctx + ctx)) & MMU_CTX_PMASK) << 4), 1);
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
		: : "i" (LEON_CNR_CTX_NCTX),
		"i"(MMU_GTABLE_SIZE) ,
		"i"(MMU_MTABLE_SIZE) ,
		"i"(MMU_PTABLE_SIZE)
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
