/**
 * @file
 * @brief nxt touch sensor driver
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#include <stdint.h>
#include <embox/test.h>
#include <unistd.h>
#include <drivers/nxt/avr.h>
#include <drivers/nxt/touch_sensor.h>

#define TOUCH_ADC_EDGE 500

static touch_hnd_t touch_sens_hnds[NXT_AVR_N_INPUTS];

typedef enum {
	NXT_TOUCH_STATE_UP,
	NXT_TOUCH_STATE_DOWN
} touch_state_t;

static touch_state_t state = NXT_TOUCH_STATE_UP;

static void touch_handler(nxt_sensor_t *sensor, sensor_val_t val) {
	touch_state_t new_state = (val <= TOUCH_ADC_EDGE ? NXT_TOUCH_STATE_DOWN : NXT_TOUCH_STATE_UP);
	if (new_state != state) {
		state = new_state;
		touch_sens_hnds[sensor->id](sensor);
	}
}

void touch_sensor_init (nxt_sensor_t *sensor, touch_hnd_t handler) {
	touch_sens_hnds[sensor->id] = handler;
	nxt_sensor_conf_pass(sensor, (sensor_handler_t) touch_handler);
}
