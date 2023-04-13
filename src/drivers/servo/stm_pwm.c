/**
 * @file
 * @brief Manage servo with STM32
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-04-04
 */

/* Read "servo.h" for details of how servo actually works */

#include <string.h>
#include <hal/reg.h>
#include <drivers/servo/servo.h>
#include <drivers/servo/stm32_servo_conf.h>
#include <util/log.h>
#include <kernel/printk.h>

#include <config/board_config.h>

static TIM_HandleTypeDef TimHandle;
static TIM_OC_InitTypeDef sConfig;

/*
 * @brief Initialize PWM parameters
 *
 * @return Zero. Code is too simple for considering any errors
 */
static int stm32_pwm_init(void) {
	CONF_PWM0_CLK_ENABLE_GPIO();

	GPIO_InitTypeDef PORT;
	memset(&PORT, 0, sizeof(PORT));
	PORT.Pin = CONF_PWM0_PIN_TIM_NR;
	PORT.Pull = GPIO_PULLUP;
	PORT.Mode = GPIO_MODE_AF_PP;
	PORT.Speed = GPIO_SPEED_FREQ_HIGH;
	PORT.Alternate = CONF_PWM0_PIN_TIM_AF;
	HAL_GPIO_Init(CONF_PWM0_PIN_TIM_PORT, &PORT);

	CONF_PWM0_CLK_ENABLE_TIM();
        memset(&TimHandle, 0, sizeof(TimHandle));

        TimHandle.Instance = CONF_PWM0_TIM_INSTANCE();
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
        sConfig.Pulse = 10000;
        if (HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfig, CONF_PWM0_CHANNEL_CHANNEL_TIM()) != HAL_OK)
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
static int stm32_servo_init(struct servo_dev *dev) {
	(void)dev;

	if (stm32_pwm_init()) {
		log_error("Failed to initialize stm32servo\n");
		return -1;
	}

	return 0;
}

#define SERVO_0    CONF_PWM0_SERVO_POS_LOW  /*   0 degrees */
#define SERVO_180  CONF_PWM0_SERVO_POS_HIGH /* 180 degrees */

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
static int stm32_servo_set(struct servo_dev *dev, int pos) {
	(void)dev;

	if (pos < MIN_POS)
		pos = MIN_POS;

	if (pos > MAX_POS)
		pos = MAX_POS;

	pos = SERVO_0 + pos * (SERVO_180 - SERVO_0) / (MAX_POS - MIN_POS);

	sConfig.Pulse = pos;

	if (HAL_TIM_PWM_ConfigChannel(&TimHandle,
	                              &sConfig,
	                              CONF_PWM0_CHANNEL_CHANNEL_TIM()) != HAL_OK) {
		return -1;
	}

	if (HAL_TIM_PWM_Start(&TimHandle, CONF_PWM0_CHANNEL_CHANNEL_TIM()) != HAL_OK) {
		return -1;
	}

	return 0;
}

const static struct servo_ops stm32_servo_ops = {
	.init = stm32_servo_init,
	.set_pos = stm32_servo_set,
};

SERVO_DEV_DEF(&stm32_servo_ops, NULL);
