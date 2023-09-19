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

#include <hal/cache.h>
#include <asm/ptrace.h>
#include <debug/gdbstub.h>
#include <asm/hal/env/traps_core.h>

#define MAX_SW_BREAKPOINTS 32

struct gdb_regs {
	uint32_t r[10];
};

extern __trap_handler __exception_table[0x20];

static const uint8_t bpt_instr = 0xcc;

static void (*gdb_entry)(struct gdb_regs *);

static struct {
	void *addr;
	uint8_t orig;
} sw_breakpoints[MAX_SW_BREAKPOINTS];

static bool x86_insert_bpt(void *addr, int type) {
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

static bool x86_remove_bpt(void *addr, int type) {
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

static void x86_remove_all_bpts(void) {
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

static size_t x86_read_reg(struct gdb_regs *regs, unsigned regnum,
    void *regval) {
	size_t regsize;

	if (regnum < 10) {
		memcpy(regval, &regs->r[regnum], sizeof(uint32_t));
		regsize = sizeof(uint32_t);
	}
	else {
		regsize = 0;
	}
	return regsize;
}

static int x86_handle_bpt_excpt(uint32_t nr, void *data) {
	struct pt_regs *ptregs;
	struct gdb_regs regs;

	ptregs = (struct pt_regs *)data;

	regs.r[0] = ptregs->eax;
	regs.r[1] = ptregs->ecx;
	regs.r[2] = ptregs->edx;
	regs.r[3] = ptregs->ebx;
	regs.r[4] = ptregs->esp;
	regs.r[5] = ptregs->ebp;
	regs.r[6] = ptregs->esi;
	regs.r[7] = ptregs->edi;
	regs.r[8] = ptregs->eip -= sizeof(uint8_t);
	regs.r[9] = ptregs->eflags;

	gdb_entry(&regs);

	return 0;
}

void gdb_arch_init(struct gdb_arch *arch) {
	arch->insert_bpt = x86_insert_bpt;
	arch->remove_bpt = x86_remove_bpt;
	arch->remove_all_bpts = x86_remove_all_bpts;
	arch->read_reg = x86_read_reg;
}

void gdb_arch_prepare(void (*entry)(struct gdb_regs *)) {
	gdb_entry = entry;
	__exception_table[0x3] = x86_handle_bpt_excpt;
}

void gdb_arch_cleanup(void) {
	__exception_table[0x3] = NULL;
}
