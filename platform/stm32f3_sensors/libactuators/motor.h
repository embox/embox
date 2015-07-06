/**
 * @file
 *
 * @date 06 july 2015
 * @author Anton Bondarev
 */

#ifndef PLATFORM_STM32F3_SENSORS_LIBACTUATORS_MOTOR_H_
#define PLATFORM_STM32F3_SENSORS_LIBACTUATORS_MOTOR_H_

#include <stdint.h>
#include <stm32f3xx_hal_gpio.h>

/* motor 1*/
#define MOTOR_ENABLE1  GPIO_PIN_3
#define MOTOR_INPUT1   GPIO_PIN_5
#define MOTOR_INPUT2   GPIO_PIN_7
/* motor 2 */
#define MOTOR_ENABLE2  GPIO_PIN_2
#define MOTOR_INPUT3   GPIO_PIN_4
#define MOTOR_INPUT4   GPIO_PIN_6

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

extern void motor_init(struct motor *m, GPIO_TypeDef  *GPIOx,
		uint16_t enable, uint16_t in1, uint16_t in2, uint8_t mask);

extern void motor_enable(struct motor *m);

extern void motor_disable(struct motor *m);

extern void motor_run(struct motor *m, enum motor_run_direction dir);

extern void motor_stop(struct motor *m);

#endif /* PLATFORM_STM32F3_SENSORS_LIBACTUATORS_MOTOR_H_ */
