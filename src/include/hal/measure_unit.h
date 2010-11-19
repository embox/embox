/**
 * @file
 * @brief Interface for hardware-enabled clock ticks measuring unit
 *
 * @date 16.11.2010
 * @author Anton Kozlov
 */

#ifndef MEASURE_UNIT_H_
#define MEASURE_UNIT_H_

#include <types.h>

/**
 * Get timer ticks between system ticks
 * @return timer ticks
 */
uint32_t measure_ticks(void);

/**
 * Get count of unhandled system ticks
 * @return count unhandled system ticks
 */
uint8_t unhandled_ticks(void);

#endif /* MEASURE_UNIT_H_ */
