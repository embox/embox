/**
 * @file
 * @brief RK3568 SoC glue for the DesignWare EQoS MAC core: CRU
 *        clocks, GRF interface/delay config, pin iomux and the PHY
 *        reset GPIO.
 *
 * See dwc_eqos_rk3568.h.  The core driver reaches this glue only
 * through the soc_init/soc_swr_quirk hooks of struct dwc_eqos_plat,
 * so other SoCs plug in their own glue without touching the core.
 */

#include <stdint.h>
#include <unistd.h>

#include <framework/mod/options.h>
#include <hal/reg.h>
#include <util/log.h>

#include <drivers/common/memory.h>

#include "dwc_eqos_rk3568.h"

#define CRU_BASE  OPTION_GET(NUMBER, cru_base)
#define GRF_BASE  OPTION_GET(NUMBER, grf_base)

/* GRF iomux register for a pin of a bank: 8 pins per register pair,
 * 4 pins per 32-bit word. */
static unsigned long grf_iomux_reg(unsigned int bank_off, unsigned int pin,
		unsigned int *shift) {
	*shift = (pin % 4) * 4;

	return GRF_BASE + bank_off + (pin / 8) * 8 + ((pin % 8) / 4) * 4;
}

static void grf_write(unsigned long off, uint32_t mask, uint32_t val) {
	REG32_STORE(GRF_BASE + off, ((mask & 0xffffu) << 16) | (val & mask));
}

static void rk3568_gmac_clocks_enable(const struct rk3568_gmac_soc *soc) {
	/* Gate bits are active-low: write 0 to the hiword-masked field to
	 * enable the clock. */
	log_debug("cru gate %#x: %#x -> ", soc->clkgate_con_off,
			REG32_LOAD(CRU_BASE + soc->clkgate_con_off));
	REG32_STORE(CRU_BASE + soc->clkgate_con_off,
			(uint32_t)soc->clkgate_mask << 16);
	log_debug("%#x", REG32_LOAD(CRU_BASE + soc->clkgate_con_off));
	/* Soft reset bits are active-low too: write 0 to deassert. */
	log_debug("cru rst %#x: %#x -> ", soc->softrst_con_off,
			REG32_LOAD(CRU_BASE + soc->softrst_con_off));
	REG32_STORE(CRU_BASE + soc->softrst_con_off,
			(uint32_t)soc->softrst_mask << 16);
	log_debug("%#x", REG32_LOAD(CRU_BASE + soc->softrst_con_off));
	/* GMAC clock muxes: all-zero selector = 125 MHz RGMII output mode
	 * (rx/tx from the 125M source, ptp ref 62.5M). */
	log_debug("cru sel %#x: %#x -> ", soc->clksel_con_off,
			REG32_LOAD(CRU_BASE + soc->clksel_con_off));
	REG32_STORE(CRU_BASE + soc->clksel_con_off, 0xffffu << 16);
	log_debug("%#x", REG32_LOAD(CRU_BASE + soc->clksel_con_off));
}

static void rk3568_gmac_iomux(const struct rk3568_gmac_soc *soc) {
	int i;

	if (soc->grf_route_off != 0) {
		/* Full register word: write-enable mask in bits[31:16]. */
		REG32_STORE(GRF_BASE + soc->grf_route_off, soc->grf_route_val);
	}

	for (i = 0; i < soc->pin_num; i++) {
		const struct rk3568_gmac_pin *p = &soc->pins[i];
		unsigned int shift;
		unsigned long reg;

		reg = grf_iomux_reg(soc->grf_iomux_off, p->pin, &shift);
		REG32_STORE(reg, ((0xfu << shift) << 16) | (p->func << shift));
	}
}

static void rk3568_gmac_rgmii_setup(const struct rk3568_gmac_soc *soc) {
	grf_write(soc->grf_maccon1_off, soc->grf_maccon1_mask,
			soc->grf_maccon1_val);
	grf_write(soc->grf_maccon0_off, soc->grf_maccon0_mask,
			soc->grf_maccon0_val);
}

/* GPIO bank, version 2 layout: data at 0x00/0x04, direction at 0x08/0x0c,
 * value in bits[15:0], write-enable in bits[31:16]. */
static void gpio_write(unsigned long bank, unsigned int pin, int output,
		int value) {
	unsigned long off = (pin < 16) ? 0x0 : 0x4;
	uint32_t bit = 1u << (pin & 15);
	uint32_t we = bit << 16;

	/* set the data first, then switch the direction */
	REG32_STORE(bank + off, we | (value ? bit : 0));
	REG32_STORE(bank + off + 0x8, we | (output ? bit : 0));
}

static void rk3568_gmac_phy_reset_pulse(const struct rk3568_gmac_soc *soc) {
	/* Active low: assert for 20 ms, release, then wait 100 ms for the
	 * PHY to boot and re-latch its straps. */
	gpio_write(soc->rst_gpio_base, soc->rst_gpio_pin, 1, 0);
	usleep(20 * 1000);
	gpio_write(soc->rst_gpio_base, soc->rst_gpio_pin, 1, 1);
	usleep(100 * 1000);
}

int rk3568_gmac_soc_init(const struct dwc_eqos_plat *plat) {
	const struct rk3568_gmac_soc *soc = plat->soc_data;

	rk3568_gmac_clocks_enable(soc);
	rk3568_gmac_iomux(soc);
	rk3568_gmac_rgmii_setup(soc);
	rk3568_gmac_phy_reset_pulse(soc);

	return 0;
}

void rk3568_gmac_soc_swr_quirk(const struct dwc_eqos_plat *plat, int enter) {
	const struct rk3568_gmac_soc *soc = plat->soc_data;
	uint32_t val = (enter != 0) ? 0x1u : 0x0u;

	/* During DMA_MODE.SWR the GMAC clock mux must take the RMII
	 * (62.5M) path or the reset never clears: on gmac0 the 125M
	 * direct path is not active at cold boot, and gmac1 needs the
	 * same walk on some boots. */
	REG32_STORE(CRU_BASE + soc->clksel_con_off, (0x3u << 16) | val);
}

PERIPH_MEMORY_DEFINE(dwc_eqos_rk3568_grf, GRF_BASE, 0x1000);
PERIPH_MEMORY_DEFINE(dwc_eqos_rk3568_cru, CRU_BASE, 0x1000);
PERIPH_MEMORY_DEFINE(dwc_eqos_rk3568_gpio2, 0xFE750000, 0x1000);
PERIPH_MEMORY_DEFINE(dwc_eqos_rk3568_gpio3, 0xFE760000, 0x1000);
