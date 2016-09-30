/**
 * @file
 *
 * @date 06 july 2015
 * @author Anton Bondarev
 */
#include <libactuators/motor.h>
#include <stm32f3xx_hal_gpio.h>
#include <stm32f3_discovery.h>

static void stm32f3_delay(uint32_t delay) {
	while(delay--)
		;
}


static void init_pins(GPIO_TypeDef  *GPIOx, uint16_t pins) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	//TODO Is this required?
	__GPIOD_CLK_ENABLE();

	GPIO_InitStruct.Pin = pins;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void motor_init(struct motor *m, GPIO_TypeDef  *GPIOx, uint16_t enable,
		uint16_t in1, uint16_t in2) {
	m->GPIOx = GPIOx;
	m->enable = enable;
	m->input[0] = in1;
	m->input[1] = in2;

	init_pins(GPIOx, enable | in1 | in2);
}

void motor_enable(struct motor *m) {
	HAL_GPIO_WritePin(GPIOD, m->enable, GPIO_PIN_SET);
}


void motor_disable(struct motor *m) {
	HAL_GPIO_WritePin(GPIOD, m->enable, GPIO_PIN_RESET);
}


void motor_run(struct motor *m, enum motor_run_direction dir) {
	uint8_t input;

	if (dir == MOTOR_RUN_LEFT) {
		input = 0;
	} else if (dir == MOTOR_RUN_RIGHT) {
		input = 1;
	}
	HAL_GPIO_WritePin(GPIOD, m->input[!input], GPIO_PIN_RESET);
	stm32f3_delay(0xFF); /* FIXME may be it is not needed */
	HAL_GPIO_WritePin(GPIOD, m->input[input], GPIO_PIN_SET);
}

void motor_stop(struct motor *m) {
	stm32f3_delay(1000);
	HAL_GPIO_WritePin(GPIOD, m->enable, GPIO_PIN_RESET);
	stm32f3_delay(1000);
	HAL_GPIO_WritePin(GPIOD, m->input[0], GPIO_PIN_RESET);
	stm32f3_delay(1000);
	HAL_GPIO_WritePin(GPIOD, m->input[1], GPIO_PIN_RESET);
	stm32f3_delay(1000);
}
