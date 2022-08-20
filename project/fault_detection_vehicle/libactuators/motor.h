/**
 * @file
 *
 * @date 06 july 2015
 * @author Anton Bondarev
 */

#ifndef PLATFORM_STM32F3_SENSORS_LIBACTUATORS_MOTOR_H_
#define PLATFORM_STM32F3_SENSORS_LIBACTUATORS_MOTOR_H_

#include <stdint.h>

/* motor 1*/
//#define MOTOR_ENABLE1  GPIO_PIN_3
//#define MOTOR_INPUT1   GPIO_PIN_5
//#define MOTOR_INPUT2   GPIO_PIN_7
#define MOTOR_ENABLE1_PIN  3
#define MOTOR_INPUT1_PIN   5
#define MOTOR_INPUT2_PIN   7

#define MOTOR_ENABLE1  (1 << MOTOR_ENABLE1_PIN)
#define MOTOR_INPUT1   (1 << MOTOR_INPUT1_PIN)
#define MOTOR_INPUT2   (1 << MOTOR_INPUT2_PIN)

/* motor 2 */
#define MOTOR_ENABLE2  GPIO_PIN_2
#define MOTOR_INPUT3   GPIO_PIN_4
#define MOTOR_INPUT4   GPIO_PIN_6

struct motor {
	uint16_t port;
	uint16_t enable;
	uint16_t input[2];
};

enum motor_run_direction {
	MOTOR_RUN_FORWARD,
	MOTOR_RUN_BACKWARD
};

extern void motor_init(struct motor *m);

extern void motor_enable(struct motor *m);

extern void motor_disable(struct motor *m);

extern void motor_run(struct motor *m, enum motor_run_direction dir);

extern void motor_stop(struct motor *m);

#endif /* PLATFORM_STM32F3_SENSORS_LIBACTUATORS_MOTOR_H_ */
