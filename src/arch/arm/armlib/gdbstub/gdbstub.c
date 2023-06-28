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

#include <asm/cp14.h>
#include <arm/exception.h>
#include <debug/gdbstub.h>

#define MAX_SW_BREAKPOINTS 32

struct gdb_regs {
	uint32_t r[16];
	uint32_t psr;
};

extern void dcache_flush(const void *p, size_t size);

static const uint32_t bpt_instr = 0xe1200070;

static void (*gdb_entry)(struct gdb_regs *);

static struct {
	void *addr;
	uint32_t orig;
} sw_breakpoints[MAX_SW_BREAKPOINTS];

static int arm_insert_bpt(void *addr, int type) {
	int i;

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

static int arm_remove_bpt(void *addr, int type) {
	int i;

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
	arm_inst_fault_table[DEBUG_FAULT] = arm_debug_fault_handler;
	gdb_entry = entry;
}

void gdb_arch_cleanup(void) {
	arm_remove_all_bpts();
	arm_inst_fault_table[DEBUG_FAULT] = NULL;
}
