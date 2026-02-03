/**
 * @file
 *
 * @date   13.10.2020
 * @author Alexander Kalmuk
 */

#include <embox/unit.h>
#include <kernel/time/clock_source.h>
#include <hal/ipl.h>
#include <hal/clock.h>
#include <time.h>
#include <kernel/time/time.h>
#include <kernel/time/sys_timer.h>
#include <drivers/rtc.h>

#include <config/custom_config_qspi.h>
#include <hw_clk.h>
#include <hw_gpio.h>
#include <hw_bod.h>
#include <hw_pmu.h>
#include <hw_pdc.h>
#include <hw_timer.h>
#include <hw_watchdog.h>
#include <qspi_automode.h>

EMBOX_UNIT_INIT(deepsleep_init);

extern void lp_clock_enable(void);
extern void set_wakeup_reset_handler(void);
extern bool goto_deepsleep(void);

static struct clock_source *da1469x_timer;
static struct rtc_device *rtc_dev;

static __RETAINED_CODE void prepare_for_deepsleep(void) {
	hw_sys_pd_com_disable();
	hw_sys_pd_periph_disable();
	qspi_automode_flash_power_down();
}

static __RETAINED_CODE void resume_after_deepsleep(void) {
	qspi_automode_flash_power_up();
	hw_sys_pd_periph_enable();
	hw_sys_pd_com_enable();
}

static int deepsleep_rtc_set(int systimer_ticks) {
	time_t time;
	struct rtc_wkalrm rtc_wk;

	if (!rtc_dev) {
		rtc_dev = rtc_get_device(NULL);
		if (!rtc_dev) {
			return -1;
		}
	}

	if (systimer_ticks == -1) {
	    /* There is no next event, so sleep as long as possible, no alarm.
	     * We will be wake up with CMAC interrupt or another trigger. */
		return 0;
	}

	rtc_get_time(rtc_dev, &rtc_wk.tm);
	time = mktime(&rtc_wk.tm) + systimer_ticks / clock_freq();

	gmtime_r(&time, &rtc_wk.tm);

	rtc_set_alarm(rtc_dev, &rtc_wk);

	return 0;
}

static int deepsleep_gp_timer_set(int systimer_ticks) {
	unsigned int lp_timer_ticks;

	if (!da1469x_timer) {
		da1469x_timer = clock_source_get_by_name("da1469x_timer");
		if (!da1469x_timer) {
			return -1;
		}
	}

	lp_timer_ticks =
		(systimer_ticks * da1469x_timer->counter_device->cycle_hz) /
			clock_freq();

	clock_source_set_oneshot(da1469x_timer);
	clock_source_set_next_event(da1469x_timer, lp_timer_ticks);

	return 0;
}

__RETAINED_CODE int deepsleep_enter(void) {
	bool entered_sleep;
	ipl_t ipl;
	int systimer_ticks;
	clock_t next_event;
	int res;
	bool use_rtc = false;

	if (hw_sys_is_debugger_attached()) {
		return -1;
	}

	res = timer_strat_get_next_event(&next_event);

	if (res) {
		/* No next event */
		systimer_ticks = -1;
	} else {
		systimer_ticks = next_event - clock_sys_ticks();
	}

	if (res || (systimer_ticks / clock_freq() >= 1)) {
		/* Use RTC if want to sleep for more than 1 sec. */
		use_rtc = true;
	}

	if (use_rtc) {
		if (0 != deepsleep_rtc_set(systimer_ticks)) {
			return -1;
		}
	} else {
		if (0 != deepsleep_gp_timer_set(systimer_ticks)) {
			return -1;
		}
	}

	ipl = ipl_save();

	prepare_for_deepsleep();

	entered_sleep = goto_deepsleep();

	resume_after_deepsleep();
	ipl_restore(ipl);

	if (!entered_sleep) {
		/* TODO Stop timer */
		return -1;
	}

	hw_watchdog_freeze();                   // Stop watchdog
	hw_watchdog_set_pos_val(dg_configWDOG_IDLE_RESET_VALUE);

	/* Re-enable system timer */
	jiffies_init();

	if (systimer_ticks != -1) {
		jiffies_update(systimer_ticks);
	}

	return 0;
}

/* Based on configure_pdc() */
static void configure_trigger(void) {
	uint32_t pdc_entry_index;

	REG_SETF(CRG_TOP, PMU_CTRL_REG, TIM_SLEEP, 0);
	while (REG_GETF(CRG_TOP, SYS_STAT_REG, TIM_IS_UP) == 0) {
	}

	/* Timer 2 */
	pdc_entry_index = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(
	                                        HW_PDC_TRIG_SELECT_PERIPHERAL,
	                                        HW_PDC_PERIPH_TRIG_ID_TIMER2,
	                                        HW_PDC_MASTER_CM33,
	                                        (dg_configENABLE_XTAL32M_ON_WAKEUP ? HW_PDC_LUT_ENTRY_EN_XTAL : 0)));
	hw_pdc_set_pending(pdc_entry_index);
	hw_pdc_acknowledge(pdc_entry_index);

	/* RTC */
	pdc_entry_index = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(
	                                        HW_PDC_TRIG_SELECT_PERIPHERAL,
	                                        HW_PDC_PERIPH_TRIG_ID_RTC_ALARM,
	                                        HW_PDC_MASTER_CM33,
	                                        (dg_configENABLE_XTAL32M_ON_WAKEUP ? HW_PDC_LUT_ENTRY_EN_XTAL : 0)));
	hw_pdc_set_pending(pdc_entry_index);
	hw_pdc_acknowledge(pdc_entry_index);
}

static int deepsleep_init(void) {
	lp_clock_enable();

	configure_trigger();

	set_wakeup_reset_handler();

	return 0;
}
