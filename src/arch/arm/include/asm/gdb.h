/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 14.01.23
 */
#ifndef ASM_GDB_H_
#define ASM_GDB_H_

#if !GDBSTUB_IMPL
#error "Do not include this file directly!"
#endif

#include <stddef.h>
#include <stdint.h>

#define PACKET_SIZE 0x80
#define FEATURE_STR "l<target><architecture>arm</architecture></target>"

#define gdb_reg_sz(n) sizeof(uint32_t)

#ifdef __thumb2__
#include <hal/reg.h>
#include <asm/arm_m_regs.h>

struct breakpoint {};

static inline void gdb_set_breakpoint(void *addr, struct breakpoint *bpt) {
	REG32_STORE(bpt, (uint32_t)addr | 0x1);
}

static inline void gdb_remove_breakpoint(struct breakpoint *bpt) {
	REG32_STORE(bpt, 0);
}

static inline void gdb_remove_all_breakpoints(struct breakpoint *bpts) {
	uint32_t fp_ctrl;
	size_t bpt_count;
	int i;

	fp_ctrl = REG32_LOAD(FP_CTRL);
	bpt_count = (((fp_ctrl >> 12) & 0x7) << 4) | ((fp_ctrl >> 4) & 0xf);

	for (i = 0; i < bpt_count; i++) {
		REG32_STORE(FP_COMP(i), 0);
	}
}

static inline struct breakpoint *
gdb_find_breakpoint(void *addr, struct breakpoint *bpts) {
	uint32_t fp_ctrl;
	size_t bpt_count;
	int i;

	fp_ctrl = REG32_LOAD(FP_CTRL);
	bpt_count = (((fp_ctrl >> 12) & 0x7) << 4) | ((fp_ctrl >> 4) & 0xf);

	if (!(fp_ctrl & 0x1)) {
		REG32_STORE(FP_LAR, FP_LAR_UNLOCK_KEY);
		REG32_STORE(FP_CTRL, fp_ctrl | 0x3);
		REG32_STORE(DCB_DEMCR, REG32_LOAD(DCB_DEMCR) | DCB_DEMCR_MON_EN);
	}

	for (i = 0; i < bpt_count; i++) {
		if ((uint32_t)addr == (REG32_LOAD(FP_COMP(i)) & ~0x1)) {
			break;
		}
	}
	return (i != bpt_count) ? (void *)FP_COMP(i) : NULL;
}

static inline void *gdb_reg(void *ctx, size_t n) {
	return (n < 13)               ? ((uint32_t *)(ctx) + n) :
	       (n == 13)              ? ((uint32_t *)(ctx) + 14) :
	       (n == 14) || (n == 25) ? ((uint32_t *)(ctx) + 15) :
	       (n == 15)              ? ((uint32_t *)(ctx) + 13) :
                                    NULL;
}
#else
struct breakpoint {
	void *addr;
	uint32_t orig;
};

extern void dcache_flush(const void *p, size_t size);

static inline void gdb_set_breakpoint(void *addr, struct breakpoint *bpt) {
	bpt->addr = addr;
	bpt->orig = *(uint32_t *)addr;
	*(uint32_t *)addr = 0xe1200070;
	dcache_flush(addr, sizeof(uint32_t));
}

static inline void gdb_remove_breakpoint(struct breakpoint *bpt) {
	*(uint32_t *)bpt->addr = bpt->orig;
	dcache_flush(bpt->addr, sizeof(uint32_t));
	bpt->addr = NULL;
}

static inline void gdb_remove_all_breakpoints(struct breakpoint *bpts) {
	int i;

	for (i = 0; i < MAX_BPTS; i++) {
		if (bpts[i].addr) {
			gdb_remove_breakpoint(&bpts[i]);
		}
	}
}

static inline struct breakpoint *
gdb_find_breakpoint(void *addr, struct breakpoint *bpts) {
	int i;

	for (i = 0; i < MAX_BPTS; i++) {
		if (bpts[i].addr == addr) {
			break;
		}
	}
	return (i != MAX_BPTS) ? &bpts[i] : NULL;
}

static inline void *gdb_reg(void *ctx, size_t n) {
	return (n < 16)  ? ((uint32_t *)(ctx) + n) :
	       (n == 25) ? ((uint32_t *)(ctx) + 16) :
                       NULL;
}
#endif /* !__thumb2__ */

#endif /* ASM_GDB_H_ */
