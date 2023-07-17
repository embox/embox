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

#include <asm/regs.h>
#include <debug/gdbstub.h>

#define RISCV_MAX_TRIGGERS 32

struct gdb_regs {
	unsigned long r[31];
	unsigned long mstatus;
	unsigned long pc;
};

extern int (*riscv_excpt_table[16])(uint32_t nr, void *data);

static void *triggers[RISCV_MAX_TRIGGERS];
static unsigned long trigger_count;

static void (*gdb_entry)(struct gdb_regs *);

static bool riscv_insert_bpt(void *addr, int type) {
	unsigned long tdata1;
	unsigned long i;

	for (i = 0; i < trigger_count; i++) {
		if (triggers[i] == NULL) {
			triggers[i] = addr;
			write_csr(tselect, i);
			tdata1 = read_csr(tdata1);
			tdata1 |= MCONTROL_EXECUTE;
			tdata1 |= MCONTROL_M;
			tdata1 &= ~MCONTROL_MATCH;
			tdata1 &= ~MCONTROL_ACTION;
			tdata1 &= ~MCONTROL_SIZELO;
			write_csr(tdata1, tdata1);
			write_csr(tdata2, (unsigned long)addr);
			break;
		}
	}
	return (i != trigger_count);
}

static bool riscv_remove_bpt(void *addr, int type) {
	unsigned long i;

	for (i = 0; i < trigger_count; i++) {
		if (triggers[i] == addr) {
			triggers[i] = NULL;
			write_csr(tselect, i);
			write_csr(tdata1, 0);
			break;
		}
	}
	return (i != trigger_count);
}

static void riscv_remove_all_bpts(void) {
	unsigned long i;

	for (i = 0; i < trigger_count; i++) {
		if (triggers[i] != NULL) {
			triggers[i] = NULL;
			write_csr(tselect, i);
			write_csr(tdata1, 0);
			break;
		}
	}
}

static size_t riscv_read_reg(struct gdb_regs *regs, unsigned regnum,
    void *regval) {
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

static int riscv_handle_bpt_excpt(uint32_t nr, void *data) {
	((struct gdb_regs *)data)->r[33] -= 4;
	gdb_entry(data);
	return 0;
}

void gdb_arch_init(struct gdb_arch *arch) {
	arch->insert_bpt = riscv_insert_bpt;
	arch->remove_bpt = riscv_remove_bpt;
	arch->remove_all_bpts = riscv_remove_all_bpts;
	arch->read_reg = riscv_read_reg;
}

void gdb_arch_prepare(void (*entry)(struct gdb_regs *)) {
	unsigned long tselect;
	unsigned long tdata1;
	int type;

	gdb_entry = entry;
	riscv_excpt_table[EXC_BREAKPOINT] = riscv_handle_bpt_excpt;

	for (trigger_count = 0; trigger_count < RISCV_MAX_TRIGGERS;
	     trigger_count++) {
		write_csr(tselect, trigger_count);
		tselect = read_csr(tselect);
		tselect &= ~(1UL << (__riscv_xlen - 1));
		if (tselect != trigger_count) {
			break;
		}
		tdata1 = read_csr(tdata1);
		type = (tdata1 >> (__riscv_xlen - 4)) & 0xf;
		if (type == 0) {
			break;
		}
	}
}

void gdb_arch_cleanup(void) {
	gdb_entry = NULL;
	riscv_excpt_table[EXC_BREAKPOINT] = NULL;
}
