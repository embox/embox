/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.08.23
 */
#include <stdbool.h>
#include <stdint.h>

#include <asm/cache.h>
#include <debug/breakpoint.h>
#include <util/math.h>

#ifdef __riscv_c
static const bpt_instr_t bpt_instr = 0x9002; /* c.ebreak */
#else
static const bpt_instr_t bpt_instr = 0x00100073; /* ebreak */
#endif

static void riscv_sw_bpt_set(struct bpt *bpt) {
	bpt->orig = *(volatile bpt_instr_t *)bpt->addr;
	*(volatile bpt_instr_t *)bpt->addr = bpt_instr;

	riscv_icache_flush();
}

static void riscv_sw_bpt_remove(struct bpt *bpt) {
	*(volatile bpt_instr_t *)bpt->addr = bpt->orig;

	riscv_icache_flush();
}

static const struct bpt_info *riscv_sw_bpt_info(void) {
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
    .set = riscv_sw_bpt_set,
    .remove = riscv_sw_bpt_remove,
    .info = riscv_sw_bpt_info,
});
