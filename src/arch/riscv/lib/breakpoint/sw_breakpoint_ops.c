/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 22.08.23
 */
#include <asm/cache.h>
#include <debug/breakpoint.h>

#ifdef __riscv_c
static const bpt_instr_t bpt_instr = 0x9002; /* c.ebreak */
#else
static const bpt_instr_t bpt_instr = 0x00100073; /* ebreak */
#endif

static void riscv_sw_bpt_set(struct bpt *bpt) {
	bpt->orig = *(bpt_instr_t *)bpt->addr;
	*(bpt_instr_t *)bpt->addr = bpt_instr;

	riscv_icache_flush();
}

static void riscv_sw_bpt_remove(struct bpt *bpt) {
	*(bpt_instr_t *)bpt->addr = bpt->orig;

	riscv_icache_flush();
}

SW_BREAKPOINT_OPS_DEF({
    .set = riscv_sw_bpt_set,
    .remove = riscv_sw_bpt_remove,
});
