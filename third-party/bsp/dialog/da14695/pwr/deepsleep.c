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

#include <config/custom_config_qspi.h>
#include <hw_clk.h>
#include <hw_gpio.h>
#include <hw_bod.h>
#include <hw_pmu.h>
#include <hw_pdc.h>
#include <hw_watchdog.h>
#include <qspi_automode.h>

EMBOX_UNIT_INIT(deepsleep_init);

extern void lp_clock_enable(void);
extern void da1469x_timer_set(int trigger);
extern void set_wakeup_reset_handler(void);
extern bool goto_deepsleep(void);

extern struct clock_source *cs_jiffies;

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

__RETAINED_CODE int deep_usleep(useconds_t usec) {
	bool entered_sleep;
	ipl_t ipl;
	unsigned systimer_ticks, lp_timer_ticks;

	lp_timer_ticks = (usec * 100) / USEC_PER_SEC;
	/* Calculate to how many ticks system timer should be updated after sleep. */
	systimer_ticks = (lp_timer_ticks * cs_jiffies->event_device->event_hz) / 100;

	da1469x_timer_set(lp_timer_ticks);

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

	clock_handle_ticks(cs_jiffies, systimer_ticks);

	return 0;
}

/* Based on configure_pdc() */
static void configure_trigger(void) {
	uint32_t pdc_entry_index;

	REG_SETF(CRG_TOP, PMU_CTRL_REG, TIM_SLEEP, 0);
	while (REG_GETF(CRG_TOP, SYS_STAT_REG, TIM_IS_UP) == 0) {
	}

	pdc_entry_index = hw_pdc_add_entry(HW_PDC_LUT_ENTRY_VAL(
	                                        HW_PDC_TRIG_SELECT_PERIPHERAL,
	                                        HW_PDC_PERIPH_TRIG_ID_TIMER2,
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
