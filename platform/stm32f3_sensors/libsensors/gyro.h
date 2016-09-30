/**
 * @file
 *
 * @date 06 july 2015
 * @author Anton Bondarev
 */

#ifndef PLATFORM_STM32F3_SENSORS_LIBSENSORS_GYRO_H_
#define PLATFORM_STM32F3_SENSORS_LIBSENSORS_GYRO_H_

#define GYRO_TOTAL ACC_TOTAL
#define GYRO_VAL_SIZE (4)

extern void gyro_get(float *value);

extern void gyro_data_normalize(float *in, float *out);

extern void gyro_data_obtain(float *out);

extern void gyro_data_obtain(float *out);

#endif /* PLATFORM_STM32F3_SENSORS_LIBSENSORS_GYRO_H_ */
