/**
 * @file
 * @brief timer test -- blinking led
 *
 * @date 01.10.10
 * @author Anton Kozlov
 */

#include <embox/test.h>
#include <hal/clock.h>
#include <drivers/at91_olimex_debug_led.h>

EMBOX_TEST(run);

int state = false;
int changed = false;
/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
#if 0
void clock_tick_handler(int irq_num, void *dev_id) {
	changed = true;
	if (state) {
		led1_off();
		state = false;
	} else {
		led1_on();
		state = true;
	}
}
#endif
static int run(void) {
	int i = 0;
#if 0
	clock_init();
	led_init();

	clock_setup((useconds_t) 20);

	for (i = 0xffffff; i; i--) {
	}
#endif
	return (changed ? 0 : -1);
}
