/**
 * @file stm32_gpio_cube.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-02-12
 */

#include <assert.h>
#include <string.h>

#include <drivers/gpio.h>
#include <drivers/gpio/stm32.h>

static void stm32_gpio_clk_enable(void *gpio_base) {
	if (gpio_base == GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
	else if (gpio_base == GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
	else if (gpio_base == GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();
	else if (gpio_base == GPIOD)
		__HAL_RCC_GPIOD_CLK_ENABLE();
	else if (gpio_base == GPIOE)
		__HAL_RCC_GPIOE_CLK_ENABLE();
	else if (gpio_base == GPIOF)
		__HAL_RCC_GPIOF_CLK_ENABLE();
	else
		assert(0);
}

static void stm32_gpio_init(void *gpio_base) {
	GPIO_InitTypeDef GPIO_InitStruct;

	stm32_gpio_clk_enable(gpio_base);

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

	GPIO_InitStruct.Pin = GPIO_PIN_All;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;

	HAL_GPIO_Init(gpio_base, &GPIO_InitStruct);
}

void gpio_set_level(struct gpio *gpio, gpio_mask_t mask, char level) {
	stm32_gpio_init(gpio);
	HAL_GPIO_WritePin((void*) gpio, mask, level ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

extern gpio_mask_t gpio_get_level(struct gpio *gpio, gpio_mask_t mask) {
	stm32_gpio_init(gpio);
	return (gpio_mask_t) HAL_GPIO_ReadPin((void*) gpio, mask);
}
