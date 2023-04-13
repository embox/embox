/**
 * @file
 * @brief Manage servo with STM32F746G-Discovery
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2020-08-27
 */

/* Read "servo.h" for details of how servo actually works */

#include <string.h>
#include <hal/reg.h>
#include <drivers/servo/servo.h>
//#include <stm32f7_discovery.h>
#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_tim.h>
#include <util/log.h>

static TIM_HandleTypeDef servo_thandle;
static TIM_OC_InitTypeDef servo_output_config;


/**
 * @brief  Timer setup
 *
 * @return Zero. If fails, handle_error() is called
 */
static int stm32f7_pwm_timer_init(void) {
	__HAL_RCC_TIM3_CLK_ENABLE();

	servo_thandle.Instance = TIM3;

	servo_thandle.Init.Prescaler         = 72;
	servo_thandle.Init.Period            = 20000;
	servo_thandle.Init.ClockDivision     = 0;
	servo_thandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	servo_thandle.Init.RepetitionCounter = 0;

	if (HAL_TIM_PWM_Init(&servo_thandle) != HAL_OK) {
		return -1;
	}

	return 0;
}

/*
 * @brief Initialize PWM parameters
 *
 * @return Zero. Code is too simple for considering any errors
 */
static int stm32f7_pwm_init(void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef PORT;
	memset(&PORT, 0, sizeof(PORT));
	PORT.Pin = GPIO_PIN_4;
	PORT.Pull = GPIO_PULLUP;
	PORT.Mode = GPIO_MODE_AF_PP;
	PORT.Speed = GPIO_SPEED_FREQ_HIGH;
	PORT.Alternate = GPIO_AF2_TIM3;
	HAL_GPIO_Init(GPIOB, &PORT);

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
static int stm32f7_servo_init(struct servo_dev *dev) {
	(void)dev;

	if (stm32f7_pwm_timer_init()) {
		return -1;
	}


	if (stm32f7_pwm_init()) {
		log_error("Failed to initialize stm32f7servo\n");
		return -1;
	}

	return 0;
}

#define SERVO_0    500  /* 180 degrees */
#define SERVO_180  3400 /* 0   degrees */

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
static int stm32f7_servo_set(struct servo_dev *dev, int pos) {
	(void) dev;

	if (pos < MIN_POS)
		pos = MIN_POS;

	if (pos > MAX_POS)
		pos = MAX_POS;

	pos = SERVO_0 + pos * (SERVO_180 - SERVO_0) / (MAX_POS - MIN_POS);

	servo_output_config.Pulse = pos;

	if (HAL_TIM_PWM_ConfigChannel(&servo_thandle,
	                              &servo_output_config,
	                              TIM_CHANNEL_1) != HAL_OK) {
		return -1;
	}

	if (HAL_TIM_PWM_Start(&servo_thandle, TIM_CHANNEL_1) != HAL_OK) {
		return -1;
	}

	return 0;
}

const static struct servo_ops stm32f7_servo_ops = {
	.init = stm32f7_servo_init,
	.set_pos = stm32f7_servo_set,
};

SERVO_DEV_DEF(&stm32f7_servo_ops, NULL);
