/**
 * @file
 * @brief i.MX General Purpose Timer.
 * @note See i.MX 8M Dual/8M QuadLite/8M Quad Applications Processors Reference
 *       Manual for more details
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2019-07-23
 */

#include <sys/mman.h>

#include <drivers/common/memory.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/printk.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <embox/unit.h>
#include <util/log.h>

#define GPT_BASE_ADDR OPTION_GET(NUMBER, base_addr)
#define GPT_IRQ       OPTION_GET(NUMBER, irq_num)

#define GPT_CR                 (GPT_BASE_ADDR + 0x00)
# define GPT_CR_EN             (1 << 0)
# define GPT_CR_ENMOD          (1 << 1)
# define GPT_CR_CLKSRC_OFFT    6
# define GPT_CR_CLKSRC_MASK    0x7
# define GPT_CR_OM1_OFFT       20
# define GPT_CR_OM1_MASK       0x7
# define GPT_CR_SWR            (1 << 15)
#define GPT_PR                 (GPT_BASE_ADDR + 0x04)
# define GPT_PR_PRESCALER_MASK (0xFFF)
#define GPT_SR                 (GPT_BASE_ADDR + 0x08)
# define GPT_SR_OF1            (1 << 0)
# define GPT_SR_OF2            (1 << 1)
# define GPT_SR_OF3            (1 << 2)
# define GPT_SR_ROV            (1 << 5)
# define GPT_SR_MASK           0x1F
#define GPT_IR                 (GPT_BASE_ADDR + 0x0C)
# define GPT_IR_OF1            (1 << 0)
# define GPT_IR_OF2            (1 << 1)
# define GPT_IR_OF3            (1 << 2)
# define GPT_IR_ROVIE          (1 << 5)
#define GPT_OCR1               (GPT_BASE_ADDR + 0x10)
#define GPT_OCR2               (GPT_BASE_ADDR + 0x14)
#define GPT_OCR3               (GPT_BASE_ADDR + 0x18)
#define GPT_ICR1               (GPT_BASE_ADDR + 0x1C)
#define GPT_ICR2               (GPT_BASE_ADDR + 0x20)
#define GPT_CNT                (GPT_BASE_ADDR + 0x24)

#define PERIPHCLK       (SYS_CLOCK / 2)
#define TARGET_FREQ		OPTION_GET(NUMBER, freq)
#define LOAD_VALUE		PERIPHCLK / (TARGET_FREQ - 1)

static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	clock_tick_handler(data);

	REG32_STORE(GPT_IR, GPT_IR_OF1);

	return IRQ_HANDLED;
}

static int gpt_timer_init(struct clock_source *cs) {
	REG32_STORE(GPT_CR, 0);

	irq_attach(GPT_IRQ,
		  clock_handler,
		  0,
		  cs,
		  "i.MX General Purpose Timer");
	return 0;
}

static int this_set_periodic(struct clock_source *cs) {
	/* Init timer as described in 12.1.5.1 in IMX8MDQLQRM.pdf */
	REG32_STORE(GPT_CR, 0);

	REG32_STORE(GPT_IR, 0);

	REG32_STORE(GPT_CR, GPT_CR_SWR);

	while (REG32_LOAD(GPT_CR) & GPT_CR_SWR);

	REG32_STORE(GPT_CR, (1 << GPT_CR_CLKSRC_OFFT));

	REG32_STORE(GPT_SR, GPT_SR_MASK);

	REG32_ORIN(GPT_CR, GPT_CR_ENMOD);

	REG32_ORIN(GPT_CR, GPT_CR_EN);

	REG32_ORIN(GPT_CR, 0x1 << 20);

	REG32_STORE(GPT_IR, GPT_IR_OF1);

	REG32_STORE(GPT_OCR1, LOAD_VALUE);

	return 0;
}

static cycle_t this_get_cycles(struct clock_source *cs) {
	return REG32_LOAD(GPT_CNT);
}

static struct time_event_device gpt_timer_event = {
	.set_periodic = this_set_periodic,
	.irq_nr = GPT_IRQ,
};

static struct time_counter_device gpt_timer_counter = {
	.get_cycles = this_get_cycles,
	.cycle_hz = 1000,
};

PERIPH_MEMORY_DEFINE(gpt_timer, GPT_BASE_ADDR, 0x28);

CLOCK_SOURCE_DEF(gpt_timer, gpt_timer_init, NULL,
	&gpt_timer_event, &gpt_timer_counter);
