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

typedef struct {
	uint32_t ticks; /* system ticks */
	uint32_t clocks;/* timer clocks */
	/* choosed so for good packing (is it good?) */
} measure_time_t;

/**
 * Process measuring of single interrupt request
 * @param interrupt measured interrupt
 * @param ticks count of ticks spent in handler
 */
extern void measure_irq_process(interrupt_nr_t interrupt, measure_time_t *time);

/**
 * Output accumulated info
 */
extern void measure_irq_print(void);

/**
 * Process measuring of pre-dispatch irq handler
 */
extern void measure_irq_head_process(measure_time_t *time);

/**
 * Process measuring of post-dispatch irq handler
 */
extern void measure_irq_tail_process(measure_time_t *time);

/**
 * Process mesaure overhead, i.e. *time spent on
 * start + stop measure
 */
extern void measure_irq_measure_overhead(measure_time_t *time);

measure_time_t *measure_get_time(void);

void measure_start(void);

measure_time_t *measure_stop(void);
#endif /* MEASURE_H_ */
