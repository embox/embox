/**
 * @file
 *
 * @date 06 july 2015
 * @author Anton Bondarev
 */

#ifndef PLATFORM_STM32F3_SENSORS_LIBSENSORS_ACC_H_
#define PLATFORM_STM32F3_SENSORS_LIBSENSORS_ACC_H_

#include <stdint.h>

/* g - Gravitational constant */
#define g 9.80665f

#define ACC_TOTAL    (512 * 4 + 128)
#define ACC_VAL_SIZE (2)

extern void acc_get(int16_t *value);

extern void acc_data_normalize(int16_t *in, float *out);

extern void acc_data_obtain(float *out);

extern void acc_calculate_offset(void);

#endif /* PLATFORM_STM32F3_SENSORS_LIBSENSORS_ACC_H_ */
