#include <string.h>
#include <assert.h>
#include <util/log.h>
#include <time.h>
#include <hal/clock.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

#include <config/custom_config_qspi.h>

#include <sys_clock_mgr.h>
#include <hw_sys.h>

extern const struct clock_source *cs_jiffies;

static void switch_jiffies_to(int freq) {
	struct clock_source *cs = (struct clock_source *) cs_jiffies;
	struct time_counter_device *cd = cs->counter_device;

	cd->cycle_hz = freq;

	clock_source_set_periodic(cs, clock_freq());
}

void clk_update_hook(void) {
	static uint16_t cur_clk = (1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_XTAL32M_Pos);
	uint16_t clk;

#define FREQ_MASK \
	((1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_LP_CLK_Pos) | \
	(1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_RC32M_Pos) | \
	(1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_XTAL32M_Pos) | \
	(1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_PLL96M_Pos))

	clk = CRG_TOP->CLK_CTRL_REG & FREQ_MASK;

	switch (clk) {
	case (1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_XTAL32M_Pos):
		if (cur_clk != clk) {
			switch_jiffies_to(32000000);
		}
		break;
	case (1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_PLL96M_Pos):
		if (cur_clk != clk) {
			switch_jiffies_to(96000000);
		}
		break;
	case (1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_LP_CLK_Pos):
	case (1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_RC32M_Pos):
	default:
		break;
	}

	cur_clk = clk;

#undef FREQ_MASK
}
