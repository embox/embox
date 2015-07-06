/**
 * @file
 *
 * @date 06 july 2015
 * @author Anton Bondarev
 */

#ifndef PLATFORM_STM32F3_SENSORS_LIBSENSORS_ACC_H_
#define PLATFORM_STM32F3_SENSORS_LIBSENSORS_ACC_H_

#include <stdint.h>

#define ACC_TOTAL    (512 * 4 + 128)
#define ACC_VAL_SIZE (2)

extern void acc_get(int16_t *value);

#endif /* PLATFORM_STM32F3_SENSORS_LIBSENSORS_ACC_H_ */
