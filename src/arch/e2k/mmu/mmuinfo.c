/**
 * @file
 *
 * @date May 4, 2019
 * @author Anton Bondarev
 */

#include <string.h>

#include <lib/libds/array.h>
#include <util/math.h>

#include <hal/mmu.h>
#include <mmu_regs.h>

mmu_reg_t mmureg_get_mmucr(void) {
	return READ_MMU_CR();
}

mmu_reg_t mmureg_get_cont(void) {
	return READ_MMU_CONT();
}

mmu_reg_t mmureg_get_elb_ptb(void) {
	return READ_MMU_ELB_PTB();
}

mmu_reg_t mmureg_get_root_ptb(void) {
	return READ_MMU_ROOT_PTB();
}

struct mmuinfo_reg_access {
	const char *reg_name;
	mmu_reg_t (*mmureg_getter)(void);
};

static const struct mmuinfo_reg_access mmuinfo_regs[] = {
		{"MMU_CR", mmureg_get_mmucr},
		{"MMU_CONT", mmureg_get_cont},
		{"MMU_ELB_PTB", mmureg_get_elb_ptb},
		{"MMU_ROOT_PTB", mmureg_get_root_ptb},
};

int arch_mmu_get_regs_table_size(void) {
	return ARRAY_SIZE(mmuinfo_regs);
}

int arch_mmu_get_regs_table(struct mmuinfo_regs *buf, int buf_size, int offset) {
	int cnt;
	int i;

	cnt = ARRAY_SIZE(mmuinfo_regs) - offset;
	if (cnt <= 0) {
		return 0;
	}
	cnt = min(buf_size, cnt);
	for (i= 0; i < cnt; i ++) {
		strncpy(buf[i].mmu_reg_name, mmuinfo_regs[offset + i].reg_name, sizeof(buf[i].mmu_reg_name));
		buf[i].mmu_reg_name[sizeof(buf[i].mmu_reg_name)-1] = '\0';

		buf[i].mmu_reg_value = mmuinfo_regs[offset + i].mmureg_getter();
	}

	return cnt;
}
