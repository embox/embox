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

#define PWM_FREQ 8 /* KHz */

/* pins for tacho impulse */
#define NXT_PIN_MOTOR_A0 15
#define NXT_PIN_MOTOR_A1 1
#define NXT_PIN_MOTOR_B0 26
#define NXT_PIN_MOTOR_B1 9
#define NXT_PIN_MOTOR_C0 0
#define NXT_PIN_MOTOR_C1 8

int pin_motor_S0[] = {
	NXT_PIN_MOTOR_A0,
	NXT_PIN_MOTOR_B0,
	NXT_PIN_MOTOR_C0
};

int pin_motor_S1[] = {
	NXT_PIN_MOTOR_A1,
	NXT_PIN_MOTOR_B1,
	NXT_PIN_MOTOR_C1
};

EMBOX_UNIT_INIT(nxt_motor_init);

motor_t motors[3];

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
	data_to_avr.output_percent[motor->id] = power;
}

static void motor_pin_handler(int ch_mask, int mon_mask) {
	size_t i;
	for (i = 0; i < NXT_AVR_N_OUTPUTS; i++) {
		if (pin_motor_S0[i] & mon_mask) {
			motors[i].tacho_count--;
			if (motors[i].tacho_count == 0) {
				motors[i].tacho_count = motors[i].tacho_limit;
				if (motors[i].limit_hnd != NULL) {
					motors[i].limit_hnd();
				}
			}
			break;
		}
	}
}

static int nxt_motor_init(void) {
	size_t i;
	for (i = 0; i < NXT_N_MOTORS; i++) {
		motors[i].m_0 = pin_motor_S0[i];
		motors[i].m_1 = pin_motor_S1[i];
		motors[i].pin_handler = (pin_handler_t) motor_pin_handler;
		motors[i].id = i;
	}

	data_to_avr.output_mode = PWM_FREQ;
	return 0;
}

