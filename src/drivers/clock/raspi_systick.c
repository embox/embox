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

#include <embox/unit.h>

#define BCM2835_SYSTEM_TIMER_BASE 0x20003000

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

static struct clock_source this_clock_source;
static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	raspi_systick_clear();
	raspi_systick_comare(RELOAD_VALUE);

	clock_tick_handler(irq_nr, data);

	return IRQ_HANDLED;
}

static int this_init(void) {
	clock_source_register(&this_clock_source);
	irq_attach(SYSTICK_IRQ, clock_handler, 0, &this_clock_source,
		"Raspberry PI systick timer");
	return 0;
}

static int this_config(struct time_dev_conf * conf) {
	raspi_systick_clear();
	/* From that point interrupts will occur. */
	raspi_systick_comare(RELOAD_VALUE);

	return 0;
}

static cycle_t this_read(void) {
	return regs->CLO;
}

static struct time_event_device this_event = {
	.config = this_config,
	.event_hz = 1000,
	.irq_nr = SYSTICK_IRQ,
};

static struct time_counter_device this_counter = {
	.read = this_read,
	.cycle_hz = SYS_CLOCK / CLOCK_DIVIDER,
};

static struct clock_source this_clock_source = {
	.name = "system_tick",
	.event_device = &this_event,
	.counter_device = &this_counter,
	.read = clock_source_read,
};

EMBOX_UNIT_INIT(this_init);

PERIPH_MEMORY_DEFINE(raspi_systick, BCM2835_SYSTEM_TIMER_BASE, sizeof(struct raspi_timer_regs));
