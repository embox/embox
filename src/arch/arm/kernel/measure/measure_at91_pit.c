/**
 * @file
 * @brief Implements measure unit interface by AT91 PIT
 *
 * @date 17.11.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <hal/measure_unit.h>
#include <drivers/at91_timer_counter.h>
#include <embox/unit.h>
#include <hal/reg.h>

#include <drivers/at91sam7s256.h>

uint32_t measure_unit_clocks(void) {
	return (REG_LOAD(AT91C_PITC_PIIR) & 0xfffff) * 16;
}

uint8_t unhandled_ticks(void) {
	return REG_LOAD(AT91C_PITC_PIIR) >> 20;
}

uint32_t measure_unit_clocks_per_tick(void) {
	return 1000 * AT91C_PIT_USECOND;
}
