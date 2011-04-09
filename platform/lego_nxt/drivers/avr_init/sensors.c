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

#define DIGIA0 23
#define DIGIA1 18
#define DIGIB0 28
#define DIGIB1 19
#define DIGIC0 29
#define DIGIC1 20
#define DIGID0 30
#define DIGID1 2

static int digiS0[] = { DIGIA0, DIGIB0, DIGIC0, DIGID0};
static int digiS1[] = { DIGIA1, DIGIB1, DIGIC1, DIGID1};

sensor_t sensors[NXT_AVR_N_INPUTS];
static sensor_val_t active_sensor_vals[NXT_AVR_N_INPUTS];

static sensor_hnd_t handlers[NXT_AVR_N_INPUTS];

void nxt_sensor_conf_pass(sensor_t *sensor, sensor_hnd_t handler) {
	handlers[sensor->id] = handler;
	sensor->type = PASSIVE;
}

sensor_val_t nxt_sensor_get_value(sensor_t *sensor) {
	if (sensor->type == PASSIVE) {
		return data_from_avr.adc_value[sensor->id];
	}
	return SENSOR_NOT_CONF;
	/*FIXME to handle active sensors */
}

void sensors_updated(sensor_val_t sensor_vals[]) {
	size_t i;
	for (i = 0; i < NXT_AVR_N_INPUTS; i++) {
		if (sensors[i].type == PASSIVE && handlers[i]) {
			handlers[i](&sensors[i], sensor_vals[i]);
		}
	}
}

void sensors_init(void) {
	size_t i;
	for (i = 0; i < NXT_AVR_N_INPUTS; i++) {
		sensors[i].id = i;
		sensors[i].type = NONE;
		sensors[i].i2c_port.scl = 1 << digiS0[i];
		sensors[i].i2c_port.sda = 1 << digiS1[i];
		sensors[i].i2c_port.state = OFF;
	}
}

