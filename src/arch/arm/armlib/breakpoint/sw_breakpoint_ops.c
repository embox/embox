/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.08.23
 */
#include <asm/cp15.h>
#include <debug/breakpoint.h>
#include <hal/cache.h>

static const bpt_instr_t bpt_instr = 0xe1200070;

static void arm_sw_bpt_set(struct bpt *bpt) {
	bpt->orig = *(bpt_instr_t *)bpt->addr;
	*(bpt_instr_t *)bpt->addr = bpt_instr;

	dcache_flush(bpt->addr, sizeof(bpt_instr_t));
	cp15_icache_inval();
}

static void arm_sw_bpt_remove(struct bpt *bpt) {
	*(bpt_instr_t *)bpt->addr = bpt->orig;

	dcache_flush(bpt->addr, sizeof(bpt_instr_t));
	cp15_icache_inval();
}

SW_BREAKPOINT_OPS_DEF({
    .set = arm_sw_bpt_set,
    .remove = arm_sw_bpt_remove,
});
