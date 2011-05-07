/**
 * @file
 *
 * @date 10.03.10
 * @author Anton Bondarev
 * @author Alexander Batyukov
 * @author Nikolay Korotky
 */

#include <types.h>
#include <hal/mm/mmu_core.h>
#include <hal/test/mmu_core.h>

static mmu_env_t env;

extern unsigned long pg0, pm0, pt0, ctx;

mmu_env_t *testmmu_env(void) {
	/* alloc mem for tables, context table */
	__asm__(
		".section .data\n\t"
		".align %0\n\t"
		"ctx: .skip %1\n\t"
		".align %1\n\t"
		"pg0: .skip %1\n\t"
		".align %2\n\t"
		"pm0: .skip %2\n\t"
		".align %3\n\t"
		"pt0: .skip %3\n\t"
		".text\n"
		: : "i" (LEON_CNR_CTX_NCTX),
		"i"(MMU_GTABLE_SIZE) ,
		"i"(MMU_MTABLE_SIZE) ,
		"i"(MMU_PTABLE_SIZE)
	);

	(&env)->ctx = (mmu_ctx_t *) &ctx;
	(&env)->cur_ctx = (mmu_ctx_t) 0;
	mmu_ctxd_set((&env)->ctx, &pg0);
	mmu_pgd_set(&pg0, &pm0);
	mmu_pmd_set(&pm0, &pt0);
	(&env)->fault_addr = 0;
	(&env)->status = 0;
	(&env)->inst_fault_cnt = (&env)->data_fault_cnt = 0;
	return &env;
}
