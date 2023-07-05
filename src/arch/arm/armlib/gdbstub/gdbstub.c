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
#include <stdbool.h>

#include <asm/cp14.h>
#include <arm/exception.h>
#include <debug/gdbstub.h>

#define MAX_SW_BREAKPOINTS 32

#define DBGBCR_EN      (1U << 0U)
#define DBGBCR_PMC_ANY (0b11U << 1U)
#define DBGBCR_BAS_ANY (0b1111U << 5U)

#define DBGDSCR_ITREN            (1U << 13U)
#define DBGDSCR_MDBGEN           (1U << 15U)
#define DBGDSCR_EXTDCCMODE_MASK  (3U << 20U)
#define DBGDSCR_EXTDCCMODE_STALL (1U << 20U)

struct gdb_regs {
	uint32_t r[16];
	uint32_t psr;
};

extern void dcache_flush(const void *p, size_t size);

static const uint32_t bpt_instr = 0xe1200070;

static size_t max_hw_breakpoints;

static void *hw_breakpoints[16];

static struct {
	void *addr;
	uint32_t orig;
} sw_breakpoints[MAX_SW_BREAKPOINTS];

static void (*gdb_entry)(struct gdb_regs *);

static void set_hw_breakpoint_regs(uint32_t dbgbvr, uint32_t dbgbcr, int n) {
	switch (n) {
	case 0:
		CP14_STORE(DBGBVR0, dbgbvr);
		CP14_STORE(DBGBCR0, dbgbcr);
		break;
	case 1:
		CP14_STORE(DBGBVR1, dbgbvr);
		CP14_STORE(DBGBCR1, dbgbcr);
		break;
	case 2:
		CP14_STORE(DBGBVR2, dbgbvr);
		CP14_STORE(DBGBCR2, dbgbcr);
		break;
	case 3:
		CP14_STORE(DBGBVR3, dbgbvr);
		CP14_STORE(DBGBCR3, dbgbcr);
		break;
	case 4:
		CP14_STORE(DBGBVR4, dbgbvr);
		CP14_STORE(DBGBCR4, dbgbcr);
		break;
	case 5:
		CP14_STORE(DBGBVR5, dbgbvr);
		CP14_STORE(DBGBCR5, dbgbcr);
		break;
	case 6:
		CP14_STORE(DBGBVR6, dbgbvr);
		CP14_STORE(DBGBCR6, dbgbcr);
		break;
	case 7:
		CP14_STORE(DBGBVR7, dbgbvr);
		CP14_STORE(DBGBCR7, dbgbcr);
		break;
	case 8:
		CP14_STORE(DBGBVR8, dbgbvr);
		CP14_STORE(DBGBCR8, dbgbcr);
		break;
	case 9:
		CP14_STORE(DBGBVR9, dbgbvr);
		CP14_STORE(DBGBCR9, dbgbcr);
		break;
	case 10:
		CP14_STORE(DBGBVR10, dbgbvr);
		CP14_STORE(DBGBCR10, dbgbcr);
		break;
	case 11:
		CP14_STORE(DBGBVR11, dbgbvr);
		CP14_STORE(DBGBCR11, dbgbcr);
		break;
	case 12:
		CP14_STORE(DBGBVR12, dbgbvr);
		CP14_STORE(DBGBCR12, dbgbcr);
		break;
	case 13:
		CP14_STORE(DBGBVR13, dbgbvr);
		CP14_STORE(DBGBCR13, dbgbcr);
		break;
	case 14:
		CP14_STORE(DBGBVR14, dbgbvr);
		CP14_STORE(DBGBCR14, dbgbcr);
		break;
	case 15:
		CP14_STORE(DBGBVR15, dbgbvr);
		CP14_STORE(DBGBCR15, dbgbcr);
		break;
	default:
		break;
	}
}

static bool arm_insert_bpt(void *addr, int type) {
	int i;

	for (i = 0; i < max_hw_breakpoints; i++) {
		if (hw_breakpoints[i] == NULL) {
			hw_breakpoints[i] = addr;
			set_hw_breakpoint_regs((uint32_t)addr,
			    DBGBCR_EN | DBGBCR_PMC_ANY | DBGBCR_BAS_ANY, i);
			break;
		}
	}

	if (i != max_hw_breakpoints) {
		return true;
	}

	for (i = 0; i < MAX_SW_BREAKPOINTS; i++) {
		if (sw_breakpoints[i].addr == NULL) {
			memcpy(&sw_breakpoints[i].orig, addr, sizeof(bpt_instr));
			memcpy(addr, &bpt_instr, sizeof(bpt_instr));
			dcache_flush(addr, sizeof(bpt_instr));
			sw_breakpoints[i].addr = addr;
			break;
		}
	}

	return (i != MAX_SW_BREAKPOINTS);
}

static bool arm_remove_bpt(void *addr, int type) {
	int i;

	for (i = 0; i < max_hw_breakpoints; i++) {
		if (hw_breakpoints[i] == addr) {
			hw_breakpoints[i] = NULL;
			set_hw_breakpoint_regs(0, 0, i);
			break;
		}
	}

	if (i != max_hw_breakpoints) {
		return true;
	}

	for (i = 0; i < MAX_SW_BREAKPOINTS; i++) {
		if (sw_breakpoints[i].addr == addr) {
			memcpy(addr, &sw_breakpoints[i].orig, sizeof(bpt_instr));
			dcache_flush(addr, sizeof(bpt_instr));
			sw_breakpoints[i].addr = NULL;
			break;
		}
	}

	return (i != MAX_SW_BREAKPOINTS);
}

static void arm_remove_all_bpts(void) {
	int i;

	for (i = 0; i < max_hw_breakpoints; i++) {
		if (hw_breakpoints[i] != NULL) {
			hw_breakpoints[i] = NULL;
			set_hw_breakpoint_regs(0, 0, i);
		}
	}

	for (i = 0; i < MAX_SW_BREAKPOINTS; i++) {
		if (sw_breakpoints[i].addr != NULL) {
			memcpy(sw_breakpoints[i].addr, &sw_breakpoints[i].orig,
			    sizeof(bpt_instr));
			dcache_flush(sw_breakpoints[i].addr, sizeof(bpt_instr));
			sw_breakpoints[i].addr = NULL;
		}
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

int arm_debug_fault_handler(uint32_t nr, void *data) {
	gdb_entry((struct gdb_regs *)&((excpt_context_t *)data)->ptregs);
	return 0;
}

void gdb_arch_init(struct gdb_arch *arch) {
	arch->insert_bpt = arm_insert_bpt;
	arch->remove_bpt = arm_remove_bpt;
	arch->remove_all_bpts = arm_remove_all_bpts;
	arch->read_reg = arm_read_reg;
}

void gdb_arch_prepare(void (*entry)(struct gdb_regs *)) {
	uint32_t dbgdscr;

	gdb_entry = entry;
	arm_inst_fault_table[DEBUG_FAULT] = arm_debug_fault_handler;

	max_hw_breakpoints = ((CP14_LOAD(DBGDIDR) >> 24) & 0b1111) + 1;

	dbgdscr = CP14_LOAD(DBGDSCRext);

	dbgdscr &= ~DBGDSCR_EXTDCCMODE_MASK;
	dbgdscr |= DBGDSCR_MDBGEN | DBGDSCR_ITREN | DBGDSCR_EXTDCCMODE_STALL;

	CP14_STORE(DBGDSCRext, dbgdscr);
}

void gdb_arch_cleanup(void) {
	gdb_entry = NULL;
	arm_inst_fault_table[DEBUG_FAULT] = NULL;

	CP14_CLEAR(DBGDSCRext, DBGDSCR_MDBGEN | DBGDSCR_ITREN);
}
