/**
 * @file
 *
 * @date 06 july 2015
 * @author Anton Bondarev
 */

#include <stm32f3_discovery_gyroscope.h>

void gyro_get(float *value) {
	BSP_GYRO_GetXYZ(value);
}
