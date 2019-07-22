/**
 * @file
 *
 * @date Apr 30, 2019
 * @author Anton Bondarev
 */
#include <string.h>

#include <util/array.h>
#include <util/math.h>

#include <hal/mmu.h>
#include <asm/cp15.h>

extern uint32_t _get_mmu_tlb_type(void);
extern uint32_t _get_mmu_translation_table_base_0(void);
extern uint32_t _get_mmu_translation_table_base_1(void);
extern uint32_t _get_mmu_domain_access_control(void);

struct mmuinfo_reg_access {
	const char *reg_name;
	mmu_reg_t (*mmureg_getter)(void);
};

static const struct mmuinfo_reg_access mmuinfo_regs[] = {
		{"TLB Type", _get_mmu_tlb_type},
		{"SCTRL", cp15_get_sctrl},
		/* {"ACTRL", cp15_get_actrl}, */
		/* {"CPACR", cp15_get_cpacr}, */
		/* {"Non-Secure Access Control", cp15_get_nsacr}, */
		{"Translation Table Base 0", _get_mmu_translation_table_base_0},
		{"Translation Table Base 1", _get_mmu_translation_table_base_1},
		{"Domain Access Control", _get_mmu_domain_access_control}
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
