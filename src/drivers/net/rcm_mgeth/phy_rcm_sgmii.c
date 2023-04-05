/**
 * @file
 *
 * @date Apr 3, 2023
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <stdint.h>

#include <driver/rcm_sctl.h>
#include <driver/phy_rcm_sgmii.h>

#include <hal/reg.h>

#include <drivers/common/memory.h>

#include <framework/mod/options.h>

#define BASE_ADDR   OPTION_GET(NUMBER, base_addr)

int phy_rcm_sgmii_init(int auto_negotiation_en) {
	rcm_sctl_write_reg(RCM_SCTL_SGMII_CTRL_STAT, 0x1);

	if (!auto_negotiation_en) {
		/* disable auto-negotiation */
		REG32_STORE(BASE_ADDR + 0 * 0x400 + 0x0200, 0x00000140);
		REG32_STORE(BASE_ADDR + 1 * 0x400 + 0x0200, 0x00000140);
		REG32_STORE(BASE_ADDR + 2 * 0x400 + 0x0200, 0x00000140);
		REG32_STORE(BASE_ADDR + 3 * 0x400 + 0x0200, 0x00000140);
	}

	REG32_STORE(BASE_ADDR + 0 * 0x400 + 0x0000, 0x40803004);
	REG32_STORE(BASE_ADDR + 1 * 0x400 + 0x0000, 0x40803004);
	REG32_STORE(BASE_ADDR + 2 * 0x400 + 0x0000, 0x40803004);
	REG32_STORE(BASE_ADDR + 3 * 0x400 + 0x0000, 0x40803004);

	REG32_STORE(BASE_ADDR + 0x1004, 0x00130000);
	REG32_STORE(BASE_ADDR + 0x1008, 0x710001F0);
	REG32_STORE(BASE_ADDR + 0x100C, 0x00000002);
	REG32_STORE(BASE_ADDR + 0x1020, 0x07000000);

	REG32_STORE(BASE_ADDR + 0 * 0x400 + 0x0108, 0x0000CEA6);
	REG32_STORE(BASE_ADDR + 1 * 0x400 + 0x0108, 0x0000CEA6);
	REG32_STORE(BASE_ADDR + 2 * 0x400 + 0x0108, 0x0000CEA6);
	REG32_STORE(BASE_ADDR + 3 * 0x400 + 0x0108, 0x0000CEA6);

	while (rcm_sctl_read_reg(RCM_SCTL_SGMII_CTRL_STAT) != 0x000001F1) {
	}

	return 0;
}

PERIPH_MEMORY_DEFINE(rcm_phy_sgmii, BASE_ADDR, 0x2000);

