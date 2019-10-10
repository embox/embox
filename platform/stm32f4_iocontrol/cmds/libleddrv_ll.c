/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    10.11.2014
 */

#include <assert.h>
#include <stdbool.h>
#include <util/array.h>

#include <drivers/gpio/gpio_driver.h>
#include <drivers/gpio/stm32.h>

#include "libleddrv_ll.h"

#define LEDS_PER_LINE 8
#define LINES_N 10
static_assert(LINES_N * LEDS_PER_LINE == LEDDRV_LED_N);

struct leddrv_pin_desc {
	int gpio; /**< port */
	int pin; /**< pin mask */
};

#define LEDBLOCK_MAJOR 2
#define LEDBLOCK_MINOR 0

#if LEDBLOCK_MAJOR == 0
static const struct leddrv_pin_desc leddrv_clk =
	{ .gpio = GPIO_PORT_E, .pin = (1 << 7) };

static const struct leddrv_pin_desc leddrv_datas[] = {
	/* prototype register numeration */
	{ .gpio = GPIO_PORT_B, .pin = (1 << 14) },
	{ .gpio = GPIO_PORT_B, .pin = (1 << 15) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 15) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 13) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 14) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 11) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 12) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 10) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 9) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 8) },
};
#endif

#if LEDBLOCK_MAJOR == 1 && LEDBLOCK_MINOR == 0
static const struct leddrv_pin_desc leddrv_clk =
	{ .gpio = GPIO_PORT_E, .pin = (1 << 7) };

static const struct leddrv_pin_desc leddrv_datas[] = {
	/* v1 register numeration, forward, wrong */
	{ .gpio = GPIO_PORT_E, .pin = (1 << 12) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 14) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 13) },
	{ .gpio = GPIO_PORT_B, .pin = (1 << 14) },
	{ .gpio = GPIO_PORT_B, .pin = (1 << 15) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 15) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 11) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 10) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 9) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 8) },
};
#endif

#if LEDBLOCK_MAJOR == 1 && LEDBLOCK_MINOR == 1
static const struct leddrv_pin_desc leddrv_clk =
	{ .gpio = GPIO_PORT_E, .pin = (1 << 8) };

static const struct leddrv_pin_desc leddrv_datas[] = {
	/* prototype register numeration */
	{ .gpio = GPIO_PORT_E, .pin = (1 << 11) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 13) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 14) },
	{ .gpio = GPIO_PORT_B, .pin = (1 << 14) },
	{ .gpio = GPIO_PORT_D, .pin = (1 << 8) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 15) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 12) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 9) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 10) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 7) },
};
#endif

#if LEDBLOCK_MAJOR == 2
static const struct leddrv_pin_desc leddrv_shf_clk =
	{ .gpio = GPIO_PORT_E, .pin = (1 << 8) };
static const struct leddrv_pin_desc leddrv_str_clk =
	{ .gpio = GPIO_PORT_D, .pin = (1 << 9) };
static const struct leddrv_pin_desc leddrv_datas[] = {
	{ .gpio = GPIO_PORT_E, .pin = (1 << 11) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 13) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 14) },
	{ .gpio = GPIO_PORT_B, .pin = (1 << 14) },
	{ .gpio = GPIO_PORT_D, .pin = (1 << 8) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 15) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 12) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 9) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 10) },
	{ .gpio = GPIO_PORT_E, .pin = (1 << 7) },
};

static const struct leddrv_pin_desc leddrv_alarms[] = {
	{ .gpio = GPIO_PORT_D, .pin = (1 << 3) },
	{ .gpio = GPIO_PORT_D, .pin = (1 << 4) },
	{ .gpio = GPIO_PORT_D, .pin = (1 << 1) },
	{ .gpio = GPIO_PORT_C, .pin = (1 << 12) },
	{ .gpio = GPIO_PORT_A, .pin = (1 << 10) },
	{ .gpio = GPIO_PORT_A, .pin = (1 << 8) },
	{ .gpio = GPIO_PORT_D, .pin = (1 << 2) },
	{ .gpio = GPIO_PORT_D, .pin = (1 << 7) },
	{ .gpio = GPIO_PORT_D, .pin = (1 << 5) },
	{ .gpio = GPIO_PORT_D, .pin = (1 << 6) },
};
#endif

static_assert(ARRAY_SIZE(leddrv_datas) == LINES_N);
static_assert(ARRAY_SIZE(leddrv_alarms) == LINES_N);

int leddrv_ll_error(int n) {
	int line = n / LEDS_PER_LINE;
	return !(gpio_get(leddrv_alarms[line].gpio, leddrv_alarms[line].pin)
			& leddrv_alarms[line].pin);
}

static inline void leddrv_pin_init(const struct leddrv_pin_desc *pd, unsigned int mode) {
	gpio_setup_mode(pd->gpio, pd->pin, mode);
}

void leddrv_ll_init(void) {
	leddrv_pin_init(&leddrv_shf_clk, GPIO_MODE_OUTPUT);
	leddrv_pin_init(&leddrv_str_clk, GPIO_MODE_OUTPUT);
	for (int i = 0; i < LINES_N; ++i) {
		leddrv_pin_init(&leddrv_datas[i], GPIO_MODE_OUTPUT);
	}

	for (int i = 0; i < LINES_N; ++i) {
		leddrv_pin_init(&leddrv_alarms[i], GPIO_MODE_INPUT);
	}
}

#if LEDBLOCK_MAJOR == 1
static void leddrv_ll_shift_out(int led_in_line, unsigned char leds_state[LEDDRV_LED_N]) {

	/* set clock low */
	gpio_set(leddrv_clk.gpio, leddrv_clk.pin, GPIO_PIN_LOW);

	/* set data in for all `led_in_line'-th led */
	for (int line = 0; line < LINES_N; ++line) {
		const struct leddrv_pin_desc *line_desk = &leddrv_datas[line];
		const bool led_state = leds_state[led_in_line + line * LEDS_PER_LINE ];

		if (led_state) {
			/* set data high */
			gpio_set(line_desk->gpio, line_desk->pin, GPIO_PIN_HIGH);
		} else {
			/* set data low */
			gpio_set(line_desk->gpio, line_desk->pin, GPIO_PIN_LOW);
		}
	}

	/* make clock high, shift data to regsiter */
	gpio_set(leddrv_clk.gpio, leddrv_clk.pin, GPIO_PIN_HIGH);
}


void leddrv_ll_update(unsigned char leds_state[LEDDRV_LED_N]) {

	/* shift-register connected as
	 * shift-out-0 -> led-4
	 * shift-out-1 -> led-5
	 * shift-out-2 -> led-6
	 * shift-out-3 -> led-7
	 * shift-out-4 -> led-3
	 * shift-out-5 -> led-2
	 * shift-out-6 -> led-1
	 * shift-out-7 -> led-0
	 * so have to shift first 4 in asceding, then next 4 in desceding
	 */

	for (int led_in_line = 0; led_in_line < 4; ++led_in_line) {
		leddrv_ll_shift_out(led_in_line, leds_state);
	}

	for (int led_in_line = LEDS_PER_LINE - 1; led_in_line >= 4; --led_in_line) {
		leddrv_ll_shift_out(led_in_line, leds_state);
	}

}
#endif

#if LEDBLOCK_MAJOR == 2
void leddrv_ll_update(unsigned char leds_state[LEDDRV_LED_N]) {

	gpio_set(leddrv_str_clk.gpio, leddrv_str_clk.pin, GPIO_PIN_HIGH);

	for (int i_led = LEDS_PER_LINE - 1; i_led >= 0; --i_led) {

		gpio_set(leddrv_shf_clk.gpio, leddrv_shf_clk.pin, GPIO_PIN_LOW);

		for (int i_line = 0; i_line < LINES_N; ++i_line) {
			const struct leddrv_pin_desc *line_desk = &leddrv_datas[i_line];
			const bool led_state = leds_state[i_line * LEDS_PER_LINE + i_led];

			if (led_state) {
				gpio_set(line_desk->gpio, line_desk->pin, GPIO_PIN_HIGH);
			} else {
				gpio_set(line_desk->gpio, line_desk->pin, GPIO_PIN_LOW);
			}
		}

		gpio_set(leddrv_shf_clk.gpio, leddrv_shf_clk.pin, GPIO_PIN_HIGH);
	}

	gpio_set(leddrv_str_clk.gpio, leddrv_str_clk.pin, GPIO_PIN_HIGH);
}
#endif
