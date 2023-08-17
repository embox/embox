/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 13.05.23
 */
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include <debug/gdbstub.h>
#include <framework/mod/options.h>

#define SW_BREAKPOINTS OPTION_GET(BOOLEAN, sw_breakpoints)
#define HW_BREAKPOINTS OPTION_GET(BOOLEAN, hw_breakpoints)
#define WATCHPOINTS    OPTION_GET(BOOLEAN, watchpoints)

extern bool arm_m_set_hw_bpt(void *addr);
extern bool arm_m_remove_hw_bpt(void *addr);
extern void arm_m_remove_hw_bpts(void);
extern void arm_m_activate_hw_bpts(void);
extern void arm_m_deactivate_hw_bpts(void);
extern void arm_m_enable_hw_bpts(void);
extern void arm_m_disable_hw_bpts(void);

static gdb_handler_t __gdb_handler;

void arm_debug_monitor_handler(struct gdb_regs *regs) {
	assert(__gdb_handler);

	__gdb_handler(regs);
}

void gdb_set_handler(gdb_handler_t handler) {
	__gdb_handler = handler;
}

bool gdb_set_bpt(int type, void *addr, int kind) {
	bool ret;

	switch (type) {
	case GDB_BPT_TYPE_HARD:
		ret = arm_m_set_hw_bpt(addr);
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
	case GDB_BPT_TYPE_HARD:
		ret = arm_m_remove_hw_bpt(addr);
		break;

	default:
		ret = false;
		break;
	}

	return ret;
}

void gdb_remove_all_bpts(void) {
	arm_m_remove_hw_bpts();
}

void gdb_activate_all_bpts(void) {
	arm_m_activate_hw_bpts();
}

void gdb_deactivate_all_bpts(void) {
	arm_m_deactivate_hw_bpts();
}

bool gdb_enable_bpts(int type) {
	bool ret;

	switch (type) {
#if HW_BREAKPOINTS
	case GDB_BPT_TYPE_HARD:
		arm_m_enable_hw_bpts();
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
	switch (type) {
	case GDB_BPT_TYPE_HARD:
		arm_m_disable_hw_bpts();
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
