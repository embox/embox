/**
 * @file
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 * @author Alexander Batyukov
 * @author Nikolay Korotky
 */

#include <types.h>
#include <hal/mm/mmu_core.h>
#include <test/testmmu_core.h>

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
		: : "i" (PAGE_SIZE),
		"i"(MMU_PGD_TABLE_SIZE) ,
		"i"(MMU_PMD_TABLE_SIZE) ,
		"i"(MMU_PTE_TABLE_SIZE)
	);

	(&env)->pg0 = &pg0;
	(&env)->pm0 = &pm0;
	(&env)->pt0 = &pt0;
	(&env)->ctx = &ctx;
	(&env)->fault_addr = 0;
	(&env)->status = 0;
	(&env)->inst_fault_cnt = (&env)->data_fault_cnt = 0;
	return &env;
}
