/**
 * @file
 * @brief Touch sensor interface of nxt
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#ifndef NXT_TOUCH_SENSOR_H_
#define NXT_TOUCH_SENSOR_H_

#include <drivers/nxt/sensor.h>

/**
 * Touch handler type. Called when touch sensor changes measuring
 * from 'touched' to 'not touched' and vice versa
 * @param sensor
 */
typedef void (*touch_hnd_t)(nxt_sensor_t *sensor);

/**
 * Dynamically defines @link sensor @link as touch sensor
 * @param sensor Sensor to be defined
 * @param handler Handler to be used as @link touch_hnd_t @link
 */
extern void touch_sensor_init(nxt_sensor_t *sensor, touch_hnd_t handler);

#endif /* NXT_TOUCH_SENSOR_H_ */
