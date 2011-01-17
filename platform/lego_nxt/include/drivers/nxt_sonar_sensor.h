/**
 * @file
 * @brief nxt sonar sensor api
 *
 * @date 17.01.11
 * @author Alexander Batyukov
 */

#ifndef NXT_SONAR_SENSOR_H_
#define NXT_SONAR_SENSOR_H_

#include <drivers/nxt_sensor.h>

extern int sonar_sensor_get_val (sensor_t *sensor);
extern void sonar_sensor_init (sensor_t *sensor);

#endif /* NXT_SONAR_SENSOR_H_ */
