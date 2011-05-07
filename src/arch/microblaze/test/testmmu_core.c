/**
 * @file
 *
 * @date 10.03.10
 * @author Anton Bondarev
 */

#include <types.h>
#include <hal/mm/mmu_core.h>
#include <hal/test/mmu_core.h>

/* buffer for all records in MMU table */
static __mmu_table_t utlb;

/* test MMU environment */
static mmu_env_t env;

mmu_env_t *testmmu_env(void) {
	(&env)->utlb_table = utlb;
	(&env)->fault_addr = 0;
	(&env)->inst_fault_cnt = (&env)->data_fault_cnt = 0;

	return &env;
}
