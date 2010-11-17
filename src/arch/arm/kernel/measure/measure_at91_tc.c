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
}

void measure_start(void) {
	tc_reset(MEASURE_CHANNEL);
}

uint32_t measure_reset(void) {
	uint32_t ret = tc_counter_value(MEASURE_CHANNEL) << 1; /*shift determed by mode*/
	tc_reset(MEASURE_CHANNEL);
	return ret;
}

void measure_pause(void) {
	tc_stop(MEASURE_CHANNEL);
}

uint32_t measure_stop(void) {
	uint32_t ret = measure_reset();
	tc_stop(MEASURE_CHANNEL);
	return ret;
}


