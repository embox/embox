/**
 * @file
 * @brief  Test for extended sleep. It enters extended sleep,
 *         then exits on timer trigger and blinks LED.
 *
 * @date   07.10.2020
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <kernel/time/time.h>
#include <framework/cmd/api.h>
#include <framework/mod/options.h>

#include <config/custom_config_qspi.h>
#include <hw_gpio.h>

#define LOOP_CNT     OPTION_GET(NUMBER, loop_cnt)
#define SLEEP_SEC    OPTION_GET(NUMBER, sleep_sec)

/* LED configuration section */
#define LED1_PORT       (HW_GPIO_PORT_1)
#define LED1_PIN        (HW_GPIO_PIN_1)
#define LED1_MODE       (HW_GPIO_MODE_OUTPUT)
#define LED1_FUNC       (HW_GPIO_FUNC_GPIO)

extern int deep_usleep(useconds_t usec);

static void busy_loop(unsigned volatile n) {
	while (n--) {

	}
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

static void deepsleep_test(void) {
	int led;
	int cnt;

	led = 1;
	toggle_led(led);
	busy_loop(1 * 1000000);
	led ^= 1;
	toggle_led(led);

	for (cnt = 0; cnt < LOOP_CNT; cnt++) {
		deep_usleep(SLEEP_SEC * USEC_PER_SEC);

		led ^= 1;
		toggle_led(led);
	}
}

int main(int argc, char **argv) {
	deepsleep_test();

	return 0;
}
