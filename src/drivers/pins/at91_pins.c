/**
 * @file
 * @brief AT91 pio pin driver
 *
 * @date 26.11.2010
 * @author Anton Kozlov
 */

#include <embox/unit.h>
#include <drivers/at91sam7s256.h>
#include <hal/reg.h>
#include <drivers/pins.h>
#include <hal/pins.h>
#include <kernel/irq.h>

EMBOX_UNIT_INIT(at91_pin_init);

static int at91_pin_init(void) {
	REG_STORE(AT91C_PMC_PCER, 1L << AT91C_ID_PIOA);
	irq_attach(AT91C_ID_PIOA, (irq_handler_t) &irq_pin_handler,
			1, NULL, "AT91 PIO pins");
	pin_get_input_changed();
	return 0;
}

static inline void pin_disable_perf(int mask) {
	REG_STORE(AT91C_PIOA_PER, (uint32_t) mask);
}

void pin_config_input(pin_mask_t mask) {
	pin_disable_perf(mask);
	REG_STORE(AT91C_PIOA_ODR, (uint32_t) mask);
}

void pin_config_output(pin_mask_t mask) {
	pin_disable_perf(mask);
	//REG_STORE(AT91C_PIOA_OWER, (uint32_t) mask);
	REG_STORE(AT91C_PIOA_OER, (uint32_t) mask);
}

pin_mask_t pin_get_input(pin_mask_t mask) {
	return mask & ((int) REG_LOAD(AT91C_PIOA_PDSR));
}

void pin_set_output(pin_mask_t mask) {
	REG_STORE(AT91C_PIOA_SODR, mask);
}

void pin_clear_output(pin_mask_t mask) {
	REG_STORE(AT91C_PIOA_CODR, mask);
}

void pin_set_input_interrupt(pin_mask_t mask) {
	REG_STORE(AT91C_PIOA_IER, mask);
}

pin_mask_t pin_get_input_changed(void) {
	return (int) REG_LOAD(AT91C_PIOA_ISR);
}

