/**
 * @file
 * @brief
 *
 * @date 18.03.12
 * @author Anton Kozlov
 */

#include <unistd.h>
#include <drivers/gpio.h>
#include <embox/test.h>

EMBOX_TEST(blinking_led);

#define LED_BLUE (1 << 8) /* port C, pin 8 */
#define LED_GREEN (1 << 9) /* port C, pin 9 */

static inline void led1_level(unsigned int level) {
	gpio_set_level(GPIO_C, LED_BLUE, level);
}

static int blinking_led(void) {
	int count = 5;
	int state;

	gpio_settings(GPIO_C, LED_BLUE | LED_GREEN, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_C, LED_BLUE, 0x1);
	gpio_set_level(GPIO_C, LED_GREEN, 0);

	state = 0;
	while (count) {
		state = !state;
		led1_level(state);
		if (!state) {
			count--;
		}
		sleep(1);
	}
	gpio_set_level(GPIO_C, LED_GREEN, 0);

	return 0;
}
