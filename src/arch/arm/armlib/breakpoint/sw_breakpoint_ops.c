/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.08.23
 */
#include <stdbool.h>
#include <stdint.h>

#include <debug/breakpoint.h>
#include <hal/cache.h>
#include <hal/mem_barriers.h>
#include <hal/reg.h>
#include <util/math.h>

static const bpt_instr_t bpt_instr = 0xe1200070;

static void arm_sw_bpt_set(struct bpt *bpt) {
	bpt->orig = *(bpt_instr_t *)bpt->addr;
	*(bpt_instr_t *)bpt->addr = bpt_instr;

	dcache_flush(bpt->addr, sizeof(bpt_instr_t));

	/* Invalidate instruction cache */
	ARCH_REG_STORE(ICIALLU, 0);
	isb();
}

static void arm_sw_bpt_remove(struct bpt *bpt) {
	*(bpt_instr_t *)bpt->addr = bpt->orig;

	dcache_flush(bpt->addr, sizeof(bpt_instr_t));

	/* Invalidate instruction cache */
	ARCH_REG_STORE(ICIALLU, 0);
	isb();
}

static const struct bpt_info *arm_sw_bpt_info(void) {
	extern char _text_vma, _text_len, _ram_base, _ram_size;

	static bool initialized = false;
	static struct bpt_info info;

	if (!initialized) {
		info.count = -1;
		info.start = max((void *)&_text_vma, (void *)&_ram_base);
		info.end = min((void *)&_text_vma + (uintptr_t)&_text_len,
		    (void *)&_ram_base + (uintptr_t)&_ram_size);
		initialized = true;
	}

	return &info;
}

SW_BREAKPOINT_OPS_DEF({
    .set = arm_sw_bpt_set,
    .remove = arm_sw_bpt_remove,
    .info = arm_sw_bpt_info,
});
