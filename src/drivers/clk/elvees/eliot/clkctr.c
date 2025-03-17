/**
 * @file
 *
 * @date Jan 24, 2023
 * @author Anton Bondarev
 */
#include <stddef.h>
#include <stdint.h>


#include "hal_clkctr.h"

#include "clkctr.h"

#include <config/board_config.h>

/* ========================================================  PLLCFG  ========================================================= */
#define CLKCTR_PLLCFG_SEL_Pos             (0UL)                     /*!< SEL (Bit 0)                                           */
#define CLKCTR_PLLCFG_SEL_Msk             (0x1ffUL)                 /*!< SEL (Bitfield-Mask: 0x1ff)                            */
#define CLKCTR_PLLCFG_MAN_Pos             (9UL)                     /*!< MAN (Bit 9)                                           */
#define CLKCTR_PLLCFG_MAN_Msk             (0x200UL)                 /*!< MAN (Bitfield-Mask: 0x01)                             */
#define CLKCTR_PLLCFG_OD_MAN_Pos          (10UL)                    /*!< OD_MAN (Bit 10)                                       */
#define CLKCTR_PLLCFG_OD_MAN_Msk          (0x3c00UL)                /*!< OD_MAN (Bitfield-Mask: 0x0f)                          */
#define CLKCTR_PLLCFG_NF_MAN_Pos          (14UL)                    /*!< NF_MAN (Bit 14)                                       */
#define CLKCTR_PLLCFG_NF_MAN_Msk          (0x7ffc000UL)             /*!< NF_MAN (Bitfield-Mask: 0x1fff)                        */
#define CLKCTR_PLLCFG_NR_MAN_Pos          (27UL)                    /*!< NR_MAN (Bit 27)                                       */
#define CLKCTR_PLLCFG_NR_MAN_Msk          (0x78000000UL)            /*!< NR_MAN (Bitfield-Mask: 0x0f)                          */
#define CLKCTR_PLLCFG_LOCK_Pos            (31UL)                    /*!< LOCK (Bit 31)                                         */
#define CLKCTR_PLLCFG_LOCK_Msk            (0x80000000UL)            /*!< LOCK (Bitfield-Mask: 0x01)                            */

/* ==========================================================  CFG  ========================================================== */
#define CLKCTR_CFG_FCLK_SCALE_EN_Pos      (0UL)                     /*!< FCLK_SCALE_EN (Bit 0)                                 */
#define CLKCTR_CFG_FCLK_SCALE_EN_Msk      (0x1UL)                   /*!< FCLK_SCALE_EN (Bitfield-Mask: 0x01)                   */
#define CLKCTR_CFG_MAINCLK_SEL_Pos        (4UL)                     /*!< MAINCLK_SEL (Bit 4)                                   */
#define CLKCTR_CFG_MAINCLK_SEL_Msk        (0x30UL)                  /*!< MAINCLK_SEL (Bitfield-Mask: 0x03)                     */
#define CLKCTR_CFG_PLLREF_SEL_Pos         (6UL)                     /*!< PLLREF_SEL (Bit 6)                                    */
#define CLKCTR_CFG_PLLREF_SEL_Msk         (0x40UL)                  /*!< PLLREF_SEL (Bitfield-Mask: 0x01)                      */
#define CLKCTR_CFG_USBCLK_SEL_Pos         (7UL)                     /*!< USBCLK_SEL (Bit 7)                                    */
#define CLKCTR_CFG_USBCLK_SEL_Msk         (0x80UL)                  /*!< USBCLK_SEL (Bitfield-Mask: 0x01)                      */
#define CLKCTR_CFG_I2SCLK_SEL_Pos         (8UL)                     /*!< I2SCLK_SEL (Bit 8)                                    */
#define CLKCTR_CFG_I2SCLK_SEL_Msk         (0x100UL)                 /*!< I2SCLK_SEL (Bitfield-Mask: 0x01)                      */
#define CLKCTR_CFG_MCO_SEL_Pos            (9UL)                     /*!< MCO_SEL (Bit 9)                                       */
#define CLKCTR_CFG_MCO_SEL_Msk            (0xe00UL)                 /*!< MCO_SEL (Bitfield-Mask: 0x07)                         */
#define CLKCTR_CFG_MCO_EN_Pos             (13UL)                    /*!< MCO_EN (Bit 13)                                       */
#define CLKCTR_CFG_MCO_EN_Msk             (0x2000UL)                /*!< MCO_EN (Bitfield-Mask: 0x01)                          */


/* ========================================================  FCLKDIV  ======================================================== */
#define CLKCTR_FCLKDIV_FCLKDIV_Pos        (0UL)                     /*!< FCLKDIV (Bit 0)                                       */
#define CLKCTR_FCLKDIV_FCLKDIV_Msk        (0x1fUL)                  /*!< FCLKDIV (Bitfield-Mask: 0x1f)                         */
#define CLKCTR_FCLKDIV_FCLKDIV_CUR_Pos    (16UL)                    /*!< FCLKDIV_CUR (Bit 16)                                  */
#define CLKCTR_FCLKDIV_FCLKDIV_CUR_Msk    (0x1f0000UL)              /*!< FCLKDIV_CUR (Bitfield-Mask: 0x1f)                     */
/* =======================================================  SYSCLKDIV  ======================================================= */
#define CLKCTR_SYSCLKDIV_STSCLKDIV_Pos    (0UL)                     /*!< STSCLKDIV (Bit 0)                                     */
#define CLKCTR_SYSCLKDIV_STSCLKDIV_Msk    (0x1fUL)                  /*!< STSCLKDIV (Bitfield-Mask: 0x1f)                       */
#define CLKCTR_SYSCLKDIV_STSCLKDIV_CUR_Pos (16UL)                   /*!< STSCLKDIV_CUR (Bit 16)                                */
#define CLKCTR_SYSCLKDIV_STSCLKDIV_CUR_Msk (0x1f0000UL)             /*!< STSCLKDIV_CUR (Bitfield-Mask: 0x1f)                   */
/* ======================================================  QSPICLKDIV  ======================================================= */
#define CLKCTR_QSPICLKDIV_QSPICLKDIV_Pos  (0UL)                     /*!< QSPICLKDIV (Bit 0)                                    */
#define CLKCTR_QSPICLKDIV_QSPICLKDIV_Msk  (0x1fUL)                  /*!< QSPICLKDIV (Bitfield-Mask: 0x1f)                      */
#define CLKCTR_QSPICLKDIV_QSPICLKDIV_CUR_Pos (16UL)                 /*!< QSPICLKDIV_CUR (Bit 16)                               */
#define CLKCTR_QSPICLKDIV_QSPICLKDIV_CUR_Msk (0x1f0000UL)           /*!< QSPICLKDIV_CUR (Bitfield-Mask: 0x1f)                  */

/* ======================================================  GNSSCLKDIV  ======================================================= */
#define CLKCTR_GNSSCLKDIV_GNSSCLKDIV_Pos  (0UL)                     /*!< GNSSCLKDIV (Bit 0)                                    */
#define CLKCTR_GNSSCLKDIV_GNSSCLKDIV_Msk  (0x7UL)                   /*!< GNSSCLKDIV (Bitfield-Mask: 0x07)                      */
#define CLKCTR_GNSSCLKDIV_GNSSCLKDIV_CUR_Pos (16UL)                 /*!< GNSSCLKDIV_CUR (Bit 16)                               */
#define CLKCTR_GNSSCLKDIV_GNSSCLKDIV_CUR_Msk (0x70000UL)            /*!< GNSSCLKDIV_CUR (Bitfield-Mask: 0x07)                  */

#define CLKCTR_FORCE_CLK_DEV_EN 1


#define CLKCTR_PLL_CFG_NR_MAN(x) \
	(((x) << CLKCTR_PLLCFG_NR_MAN_Pos) & CLKCTR_PLLCFG_NR_MAN_Msk)
#define CLKCTR_PLL_CFG_NF_MAN(x) \
	(((x) << CLKCTR_PLLCFG_NF_MAN_Pos) & CLKCTR_PLLCFG_NF_MAN_Msk)
#define CLKCTR_PLL_CFG_OD_MAN(x) \
	(((x) << CLKCTR_PLLCFG_OD_MAN_Pos) & CLKCTR_PLLCFG_OD_MAN_Msk)
#define CLKCTR_PLL_CFG_MAN(x) \
	(((x)<< CLKCTR_PLLCFG_MAN_Pos) & CLKCTR_PLLCFG_MAN_Msk)
#define CLKCTR_PLL_CFG_SEL(x) \
	(((x) << CLKCTR_PLLCFG_SEL_Pos) & CLKCTR_PLLCFG_SEL_Msk)


static uint32_t MAINCLK_FREQUENCY = CONF_CLKCTRL_CLK_ENABLE_HFI_FREQ();

int clkctr_get_pll_config(struct clkctr_regs *base,
		struct clkctr_pll_cfg *config) {
	uint32_t reg = base->PLLCFG;

	config->lock = (reg & CLKCTR_PLLCFG_LOCK_Msk) >> CLKCTR_PLLCFG_LOCK_Pos;
	config->nr_man = (reg & CLKCTR_PLLCFG_NR_MAN_Msk)
			>> CLKCTR_PLLCFG_NR_MAN_Pos;
	config->nf_man = (reg & CLKCTR_PLLCFG_NF_MAN_Msk)
			>> CLKCTR_PLLCFG_NF_MAN_Pos;
	config->od_man = (reg & CLKCTR_PLLCFG_OD_MAN_Msk)
			>> CLKCTR_PLLCFG_OD_MAN_Pos;
	config->man = (reg & CLKCTR_PLLCFG_MAN_Msk) >> CLKCTR_PLLCFG_MAN_Pos;
	config->sel = (reg & CLKCTR_PLLCFG_SEL_Msk) >> CLKCTR_PLLCFG_SEL_Pos;

	return 0;
}

int clkctr_set_pll_config(struct clkctr_regs *base,
		struct clkctr_pll_cfg config) {
	uint32_t pll_cfg = CLKCTR_PLL_CFG_NR_MAN(
			config.nr_man) | CLKCTR_PLL_CFG_NF_MAN(config.nr_man)
			| CLKCTR_PLL_CFG_OD_MAN(config.od_man)
			| CLKCTR_PLL_CFG_MAN(config.man)
			| CLKCTR_PLL_CFG_SEL(config.sel);

	if (config.sel != 0) {
		/* stop PLL. */
		base->PLLCFG = 0;
		/* start PLL. */
		base->PLLCFG = pll_cfg;
		while ((base->PLLCFG & CLKCTR_PLLCFG_LOCK_Msk) == 0) {
			;
		}
	} else {
		base->PLLCFG = pll_cfg;
	}

	return 0;
}

int clkctr_set_switch_main_clk(struct clkctr_regs *base, uint32_t value) {
	uint32_t cfgReg = base->CFG;

	cfgReg &= ~CLKCTR_CFG_MAINCLK_SEL_Msk;
	cfgReg |= (value << CLKCTR_CFG_MAINCLK_SEL_Pos);
	base->CFG = cfgReg;

	return 0;
}

int clkctr_set_switch_pll_ref(struct clkctr_regs *base, uint32_t value) {
	uint32_t cfgReg = base->CFG;

	cfgReg &= ~(CLKCTR_CFG_PLLREF_SEL_Msk);
	cfgReg |= (value << CLKCTR_CFG_PLLREF_SEL_Pos);
	base->CFG = cfgReg;

	return 0;
}

void clkctr_set_pll(struct clkctr_regs *base, uint32_t xti_hz, uint16_t pll_mul) {
	struct clkctr_pll_cfg config;

	clkctr_set_switch_main_clk(base, CLKCTR_CLK_TYPE_HFI);
	clkctr_get_pll_config(base, &config);
	config.sel = 0;
	clkctr_set_pll_config(base, config);

	/* PLLCLK (HFI or XTI). */
	if (xti_hz == 0) {
		clkctr_set_switch_pll_ref(base, CLKCTR_CLK_TYPE_HFI);
		MAINCLK_FREQUENCY = CONF_CLKCTRL_CLK_ENABLE_HFI_FREQ();
	} else {
		clkctr_set_switch_pll_ref(base, CLKCTR_CLK_TYPE_XTI);
		MAINCLK_FREQUENCY = xti_hz;
	}

	if (pll_mul > PLL_MAX_MULTIPLIER) {
		pll_mul = PLL_MAX_MULTIPLIER;
	}

	if ((MAINCLK_FREQUENCY * (pll_mul + 1)) > CLKCTR_PLLCLK_MAX) {
		pll_mul = (CLKCTR_PLLCLK_MAX / MAINCLK_FREQUENCY) - 1;
	}

	if ((MAINCLK_FREQUENCY * (pll_mul + 1)) < CLKCTR_PLLCLK_OD_MIN) {
		pll_mul = PLL_MIN_MULTIPLIER;
	}

	if (pll_mul > PLL_MIN_MULTIPLIER) {
		clkctr_get_pll_config(base, &config);
		config.man = 0;
		config.sel = pll_mul;
		clkctr_set_pll_config(base, config);
		clkctr_set_switch_main_clk(base, CLKCTR_CLK_TYPE_PLL);
		MAINCLK_FREQUENCY = MAINCLK_FREQUENCY * (pll_mul + 1);
	}
}

uint32_t clk_get_sys_clk_div(struct clkctr_regs *base) {
	return ((base->SYSCLKDIV & CLKCTR_SYSCLKDIV_STSCLKDIV_Msk)
			>> CLKCTR_SYSCLKDIV_STSCLKDIV_Pos) + 1;
}

uint32_t clkctr_set_sys_clk_div(struct clkctr_regs *base, uint32_t value) {

	base->SYSCLKDIV = ((value - 1) << CLKCTR_SYSCLKDIV_STSCLKDIV_Pos)
			& CLKCTR_SYSCLKDIV_STSCLKDIV_Msk;

	while (clk_get_sys_clk_div(base) != value)
		;

	return value;
}

uint32_t clkctr_get_fclk_div(struct clkctr_regs *base) {
	return ((base->FCLKDIV & CLKCTR_FCLKDIV_FCLKDIV_Msk)
			>> CLKCTR_FCLKDIV_FCLKDIV_Pos) + 1;
}

uint32_t clkctr_set_fclk_div(struct clkctr_regs *base, uint32_t value) {

	base->FCLKDIV = ((value - 1) << CLKCTR_FCLKDIV_FCLKDIV_Pos)
			& CLKCTR_FCLKDIV_FCLKDIV_Msk;

	while (clkctr_get_fclk_div(base) != value) {
		;
	}

	return value;
}

uint32_t clkctr_get_qspi_clk_div(struct clkctr_regs *base) {
	return ((base->QSPICLKDIV & CLKCTR_QSPICLKDIV_QSPICLKDIV_Msk)
			>> CLKCTR_QSPICLKDIV_QSPICLKDIV_Pos) + 1;
}

uint32_t clkctr_set_qspi_clk_div(struct clkctr_regs *base, uint32_t value) {

	base->QSPICLKDIV = ((value - 1) << CLKCTR_QSPICLKDIV_QSPICLKDIV_Pos)
			& CLKCTR_QSPICLKDIV_QSPICLKDIV_Msk;

	while (clkctr_get_qspi_clk_div(base) != value) {
		;
	}

	return value;
}

uint32_t clkctr_set_gnss_clk_div(struct clkctr_regs *base, uint32_t value) {

	base->GNSSCLKDIV = ((value - 1) << CLKCTR_GNSSCLKDIV_GNSSCLKDIV_Pos)
			& CLKCTR_GNSSCLKDIV_GNSSCLKDIV_Msk;

#ifdef CLKCTR_NO_GNNS_BUG_ELIOT1RTL_2
    while (CLKCTR_GetGNSSClkDiv(base) != value)
        ;
#endif

	return value;
}

void clkctr_set_sys_div(struct clkctr_regs *base, uint16_t fclk_div,
		uint16_t sysclk_div, uint16_t gnssclk_div, uint16_t qspiclk_div) {
	if (sysclk_div > CLKCTR_MAX_SYSCLK_DIV) {
		sysclk_div = CLKCTR_MAX_SYSCLK_DIV;
	}
	clkctr_set_sys_clk_div(base, sysclk_div + 1);

	if (fclk_div > CLKCTR_MAX_FCLK_DIV) {
		fclk_div = CLKCTR_MAX_FCLK_DIV;
	}
	clkctr_set_fclk_div(base, fclk_div + 1);

	if (gnssclk_div > CLKCTR_MAX_GNSSCLK_DIV) {
		gnssclk_div = CLKCTR_MAX_GNSSCLK_DIV;
	}
	clkctr_set_gnss_clk_div(base, gnssclk_div + 1);

	if (qspiclk_div > CLKCTR_MAX_QSPICLK_DIV) {
		qspiclk_div = CLKCTR_MAX_QSPICLK_DIV;
	}
	clkctr_set_qspi_clk_div(base, qspiclk_div + 1);
}
