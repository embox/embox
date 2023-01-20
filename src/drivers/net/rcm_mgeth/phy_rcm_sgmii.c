/**
 * @file
 *
 * @date Apr 3, 2023
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <stdint.h>

#include <hal/reg.h>

int phy_rcm_sgmii_init(uint32_t sgmii_addr, uint32_t sctl_addr, int en) {
	uint32_t sgmii_ctrl_stat_val;
	uint32_t OFFSET_CM;
	uint32_t OFFSET_RXS_0;

	sgmii_ctrl_stat_val = REG32_LOAD(sctl_addr + 0x14);

	if (sgmii_ctrl_stat_val) {
		return 1;
	}

	if (!en) {
		REG32_STORE(sgmii_addr + 0 * 0x400 +0x0200, 0x00000140);
		REG32_STORE(sgmii_addr + 1 * 0x400 +0x0200, 0x00000140);
		REG32_STORE(sgmii_addr + 2 * 0x400 +0x0200, 0x00000140);
		REG32_STORE(sgmii_addr + 3 * 0x400 +0x0200, 0x00000140);
	}

	REG32_STORE(sgmii_addr + 0 * 0x400 + 0x0000, 0x40803004);
	REG32_STORE(sgmii_addr + 1 * 0x400 + 0x0000, 0x40803004);
	REG32_STORE(sgmii_addr + 2 * 0x400 + 0x0000, 0x40803004);
	REG32_STORE(sgmii_addr + 3 * 0x400 + 0x0000, 0x40803004);

	OFFSET_CM = sgmii_addr + 0x1000;
	REG32_STORE(OFFSET_CM + 0x04, 0x00130000);
	REG32_STORE(OFFSET_CM + 0x08, 0x710001F0);
	REG32_STORE(OFFSET_CM + 0x0C, 0x00000002);
	REG32_STORE(OFFSET_CM + 0x20, 0x07000000);

	OFFSET_RXS_0 = sgmii_addr + 0 * 0x400 + 0x0100;
	REG32_STORE(OFFSET_RXS_0 + 0 * 0x400 + 0x08, 0x0000CEA6);
	REG32_STORE(OFFSET_RXS_0 + 1 * 0x400 + 0x08, 0x0000CEA6);
	REG32_STORE(OFFSET_RXS_0 + 2 * 0x400 + 0x08, 0x0000CEA6);
	REG32_STORE(OFFSET_RXS_0 + 3 * 0x400 + 0x08, 0x0000CEA6);

	REG32_STORE(sctl_addr + 0x14, 0x1);

	while (sgmii_ctrl_stat_val != 0x000001F1) {
		sgmii_ctrl_stat_val = REG32_LOAD(sctl_addr + 0x14);
	}

	return 0;
}
