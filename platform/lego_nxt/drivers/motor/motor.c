/**
 * @file
 * @brief Motor driver
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <assert.h>
#include <drivers/nxt_avr.h>
#include <drivers/pins.h>
#include <embox/unit.h>

#include <drivers/nxt_motor.h>

#define PWM_FREQ 8/*KHz*/

/* pins for tacho impulse */
#define NXT_PIN_MOTOR_A0 15
#define NXT_PIN_MOTOR_A1 1
#define NXT_PIN_MOTOR_B0 26
#define NXT_PIN_MOTOR_B1 9
#define NXT_PIN_MOTOR_C0 0
#define NXT_PIN_MOTOR_C1 8

EMBOX_UNIT_INIT(nxt_motor_init);

motor_t motor1, motor2, motor3;

#define MOTOR_PIN_HANDLER(motor,mask) \
do {\
	(motor)->tacho_count--;\
	if ((motor)->tacho_count == 0) {\
		(motor)->tacho_count = (motor)->tacho_limit;\
		if ((motor)->limit_hnd != NULL)\
			(motor)->limit_hnd();\
	} \
} while (0)

#define MOTOR_INIT(motor, symbol) \
do {\
	(motor)->m_0 = NXT_PIN_MOTOR_##symbol##0;\
	(motor)->m_1 = NXT_PIN_MOTOR_##symbol##1;\
	(motor)->pin_handler = m##symbol##_pin_hnd; \
} while (0)

void motor_start(motor_t *motor, int8_t power, uint32_t limit,
			tacholimit_hnd_t lim_handler) {

	motor->state = RUN;
	motor->limit_hnd = lim_handler;
	motor->tacho_limit = limit;
	motor->tacho_count = limit;

	pin_set_input_monitor((1 << motor->m_0) | (1 << motor->m_1),
			(pin_handler_t) motor->pin_handler);

	data_to_avr.output_percent[motor->id] = power;
}

void motor_set_power(motor_t *motor, int8_t power) {
	assert(motor->state == RUN);
	if (power == 0) {
		motor->state = STOP;
	}
	data_to_avr.output_percent[motor->id] = power;
}

static void mA_pin_hnd(int mask) {
	MOTOR_PIN_HANDLER(&motor1, mask);
}

static void mB_pin_hnd(int mask) {
	MOTOR_PIN_HANDLER(&motor2, mask);
}

static void mC_pin_hnd(int mask) {
	MOTOR_PIN_HANDLER(&motor3, mask);
}

static int nxt_motor_init(void) {
	MOTOR_INIT(&motor1, A);
	MOTOR_INIT(&motor2, B);
	MOTOR_INIT(&motor3, C);

	data_to_avr.output_mode = PWM_FREQ;
	return 0;
}


