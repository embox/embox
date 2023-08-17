/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <arch/generic/dcache.h>
#include <arm/exception.h>
#include <asm/cp15.h>
#include <debug/gdbstub.h>

#include <framework/mod/options.h>

#define SW_BREAKPOINTS OPTION_GET(BOOLEAN, sw_breakpoints)
#define HW_BREAKPOINTS OPTION_GET(BOOLEAN, hw_breakpoints)
#define WATCHPOINTS    OPTION_GET(BOOLEAN, watchpoints)

extern bool arm_set_sw_bpt(void *addr);
extern bool arm_remove_sw_bpt(void *addr);
extern void arm_remove_sw_bpts(void);
extern void arm_activate_sw_bpts(void);
extern void arm_deactivate_sw_bpts(void);
extern void arm_enable_sw_bpts(void);
extern void arm_disable_sw_bpts(void);

extern bool arm_set_hw_bpt(void *addr);
extern bool arm_remove_hw_bpt(void *addr);
extern void arm_remove_hw_bpts(void);
extern void arm_activate_hw_bpts(void);
extern void arm_deactivate_hw_bpts(void);
extern void arm_enable_hw_bpts(void);
extern void arm_disable_hw_bpts(void);

static gdb_handler_t __gdb_handler;

int arm_debug_fault_handler(uint32_t nr, void *data) {
	assert(__gdb_handler);

	__gdb_handler((void *)&((excpt_context_t *)data)->ptregs);
	cp15_icache_inval();

	return 0;
}

void gdb_set_handler(gdb_handler_t handler) {
	__gdb_handler = handler;
}

bool gdb_set_bpt(int type, void *addr, int kind) {
	bool ret;

	switch (type) {
	case GDB_BPT_TYPE_SOFT:
		ret = arm_set_sw_bpt(addr);
		break;

	case GDB_BPT_TYPE_HARD:
		ret = arm_set_hw_bpt(addr);
		break;

	default:
		ret = false;
		break;
	}

	return ret;
}

bool gdb_remove_bpt(int type, void *addr, int kind) {
	bool ret;

	switch (type) {
	case GDB_BPT_TYPE_SOFT:
		ret = arm_remove_sw_bpt(addr);
		break;

	case GDB_BPT_TYPE_HARD:
		ret = arm_remove_hw_bpt(addr);
		break;

	default:
		ret = false;
		break;
	}

	return ret;
}

void gdb_remove_all_bpts(void) {
	arm_remove_sw_bpts();
	arm_remove_hw_bpts();
}

void gdb_activate_all_bpts(void) {
	arm_activate_sw_bpts();
	arm_activate_hw_bpts();
}

void gdb_deactivate_all_bpts(void) {
	arm_deactivate_sw_bpts();
	arm_deactivate_hw_bpts();
}

bool gdb_enable_bpts(int type) {
	bool ret;

	arm_inst_fault_table[DEBUG_FAULT] = arm_debug_fault_handler;
	arm_data_fault_table[DEBUG_FAULT] = arm_debug_fault_handler;

	switch (type) {
#if SW_BREAKPOINTS
	case GDB_BPT_TYPE_SOFT:
		arm_enable_sw_bpts();
		ret = true;
		break;
#endif
#if HW_BREAKPOINTS
	case GDB_BPT_TYPE_HARD:
		arm_enable_hw_bpts();
		ret = true;
		break;
#endif
	default:
		ret = false;
		break;
	}

	return ret;
}

void gdb_disable_bpts(int type) {
	arm_inst_fault_table[DEBUG_FAULT] = NULL;
	arm_data_fault_table[DEBUG_FAULT] = NULL;

	switch (type) {
	case GDB_BPT_TYPE_SOFT:
		arm_disable_sw_bpts();
		break;

	case GDB_BPT_TYPE_HARD:
		arm_disable_hw_bpts();
		break;

	default:
		break;
	}
}

size_t gdb_read_reg(struct gdb_regs *regs, int regnum, void *regval) {
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
