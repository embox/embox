/**
 * @file
 * @brief Programmable Interval Timer (PIT) chip driver
 *
 * @date 27.12.10
 * @author Nikolay Korotky
 */

#include <embox/unit.h>
#include <framework/mod/options.h>

#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include <asm/io.h>
#include <drivers/i8259.h>
#include <hal/clock.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>
#include <util/array.h>


#define INPUT_CLOCK        1193182L /* clock tick rate, Hz */
#define IRQ_NR             OPTION_GET(NUMBER,irq_num)
#define SLOWDOWN_FACTOR    OPTION_GET(NUMBER,slowdown_factor)

#define PIT_HZ (1000 / SLOWDOWN_FACTOR)

static int pit_clock_setup(struct time_dev_conf * conf);
static int pit_clock_init(void);

static struct clock_source pit_clock_source;
static struct time_event_device pit_event_device;
static struct time_counter_device pit_counter_device;

//EMBOX_UNIT_INIT(pit_clock_init);

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

static cycle_t i8253_read(void) {
	int cnt;

	out8(0x00, MODE_REG);
	cnt = in8(CHANNEL0);
	cnt |= in8(CHANNEL0) << 8;

	cnt = ((INPUT_CLOCK + PIT_HZ ) / PIT_HZ) - cnt;

	return cnt;
}

static irq_return_t clock_handler(unsigned int irq_nr, void *dev_id) {
        clock_tick_handler(irq_nr, dev_id);
        return IRQ_HANDLED;
}

static struct time_event_device pit_event_device = {
	.config = pit_clock_setup,
	.event_hz = PIT_HZ,
	.irq_nr = IRQ_NR,
	.pending = i8259_irq_pending
};

static struct time_counter_device pit_counter_device = {
	.read = i8253_read,
	.cycle_hz = INPUT_CLOCK * SLOWDOWN_FACTOR,
};

static struct clock_source pit_clock_source = {
	.name = "pit",
	.event_device = &pit_event_device,
	.counter_device = &pit_counter_device,
	.read = clock_source_read /* attach default read function */
};

EMBOX_UNIT_INIT(pit_clock_init);

static int pit_clock_init(void) {
	pit_clock_setup(NULL);
	clock_source_register(&pit_clock_source);

	if (ENOERR != irq_attach(IRQ_NR, clock_handler, 0, &pit_clock_source, "PIT")) {
		panic("pit timer irq_attach failed");
	}

	return ENOERR;
}

static int pit_clock_setup(struct time_dev_conf * conf) {
	uint32_t divisor = (INPUT_CLOCK + PIT_HZ / 2) /PIT_HZ;

	pit_clock_source.flags = 1;

	/* Propose switch by all modes in future */
	/* Set control byte */
	out8(PIT_RATEGEN | PIT_16BIT | PIT_SEL0, MODE_REG);

	/* Send divisor */
	out8(divisor & 0xFF, CHANNEL0);
	out8((divisor >> 8) & 0xFF, CHANNEL0);

	return ENOERR;
}
