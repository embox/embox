/**
 * @file
 * @brief
 *
 * @date 10.05.2020
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <hw_cache.h>
#include <hw_otpc.h>
#include <hw_rtc.h>
#include <hw_sys.h>
#include <hw_watchdog.h>
#include <string.h>
#include <sys_clock_mgr.h>
#include <sys_power_mgr.h>
#include <time.h>

#include <arm/fpu.h>
#include <framework/mod/options.h>
#include <hal/cpu_idle.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>
#include <util/log.h>

#include <config/custom_config_qspi.h>

extern const struct clock_source *cs_jiffies;

static void switch_jiffies_to(int freq) {
	struct clock_source *cs = (struct clock_source *)cs_jiffies;
	struct time_counter_device *cd = cs->counter_device;

	cd->cycle_hz = freq;

	clock_source_set_periodic(cs, clock_freq());
}

void clk_update_hook(void) {
	static uint16_t cur_clk = 1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_XTAL32M_Pos;
	uint16_t clk;

#define FREQ_MASK                                            \
	((1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_LP_CLK_Pos)       \
	    | (1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_RC32M_Pos)   \
	    | (1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_XTAL32M_Pos) \
	    | (1 << CRG_TOP_CLK_CTRL_REG_RUNNING_AT_PLL96M_Pos))

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

extern int deepsleep_enter(void);

static uint32_t arch_deepsleep_flags = 0;

void arch_deepsleep_disable(uint32_t mask) {
	ipl_t ipl;

	ipl = ipl_save();
	arch_deepsleep_flags |= mask;
	ipl_restore(ipl);
}

void arch_deepsleep_enable(uint32_t mask) {
	ipl_t ipl;

	ipl = ipl_save();
	{ arch_deepsleep_flags &= ~mask; }
	ipl_restore(ipl);
}

static uint32_t arch_deepsleep_start(ipl_t *ipl) {
	*ipl = ipl_save();

	return arch_deepsleep_flags;
}

static void arch_deepsleep_finish(ipl_t ipl) {
	ipl_restore(ipl);
}

void arch_cpu_idle(void) {
	ipl_t ipl;

	clk_update_hook();

	if (arch_deepsleep_start(&ipl)) {
		__asm__ __volatile__("wfi");
	}
	else {
		deepsleep_enter();
	}
	arch_deepsleep_finish(ipl);
}
