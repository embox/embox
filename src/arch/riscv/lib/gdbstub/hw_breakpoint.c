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

#define MAX_HW_BPTS 32

static bool hw_bpts_enabled = false;
static bool hw_bpts_activated = false;

static void *hw_bpts[MAX_HW_BPTS];
static size_t hw_bpt_count;

bool riscv_set_hw_bpt(void *addr) {
	unsigned long tdata1;
	unsigned long i;

	if (!hw_bpts_enabled) {
		return false;
	}

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] == NULL) {
			hw_bpts[i] = addr;
			if (hw_bpts_activated) {
				write_csr(tselect, i);
				tdata1 = read_csr(tdata1);
				tdata1 |= MCONTROL_EXECUTE;
				tdata1 |= MCONTROL_M;
				tdata1 &= ~MCONTROL_MATCH;
				tdata1 &= ~MCONTROL_ACTION;
				tdata1 &= ~MCONTROL_SIZELO;
				write_csr(tdata1, tdata1);
				write_csr(tdata2, (unsigned long)addr);
			}
			break;
		}
	}

	return (i != hw_bpt_count);
}

bool riscv_remove_hw_bpt(void *addr) {
	unsigned long i;

	if (!hw_bpts_enabled) {
		return false;
	}

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] == addr) {
			hw_bpts[i] = NULL;
			if (hw_bpts_activated) {
				write_csr(tselect, i);
				write_csr(tdata1, 0);
			}
			break;
		}
	}

	return (i != hw_bpt_count);
}

void riscv_remove_hw_bpts(void) {
	unsigned long i;

	if (!hw_bpts_enabled) {
		return;
	}

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] != NULL) {
			hw_bpts[i] = NULL;
			if (hw_bpts_activated) {
				write_csr(tselect, i);
				write_csr(tdata1, 0);
			}
		}
	}
}

void riscv_activate_hw_bpts(void) {
	unsigned long tdata1;
	unsigned long i;

	if (!hw_bpts_enabled) {
		return;
	}

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] != NULL) {
			write_csr(tselect, i);
			tdata1 = read_csr(tdata1);
			tdata1 |= MCONTROL_EXECUTE;
			tdata1 |= MCONTROL_M;
			tdata1 &= ~MCONTROL_MATCH;
			tdata1 &= ~MCONTROL_ACTION;
			tdata1 &= ~MCONTROL_SIZELO;
			write_csr(tdata1, tdata1);
			write_csr(tdata2, (unsigned long)hw_bpts[i]);
		}
	}

	hw_bpts_activated = true;
}

void riscv_deactivate_hw_bpts(void) {
	unsigned long i;

	if (!hw_bpts_enabled) {
		return;
	}

	for (i = 0; i < hw_bpt_count; i++) {
		if (hw_bpts[i] != NULL) {
			write_csr(tselect, i);
			write_csr(tdata1, 0);
		}
	}

	hw_bpts_activated = false;
}

void riscv_enable_hw_bpts(void) {
	unsigned long tselect;
	unsigned long tdata1;
	int type;

	for (hw_bpt_count = 0; hw_bpt_count < MAX_HW_BPTS; hw_bpt_count++) {
		write_csr(tselect, hw_bpt_count);
		tselect = read_csr(tselect);
		tselect &= ~(1UL << (__riscv_xlen - 1));
		if (tselect != hw_bpt_count) {
			break;
		}
		tdata1 = read_csr(tdata1);
		type = (tdata1 >> (__riscv_xlen - 4)) & 0xf;
		if (type == 0) {
			break;
		}
	}

	hw_bpts_enabled = true;
	hw_bpts_activated = false;
}

void riscv_disable_hw_bpts(void) {
	hw_bpts_enabled = false;
}
