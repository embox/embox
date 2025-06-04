/**
 * @file
 * @brief General Purpose Timers for Dialog DA1469x
 *
 * @date   07.10.2020
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <util/log.h>
#include <framework/mod/options.h>
#include <embox/unit.h>

#include <config/custom_config_qspi.h>
#include <hw_timer.h>

#define TMR                  OPTION_GET(NUMBER, timer_n)
#define DA1469X_TIMER_IRQ    OPTION_GET(NUMBER, irq)

/* FIXME Unused, remove */
#define TICK_RATE_HZ         OPTION_GET(NUMBER, hz)

#if TMR == 1
#define TIMER_ID       HW_TIMER
#elif TMR == 2
#define TIMER_ID       HW_TIMER2
#elif TMR == 3
#define TIMER_ID       HW_TIMER3
#elif TMR == 4
#define TIMER_ID       HW_TIMER4
#else
	#error Unavailable timer
#endif

/* TODO We assume xtal32k is used as LP clock.
 * But there should be more flexible setup. */
#define CLOCK_FREQ     dg_configXTAL32K_FREQ

static __RETAINED_CODE irq_return_t da1469x_timer_irq_handler(
		unsigned int irq_nr, void *data) {

	(void) data;

	switch ((int) TIMER_ID) {
	case (int) HW_TIMER:
		TIMER->TIMER_CLEAR_IRQ_REG = 1;
		break;
	case (int) HW_TIMER2:
		TIMER2->TIMER2_CLEAR_IRQ_REG = 1;
		break;
	case (int) HW_TIMER3:
		TIMER3->TIMER3_CLEAR_IRQ_REG = 1;
		break;
	case (int) HW_TIMER4:
		TIMER4->TIMER4_CLEAR_IRQ_REG = 1;
		break;
	default:
		return IRQ_HANDLED;
	}

	log_debug("irq = %d", irq_nr);

	hw_timer_disable(TIMER_ID);

	return IRQ_HANDLED;
}

static cycle_t da1469x_timer_get_cycles(struct clock_source *cs) {
	return hw_timer_get_reload(TIMER_ID) - hw_timer_get_count(TIMER_ID);
}

static int da1469x_timer_set_next_event(struct clock_source *cs,
		uint32_t next_event) {
	hw_timer_set_reload(TIMER_ID, next_event);

	hw_timer_enable_clk(TIMER_ID);
	while (hw_timer_get_count(TIMER_ID) != next_event);

	hw_timer_enable(TIMER_ID);

	return 0;
}

static struct time_event_device da1469x_timer_event = {
	.set_next_event = da1469x_timer_set_next_event,
	.irq_nr = DA1469X_TIMER_IRQ,
};

static struct time_counter_device da1469x_timer_counter = {
	.get_cycles = da1469x_timer_get_cycles,
	.cycle_hz = CLOCK_FREQ,
	.mask = 0xffffff,
};

static int da1469x_timer_init(struct clock_source *cs) {
	timer_config timer_cfg = {
		.clk_src = HW_TIMER_CLK_SRC_INT,
		.prescaler = 0,
		.timer = {
			.direction = HW_TIMER_DIR_DOWN,
			.reload_val = CLOCK_FREQ - 1,
			.free_run = false,
		},
		.pwm = { .frequency = 0, .duty_cycle = 0 },
	};

	hw_timer_init(TIMER_ID, &timer_cfg);

	if (irq_attach(DA1469X_TIMER_IRQ, da1469x_timer_irq_handler, 0, NULL, "") < 0) {
		return -1;
	}
	HW_TIMER_REG_SETF(TIMER_ID, TIMER_CTRL_REG, TIM_IRQ_EN, 1);

	/* Disable by default */
	hw_timer_disable(TIMER_ID);

	return 0;
}

STATIC_IRQ_ATTACH(DA1469X_TIMER_IRQ, da1469x_timer_irq_handler, NULL);

CLOCK_SOURCE_DEF(da1469x_timer, da1469x_timer_init, NULL,
	&da1469x_timer_event, &da1469x_timer_counter);
