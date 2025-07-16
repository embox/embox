/**
 * @file
 * @brief Programmable Interval Timer (PIT) chip driver
 *
 * @date 27.12.10
 * @author Nikolay Korotky
 */

#include <framework/mod/options.h>

#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include <lib/libds/array.h>

#include <hal/clock.h>

#include <asm/io.h>

#include <kernel/irq.h>
#include <kernel/panic.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>

#define INPUT_CLOCK        1193182L /* clock tick rate, Hz */
#define IRQ_NR             OPTION_GET(NUMBER,irq_num)

#define PIT_HZ 1000

#define PIT_LOAD ((INPUT_CLOCK + PIT_HZ / 2) / PIT_HZ)
static_assert(PIT_LOAD < 0x10000, "");

/**
 * The PIT chip uses the following I/O ports:
 * I/O port     Usage
 * 0x40         Channel 0 data port (read/write)
 * 0x41         Channel 1 data port (read/write)
 * 0x42         Channel 2 data port (read/write)
 * 0x43         Mode/Command register (write only, a read is ignored):
 * +---------------+------------+--------------+---------------+
 * |7             6|5          4|3            1|              0|
 * |Select channel |Access mode |Operating mode|BCD/Binary mode|
 *
 * Select channel:
 *           00 = Channel 0
 *           01 = Channel 1
 *           10 = Channel 2
 *           11 = Read-back command (8254 only)
 * Access mode:
 *           00 = Latch count value command
 *           01 = Access mode: lobyte only
 *           10 = Access mode: hibyte only
 *           11 = Access mode: lobyte/hibyte
 * Operating mode:
 *           000 = Mode 0 (interrupt on terminal count)
 *           001 = Mode 1 (hardware re-triggerable one-shot)
 *           010 = Mode 2 (rate generator)
 *           011 = Mode 3 (square wave generator)
 *           100 = Mode 4 (software triggered strobe)
 *           101 = Mode 5 (hardware triggered strobe)
 *           110 = Mode 2 (rate generator, same as 010b)
 *           111 = Mode 3 (square wave generator, same as 011b)
 * BCD/Binary mode: 0 = 16-bit binary, 1 = four-digit BCD
 */
#define CHANNEL0 0x40
#define CHANNEL1 0x41
#define CHANNEL2 0x42
#define MODE_REG 0x43

/* Mode register bits */
#define PIT_SEL0        0x00    /* select counter 0 */
#define PIT_SEL1        0x40    /* select counter 1 */
#define PIT_SEL2        0x80    /* select counter 2 */
#define PIT_INTTC       0x00    /* mode 0, intr on terminal cnt */
#define PIT_ONESHOT     0x02    /* mode 1, one shot */
#define PIT_RATEGEN     0x04    /* mode 2, rate generator */
#define PIT_SQWAVE      0x06    /* mode 3, square wave */
#define PIT_SWSTROBE    0x08    /* mode 4, s/w triggered strobe */
#define PIT_HWSTROBE    0x0a    /* mode 5, h/w triggered strobe */
#define PIT_LATCH       0x00    /* latch counter for reading */
#define PIT_LSB         0x10    /* r/w counter LSB */
#define PIT_MSB         0x20    /* r/w counter MSB */
#define PIT_16BIT       0x30    /* r/w counter 16 bits, LSB first */
#define PIT_BCD         0x01    /* count in BCD */

static inline void pit_out8(uint8_t val, int port) {
	out8(val, port);
}

static inline uint8_t pit_in8(int port) {
	return in8(port);
}

static cycle_t i8253_get_cycles(struct clock_source *cs) {
	unsigned char lsb, msb;

	irq_lock();
	{
		pit_out8(PIT_SEL0 | PIT_LATCH, MODE_REG);
		lsb = pit_in8(CHANNEL0);
		msb = pit_in8(CHANNEL0);
	}
	irq_unlock();

	return PIT_LOAD - ((msb << 8) | lsb);
}

static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
	clock_tick_handler(dev_id);
	return IRQ_HANDLED;
}

static int pit_clock_setup(struct clock_source *cs) {
	uint16_t divisor = PIT_LOAD;

	/* Propose switch by all modes in future */
	/* Set control byte */
	pit_out8(PIT_SEL0 | PIT_16BIT | PIT_RATEGEN, MODE_REG);

	/* Send divisor */
	pit_out8(divisor & 0xFF, CHANNEL0);
	pit_out8(divisor >> 8, CHANNEL0);

	return ENOERR;
}

static struct time_event_device pit_event_device = {
	.set_periodic = pit_clock_setup,
	.irq_nr = IRQ_NR,
};

static struct time_counter_device pit_counter_device = {
	.get_cycles = i8253_get_cycles,
	.cycle_hz = INPUT_CLOCK,
	.mask = 0xffff,
};

static int pit_clock_init(struct clock_source *cs) {
	pit_clock_setup(NULL);

	if (ENOERR != irq_attach(IRQ_NR, clock_handler, 0, cs, "PIT")) {
		panic("pit timer irq_attach failed");
	}

	return ENOERR;
}

CLOCK_SOURCE_DEF(pit, pit_clock_init, NULL,
	&pit_event_device, &pit_counter_device);
