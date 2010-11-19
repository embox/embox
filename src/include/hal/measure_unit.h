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

#define MEASURE_UNIT_TICK __MEASURE_UNIT_TICK

/**
 * Get timer clocks between system ticks
 * @return timer clocks
 */
extern uint32_t measure_unit_clocks(void);

/**
 * Get count of unhandled system ticks
 * @return count unhandled system ticks
 */
extern uint8_t unhandled_ticks(void);

/**
 * Devices have various number of clocks per tick
 * Kernel have to know it's number
 * @return Number of clocks per tick
 */
extern uint32_t measure_unit_clocks_per_tick(void);
#endif /* MEASURE_UNIT_H_ */
