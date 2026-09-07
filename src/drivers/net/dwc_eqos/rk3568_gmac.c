/**
 * @file
 * @brief RK3568 GMAC instantiation module.
 *
 * The ports themselves are wired by the board configuration: every
 * GMAC<n> enabled there gets a net device here, with its base
 * address, irq, MAC address, CRU/GRF coordinates, pin multiplexing
 * and PHY reset GPIO taken from the config.
 */

#include <config/board_config.h>

#include <drivers/common/memory.h>
#include <embox/unit.h>

#include "dwc_eqos.h"
#include "dwc_eqos_rk3568.h"

#if defined(CONF_GMAC0_ENABLED)

static const struct rk3568_gmac_pin gmac0_pins[] = {
	CONF_GMAC0_MISC_PINS_A
	CONF_GMAC0_MISC_PINS_B
	CONF_GMAC0_MISC_PINS_C
};

static const struct rk3568_gmac_soc gmac0_soc = {
	.clkgate_con_off  = CONF_GMAC0_MISC_CRU_GATE_REG,
	.clkgate_mask     = CONF_GMAC0_MISC_CRU_GATE_MASK,
	.softrst_con_off  = CONF_GMAC0_MISC_CRU_RST_REG,
	.softrst_mask     = CONF_GMAC0_MISC_CRU_RST_MASK,
	.clksel_con_off   = CONF_GMAC0_MISC_CRU_SEL_REG,

	.grf_maccon0_off  = CONF_GMAC0_MISC_GRF_MACCON0_REG,
	.grf_maccon0_val  = CONF_GMAC0_MISC_GRF_MACCON0_VAL,
	.grf_maccon0_mask = CONF_GMAC0_MISC_GRF_MACCON0_MASK,
	.grf_maccon1_off  = CONF_GMAC0_MISC_GRF_MACCON1_REG,
	.grf_maccon1_val  = CONF_GMAC0_MISC_GRF_MACCON1_VAL,
	.grf_maccon1_mask = CONF_GMAC0_MISC_GRF_MACCON1_MASK,

	.grf_iomux_off    = CONF_GMAC0_MISC_GRF_IOMUX_OFF,
	.pins             = gmac0_pins,
	.pin_num          = sizeof(gmac0_pins) / sizeof(gmac0_pins[0]),
	.grf_route_off    = CONF_GMAC0_MISC_GRF_ROUTE_REG,
	.grf_route_val    = CONF_GMAC0_MISC_GRF_ROUTE_VAL,

	.rst_gpio_base    = CONF_GMAC0_MISC_RST_GPIO_BASE,
	.rst_gpio_pin     = CONF_GMAC0_MISC_RST_GPIO_PIN,
};

static const struct dwc_eqos_plat gmac0_plat = {
	.idx             = 0,
	.base_addr       = CONF_GMAC0_REGION_BASE,
	.irq_num         = CONF_GMAC0_IRQ_NUM,
	.mac_addr        = CONF_GMAC0_MISC_MAC_ADDR,
	/* The RTL8211F is strapped to add its own RGMII TX delay; the
	 * GRF delays are calibrated with the PHY-side delay off. */
	.clear_rtl8211f_tx_delay = 1,

	.soc_init        = rk3568_gmac_soc_init,
	.soc_swr_quirk   = rk3568_gmac_soc_swr_quirk,
	.soc_data        = &gmac0_soc,
};

PERIPH_MEMORY_DEFINE(rk3568_gmac0_regs, CONF_GMAC0_REGION_BASE,
		CONF_GMAC0_REGION_BASE_LEN);

#endif /* CONF_GMAC0_ENABLED */

#if defined(CONF_GMAC1_ENABLED)

static const struct rk3568_gmac_pin gmac1_pins[] = {
	CONF_GMAC1_MISC_PINS_A
	CONF_GMAC1_MISC_PINS_B
	CONF_GMAC1_MISC_PINS_C
};

static const struct rk3568_gmac_soc gmac1_soc = {
	.clkgate_con_off  = CONF_GMAC1_MISC_CRU_GATE_REG,
	.clkgate_mask     = CONF_GMAC1_MISC_CRU_GATE_MASK,
	.softrst_con_off  = CONF_GMAC1_MISC_CRU_RST_REG,
	.softrst_mask     = CONF_GMAC1_MISC_CRU_RST_MASK,
	.clksel_con_off   = CONF_GMAC1_MISC_CRU_SEL_REG,

	.grf_maccon0_off  = CONF_GMAC1_MISC_GRF_MACCON0_REG,
	.grf_maccon0_val  = CONF_GMAC1_MISC_GRF_MACCON0_VAL,
	.grf_maccon0_mask = CONF_GMAC1_MISC_GRF_MACCON0_MASK,
	.grf_maccon1_off  = CONF_GMAC1_MISC_GRF_MACCON1_REG,
	.grf_maccon1_val  = CONF_GMAC1_MISC_GRF_MACCON1_VAL,
	.grf_maccon1_mask = CONF_GMAC1_MISC_GRF_MACCON1_MASK,

	.grf_iomux_off    = CONF_GMAC1_MISC_GRF_IOMUX_OFF,
	.pins             = gmac1_pins,
	.pin_num          = sizeof(gmac1_pins) / sizeof(gmac1_pins[0]),
	.grf_route_off    = CONF_GMAC1_MISC_GRF_ROUTE_REG,
	.grf_route_val    = CONF_GMAC1_MISC_GRF_ROUTE_VAL,

	.rst_gpio_base    = CONF_GMAC1_MISC_RST_GPIO_BASE,
	.rst_gpio_pin     = CONF_GMAC1_MISC_RST_GPIO_PIN,
};

static const struct dwc_eqos_plat gmac1_plat = {
	.idx             = 1,
	.base_addr       = CONF_GMAC1_REGION_BASE,
	.irq_num         = CONF_GMAC1_IRQ_NUM,
	.mac_addr        = CONF_GMAC1_MISC_MAC_ADDR,
	.clear_rtl8211f_tx_delay = 1,

	.soc_init        = rk3568_gmac_soc_init,
	.soc_swr_quirk   = rk3568_gmac_soc_swr_quirk,
	.soc_data        = &gmac1_soc,
};

PERIPH_MEMORY_DEFINE(rk3568_gmac1_regs, CONF_GMAC1_REGION_BASE,
		CONF_GMAC1_REGION_BASE_LEN);

#endif /* CONF_GMAC1_ENABLED */

static int rk3568_gmac_init(void) {
	int ret = 0;

#if defined(CONF_GMAC0_ENABLED)
	ret = dwc_eqos_dev_init(&gmac0_plat);
#endif
#if defined(CONF_GMAC1_ENABLED)
	ret = ret ? ret : dwc_eqos_dev_init(&gmac1_plat);
#endif
	return ret;
}

EMBOX_UNIT_INIT(rk3568_gmac_init);
