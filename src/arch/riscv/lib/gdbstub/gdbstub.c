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

#include <asm/regs.h>
#include <debug/gdbstub.h>
#include <riscv/exception.h>

#include <framework/mod/options.h>

#define SW_BREAKPOINTS OPTION_GET(BOOLEAN, sw_breakpoints)
#define HW_BREAKPOINTS OPTION_GET(BOOLEAN, hw_breakpoints)
#define WATCHPOINTS    OPTION_GET(BOOLEAN, watchpoints)

extern bool riscv_set_sw_bpt(void *addr);
extern bool riscv_remove_sw_bpt(void *addr);
extern void riscv_remove_sw_bpts(void);
extern void riscv_activate_sw_bpts(void);
extern void riscv_deactivate_sw_bpts(void);
extern void riscv_enable_sw_bpts(void);
extern void riscv_disable_sw_bpts(void);

extern bool riscv_set_hw_bpt(void *addr);
extern bool riscv_remove_hw_bpt(void *addr);
extern void riscv_remove_hw_bpts(void);
extern void riscv_activate_hw_bpts(void);
extern void riscv_deactivate_hw_bpts(void);
extern void riscv_enable_hw_bpts(void);
extern void riscv_disable_hw_bpts(void);

static gdb_handler_t __gdb_handler;

int riscv_bpt_excpt_handler(long unsigned nr, void *data) {
	assert(__gdb_handler);

	__gdb_handler((void *)&((excpt_context_t *)data)->ptregs);

	return 0;
}

void gdb_set_handler(gdb_handler_t handler) {
	__gdb_handler = handler;
}

bool gdb_set_bpt(int type, void *addr, int kind) {
	bool ret;

	switch (type) {
	case GDB_BPT_TYPE_SOFT:
		ret = riscv_set_sw_bpt(addr);
		break;

	case GDB_BPT_TYPE_HARD:
		ret = riscv_set_hw_bpt(addr);
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
		ret = riscv_remove_sw_bpt(addr);
		break;

	case GDB_BPT_TYPE_HARD:
		ret = riscv_remove_hw_bpt(addr);
		break;

	default:
		ret = false;
		break;
	}

	return ret;
}

void gdb_remove_all_bpts(void) {
	riscv_remove_sw_bpts();
	riscv_remove_hw_bpts();
}

void gdb_activate_all_bpts(void) {
	riscv_activate_sw_bpts();
	riscv_activate_hw_bpts();
}

void gdb_deactivate_all_bpts(void) {
	riscv_deactivate_sw_bpts();
	riscv_deactivate_hw_bpts();
}

bool gdb_enable_bpts(int type) {
	bool ret;

	riscv_excpt_table[EXC_BREAKPOINT] = riscv_bpt_excpt_handler;

	switch (type) {
#if SW_BREAKPOINTS
	case GDB_BPT_TYPE_SOFT:
		riscv_enable_sw_bpts();
		ret = true;
		break;
#endif
#if HW_BREAKPOINTS
	case GDB_BPT_TYPE_HARD:
		riscv_enable_hw_bpts();
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
	riscv_excpt_table[EXC_BREAKPOINT] = NULL;

	switch (type) {
	case GDB_BPT_TYPE_SOFT:
		riscv_disable_sw_bpts();
		break;

	case GDB_BPT_TYPE_HARD:
		riscv_disable_hw_bpts();
		break;

	default:
		break;
	}
}

size_t gdb_read_reg(struct gdb_regs *regs, int regnum, void *regval) {
	size_t regsize;

	if (regnum == 0) {
		memset(regval, 0, sizeof(long));
		regsize = sizeof(long);
	}
	else if (regnum < 32) {
		memcpy(regval, &regs->r[regnum - 1], sizeof(long));
		regsize = sizeof(long);
	}
	else if (regnum == 32) {
		memcpy(regval, &regs->pc, sizeof(long));
		regsize = sizeof(long);
	}
	else {
		regsize = 0;
	}

	return regsize;
}
