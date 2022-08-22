/**
 * @file
 * @brief Manage servo with STM32F4Discovery
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-04-04
 */

/* Read "servo.h" for details of how servo actually works */

#include <string.h>
#include <hal/reg.h>
#include <drivers/servo/servo.h>
#include <stm32f4_discovery.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_tim.h>
#include <util/log.h>
#include <kernel/printk.h>

static TIM_HandleTypeDef TimHandle;
static TIM_OC_InitTypeDef sConfig;

/*
 * @brief Initialize PWM parameters
 *
 * @return Zero. Code is too simple for considering any errors
 */
static int stm32f4_pwm_init(void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_TIM4_CLK_ENABLE();

	GPIO_InitTypeDef PORT;
	memset(&PORT, 0, sizeof(PORT));
	PORT.Pin = GPIO_PIN_6;
	PORT.Pull = GPIO_PULLUP;
	PORT.Mode = GPIO_MODE_AF_PP;
	PORT.Speed = GPIO_SPEED_FREQ_HIGH;
	PORT.Alternate = GPIO_AF2_TIM4;
	HAL_GPIO_Init(GPIOB, &PORT);

        memset(&TimHandle, 0, sizeof(TimHandle));

        TimHandle.Instance = TIM4;
        TimHandle.Init.Prescaler         = 72;
        TimHandle.Init.Period            = 20000;
        TimHandle.Init.ClockDivision     = 0;
        TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
        TimHandle.Init.RepetitionCounter = 0;
        TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        if (HAL_TIM_PWM_Init(&TimHandle) != HAL_OK) {
          log_error("Failed to init TIM PWM\n");
          return -1;
        }

        memset(&sConfig, 0, sizeof(sConfig));
        /* Common configuration for all channels */
        sConfig.OCMode       = TIM_OCMODE_PWM1;
        sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
        sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
        sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
        sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

        sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;

        /* Set the pulse value for channel 1 */
        sConfig.Pulse = PULSE1_VALUE;
        if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, TIM_CHANNEL_1) != HAL_OK)
        {
          log_error("Failed to config TIM PWM channel\n");
          return -1;
        }

        if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK) {
          log_error("Failed to start TIM PWM\n");
          return -1;
        }

	return 0;
}

/**
 * @brief Initialize timer and PWM output
 *
 * @return Always zero. In case of actual error handle_error()
 * is called
 */
static int stm32f4_servo_init(struct servo_dev *) {
	if (stm32f4_pwm_init()) {
		log_error("Failed to initialize stm32f4servo\n");
		return -1;
	}

	return 0;
}

#define SERVO_0    200  /* 180 degrees */
#define SERVO_180  1350 /* 0   degrees */

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
static int stm32f4_servo_set(struct servo_dev *, int pos) {
	if (pos < MIN_POS)
		pos = MIN_POS;

	if (pos > MAX_POS)
		pos = MAX_POS;

	pos = SERVO_0 + pos * (SERVO_180 - SERVO_0) / (MAX_POS - MIN_POS);

	sConfig.Pulse = pos;

	if (HAL_TIM_PWM_ConfigChannel(&TimHandle,
	                              &sConfig,
	                              TIM_CHANNEL_1) != HAL_OK) {
		return -1;
	}

	if (HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1) != HAL_OK) {
		return -1;
	}

	return 0;
}

const static struct servo_ops stm32f4_servo_ops = {
	.init = stm32f4_servo_init,
	.set_pos = stm32f4_servo_set,
};

SERVO_DEV_DEF(&stm32f4_servo_ops, NULL);
