/**
 * @file
 * @brief
 *
 * @date 11.01.2017
 * @author Alex Kalmuk
 */

#include <string.h>
#include <assert.h>

#include "stm32f4_discovery.h"
#include "stepper_motor.h"

#define DO_STEPS(gpio1, in1, gpio2, in2, state) \
			do { \
				HAL_GPIO_WritePin(gpio1, in1, state); \
				HAL_GPIO_WritePin(gpio2, in2, state); \
			} while(0)

static void stm32_delay(uint32_t delay) {
	while (--delay > 0)
		;
}

void motor_init(struct stepper_motor *m, uint16_t in1, uint16_t in2,
		uint16_t in3, uint16_t in4, GPIO_TypeDef *GPIOx) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

	/* FIXME Add ability to use any GPIO */
	assert(GPIOx == GPIOD || GPIOx == GPIOE);
	if (GPIOx == GPIOD) {
		__GPIOD_CLK_ENABLE();
	} else if (GPIOx == GPIOE){
		__GPIOE_CLK_ENABLE();
	}

	m->in1 = in1;
	m->in2 = in2;
	m->in3 = in3;
	m->in4 = in4;
	m->GPIOx = GPIOx;

	GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
	
	GPIO_InitStruct.Pin = m->in1 | m->in2 | m->in3 | m->in4;
	HAL_GPIO_Init(m->GPIOx, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(m->GPIOx, m->in1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(m->GPIOx, m->in2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(m->GPIOx, m->in3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(m->GPIOx, m->in4, GPIO_PIN_RESET);

	m->speed = MOTOR_DEFAULT_SPEED;
	m->step_size = MOTOR_STEP_SIZE;
	m->steps_cnt = 0;
}

static void motor_do_one_step2(struct stepper_motor *m1, struct stepper_motor *m2, int direction) {
	size_t delay = m1->speed * MOTOR_MIN_DELAY;

	assert(direction == MOTOR_RUN_FORWARD || direction == MOTOR_RUN_BACKWARD);

	if (direction == MOTOR_RUN_BACKWARD) {
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in1, m2->GPIOx, m2->in1, GPIO_PIN_SET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in2, m2->GPIOx, m2->in2, GPIO_PIN_SET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in1, m2->GPIOx, m2->in1, GPIO_PIN_RESET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in3, m2->GPIOx, m2->in3, GPIO_PIN_SET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in2, m2->GPIOx, m2->in2, GPIO_PIN_RESET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in4, m2->GPIOx, m2->in4, GPIO_PIN_SET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in3, m2->GPIOx, m2->in3, GPIO_PIN_RESET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in1, m2->GPIOx, m2->in1, GPIO_PIN_SET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in4, m2->GPIOx, m2->in4, GPIO_PIN_RESET);
	} else {
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in1, m2->GPIOx, m2->in1, GPIO_PIN_SET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in4, m2->GPIOx, m2->in4, GPIO_PIN_SET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in1, m2->GPIOx, m2->in1, GPIO_PIN_RESET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in3, m2->GPIOx, m2->in3, GPIO_PIN_SET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in4, m2->GPIOx, m2->in4, GPIO_PIN_RESET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in2, m2->GPIOx, m2->in2, GPIO_PIN_SET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in3, m2->GPIOx, m2->in3, GPIO_PIN_RESET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in1, m2->GPIOx, m2->in1, GPIO_PIN_SET);
		stm32_delay(delay);
		DO_STEPS(m1->GPIOx, m1->in2, m2->GPIOx, m2->in2, GPIO_PIN_RESET);
	}
}

static void motor_do_one_step(struct stepper_motor *m, int direction) {
	size_t delay = m->speed * MOTOR_MIN_DELAY;

	assert(direction == MOTOR_RUN_FORWARD || direction == MOTOR_RUN_BACKWARD);

	if (direction == MOTOR_RUN_BACKWARD) {
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(m->GPIOx, m->in2, GPIO_PIN_SET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in1, GPIO_PIN_RESET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in3, GPIO_PIN_SET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in2, GPIO_PIN_RESET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in4, GPIO_PIN_SET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in3, GPIO_PIN_RESET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in1, GPIO_PIN_SET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in4, GPIO_PIN_RESET);
	} else {
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in1, GPIO_PIN_SET);
		HAL_GPIO_WritePin(m->GPIOx, m->in4, GPIO_PIN_SET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in1, GPIO_PIN_RESET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in3, GPIO_PIN_SET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in4, GPIO_PIN_RESET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in2, GPIO_PIN_SET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in3, GPIO_PIN_RESET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in1, GPIO_PIN_SET);
		stm32_delay(delay);
		HAL_GPIO_WritePin(m->GPIOx, m->in2, GPIO_PIN_RESET);
	}
}

void motor_do_steps(struct stepper_motor *m, size_t nsteps, int direction) {
	int i = nsteps;

	while (i-- > 0) {
		motor_do_one_step(m, direction);
	}
}

void motor_do_steps2(struct stepper_motor *m1, struct stepper_motor *m2,
		 size_t nsteps, int direction) {
	int i = nsteps;

	while (i-- > 0) {
		motor_do_one_step2(m1, m2, direction);
	}
}
