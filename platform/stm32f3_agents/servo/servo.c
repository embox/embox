/**
 * @file
 * @brief Manage servo with STM32F3Discovery
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-04-04
 */

/* General plan of working with servo is taken from
 * this article: http://www.kaltpost.de/?page_id=412 */

#include <string.h>

#include <drivers/gpio.h>

#include "stm32f3xx_hal_tim.h"

static int _timer_init(void) {
        __HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM2_FORCE_RESET();

	uint32_t cr1 = TIM2->CR1;

	cr1 &= ~(TIM_CR1_CKD | TIM_CR1_CMS | TIM_CR1_DIR);
	cr1 |= TIM_CR1_ARPE | TIM_CR1_OPM;
	TIM2->CR1 = cr1;
	TIM2->PSC = 72;
	TIM2->RCR = 0;
	TIM2->ARR = 2000000;

	return 0;
}

static int _pwm_init() {
	__HAL_RCC_GPIOD_CLK_ENABLE();

	GPIO_InitTypeDef PORT;
	memset(&PORT, 0, sizeof(PORT));
	PORT.Pin = GPIO_PIN_4;
	PORT.Mode = GPIO_MODE_AF_PP;
	PORT.Speed = GPIO_SPEED_FREQ_HIGH;
	PORT.Alternate = GPIO_AF2_TIM2;
	HAL_GPIO_Init(GPIOD, &PORT);

	TIM2->CCER &= ~TIM_CCER_CC2E;

	TIM2->CCMR1 &= ~TIM_CCMR1_CC2S;
	TIM2->CCMR1 &= ~TIM_CCMR1_OC2M;
	TIM2->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
	TIM2->CCR2 = 0;

	TIM2->CCER |= TIM_CCER_CC2E;

	return 0;
}

int servo_init(void) {
	int err;

	if (HAL_Init() != HAL_OK)
		return -1;

	if ((err = _timer_init()))
		return err;

	if ((err = _pwm_init()))
		return err;

	return 0;
}

int servo_set(int pos) {
	TIM2->CCR2 = pos;
	return 0;
}
