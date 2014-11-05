/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.11.2014
 */

#include <err.h>
#include <errno.h>
#include <util/array.h>

#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>

#include "libleddrv.h"

struct leddrv_desc {
	GPIO_TypeDef *gpio;
	unsigned long pin;
};

static const struct leddrv_desc leddrv_leds[] = {
	{ GPIOD, GPIO_Pin_12 },
	{ GPIOD, GPIO_Pin_13 },
	{ GPIOD, GPIO_Pin_14 },
	{ GPIOD, GPIO_Pin_15 },
};

int leddrv_init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* NOTE GPIOD hardcoded */

	/* Enable GPIOs clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	return 0;
}

static inline const struct leddrv_desc *leddrv_get(unsigned int led_n) {
	if (led_n >= ARRAY_SIZE(leddrv_leds)) {
		return err_ptr(EINVAL);
	}

	return &leddrv_leds[led_n];
}

int leddrv_set(unsigned int led_n) {
	const struct leddrv_desc *led = leddrv_get(led_n);

	if (err(led)) {
		return err(led);
	}

	GPIO_SetBits(led->gpio, led->pin);

	return 0;
}

int leddrv_clr(unsigned int led_n) {
	const struct leddrv_desc *led = leddrv_get(led_n);

	if (err(led)) {
		return err(led);
	}

	GPIO_ResetBits(led->gpio, led->pin);

	return 0;
}
