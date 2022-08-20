/**
 * @file
 *
 * @date 06 july 2015
 * @author Anton Bondarev
 */
#include <libactuators/motor.h>
#include "stm32f3xx_hal.h"
#include <stm32f3xx_hal_gpio.h>
#include <stm32f3_discovery.h>


static void stm32f3_delay(uint32_t delay) {
	while(delay--)
		;
}


static void init_pins(GPIO_TypeDef  *GPIOx, uint16_t pins) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	//TODO Is this required?
	__HAL_RCC_GPIOD_CLK_ENABLE();

	GPIO_InitStruct.Pin = pins;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void motor_init(struct motor *m) {

	m->port = GPIOD;
	m->enable = MOTOR_ENABLE1;
	m->input[0] = MOTOR_INPUT1;
	m->input[1] = MOTOR_INPUT2;

	init_pins(m->port, m->enable | m->input[0] | m->input[1]);
}

void motor_enable(struct motor *m) {
	HAL_GPIO_WritePin(m->port, m->enable, GPIO_PIN_SET);
}


void motor_disable(struct motor *m) {
	HAL_GPIO_WritePin(m->port, m->enable, GPIO_PIN_RESET);
}


void motor_run(struct motor *m, enum motor_run_direction dir) {
	uint8_t input_pin;

	input_pin = (dir == MOTOR_RUN_FORWARD) ? 1 : 0;

	HAL_GPIO_WritePin(m->port, m->input[!input_pin], GPIO_PIN_RESET);
	stm32f3_delay(0xFF); /* FIXME may be it is not needed */
	HAL_GPIO_WritePin(m->port, m->input[input_pin], GPIO_PIN_SET);
}

void motor_stop(struct motor *m) {
	stm32f3_delay(1000);
	HAL_GPIO_WritePin(m->port, m->enable, GPIO_PIN_RESET);
	stm32f3_delay(1000);
	HAL_GPIO_WritePin(m->port, m->input[0], GPIO_PIN_RESET);
	stm32f3_delay(1000);
	HAL_GPIO_WritePin(m->port, m->input[1], GPIO_PIN_RESET);
	stm32f3_delay(1000);
}
