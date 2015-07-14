/**
 * @file
 *
 * @date 14.07.2015
 * @author Alex Kalmuk
 */

#ifndef PLATFORM_STM32F3_SENSORS_LIBFILTERS_DYNAMIC_WINDOW_H_
#define PLATFORM_STM32F3_SENSORS_LIBFILTERS_DYNAMIC_WINDOW_H_

#define WIN_SIZE 200

extern float dyn_window_avg(float val, float K);

#endif /* PLATFORM_STM32F3_SENSORS_LIBFILTERS_DYNAMIC_WINDOW_H_ */
