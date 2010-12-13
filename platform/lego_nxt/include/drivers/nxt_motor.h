/**
 * @file
 * @brief Motor driver interface
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#ifndef NXT_MOTOR_H_
#define NXT_MOTOR_H_

#include <types.h>
#include <drivers/pins.h>

#define NXT_N_MOTORS 3

typedef void (*tacholimit_hnd_t)(void);

typedef enum {
	STOP,
	RUN
} motor_state_t;

typedef struct {
	uint8_t id;
	uint32_t m_0, m_1;
	uint32_t tacho_limit;
	tacholimit_hnd_t limit_hnd;
	uint32_t tacho_count;
	pin_handler_t pin_handler;
	motor_state_t state;
} motor_t;

extern motor_t motors[];

/**
 * Start motor
 * @param motor Motor
 * @param power Power for motor:
 *		0    means stop
 *		-100 means full conterclockwise
 *		100	 measn full clockwise
 * @param limit Count of tacho ticks before handler call
 * @param lim_handler Handler called when @link limit @endlink ticks passed
 */
extern void motor_start(motor_t *motor, int8_t power, uint32_t limit,
			tacholimit_hnd_t lim_handler);
/**
 * Set power of running motor
 * @param motor Motor
 * @param power Power
 */
extern void motor_set_power(motor_t *motor, int8_t power);

#endif /*NXT_MOTOR_H_*/
