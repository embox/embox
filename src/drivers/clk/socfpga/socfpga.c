/**
 * @file socfpga.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 01.10.2019
 */

#include <stdint.h>
#include <string.h>

#include <drivers/common/memory.h>

#include <hal/reg.h>

#include <lib/libds/array.h>
#include <util/log.h>

#include <framework/mod/options.h>

#define SOCFPGA_CLK_BASE           OPTION_GET(NUMBER, base_addr)

#define CLKMGR_CTRL       (SOCFPGA_CLK_BASE + 0x00)
#define CLKMGR_BYPASS     (SOCFPGA_CLK_BASE + 0x04)
#define CLKMGR_DBCTRL     (SOCFPGA_CLK_BASE + 0x10)
#define CLKMGR_L4SRC      (SOCFPGA_CLK_BASE + 0x70)
#define CLKMGR_PERPLL_SRC (SOCFPGA_CLK_BASE + 0xAC)

/* Clock bypass bits */
#define MAINPLL_BYPASS		(1 << 0)
#define SDRAMPLL_BYPASS		(1 << 1)
#define SDRAMPLL_SRC_BYPASS	(1 << 2)
#define PERPLL_BYPASS		(1 << 3)
#define PERPLL_SRC_BYPASS	(1 << 4)

#define SOCFPGA_PLL_BG_PWRDWN   0
#define SOCFPGA_PLL_EXT_ENA     1
#define SOCFPGA_PLL_PWR_DOWN    2
#define SOCFPGA_PLL_DIVF_MASK   0x0000FFF8
#define SOCFPGA_PLL_DIVF_SHIFT  3
#define SOCFPGA_PLL_DIVQ_MASK   0x003F0000
#define SOCFPGA_PLL_DIVQ_SHIFT  16

#define CLK_MGR_PLL_CLK_SRC_SHIFT	22
#define CLK_MGR_PLL_CLK_SRC_MASK	0x3

struct clk {
	uint32_t reg_offset;
	uint32_t bit_num;
	const char *clk_name;
};

static const struct clk gate_repo[] = {
	/* Data from socfpga.dtsi */
	{ 0x60,  1 << 0, "l4_main_clk" },
	{ 0x60,  1 << 1, "l3_mp_clk" },
	{ 0x60,  1 << 2, "l4_mp_clk" },
	{ 0x60,  1 << 3, "l4_sp_clk" },
	{ 0x60,  1 << 8, "cfg_clk" },

	{ 0x40,  1 << 1, "main_pll_clk" },
	{ 0x80,  1 << 1, "periph_pll_clk" },

	{ 0x60,  1 << 9, "h2f_user0_clk" },
	{ 0xa0,  1 << 0, "emac_0_clk" },
	{ 0xa0,  1 << 1, "emac_1_clk" },
	{ 0xa0,  1 << 2, "usb_mp_clk" },
	{ 0xa0,  1 << 3, "spi_m_clk" },
	{ 0xa0,  1 << 4, "can0_clk" },
	{ 0xa0,  1 << 5, "can1_clk" },
	{ 0xa0,  1 << 6, "gpio_db_clk" },
	{ 0xa0,  1 << 7, "h2f_user1_clk" },
	{ 0xa0,  1 << 8, "sdmmc_clk" },
	{ 0xa0,  1 << 8, "sdmmc_clk_divided" },
	{ 0xa0,  1 << 9, "nand_x_clk" },
	{ 0xa0, 1 << 10, "nand_clk" },
	{ 0xa0, 1 << 11, "qspi_clk" },
	{ 0xd8,  1 << 0, "ddr_dqs_clk_gate" },
	{ 0xd8,  1 << 1, "ddr_2x_dqs_clk_gate" },
	{ 0xd8,  1 << 2, "ddr_dq_clk_gate" },
	{ 0xd8,  1 << 3, "h2f_user2_clk" },

	/* Periph repo */
	{ 0x88, 1 << 0, "emac0_clk" },
	{ 0x8c, 1 << 0, "emac1_clk" },
	{ 0x90, 1 << 0, "per_qsi_clk" },
	{ 0x94, 1 << 0, "per_nand_mmc_clk" },
	{ 0x98, 1 << 0, "per_base_clk" },
	{ 0x9c, 1 << 0, "h2f_usr1_clk" },
};

int clk_enable(char *clk_name) {
	int i;
	log_debug("%s", clk_name);
	for (i = 0; i < ARRAY_SIZE(gate_repo); i ++) {
		if (0 == strcmp(gate_repo[i].clk_name, clk_name)) {
			REG32_ORIN(gate_repo[i].reg_offset, gate_repo[i].bit_num);
			return i;
		}
	}

	return -1;
}

int clk_disable(char *clk_name) {
	int i;
	log_debug("%s", clk_name);
	for (i = 0; i < ARRAY_SIZE(gate_repo); i ++) {
		if (0 == strcmp(gate_repo[i].clk_name, clk_name)) {
			REG32_CLEAR(gate_repo[i].reg_offset, gate_repo[i].bit_num);
			return i;
		}
	}

	return -1;
}

PERIPH_MEMORY_DEFINE(socfpga_clk, SOCFPGA_CLK_BASE, 0x1000);
