/**
 * @file
 *
 * @brief Timer Counter interface realization
 *
 * @date
 * @author Anton Kozlov
 */

#include <drivers/at91sam7s256.h>
#include <drivers/at91_timer_counter.h>

#include <assert.h>
#include <hal/reg.h>

void tc_init(uint8_t channel) {
	assert(channel < 3);
	REG_STORE(AT91C_PMC_PCER, (1L << (AT91C_ID_TC0 + channel)));
}

void tc_config_input(uint8_t channel, uint8_t clock_mode) {
	assert(channel < 3);
	REG_STORE((uint8_t *) AT91C_TC0_CMR + channel * sizeof(AT91S_TCB),
			0xfff & clock_mode);
}

void tc_reset(uint8_t channel) {
	//assert(channel < 3);
	REG_STORE((uint8_t *) AT91C_TC0_CCR + channel * sizeof(AT91S_TCB),
			AT91C_TC_CLKEN | AT91C_TC_SWTRG);
}

uint32_t tc_counter_value(uint8_t channel) {
	//assert(channel < 3);
	return REG_LOAD(((uint8_t *) AT91C_TC0_CV) + channel * sizeof(AT91S_TCB));
}

bool tc_was_overflowed(uint8_t channel) {
	//assert(channel < 3);
	return REG_LOAD(((uint8_t *) AT91C_TC0_SR) + channel * sizeof(AT91S_TCB)) &
		AT91C_TC_COVFS;
}

void tc_stop(uint8_t channel) {
	//assert(channel < 3);
	REG_STORE((uint8_t *) AT91C_TC0_CCR + channel * sizeof(AT91S_TCB),
		AT91C_TC_CLKDIS);
	//REG_STORE(AT91C_PMC_PCDR, (1L << (AT91C_ID_TC0 + channel)));
}
