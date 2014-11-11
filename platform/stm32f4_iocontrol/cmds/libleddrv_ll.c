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

struct leddrv_shift_desc {
	GPIO_TypeDef *gpio; /**< gpio port of clk/data pair */
	unsigned int clk; /**< clk pin mask */
	unsigned int data; /**< data pin mask */
	unsigned led_n; /**< number of leds in line */
	unsigned shift_n; /**< capacity of shift-registers in line */
};

static const struct leddrv_shift_desc leddrv_shift_led[] = {
	{ .gpio = GPIOE, .clk = GPIO_Pin_7,  .data = GPIO_Pin_8, .led_n = 3, .shift_n = 8 },
#if 0
	{ .gpio = GPIOE, .clk = GPIO_Pin_9,  .data = GPIO_Pin_10 },
	{ .gpio = GPIOE, .clk = GPIO_Pin_11, .data = GPIO_Pin_12 },
	{ .gpio = GPIOE, .clk = GPIO_Pin_13, .data = GPIO_Pin_14 },
#endif
};

void leddrv_ll_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOs clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	/* Make GPIO config for output */
	GPIO_InitStructure.GPIO_Pin = \
		GPIO_Pin_7 |
		GPIO_Pin_8 |
		GPIO_Pin_9 |
		GPIO_Pin_10 |
		GPIO_Pin_11 |
		GPIO_Pin_12 |
		GPIO_Pin_13 |
		GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;

	/* Apply config */
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

static void leddrv_ll_shift_out(const struct leddrv_shift_desc *desc, bool val) {

	/* make clock low */
	GPIO_ResetBits(desc->gpio, desc->clk);

	if (val) {
		/* set data high */
		GPIO_SetBits(desc->gpio, desc->data);
	} else {
		/* set data low */
		GPIO_ResetBits(desc->gpio, desc->data);
	}

	/* make clock high, shift data to regsiter */
	GPIO_SetBits(desc->gpio, desc->clk);
}

void leddrv_ll_update(unsigned char leds_state[LEDDRV_LED_N]) {
	int i_desc;
	int base_led;

	base_led = 0;
	for (i_desc = 0; i_desc < ARRAY_SIZE(leddrv_shift_led); ++i_desc) {
		const struct leddrv_shift_desc *const desc = &leddrv_shift_led[i_desc];

		/* shift capacity could be larger than led number, shifting meaningless data */
		for (int i_led_skip = desc->shift_n - desc->led_n; i_led_skip > 0; --i_led_skip) {
			leddrv_ll_shift_out(desc, 0);
		}

		/* shift data in desceding order as shift-registers require */
		for (int i_led_shift = desc->led_n - 1; i_led_shift >= 0; --i_led_shift) {
			leddrv_ll_shift_out(desc, leds_state[base_led + i_led_shift]);
		}

		base_led += desc->led_n;
	}
}
