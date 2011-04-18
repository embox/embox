/**
 * @file
 * @brief Pins managment system
 *
 * @date 26.11.2010
 * @author Anton Kozlov
 */

#include <embox/unit.h>
#include <drivers/pins.h>
#include <hal/pins.h>

/**
 * Assuming each pin have a single strictly determined function
 * (very strange if it doesn't, using simply scheme)
 */

#define N_PINS 8 * (sizeof(int))

struct handler_item_t {
	int mask;
	pin_handler_t handler;
};

static struct handler_item_t handlers[N_PINS];

static int n_handler = 0;

static int mon_mask = 0; /* mask of monitorable pins */

static inline int get_first_bit(int val) {
	return val & (~val + 1);
}

static inline int get_first_bit_n(int val) {
    register int i = get_first_bit(val);
	register int j = 0;
	i >>= 1;
	while (i) {
		j ++;
		i >>= 1;
	}
	return j;
}

irq_return_t irq_pin_handler(irq_nr_t irq_nr, void *data) {
	int current = pin_get_input(~0);
	int changed = pin_get_input_changed() & mon_mask;
	while (changed) {
		int i = get_first_bit_n(changed);
		struct handler_item_t *hnd = &handlers[i];
		if (hnd->handler != NULL) {
			hnd->handler(hnd->mask & current, hnd->mask);
		}
		changed &= ~(get_first_bit(changed));
	}
	return IRQ_HANDLED;
}

void pin_set_input_monitor(int mask, pin_handler_t pin_handler) {
	int tmask = 0x1;
	int i;
	for (i = 0; i < N_PINS; i++) {
		if (mask & tmask) {
			handlers[i].handler = pin_handler;
			handlers[i].mask = mask;
		}
		tmask <<= 1;
	}
	mon_mask |= mask;
	pin_config_input(mask);
	pin_set_input_interrupt(mask);
}

