/**
 * @file
 *
 * @date Mar 25, 2023
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <stdint.h>

#include <hal/reg.h>

#include "rcm_mdio.h"

void rcm_mdio_write(uint32_t mdio_addr, uint32_t reg_addr, uint32_t write_data) {
	uint32_t phy_addr = 0x0;
	uint32_t ctrl;
	uint32_t read_data = 0x4;

	ctrl = 0x00000001 | phy_addr << 4 | reg_addr << 8 | write_data << 16;

	REG32_STORE(mdio_addr + MDIO_CONTROL, ctrl);

	while (read_data & 0x4) {
		read_data = REG32_LOAD(mdio_addr + MDIO_CONTROL);
	}
}

void rcm_mdio_init(uint32_t mdio_base) {

	REG32_STORE(mdio_base + MDIO_ETH_RST_N, 0x1);
	REG32_STORE(mdio_base + MDIO_EN, 0x1);

	log_debug("line %d", __LINE__);
}

void rcm_mdio_en(uint32_t mdio_base, int en)  {

	if (!en) {
		log_debug("line %d", __LINE__);
		rcm_mdio_write(mdio_base, MDIO_CONTROL, 0x2947);
	}
}
