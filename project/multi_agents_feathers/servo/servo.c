/**
 * @file
 * @brief Manage servo with STM32F3Discovery
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-04-04
 */

/* Read "servo.h" for details of how servo actually works */

#include <string.h>
#include <hal/reg.h>
#include <stm32f3_discovery.h>
#include <stm32f3xx_hal.h>
#include <stm32f3xx_hal_tim.h>

static TIM_HandleTypeDef servo_thandle;
static TIM_OC_InitTypeDef servo_output_config;

/**
 * @brief Turn on red led on board and do nothing
 */
static void handle_error(void) {
	BSP_LED_On(LED3);
	while(1);
}

/**
 * @brief  Timer setup
 *
 * @return Zero. If fails, handle_error() is called
 */
static int _timer_init(void) {
	__HAL_RCC_TIM2_CLK_ENABLE();

	servo_thandle.Instance = TIM2;

	servo_thandle.Init.Prescaler         = 72;
	servo_thandle.Init.Period            = 20000;
	servo_thandle.Init.ClockDivision     = 0;
	servo_thandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	servo_thandle.Init.RepetitionCounter = 0;

	if (HAL_TIM_PWM_Init(&servo_thandle) != HAL_OK) {
		handle_error();
	}

	return 0;
}

/*
 * @brief Initialize PWM parameters
 *
 * @return Zero. Code is too simple for considering any errors
 */
static int _pwm_init() {
	__HAL_RCC_GPIOD_CLK_ENABLE();

	GPIO_InitTypeDef PORT;
	memset(&PORT, 0, sizeof(PORT));
	PORT.Pin = GPIO_PIN_4;
	PORT.Pull = GPIO_PULLUP;
	PORT.Mode = GPIO_MODE_AF_PP;
	PORT.Speed = GPIO_SPEED_FREQ_HIGH;
	PORT.Alternate = GPIO_AF2_TIM2;
	HAL_GPIO_Init(GPIOD, &PORT);

	servo_output_config.OCMode       = TIM_OCMODE_PWM1;
	servo_output_config.OCPolarity   = TIM_OCPOLARITY_HIGH;
	servo_output_config.OCFastMode   = TIM_OCFAST_DISABLE;
	servo_output_config.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
	servo_output_config.OCNIdleState = TIM_OCNIDLESTATE_RESET;

	servo_output_config.OCIdleState  = TIM_OCIDLESTATE_RESET;

	return 0;
}

/**
 * @brief Initialize timer and PWM output
 *
 * @return Always zero. In case of actual error handle_error()
 * is called
 */
int servo_init(void) {
	/* Required for handle_error() */
	BSP_LED_Init(LED3);

	if (HAL_Init() != HAL_OK)
		handle_error();

	if (_timer_init())
		handle_error();

	if (_pwm_init())
		handle_error();

	return 0;
}

#define SERVO_0    430  /* 180 degrees */
#define SERVO_180  2175 /* 0   degrees */

#define MIN_POS 0
#define MAX_POS 100

/**
 * @brief Set servo position
 *
 * @param pos  Should  be  between MIN_POS and MAX_POS,  but if it's
 * out of range, it will be adjusted. Without adjustment servo gears
 * can be damaged.
 *
 * @return Always zero.  In case of error board will turn on red LED
 * and stall. Look handle_error() for details.
 */
int servo_set(int pos) {
	if (pos < MIN_POS)
		pos = MIN_POS;

	if (pos > MAX_POS)
		pos = MAX_POS;

	pos = SERVO_0 + pos * (SERVO_180 - SERVO_0) / (MAX_POS - MIN_POS);

	servo_output_config.Pulse = pos;

	if (HAL_TIM_PWM_ConfigChannel(&servo_thandle,
	                              &servo_output_config,
	                              TIM_CHANNEL_2) != HAL_OK) {
		handle_error();
	}

	if (HAL_TIM_PWM_Start(&servo_thandle, TIM_CHANNEL_2) != HAL_OK) {
		handle_error();
	}

	return 0;
}
