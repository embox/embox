/**
 * @file
 * @brief Input interface of nxt
 *
 * @date 02.12.2010
 * @author Anton Kozlov
 */


#ifndef NXT_SENSOR_H_
#define NXT_SENSOR_H_

typedef uint8_t sensor_t;
typedef uint16_t sensor_val_t;

typedef void (*sensor_hnd_t)(sensor_t sensor, sensor_val_t sensor_val);

extern void nxt_sensor_conf_pass(sensor_t sensor, sensor_hnd_t handler);

extern void nxt_sensor_conf_actv(sensor_t sensor, sensor_hnd_t handler);

extern sensor_val_t nxt_sensor_get_value(sensor_t sensor);

extern void sensors_updated(sensor_val_t sensor_vals[]);

#endif /*NXT_SENSOR_H_*/
