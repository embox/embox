/**
 * @file
 * @brief Sonar sensor interface of nxt
 *
 * @date 27.12.2010
 * @author Anton Kozlov
 */


#ifndef NXT_SONAR_H_
#define NXT_SONAR_H_

#include <drivers/nxt_sensor.h>

#define NXT_SONAR_DISTANCE_COMM 0x42

extern void nxt_sonar_init(sensor_t *sensor);

#endif /*NXT_SONAR_H_*/


