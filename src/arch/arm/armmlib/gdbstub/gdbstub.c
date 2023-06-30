/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <hal/reg.h>
#include <debug/gdbstub.h>
#include <asm/arm_m_regs.h>

struct gdb_regs {
	uint32_t r[16];
	uint32_t psr;
};

static size_t max_hw_breakpoints;

static void (*gdb_entry)(struct gdb_regs *);

static int arm_insert_bpt(void *addr, int type) {
	int i;

	for (i = 0; i < max_hw_breakpoints; i++) {
		if ((REG32_LOAD(FP_COMP(i)) & ~0x1) == 0) {
			REG32_STORE(FP_COMP(i), (uint32_t)addr | 0x1);
			break;
		}
	}
	return (i != max_hw_breakpoints);
}

static int arm_remove_bpt(void *addr, int type) {
	int i;

	for (i = 0; i < max_hw_breakpoints; i++) {
		if ((REG32_LOAD(FP_COMP(i)) & ~0x1) == (uint32_t)addr) {
			REG32_STORE(FP_COMP(i), 0);
			break;
		}
	}
	return (i != max_hw_breakpoints);
}

static void arm_remove_all_bpts(void) {
	int i;

	for (i = 0; i < max_hw_breakpoints; i++) {
		REG32_CLEAR(FP_COMP(i), 0x1);
	}
}

static size_t arm_read_reg(struct gdb_regs *regs, unsigned regnum,
    void *regval) {
	size_t regsize;

	if (regnum < 16) {
		memcpy(regval, &regs->r[regnum], sizeof(uint32_t));
		regsize = sizeof(uint32_t);
	}
	else if (regnum == 25) {
		memcpy(regval, &regs->psr, sizeof(uint32_t));
		regsize = sizeof(uint32_t);
	}
	else {
		regsize = 0;
	}
	return regsize;
}

void arm_debug_monitor_handler(struct gdb_regs *regs) {
	gdb_entry(regs);
}

void gdb_arch_init(struct gdb_arch *arch) {
	arch->insert_bpt = arm_insert_bpt;
	arch->remove_bpt = arm_remove_bpt;
	arch->remove_all_bpts = arm_remove_all_bpts;
	arch->read_reg = arm_read_reg;
}

void gdb_arch_prepare(void (*entry)(struct gdb_regs *)) {
	uint32_t fp_ctrl;

	gdb_entry = entry;

	fp_ctrl = REG32_LOAD(FP_CTRL);
	max_hw_breakpoints = (((fp_ctrl >> 12) & 0x7) << 4) |
	                     ((fp_ctrl >> 4) & 0xf);

	REG32_STORE(FP_LAR, FP_LAR_UNLOCK_KEY);
	REG32_STORE(FP_CTRL, fp_ctrl | 0x3);
	REG32_ORIN(DCB_DEMCR, DCB_DEMCR_MON_EN);
}

void gdb_arch_cleanup(void) {
	arm_remove_all_bpts();

	REG32_STORE(FP_CTRL, (REG32_LOAD(FP_CTRL) & ~0x1) | 0x2);
	REG32_STORE(FP_LAR, 0);
	REG32_CLEAR(DCB_DEMCR, DCB_DEMCR_MON_EN);
}
