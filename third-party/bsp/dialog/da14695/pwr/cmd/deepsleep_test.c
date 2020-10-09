/**
 * @file
 * @brief  Test for extended sleep. It enters extended sleep,
 *         then exits on timer trigger and blinks LED.
 *
 * @date   07.10.2020
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <string.h>
#include <framework/cmd/api.h>
#include <hal/ipl.h>
#include <kernel/time/time.h>

#include <config/custom_config_qspi.h>
#include <hw_clk.h>
#include <hw_gpio.h>
#include <hw_bod.h>
#include <hw_pmu.h>
#include <hw_pdc.h>
#include <hw_watchdog.h>
#include <qspi_automode.h>

#include <framework/mod/options.h>

#define LOOP_CNT OPTION_GET(NUMBER, loop_cnt)
#define SLEEP_TIME OPTION_GET(NUMBER, sleep_time)

/* LED configuration section */
#define LED1_PORT       (HW_GPIO_PORT_1)
#define LED1_PIN        (HW_GPIO_PIN_1)
#define LED1_MODE       (HW_GPIO_MODE_OUTPUT)
#define LED1_FUNC       (HW_GPIO_FUNC_GPIO)

static void busy_loop(unsigned volatile n) {
	while (n--) {

	}
}

extern void da1469x_timer_set(int trigger);
extern void set_wakeup_reset_handler(void);
extern bool goto_deepsleep(void);

static __RETAINED_CODE void prepare_for_deepsleep(void) {
	//hw_sys_pd_com_disable();
	//hw_sys_pd_periph_disable();
	qspi_automode_flash_power_down();
}

static __RETAINED_CODE void resume_after_deepsleep(void) {
	qspi_automode_flash_power_up();
	//hw_sys_pd_periph_enable();
	//hw_sys_pd_com_enable();
}

static void toggle_led(int led_is_on) {
	if (led_is_on) {

	HW_GPIO_SET_PIN_FUNCTION(LED1);
	hw_gpio_set_active(LED1_PORT, LED1_PIN);
	HW_GPIO_PAD_LATCH_ENABLE(LED1);
	HW_GPIO_PAD_LATCH_DISABLE(LED1);
	} else {


	HW_GPIO_SET_PIN_FUNCTION(LED1);
	hw_gpio_set_inactive(LED1_PORT, LED1_PIN);
	HW_GPIO_PAD_LATCH_ENABLE(LED1);
	HW_GPIO_PAD_LATCH_DISABLE(LED1);

	}
}

static __RETAINED_CODE void deepsleep_test(void) {
	bool entered_sleep;
	int led;
	int cnt;
	ipl_t ipl;

	led = 1;
	toggle_led(led);
	busy_loop(1 * 1000000);
	led ^= 1;
	toggle_led(led);

	for (cnt = 0; cnt < LOOP_CNT; cnt++) {
		ipl = ipl_save();

		da1469x_timer_set(SLEEP_TIME * 100);

		prepare_for_deepsleep();

		entered_sleep = goto_deepsleep();
		if (!entered_sleep) {
			while (1) {
			}
		}

		resume_after_deepsleep();
		ipl_restore(ipl);

		hw_watchdog_freeze();                   // Stop watchdog
		hw_watchdog_set_pos_val(dg_configWDOG_IDLE_RESET_VALUE);

		/* Re-enable system timer */
		jiffies_init();

		led ^= 1;
		toggle_led(led);
	}
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

int main(int argc, char **argv) {
	hw_clk_set_lpclk(LP_CLK_IS_XTAL32K);
	/* I am not sure if it's required, but it's for LP clock settling time.
	 * Should be removed or improved. */
	busy_loop(1000000);

	configure_trigger();

	set_wakeup_reset_handler();

	deepsleep_test();

	return 0;
}
