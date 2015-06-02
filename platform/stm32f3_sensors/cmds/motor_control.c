/**
 * @file
 * @brief
 *
 * @author  Alex Kalmuk
 * @date    31.05.2015
 */

#include <stm32f3xx.h>
#include <stm32f3_discovery.h>
#include <stm32f3xx_hal_gpio.h>
#include <stm32f3xx_hal_rcc.h>

#define MOTOR_ENABLE1  GPIO_PIN_3
#define MOTOR_INPUT1   GPIO_PIN_5
#define MOTOR_INPUT2   GPIO_PIN_7

static uint8_t user_button_pressed;

struct motor {
	GPIO_TypeDef  *GPIOx;
	uint16_t enable;
	uint16_t input[2];
	/* 10, 01, 11 */
	uint8_t enabled_inputs:2;
};

enum motor_run_direction {
	MOTOR_RUN_LEFT,
	MOTOR_RUN_RIGHT
};

static struct motor motor1;

static void stm32f3_delay(uint32_t delay) {
	while(delay--)
		;
}

static void init_pins(GPIO_TypeDef  *GPIOx, uint16_t pins) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	GPIO_InitStruct.Pin = pins;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

static void motor_init(struct motor *m, GPIO_TypeDef  *GPIOx, 
		uint16_t enable, uint16_t in1, uint16_t in2, uint8_t mask) {
	m->GPIOx = GPIOx;
	m->enable = enable;
	m->input[0] = in1;
	m->input[1] = in2;
	m->enabled_inputs = mask;
}

static void motor_enable(struct motor *m) {
	HAL_GPIO_WritePin(GPIOD, m->enable, GPIO_PIN_SET);
}

#if 0
static void motor_disable(struct motor *m) {
	HAL_GPIO_WritePin(GPIOD, m->enable, GPIO_PIN_RESET);
}
#endif

static void motor_run(struct motor *m, enum motor_run_direction dir) {
	uint8_t input;

	if (dir == MOTOR_RUN_LEFT) {
		input = 0;
	} else if (dir == MOTOR_RUN_RIGHT) {
		input = 1;
	}
	HAL_GPIO_WritePin(GPIOD, m->input[~input], GPIO_PIN_RESET);
	stm32f3_delay(0xFF); /* FIXME may be it is not needed */
	HAL_GPIO_WritePin(GPIOD, m->input[input], GPIO_PIN_SET);
}

static void motor_stop(struct motor *m) {
	HAL_GPIO_WritePin(GPIOD, m->input[0], GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, m->input[1], GPIO_PIN_RESET);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (USER_BUTTON_PIN == GPIO_Pin) {
		user_button_pressed = 1;
		while (BSP_PB_GetState(BUTTON_USER) != RESET);
		user_button_pressed = 0;
	}
}

int main(void) {
	HAL_Init();

	__GPIOD_CLK_ENABLE();

	init_pins(GPIOD, MOTOR_ENABLE1 | MOTOR_INPUT1 | MOTOR_INPUT2);
	motor_init(&motor1, GPIOD, MOTOR_ENABLE1, MOTOR_INPUT1, 
		MOTOR_INPUT2, 0x3);

	BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
	
	motor_enable(&motor1);
	while(1) {
		while(!user_button_pressed)
			;
		motor_run(&motor1, MOTOR_RUN_LEFT);
		while(user_button_pressed)
			;
		motor_run(&motor1, MOTOR_RUN_RIGHT);
	}

	return 0;
}
