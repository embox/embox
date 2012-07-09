/**
 * @file
 * @brief
 *
 * @date 18.03.12
 * @author Anton Kozlov
 */

#include <unistd.h>
#include <hal/reg.h>
#include <kernel/time/timer.h>
#include <drivers/gpio.h>
#include <embox/test.h>


EMBOX_TEST(blinking_led);

#define LED_BLUE (1 << 8) /* port C, pin 8 */
#define LED_GREEN (1 << 9) /* port C, pin 9 */

static inline void led1_on(void) {
	gpio_set(GPIO_C, LED_BLUE);
}

static inline void led1_off(void) {
	gpio_clear(GPIO_C, LED_BLUE);
}

static void timer_hdn(sys_timer_t *tmd, void *param) {
	static int state;
	int *count = (int *) param;

	state ^= 1;
	*count -= 1;

	if (state) {
		led1_on();
		return;
	}

	led1_off();
}


static int blinking_led(void) {
	sys_timer_t tmr;
	int count = 10;

	gpio_out(GPIO_C, LED_BLUE | LED_GREEN, 0);
	gpio_clear(GPIO_C, LED_BLUE);
	gpio_set(GPIO_C, LED_GREEN);

	timer_init(&tmr, TIMER_PERIODIC, 1000, timer_hdn, &count);

	while (count) { } ;

	timer_close(&tmr);
	gpio_clear(GPIO_C, LED_GREEN);

	return 0;
}
