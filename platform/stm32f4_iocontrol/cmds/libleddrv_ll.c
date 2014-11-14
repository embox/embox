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

#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>

#include "libleddrv_ll.h"

#define LEDS_PER_LINE 8
#define LINES_N 10
static_assert(LINES_N * LEDS_PER_LINE == LEDDRV_LED_N);

struct leddrv_pin_desc {
	GPIO_TypeDef *gpio; /**< gpio port  */
	unsigned int pin; /**< pin mask */
};

static const struct leddrv_pin_desc leddrv_clk =
	{ .gpio = GPIOE, .pin = GPIO_Pin_7 };
static const struct leddrv_pin_desc leddrv_datas[] = {
	{ .gpio = GPIOE, .pin = GPIO_Pin_8 },
	{ .gpio = GPIOE, .pin = GPIO_Pin_9 },
	{ .gpio = GPIOE, .pin = GPIO_Pin_10 },
	{ .gpio = GPIOE, .pin = GPIO_Pin_11 },
	{ .gpio = GPIOE, .pin = GPIO_Pin_12 },
	{ .gpio = GPIOE, .pin = GPIO_Pin_13 },
	{ .gpio = GPIOE, .pin = GPIO_Pin_14 },
	{ .gpio = GPIOE, .pin = GPIO_Pin_15 },
	{ .gpio = GPIOB, .pin = GPIO_Pin_14 },
	{ .gpio = GPIOB, .pin = GPIO_Pin_15 },
};
static_assert(ARRAY_SIZE(leddrv_datas) == LINES_N);

void leddrv_ll_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOs clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOB, ENABLE);

	/* Base GPIOs config for output */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = \
		GPIO_Pin_7 |
		GPIO_Pin_8 |
		GPIO_Pin_9 |
		GPIO_Pin_10 |
		GPIO_Pin_11 |
		GPIO_Pin_12 |
		GPIO_Pin_13 |
		GPIO_Pin_14 |
		GPIO_Pin_15;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = \
		GPIO_Pin_14 |
		GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void leddrv_ll_shift_out(int led_in_line, unsigned char leds_state[LEDDRV_LED_N]) {

	/* set clock low */
	GPIO_ResetBits(leddrv_clk.gpio, leddrv_clk.pin);

	/* set data in for all `led_in_line'-th led */
	for (int line = 0; line < LINES_N; ++line) {
		const struct leddrv_pin_desc *line_desk = &leddrv_datas[line];
		const bool led_state = leds_state[led_in_line + line * LEDS_PER_LINE ];

		if (led_state) {
			/* set data high */
			GPIO_SetBits(line_desk->gpio, line_desk->pin);
		} else {
			/* set data low */
			GPIO_ResetBits(line_desk->gpio, line_desk->pin);
		}
	}

	/* make clock high, shift data to regsiter */
	GPIO_SetBits(leddrv_clk.gpio, leddrv_clk.pin);
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
