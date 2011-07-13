/**
 * @file
 * @brief nxt sensors driver
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <drivers/nxt/avr.h>
#include <drivers/nxt/sensor.h>
#include <drivers/soft_i2c.h>

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

extern from_avr_t data_from_avr;

static nxt_sensor_t sensors[NXT_AVR_N_INPUTS];

static sensor_handler_t handlers[NXT_AVR_N_INPUTS];
struct nxt_sensor *nxt_get_sensor(int num) {
	if (0 > num || NXT_AVR_N_INPUTS > num) {
		return NULL;
	}
	return &sensors[num];
}

void nxt_sensor_conf_pass(nxt_sensor_t *sensor, sensor_handler_t handler) {
	handlers[sensor->id] = handler;
	sensor->type = NXT_SENSOR_PASSIVE;
}

void nxt_sensor_conf_active(nxt_sensor_t *sensor) {
	i2c_port_t *port = &(sensor->i2c_port);
	i2c_init(port);
	sensor->type = NXT_SENSOR_ACTIVE;
}

sensor_val_t nxt_sensor_active_get_val(nxt_sensor_t *sensor, uint8_t command) {
	i2c_port_t *port = &(sensor->i2c_port);
	uint8_t active_val;

	i2c_write(port, 1, &command, 1);
	while (port->state != SOFT_I2C_IDLE) {
	}
	i2c_read(port, 1, &active_val, 1);
	while (port->state != SOFT_I2C_IDLE) {
	}
	return active_val;
}


sensor_val_t nxt_sensor_get_val(nxt_sensor_t *sensor) {
	if (sensor->type == NXT_SENSOR_PASSIVE) {
		return data_from_avr.adc_value[sensor->id];
	}
	if (sensor->type == NXT_SENSOR_ACTIVE) {
		return nxt_sensor_active_get_val(sensor, sensor->def_comm);
	}
	return NXT_SENSOR_ERR_NOT_CONFIG;
}

void sensors_updated(sensor_val_t sensor_vals[]) {
	size_t i;
	for (i = 0; i < NXT_AVR_N_INPUTS; i++) {
		if (sensors[i].type == NXT_SENSOR_PASSIVE && handlers[i]) {
			handlers[i](&sensors[i], sensor_vals[i]);
		}
	}
}

void sensors_init(void) {
	size_t i;
	for (i = 0; i < NXT_AVR_N_INPUTS; i++) {
		sensors[i].id = i;
		sensors[i].type = NXT_SENSOR_NONE;
		sensors[i].i2c_port.scl = 1 << digiS0[i];
		sensors[i].i2c_port.sda = 1 << digiS1[i];
		sensors[i].i2c_port.state = SOFT_I2C_OFF;
	}
}
