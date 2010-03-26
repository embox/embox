/**
 * @file
 * @brief Implements MMU core interface @link #mmu_core.h @endllink
 *        for SPARC architecture.
 *
 * @date 10.03.2010
 * @author Anton Bondarev
 */
#include <types.h>
#include <hal/mm/mmu_core.h>
#include <asm/asi.h>

/* main system MMU environment*/
static mmu_env_t system_env;
static mmu_env_t *cur_env;

void mmu_save_status(uint32_t *status) {

}

void mmu_restore_status(uint32_t *status) {

}

void mmu_save_table(uint32_t *status) {

}

void mmu_restore_table(uint32_t *status) {

}

int mmu_enable(uint32_t *table) {
	return 0;
}

void mmu_on(void) {
	printf("mmu_on\n");
	unsigned long val;
	val = mmu_get_mmureg(LEON_CNR_CTRL);
	val |= 0x1;
	mmu_set_mmureg(LEON_CNR_CTRL, val);
}

void mmu_off(void) {
	unsigned long val;
	val = mmu_get_mmureg(LEON_CNR_CTRL);
	val &= ~0x1;
	mmu_set_mmureg(LEON_CNR_CTRL, val);
}

int mmu_map_region(uint32_t phy_addr, uint32_t virt_addr,
		size_t reg_size, uint32_t flags) {
	return -1;
}

void mmu_restore_env(mmu_env_t *env) {
	/* disable virtual mode*/
	mmu_off();

	/* change cur_env pointer */
	cur_env = env;

	/* restore MMU mode */
	env->status ? mmu_on(): mmu_off();
}

void mmu_save_env(mmu_env_t *env) {
	/* disable virtual mode*/
	mmu_off();

	/* save cur_env pointer */
	env = cur_env;

	/* restore MMU mode */
	env->status ? mmu_on(): mmu_off();
}

void mmu_set_env(mmu_env_t *env) {

}
