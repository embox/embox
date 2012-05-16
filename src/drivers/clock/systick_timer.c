/**
 * @file
 * @brief Core clocking device in Cortex-M3
 *
 * @date 23.03.12
 * @author Anton Kozlov
 */

#include <hal/clock.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/clock_source.h>
#include <types.h>
#include <hal/interrupt.h>

#define SYSTICK_IRQ 15

#define SYSTICK_BASE 0xe000e010

#define SYSTICK_CTRL SYSTICK_BASE

#define SYSTICK_CALIB ((int) SYSTICK_BASE + 0x0c)

#define SYSTICK_RELOAD (SYSTICK_BASE + 0x04)
#define SYSTICK_VAL (SYSTICK_BASE + 0x08)

#define SYSTICK_ENABLE 0x01
#define SYSTICK_TICKINT (0x01 << 1)
#define SYSTICK_CLOCKINIT (0x01 << 2)

#define SCB_SHP_BASE ((unsigned int *) 0xe000ed18)
#define SCB_SHP_PERIF_N 8

static struct clock_source systick;

void clock_handler(void) {
	clock_tick_handler(SYSTICK_IRQ, NULL);
}

void clock_init(void) {
	systick.flags = 1;
	systick.resolution = 1000;
	clock_source_register(&systick);
}

void clock_setup(useconds_t usec) {
	int ticks_per_10ms = REG_LOAD(SYSTICK_CALIB) & 0xffffff;

	REG_STORE(SYSTICK_CTRL, 0);

	REG_STORE(SYSTICK_RELOAD, (ticks_per_10ms * usec)/10000 - 1);

	REG_STORE(SYSTICK_VAL, 0);

	REG_STORE(SCB_SHP_BASE + 2, 0xf0 << (3 * SCB_SHP_PERIF_N));

	REG_STORE(SYSTICK_CTRL, SYSTICK_ENABLE | SYSTICK_TICKINT |
			SYSTICK_CLOCKINIT);

}
