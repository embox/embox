/**
 * @file
 * @brief
 *
 * @date 11.01.2017
 * @author Alex Kalmuk
 */

#ifndef STEPPER_MOTOR_H_
#define STEPPER_MOTOR_H_

#include "stm32f4xx_hal.h"

#include "stm32f4xx_hal_gpio.h"

#define MOTOR_MIN_DELAY            15000
#define MOTOR_MAX_SPEED            1 // Corresponds to MOTOR_MIN_DELAY
#define MOTOR_DEFAULT_SPEED        10 // Corresponds to MOTOR_MIN_DELAY * 10
#define MOTOR_STEP_SIZE            64
#define MOTOR_STEPS_PER_REVOLUTION 512

#define MOTOR_RUN_FORWARD          0
#define MOTOR_RUN_BACKWARD         1

struct stepper_motor {
	uint8_t speed;
	uint8_t step_size;
	uint8_t steps_cnt;
	uint16_t in1;
	uint16_t in2;
	uint16_t in3;
	uint16_t in4;
	GPIO_TypeDef  *GPIOx;
};

extern void motor_init(struct stepper_motor *m, uint16_t in1, uint16_t in2,
	uint16_t in3, uint16_t in4, GPIO_TypeDef  *GPIOx);
extern void motor_do_steps(struct stepper_motor *m, size_t nsteps, int direction);
extern void motor_do_steps2(struct stepper_motor *m1, struct stepper_motor *m2, size_t nsteps, int direction);

static inline void motor_set_speed(struct stepper_motor *m, size_t speed) {
	m->speed = speed;
}

#endif /* STEPPER_MOTOR_H_ */
