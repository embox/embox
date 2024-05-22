/**
 * @file
 *
 * @date 07 июля 2015 г.
 * @author: Anton Bondarev
 */

#ifndef PLATFORM_STM32F3_SENSORS_LIBFILTERS_FILTERED_DERIVATIVE_H_
#define PLATFORM_STM32F3_SENSORS_LIBFILTERS_FILTERED_DERIVATIVE_H_

#define FD_WIN_SIZE   150
#define FD_THRESHOLD  2600

extern float filtered_derivative(float inc);
extern int filtered_derivative_check(float val);

#endif /* PLATFORM_STM32F3_SENSORS_LIBFILTERS_FILTERED_DERIVATIVE_H_ */
