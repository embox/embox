/**
 * @file
 * @brief Input interface of nxt
 *
 * @date 02.12.2010
 * @author Anton Kozlov
 */


#ifndef NXT_SENSOR_H_
#define NXT_SENSOR_H_

#include <drivers/nxt_avr.h>

enum port_type_t { PASSIVE, ACTIVE};

typedef struct {
	uint8_t id;
	uint8_t n0p;
	uint8_t n1p;
	enum port_type_t type;
} sensor_t;

extern sensor_t sensors[NXT_AVR_N_INPUTS];

typedef uint16_t sensor_val_t;

typedef void (*sensor_hnd_t)(sensor_t *sensor, sensor_val_t sensor_val);

extern void nxt_sensor_conf_pass(sensor_t *sensor, sensor_hnd_t handler);

extern void nxt_sensor_conf_actv(sensor_t *sensor, sensor_hnd_t handler);

extern sensor_val_t nxt_sensor_get_value(sensor_t *sensor);

extern void sensors_updated(sensor_val_t sensor_vals[]);

extern void sensors_init(void);

#endif /*NXT_SENSOR_H_*/
