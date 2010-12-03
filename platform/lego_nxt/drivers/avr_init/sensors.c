/**
 * @file
 * @brief nxt sensors driver
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <drivers/nxt_avr.h>

#include <drivers/nxt_sensor.h>

static sensor_hnd_t handlers[NXT_AVR_N_INPUTS];

void nxt_sensor_conf_pass(sensor_t sensor, sensor_hnd_t handler) {
	handlers[sensor] = handler;
}

sensor_val_t nxt_sensor_get_value(sensor_t sensor) {
	return data_from_avr.adc_value[sensor];
}

void sensors_updated(sensor_val_t sensor_vals[]) {
	int i;
	for (i = 0; i < NXT_AVR_N_INPUTS; i++) {
		if (handlers[i]) {
			handlers[i](i, sensor_vals[i]);
		}
	}
}


