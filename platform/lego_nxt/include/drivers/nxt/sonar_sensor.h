/**
 * @file
 * @brief Sonar sensor interface of nxt
 *
 * @date 27.12.10
 * @author Anton Kozlov
 */

#ifndef NXT_SONAR_H_
#define NXT_SONAR_H_

#include <drivers/nxt/sensor.h>

/* commands to be given as 'commands'
 * to nxt_sensor_active_get_val */
#define NXT_SONAR_DISTANCE_COMM 0x42

/**
 * Dynamically defines @link sensor @link as sonar
 * @param sensor Sensor to be defined
 */
extern void nxt_sonar_init(nxt_sensor_t *sensor);

#endif /* NXT_SONAR_H_ */
