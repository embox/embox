/**
 * @file
 * @brief Interface for hardware-enabled clock ticks measuring unit
 *
 * @date 16.11.2010
 * @author Anton Kozlov
 */

#ifndef MEASURE_H_
#define MEASURE_H_

#include <types.h>

/**
 * Enable measuring, init hardware, begin measure
 */
void measure_start(void);

/**
 * Rerun tick counter
 * @return counted ticks from last start/reset
 */
uint32_t measure_reset(void);

/**
 * Invalidate counted ticks, pause measure
 */
void measure_pause(void);

/**
 * Free hardware, disable measure ability
 * @return counted ticks
 */
uint32_t measure_stop(void);

#endif /* MEASURE_H_ */
