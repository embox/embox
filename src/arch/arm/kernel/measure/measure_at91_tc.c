/**
 * @file
 * @brief Implements measure unit interface by AT91 Timer Counter
 *
 * @date 17.11.2010
 * @author Anton Kozlov
 */

#include <types.h>
#include <hal/measure_unit.h>
#include <drivers/at91_timer_counter.h>
#include <embox/unit.h>

#define MEASURE_CHANNEL 0

EMBOX_UNIT_INIT(measure_unit_init);

static int measure_unit_init(void) {
	tc_init(MEASURE_CHANNEL);
	tc_config_input(MEASURE_CHANNEL, TC_INPUT_MODE_DIV1);
	tc_reset(MEASURE_CHANNEL);
	return 0;
}

uint32_t measure_unit_clocks(void) {
	return tc_counter_value(MEASURE_CHANNEL);
}

uint8_t unhandled_ticks(void) {
	if (tc_was_overflowed(MEASURE_CHANNEL)) {
		return 1;
	}
	return 0;
}

uint32_t measure_unit_clocks_per_tick(void) {
	return 0x10000;
}


