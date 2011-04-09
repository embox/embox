/**
 * @file
 * @brief Measure framework
 *
 * @data 17.11.2010
 * @author Anton Kozlov
 */

#include <types.h>
#include <hal/measure_unit.h>
#include <hal/interrupt.h>
#include <util/math.h>
#include <kernel/measure.h>
#include <kernel/timer.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(measure_init);

static uint32_t unit_clocks_per_tick = 0;

measure_time_t irq_process[INTERRUPT_NRS_TOTAL];
measure_time_t irq_head, irq_tail;
measure_time_t measure_overhead;

static int measure_init() {
	size_t i;
	for (i = 0;; i < INTERRUPT_NRS_TOTAL; i++) {
		irq_process[i].ticks = 0;
		irq_process[i].clocks = 0;
	}

	unit_clocks_per_tick = measure_unit_clocks_per_tick();
	return 0;
}

static measure_time_t measures[3];
static int measures_pos = 0;

static measure_time_t *cur_time, *last_time, *delta;

measure_time_t *measure_get_time(void) {
	cur_time = &measures[measures_pos++ % 3];
	cur_time->ticks = cnt_system_time() + unhandled_ticks();
	cur_time->clocks = measure_unit_clocks();
	return cur_time;
}

void measure_start(void) {
	last_time = cur_time;
	measure_get_time();
}

measure_time_t *get_delta(measure_time_t *a, measure_time_t *b) {
	delta = &measures[measures_pos++ % 3];
	delta->ticks = b->ticks - a->ticks;
	if (b->clocks < a->clocks) {
		delta->clocks = (b->clocks + unit_clocks_per_tick) - a->clocks;
	} else {
		delta->clocks = b->clocks - a->clocks;
	}
	return delta;
}

measure_time_t *measure_stop(void) {
	last_time = cur_time;
	cur_time = measure_get_time();
	return get_delta(last_time, cur_time);
}

void measure_irq_process(interrupt_nr_t interrupt, measure_time_t *time) {
	irq_process[interrupt] = *time;
}

void measure_irq_measure_overhead(measure_time_t *time) {
	measure_overhead = *time;
}

void measure_irq_head_process(measure_time_t *time) {
	irq_head = *time;
}

void measure_irq_tail_process(measure_time_t *time) {
	irq_tail = *time;
}
