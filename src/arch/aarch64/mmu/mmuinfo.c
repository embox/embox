/**
 * @file
 * @brief The EL1 translation control registers, for the mmuinfo command
 *
 * SCTLR_EL1 says whether translation and the caches are on, TCR_EL1 how the
 * two virtual ranges are sized and walked, the TTBRs where each range's table
 * starts, MAIR_EL1 what the attribute indices in the descriptors stand for.
 * ESR_EL1 and FAR_EL1 come last: they are about the most recent fault rather
 * than about the configuration.
 *
 */

#include <stdint.h>
#include <string.h>

#include <hal/mmu.h>
#include <hal/reg.h>
#include <lib/libds/array.h>
#include <util/math.h>

struct mmuinfo_reg_access {
	const char *reg_name;
	mmu_reg_t (*mmureg_getter)(void);
};

#define MMUINFO_REG(reg)                     \
	static mmu_reg_t get_##reg(void) {       \
		return (mmu_reg_t)ARCH_REG_LOAD(reg); \
	}

MMUINFO_REG(SCTLR_EL1)
MMUINFO_REG(TCR_EL1)
MMUINFO_REG(TTBR0_EL1)
MMUINFO_REG(TTBR1_EL1)
MMUINFO_REG(MAIR_EL1)
MMUINFO_REG(ESR_EL1)
MMUINFO_REG(FAR_EL1)

static const struct mmuinfo_reg_access mmuinfo_regs[] = {
    {"SCTLR_EL1", get_SCTLR_EL1},
    {"TCR_EL1", get_TCR_EL1},
    {"TTBR0_EL1", get_TTBR0_EL1},
    {"TTBR1_EL1", get_TTBR1_EL1},
    {"MAIR_EL1", get_MAIR_EL1},
    {"ESR_EL1", get_ESR_EL1},
    {"FAR_EL1", get_FAR_EL1},
};

int arch_mmu_get_regs_table_size(void) {
	return ARRAY_SIZE(mmuinfo_regs);
}

int arch_mmu_get_regs_table(struct mmuinfo_regs *buf, int buf_size,
    int offset) {
	int cnt;
	int i;

	cnt = ARRAY_SIZE(mmuinfo_regs) - offset;
	if (cnt <= 0) {
		return 0;
	}
	cnt = min(buf_size, cnt);

	for (i = 0; i < cnt; i++) {
		strncpy(buf[i].mmu_reg_name, mmuinfo_regs[offset + i].reg_name,
		    sizeof(buf[i].mmu_reg_name));
		buf[i].mmu_reg_name[sizeof(buf[i].mmu_reg_name) - 1] = '\0';

		buf[i].mmu_reg_value = mmuinfo_regs[offset + i].mmureg_getter();
	}

	return cnt;
}
