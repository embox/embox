/**
 * @file
 *
 * @date Apr 30, 2019
 * @author Anton Bondarev
 */
#include <string.h>

#include <lib/libds/array.h>
#include <util/math.h>

#include <hal/mmu.h>
#include <asm/cr_regs.h>

struct mmuinfo_reg_access {
	const char *reg_name;
	mmu_reg_t (*mmureg_getter)(void);
};

static const struct mmuinfo_reg_access mmuinfo_regs[] = {
		{"CR0", get_cr0},
		/* {"CR1", get_cr1}, */
		{"CR2", get_cr2},
		{"CR3", get_cr3},
		{"CR4", get_cr4}
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
	for (i = 0; i < cnt; i ++) {
		strncpy(buf[i].mmu_reg_name, mmuinfo_regs[offset + i].reg_name, sizeof(buf[i].mmu_reg_name));
		buf[i].mmu_reg_name[sizeof(buf[i].mmu_reg_name)-1] = '\0';

		buf[i].mmu_reg_value = mmuinfo_regs[offset + i].mmureg_getter();
	}

	return cnt;
}
