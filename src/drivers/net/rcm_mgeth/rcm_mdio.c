/**
 * @file
 *
 * @date Mar 25, 2023
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <stdint.h>

#include <hal/reg.h>

#include <drivers/common/memory.h>

#include <driver/rcm_mdio.h>

#define BASE_ADDR   OPTION_GET(NUMBER, base_addr)

void rcm_mdio_write(int dev_num, uint32_t reg_addr, uint32_t write_data) {
	uint32_t mdio_addr = BASE_ADDR + dev_num * 0x1000;
	uint32_t phy_addr = 0x0;
	uint32_t ctrl;
	uint32_t read_data = 0x4;

	ctrl = 0x00000001 | phy_addr << 4 | reg_addr << 8 | write_data << 16;

	REG32_STORE(mdio_addr + MDIO_CONTROL, ctrl);

	while (read_data & 0x4) {
		read_data = REG32_LOAD(mdio_addr + MDIO_CONTROL);
	}
}

void rcm_mdio_init(int dev_num) {
	uint32_t mdio_addr = BASE_ADDR + dev_num * 0x1000;

	REG32_STORE(mdio_addr + MDIO_ETH_RST_N, 0x1);
	REG32_STORE(mdio_addr + MDIO_EN, 0x1);
}

void rcm_mdio_en(int dev_num, int eauto_negotiation)  {
	uint32_t mdio_addr = BASE_ADDR + dev_num * 0x1000;

	if (!eauto_negotiation) {
		rcm_mdio_write(mdio_addr, MDIO_CONTROL, 0x2947);
	}
}

PERIPH_MEMORY_DEFINE(rcm_mdio, BASE_ADDR, 0x4000);
