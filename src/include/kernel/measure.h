/**
 * @file
 * @brief Interface for measuring handlers
 *
 * @date 17.11.2010
 * @author Anton Kozlov
 */

#ifndef MEASURE_H_
#define MEASURE_H_

#include <types.h>
#include <hal/measure_unit.h>

/**
 * Process measuring of single interrupt request
 * @param interrupt measured interrupt
 * @param ticks count of ticks spent in handler
 */
void measure_irq_process(interrupt_nr_t interrupt, uint32_t ticks);

/**
 * Output accumulated info
 */
void measure_irq_print();

#endif /* MEASURE_H_ */
