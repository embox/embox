/**
 * @file
 * @brief Motor driver
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <assert.h>
#include <drivers/nxt/avr.h>
#include <drivers/pins.h>
#include <embox/unit.h>
#include <drivers/nxt/motor.h>

#define PWM_FREQ 8 /* KHz */

/* pins for tacho impulse */
#define NXT_PIN_MOTOR_A0 15
#define NXT_PIN_MOTOR_A1 1
#define NXT_PIN_MOTOR_B0 26
#define NXT_PIN_MOTOR_B1 9
#define NXT_PIN_MOTOR_C0 0
#define NXT_PIN_MOTOR_C1 8

static int pin_motor_S0[] = {
	NXT_PIN_MOTOR_A0,
	NXT_PIN_MOTOR_B0,
	NXT_PIN_MOTOR_C0
};

static int pin_motor_S1[] = {
	NXT_PIN_MOTOR_A1,
	NXT_PIN_MOTOR_B1,
	NXT_PIN_MOTOR_C1
};

EMBOX_UNIT_INIT(nxt_motor_unit_init);

extern to_avr_t data_to_avr;

static nxt_motor_t nxt_motors[NXT_N_MOTORS];

struct nxt_motor *nxt_get_motor(int num) {
	if (3 > num) {
		return NULL;
	}
	return &nxt_motors[num];

}

void nxt_motor_tacho_set_counter(nxt_motor_t *motor, uint32_t limit, tacho_handler_t lim_handler) {
	motor->limit_hnd = lim_handler;
	motor->tacho_limit = limit;
	motor->tacho_count = limit;

}


static void nxt_motor_init(nxt_motor_t *motor, int8_t power, uint32_t limit,
			tacho_handler_t lim_handler) {

	nxt_motor_tacho_set_counter(motor, limit, lim_handler);

	pin_set_input_monitor((1 << motor->m_0) | (1 << motor->m_1),
			(pin_handler_t) motor->pin_handler);

	data_to_avr.output_percent[motor->id] = power;
}


void nxt_motor_set_power(nxt_motor_t *motor, int8_t power) {
	data_to_avr.output_percent[motor->id] = power;
}

uint32_t nxt_motor_tacho_get_counter(nxt_motor_t *motor) {
	return motor->tacho_limit - motor->tacho_count;
}


static void motor_pin_handler(int ch_mask, int mon_mask) {
	size_t i;
	for (i = 0; i < NXT_AVR_N_OUTPUTS; i++) {
		if (pin_motor_S0[i] & mon_mask) {
			nxt_motors[i].tacho_count--;
			if (nxt_motors[i].tacho_count == 0) {
				nxt_motors[i].tacho_count = nxt_motors[i].tacho_limit;
				if (nxt_motors[i].limit_hnd != NULL) {
					nxt_motors[i].limit_hnd();
				}
			}
			break;
		}
	}
}

static int nxt_motor_unit_init(void) {
	size_t i;
	for (i = 0; i < NXT_N_MOTORS; i++) {
		nxt_motors[i].m_0 = pin_motor_S0[i];
		nxt_motors[i].m_1 = pin_motor_S1[i];
		nxt_motors[i].pin_handler = (pin_handler_t) motor_pin_handler;
		nxt_motors[i].id = i;
	}

	data_to_avr.output_mode = PWM_FREQ;

	nxt_motor_init(NXT_MOTOR_A, 0, 0xffffffff, NULL);
	nxt_motor_init(NXT_MOTOR_B, 0, 0xffffffff, NULL);
	nxt_motor_init(NXT_MOTOR_C, 0, 0xffffffff, NULL);
	return 0;
}
