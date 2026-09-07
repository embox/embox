/**
 * @file
 * @brief RK3568 SoC glue for the DesignWare EQoS MAC core.
 *
 * The glue owns everything outside the IP: CRU clock gates/soft
 * resets/clock mux, GRF interface mode and RGMII delay programming,
 * pin iomux with io routing, and the PHY hard reset GPIO.  Instances
 * (rk3568_gmac0/rk3568_gmac1) fill a struct rk3568_gmac_soc with
 * their coordinates and hook rk3568_gmac_soc_init /
 * rk3568_gmac_soc_swr_quirk into struct dwc_eqos_plat.
 *
 * Embox configures the board statically (no FDT/pinctrl layer), so
 * all coordinates are per-instance data.  The CRU, GRF and GPIO v2
 * registers all use the Rockchip hiword write-enable protocol: value
 * in bits[15:0], write-enable mask in bits[31:16].
 */

#ifndef SRC_DRIVERS_NET_DWC_EQOS_DWC_EQOS_RK3568_H_
#define SRC_DRIVERS_NET_DWC_EQOS_DWC_EQOS_RK3568_H_

#include <stdint.h>

#include "dwc_eqos.h"

/* GRF iomux entry, one pin group per word: 4 bits per pin, 4 pins
 * per GRF register. */
struct rk3568_gmac_pin {
	uint8_t pin;   /* pin number inside the GPIO bank */
	uint8_t func;  /* iomux function selector */
};

struct rk3568_gmac_soc {
	/* CRU: clock gates, soft reset and the GMAC clock mux */
	unsigned int   clkgate_con_off;  /* CRU_CLKGATE_CON[n] offset */
	uint32_t       clkgate_mask;     /* bits to ungate (write 0) */
	unsigned int   softrst_con_off;  /* CRU_SOFTRST_CON[n] offset */
	uint32_t       softrst_mask;     /* bits to deassert (write 0) */
	unsigned int   clksel_con_off;   /* CRU_CLKSEL_CON[n] offset */

	/* GRF: interface mode, RGMII delays, iomux and io route */
	unsigned int   grf_maccon0_off;  /* tx/rx delay value register */
	uint32_t       grf_maccon0_val;  /* (rx_delay << 8) | tx_delay */
	uint32_t       grf_maccon0_mask;
	unsigned int   grf_maccon1_off;  /* interface mode + delay enables */
	uint32_t       grf_maccon1_val;
	uint32_t       grf_maccon1_mask;
	unsigned int   grf_iomux_off;    /* GRF iomux bank base offset */
	const struct rk3568_gmac_pin *pins;
	int            pin_num;
	unsigned int   grf_route_off;    /* io route register, 0 if unused */
	uint32_t       grf_route_val;

	/* PHY hard reset GPIO (DW-APB GPIO bank, version 2 layout) */
	unsigned long  rst_gpio_base;
	unsigned int   rst_gpio_pin;     /* pin inside the bank */
};

int rk3568_gmac_soc_init(const struct dwc_eqos_plat *plat);
void rk3568_gmac_soc_swr_quirk(const struct dwc_eqos_plat *plat, int enter);

#endif /* SRC_DRIVERS_NET_DWC_EQOS_DWC_EQOS_RK3568_H_ */
