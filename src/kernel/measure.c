/**
 * @file
 * @brief Measure framework
 *
 * @data 17.11.2010
 * @author Anton Kozlov
 */

#include <types.h>
#include <hal/measure_unit.h>
#include <util/math.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(measure_init);

static uint32_t irq_max[INTERRUPT_NRS_TOTAL],
		 irq_min[INTERRUPT_NRS_TOTAL],
		 irq_avg[INTERRUPT_NRS_TOTAL];

static int measure_init() {
	int i = 0;
	for (; i < INTERRUPT_NRS_TOTAL; i++) {
		irq_max[i] = 0;
		irq_min[i] = ~0;
		irq_avg[i] = 0;
	}
	return 0;
}

void measure_irq_process(interrupt_nr_t interrupt, uint32_t ticks) {
	irq_max[interrupt] = max(irq_max[interrupt], ticks);
	irq_min[interrupt] = min(irq_min[interrupt], ticks);
	irq_avg[interrupt] = (irq_avg[interrupt] + ticks) / 2;
}

void measure_irq_print() {
	/*output for analyzes*/
}

