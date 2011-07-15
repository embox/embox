/**
 * @file
 * @brief Sensors interface for Lego NXT
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#ifndef NXT_SENSOR_H_
#define NXT_SENSOR_H_

#include <drivers/nxt/avr.h>
#include <drivers/soft_i2c.h>

#define NXT_SENSOR_ERR_NOT_CONFIG     42

/**
 * Sensor class type. Describes general interface for sensor
 */
typedef enum {
	NXT_SENSOR_NONE,   /**< Not used */
	NXT_SENSOR_PASSIVE,/**< Passive (touch, light,...) */
	NXT_SENSOR_ACTIVE, /**< Active (sonar, angle,...) */
} sensor_type_t;

/**
 * Sensor structure type
 */
struct nxt_sensor {
	uint8_t id;
	sensor_type_t type;
	uint8_t def_comm;
	i2c_port_t i2c_port;
};
/** type for @link #nxt_sensor sensor structure @endlink */
typedef struct nxt_sensor nxt_sensor_t;

#define NXT_N_SENSORS        NXT_AVR_N_INPUTS
/**
 * Get structure of sensor for specific port. You should use macros
 * NXT_SENSOR_X instead this function
 */
extern struct nxt_sensor *nxt_get_sensor(int num);

#define NXT_SENSOR_1     nxt_get_sensor(0)
#define NXT_SENSOR_2     nxt_get_sensor(1)
#define NXT_SENSOR_3     nxt_get_sensor(2)
#define NXT_SENSOR_4     nxt_get_sensor(3)

/**
 * Sensor value type. Returns as sensor measuring
 */
typedef uint16_t sensor_val_t;

/**
 * Sensor value changed handler. Called when sensor reads new value. Used only
 * with passive sensors
 *
 * @param sensor Sensor, which value changed
 * @param sensor_val Value of sensor
 */
typedef void (*sensor_handler_t)(nxt_sensor_t *sensor, sensor_val_t sensor_val);

/**
 * Configure sensor as passive and sets handler
 *
 * @param sensor Sensor to be configured
 * @param handler #sensor_handler_t or NULL as sensor value changed handler
 */
extern void nxt_sensor_conf_pass(nxt_sensor_t *sensor, sensor_handler_t handler);

/**
 * Configure sensor as active
 *
 * @param sensor
 */
extern void nxt_sensor_conf_active(nxt_sensor_t *sensor);

/**
 * Gets current measuring value from sensor
 *
 * @param sensor Sensor to be used
 * @return Sensor measuring value
 */
extern sensor_val_t nxt_sensor_get_val(nxt_sensor_t *sensor);

/**
 * Gets additional measuring value from sensor
 *
 * @param sensor Sensor to be used
 * @param command Special id of value. Special for each sensor
 * @return
 */
extern sensor_val_t nxt_sensor_active_get_val(nxt_sensor_t *sensor, uint8_t command);

#endif /* NXT_SENSOR_H_ */
