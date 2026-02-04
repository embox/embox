/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 23.12.24
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#include <drivers/gpio.h>
#include <kernel/time/time.h>
#include <kernel/time/sys_timer.h>
#include <lib/libds/array.h>
#include <plc/core.h>

ARRAY_SPREAD_DEF(const struct plc_config *const, __plc_config_registry);

/**
 * Variables to export to generated C softPLC
 */
struct timespec __CURRENT_TIME;
uint8_t __DEBUG;

static unsigned long tick_counter;
static struct sys_timer *plc_timer;
static const struct plc_config *current_config;

void plc_slave_handle(void (*handler)(const struct plc_slave_var *)) {
	const struct plc_slave_var *var;
	const struct plc_config *config;

	config = current_config;

	if (!config) {
		return;
	}

	plc_slave_var_foreach(var, config) {
		handler(var);
	}
}

static void plc_handler(sys_timer_t *timer, void *param) {
	const struct plc_phys_var *var;

	if (!current_config) {
		return;
	}

	tick_counter += 1;
	if (*current_config->greatest_tick_count) {
		tick_counter %= *current_config->greatest_tick_count;
	}

	clock_gettime(CLOCK_REALTIME, &__CURRENT_TIME);

	current_config->ops.run(tick_counter);

	plc_phys_var_foreach(var, current_config) {
		switch (var->type) {
		case PLC_PHYS_VAR_QX:
			gpio_set(var->port, 1 << var->pin, *(uint8_t *)var->data);
			break;

		case PLC_PHYS_VAR_IX:
			*(uint8_t *)var->data = !!gpio_get(var->port, 1 << var->pin);
			break;

		case PLC_PHYS_VAR_IW:
			/* TODO */
			break;

		case PLC_PHYS_VAR_QW:
			/* TODO */
			break;
		}
	}
}

int plc_start(const struct plc_config *config) {
	const struct plc_phys_var *var;
	uint32_t ticktime_ms;
	int err;

	if (!config) {
		return -EINVAL;
	}

	if (current_config) {
		return -EBUSY;
	}

	config->ops.init();

	plc_phys_var_foreach(var, config) {
		switch (var->type) {
		case PLC_PHYS_VAR_QX:
			gpio_setup_mode(var->port, 1 << var->pin,
			    GPIO_MODE_OUT | GPIO_MODE_ALT_SET(var->alt));
			break;

		case PLC_PHYS_VAR_IX:
			gpio_setup_mode(var->port, 1 << var->pin,
			    GPIO_MODE_IN | GPIO_MODE_ALT_SET(var->alt));
			break;

		case PLC_PHYS_VAR_IW:
			/* TODO */
			break;

		case PLC_PHYS_VAR_QW:
			/* TODO */
			break;
		}
	}

	tick_counter = 0;
	ticktime_ms = *config->common_ticktime / NSEC_PER_MSEC;
	current_config = config;

	err = sys_timer_set(&plc_timer, SYS_TIMER_PERIODIC, ticktime_ms, plc_handler, NULL);
	if (err) {
		return err;
	}

	return 0;
}

int plc_stop(void) {
	if (current_config) {
		sys_timer_close(plc_timer);
		current_config = NULL;
	}

	return 0;
}
