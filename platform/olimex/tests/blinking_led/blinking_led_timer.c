/**
 * @file
 * @brief blinking led
 *
 * @date 01.10.10
 * @author Anton Kozlov
 */

#include <embox/test.h>
#include <drivers/at91_olimex_debug_led.h>
#include <drivers/pins.h>
#include <kernel/time/sys_timer.h>
#include <unistd.h>

EMBOX_TEST_SUITE("olimex blinking_led_timer");

static volatile int count = 6;

static char state;
static void led1_on(void) {
	pin_clear_output(OLIMEX_SAM7_LED1);
}
static void led1_off(void) {
	pin_set_output(OLIMEX_SAM7_LED1);
}

static void timer_hdn(sys_timer_t *tmd, void *param) {
	state ^= 1;

	count -= 1;

	if (state) {
		led1_on();
		return;
	}

	led1_off();
}

TEST_CASE("olimex blinking_led_timer test") {
	sys_timer_t tmr;
	pin_config_output(OLIMEX_SAM7_LED1 | OLIMEX_SAM7_LED2);

	sys_timer_init_start_msec(&tmr, SYS_TIMER_PERIODIC, 1000, timer_hdn, NULL);

	while (count) { } ;

	sys_timer_close(&tmr);

	return 0;
}
