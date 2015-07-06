/**
 * @file
 *
 * @date 06 july 2015
 * @author Anton Bondarev
 */
#include <stdint.h>

#include <stm32f3_discovery_accelerometer.h>

#include <libsensors/acc.h>

void acc_get(int16_t *value) {
	BSP_ACCELERO_GetXYZ(value);
}

