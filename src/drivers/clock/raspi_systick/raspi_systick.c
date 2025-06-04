/**
 * @file
 * @brief Core clocking device in Raspberry PI
 *
 * See BCM2835-ARM-Peripherals.pdf, 12 chapter for details.
 *
 * A frequency of the System Timer is 1 MHz.
 *
 * @date 02.07.15
 * @author Vita Loginova
 */

#include <drivers/common/memory.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <sys/mman.h>
#include <hal/mmu.h>
#include <util/binalign.h>
#include <framework/mod/options.h>

#include <embox/unit.h>

#define BCM2835_SYSTEM_TIMER_BASE OPTION_GET(NUMBER,base_addr)

/**
 * Layout of the System Timer Registers.
 */
struct raspi_timer_regs {
	uint32_t CS;  /**< Control/Status */
	uint32_t CLO; /**< Counter Lower 32 bits */
	uint32_t CHI; /**< Counter Higher 32 bits */
	uint32_t C0;  /**< Compare 0. DO NOT USE; is used by GPU.  */
	uint32_t C1;  /**< Compare 1 */
	uint32_t C2;  /**< Compare 2. DO NOT USE; is used by GPU.  */
	uint32_t C3;  /**< Compare 3 */
};

/**
 * Need to write one of these values into CS register to clear the
 * match detect status bit and the corresponding interrupt request line.
 */
#define BCM2835_SYSTEM_TIMER_MATCH_0 (1 << 0)
#define BCM2835_SYSTEM_TIMER_MATCH_1 (1 << 1)
#define BCM2835_SYSTEM_TIMER_MATCH_2 (1 << 2)
#define BCM2835_SYSTEM_TIMER_MATCH_3 (1 << 3)

#define SYSTICK_IRQ 3
#define CLOCK_DIVIDER 70

#define RELOAD_VALUE (SYS_CLOCK / (CLOCK_DIVIDER * 1000))

static volatile struct raspi_timer_regs * const regs =
		(volatile struct raspi_timer_regs*)BCM2835_SYSTEM_TIMER_BASE;

static inline void raspi_systick_clear(void) {
	regs->CS = BCM2835_SYSTEM_TIMER_MATCH_3;
}

static inline void raspi_systick_comare(uint32_t value) {
	regs->C3 = regs->CLO + value;
}

static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	raspi_systick_clear();
	raspi_systick_comare(RELOAD_VALUE);

	clock_tick_handler(data);

	return IRQ_HANDLED;
}

static int raspi_systick_init(struct clock_source *cs) {
	irq_attach(SYSTICK_IRQ, clock_handler, 0, cs, "Raspberry PI systick timer");
	return 0;
}

static int this_set_periodic(struct clock_source *cs) {
	raspi_systick_clear();
	/* From that point interrupts will occur. */
	raspi_systick_comare(RELOAD_VALUE);

	return 0;
}

static cycle_t this_get_cycles(struct clock_source *cs) {
	return regs->CLO;
}

static struct time_event_device raspi_systick_event = {
	.set_periodic = this_set_periodic,
	.irq_nr = SYSTICK_IRQ,
};

static struct time_counter_device raspi_systick_counter = {
	.get_cycles = this_get_cycles,
	.cycle_hz = SYS_CLOCK / CLOCK_DIVIDER,
};

PERIPH_MEMORY_DEFINE(raspi_systick, BCM2835_SYSTEM_TIMER_BASE, sizeof(struct raspi_timer_regs));

CLOCK_SOURCE_DEF(raspi_systick, raspi_systick_init, NULL,
	&raspi_systick_event, &raspi_systick_counter);
