/**
 * @file
 * @brief ESP32-C5 peripheral clock and reset initialization.
 *
 * @date Oct 31, 2026
 * @author Dmitrii Piliuk
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/clk/esp32c5_pcr.h>
#include <hal/reg.h>

#include <config/board_config.h>

#define PCR_BASE       CONF_PCR_REGION_BASE
#define PCR_CLK_EN     (1U << 0)
#define PCR_RST_EN     (1U << 1)
#define PCR_READY      (1U << 2)
#define PCR_SCLK_EN    (1U << 22)
#define PCR_DIV_SHIFT  12
#define PCR_DIV_MASK   (0xffU << PCR_DIV_SHIFT)
#define PCR_SEL_SHIFT  20
#define PCR_FRAC_MASK  0xfffU
#define PCR_POLL_LIMIT 100000U

struct esp32_clk_desc {
	uint16_t conf;
	uint16_t sclk;
	uint32_t sel_mask;
	uint32_t frac_mask;
	uint32_t ready_mask;
	/* Hardware mux encodings indexed by logical source, -1 if unsupported. */
	int8_t sources[ESP32_CLK_SRC_COUNT];
};

static const struct esp32_clk_desc clocks[ESP32_CLK_COUNT] = {
    [ESP32_CLK_UART0] =
        {
            0x00,
            0x04,
            3U << PCR_SEL_SHIFT,
            PCR_FRAC_MASK,
            PCR_READY,
            {0, 2, -1},
        },
    [ESP32_CLK_UART1] =
        {
            0x0c,
            0x10,
            3U << PCR_SEL_SHIFT,
            PCR_FRAC_MASK,
            PCR_READY,
            {0, 2, -1},
        },
    [ESP32_CLK_SPI2] =
        {
            0xc4,
            0xc8,
            3U << PCR_SEL_SHIFT,
            0,
            PCR_READY,
            {0, -1, 1},
        },
    [ESP32_CLK_I2C0] =
        {
            0x20,
            0x24,
            1U << PCR_SEL_SHIFT,
            PCR_FRAC_MASK,
            0,
            {0, -1, -1},
        },
};

int esp32_clk_get_source_rate(unsigned int source, uint32_t *rate) {
	if (!rate) {
		return -EINVAL;
	}
	switch (source) {
	case ESP32_CLK_SRC_XTAL:
		*rate = CONF_PCR_CLK_DEF_XTAL_FREQ;
		break;
	case ESP32_CLK_SRC_PLL80M:
		*rate = 80000000U;
		break;
	case ESP32_CLK_SRC_PLL160M:
		*rate = 160000000U;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

int esp32_clk_configure(unsigned int id, unsigned int source, unsigned int divider) {
	const struct esp32_clk_desc *clk;
	uint32_t reg, enabled;

	if (id >= ESP32_CLK_COUNT || source >= ESP32_CLK_SRC_COUNT || !divider
	    || divider > 256) {
		return -EINVAL;
	}
	clk = &clocks[id];
	if (clk->sources[source] < 0) {
		return -ENOTSUP;
	}

	reg = REG32_LOAD(PCR_BASE + clk->sclk);
	enabled = reg & PCR_SCLK_EN;
	REG32_STORE(PCR_BASE + clk->sclk, reg & ~PCR_SCLK_EN);
	reg &= ~(PCR_SCLK_EN | PCR_DIV_MASK | clk->sel_mask | clk->frac_mask);
	reg |= (divider - 1) << PCR_DIV_SHIFT;
	reg |= (uint32_t)clk->sources[source] << PCR_SEL_SHIFT;
	REG32_STORE(PCR_BASE + clk->sclk, reg);
	if (enabled) {
		REG32_STORE(PCR_BASE + clk->sclk, reg | enabled);
	}
	return 0;
}

int esp32_clk_enable(unsigned int id) {
	if (id >= ESP32_CLK_COUNT) {
		return -EINVAL;
	}
	REG32_ORIN(PCR_BASE + clocks[id].conf, PCR_CLK_EN);
	REG32_ORIN(PCR_BASE + clocks[id].sclk, PCR_SCLK_EN);
	return 0;
}

int esp32_clk_disable(unsigned int id) {
	if (id >= ESP32_CLK_COUNT) {
		return -EINVAL;
	}
	REG32_CLEAR(PCR_BASE + clocks[id].sclk, PCR_SCLK_EN);
	REG32_CLEAR(PCR_BASE + clocks[id].conf, PCR_CLK_EN);
	return 0;
}

int esp32_clk_get_rate(unsigned int id, uint32_t *rate) {
	const struct esp32_clk_desc *clk;
	uint32_t reg, mux, divider, numerator, denominator, source_rate;
	unsigned int source;
	int ret;

	if (id >= ESP32_CLK_COUNT || !rate) {
		return -EINVAL;
	}
	clk = &clocks[id];
	reg = REG32_LOAD(PCR_BASE + clk->sclk);
	mux = (reg & clk->sel_mask) >> PCR_SEL_SHIFT;
	for (source = 0; source < ESP32_CLK_SRC_COUNT; source++) {
		if (clk->sources[source] == (int)mux) {
			break;
		}
	}
	if (source == ESP32_CLK_SRC_COUNT) {
		return -ENOTSUP;
	}
	ret = esp32_clk_get_source_rate(source, &source_rate);
	if (ret) {
		return ret;
	}
	divider = ((reg & PCR_DIV_MASK) >> PCR_DIV_SHIFT) + 1;
	numerator = (reg & clk->frac_mask) >> 6;
	denominator = reg & clk->frac_mask & 0x3fU;
	if (numerator) {
		if (!denominator) {
			return -EINVAL;
		}
		*rate = (uint64_t)source_rate * denominator
		        / (divider * denominator + numerator);
	}
	else {
		*rate = source_rate / divider;
	}
	return 0;
}

int esp32_periph_reset(unsigned int id) {
	const struct esp32_clk_desc *clk;
	unsigned int timeout;

	if (id >= ESP32_CLK_COUNT) {
		return -EINVAL;
	}
	clk = &clocks[id];
	if (!(REG32_LOAD(PCR_BASE + clk->conf) & PCR_CLK_EN)) {
		return -EIO;
	}
	REG32_ORIN(PCR_BASE + clk->conf, PCR_RST_EN);
	REG32_CLEAR(PCR_BASE + clk->conf, PCR_RST_EN);
	for (timeout = PCR_POLL_LIMIT; timeout; timeout--) {
		if (!clk->ready_mask
		    || (REG32_LOAD(PCR_BASE + clk->conf) & clk->ready_mask)) {
			return 0;
		}
	}
	return -ETIMEDOUT;
}
