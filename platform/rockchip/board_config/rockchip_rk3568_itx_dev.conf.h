/**
 * @file
 * @brief Board configuration for the Rockchip RK3568 ITX board.
 *
 * Describes the two GMAC ports the way the board is wired: RGMII
 * to an RTL8211F each, with the CRU clock gates / soft resets, the
 * GRF interface mode and RGMII delay registers, the pin multiplexing
 * and the PHY reset GPIOs.  Each pin batch is an initializer-list
 * fragment of `struct rk3568_gmac_pin` (GRF pin number, function).
 *
 * All registers use the Rockchip hiword write-enable protocol.
 */

#include <gen_board_conf.h>

#define GMAC0_MAC_ADDR  "02:e4:a5:35:68:00"
#define GMAC1_MAC_ADDR  "02:e4:a5:35:68:01"

/* gmac0_rgmii_bus, gmac0_rgmii_clk */
#define GMAC0_PINS_A  {3, 2}, {4, 2}, {5, 2}, {6, 2}, {7, 2}, {8, 2},
/* gmac0_tx_bus2, gmac0_rx_bus2 */
#define GMAC0_PINS_B  {11, 1}, {12, 1}, {13, 1}, {14, 1}, {15, 2}, {16, 2},
/* gmac0_miim */
#define GMAC0_PINS_C  {19, 2}, {20, 2},

/* gmac1m0 tx_bus2, rx_bus2 */
#define GMAC1_PINS_A  {13, 3}, {14, 3}, {2, 3}, {3, 3}, {15, 3}, {9, 3},
/* gmac1m0 rx_bus2 (cont), rgmii_clk */
#define GMAC1_PINS_B  {10, 3}, {4, 3}, {5, 3}, {11, 3}, {6, 3}, {7, 3},
/* gmac1m0 clkinout, miim */
#define GMAC1_PINS_C  {16, 3}, {20, 3}, {21, 3},

struct eth_conf eths[] = {
	[0] = {
		.status = ENABLED,
		.name = "GMAC0",
		.dev = {
			.name = "GMAC0",
			.regs = {
				REGMAP("BASE", 0xFE2A0000, 0x2000),
			},
			.irqs = {
				VAL("NUM", 59),
			},
			.clocks = { },
			.misc = {
				VAL("MAC_ADDR", GMAC0_MAC_ADDR),
				VAL("CRU_GATE_REG", 0x33C),
				VAL("CRU_GATE_MASK", 0x1F70),
				VAL("CRU_RST_REG", 0x434),
				VAL("CRU_RST_MASK", 0x0080),
				VAL("CRU_SEL_REG", 0x17C),
				VAL("GRF_MACCON0_REG", 0x380),
				VAL("GRF_MACCON0_VAL", 0x264F),
				VAL("GRF_MACCON0_MASK", 0x7F7F),
				VAL("GRF_MACCON1_REG", 0x384),
				VAL("GRF_MACCON1_VAL", 0x0013),
				VAL("GRF_MACCON1_MASK", 0x0073),
				VAL("GRF_IOMUX_OFF", 0x020),
				VAL("GRF_ROUTE_REG", 0),
				VAL("GRF_ROUTE_VAL", 0),
				VAL("RST_GPIO_BASE", 0xFE750000),
				VAL("RST_GPIO_PIN", 9),
				VAL("PINS_A", GMAC0_PINS_A),
				VAL("PINS_B", GMAC0_PINS_B),
				VAL("PINS_C", GMAC0_PINS_C),
			},
		},
	},
	[1] = {
		.status = ENABLED,
		.name = "GMAC1",
		.dev = {
			.name = "GMAC1",
			.regs = {
				REGMAP("BASE", 0xFE010000, 0x2000),
			},
			.irqs = {
				VAL("NUM", 64),
			},
			.clocks = { },
			.misc = {
				VAL("MAC_ADDR", GMAC1_MAC_ADDR),
				VAL("CRU_GATE_REG", 0x344),
				VAL("CRU_GATE_MASK", 0x07DC),
				VAL("CRU_RST_REG", 0x438),
				VAL("CRU_RST_MASK", 0x1000),
				VAL("CRU_SEL_REG", 0x184),
				VAL("GRF_MACCON0_REG", 0x388),
				VAL("GRF_MACCON0_VAL", 0x033C),
				VAL("GRF_MACCON0_MASK", 0x7F7F),
				VAL("GRF_MACCON1_REG", 0x38C),
				VAL("GRF_MACCON1_VAL", 0x0013),
				VAL("GRF_MACCON1_MASK", 0x0073),
				VAL("GRF_IOMUX_OFF", 0x040),
				VAL("GRF_ROUTE_REG", 0x300),
				VAL("GRF_ROUTE_VAL", 0x0100),
				VAL("RST_GPIO_BASE", 0xFE760000),
				VAL("RST_GPIO_PIN", 18),
				VAL("PINS_A", GMAC1_PINS_A),
				VAL("PINS_B", GMAC1_PINS_B),
				VAL("PINS_C", GMAC1_PINS_C),
			},
		},
	},
};

EXPORT_CONFIG(ETH(eths))
