/**
 * @file imx6_gpt.c
 * @brief General Purpose Timer for i.MX6 board
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-06-12
 *
 * @author Evgeny Svirin <eugenysvirin@gmail.com>
 * @date 2021-04-9
 */
#include <sys/mman.h>

#include <drivers/common/memory.h>
#include <hal/clock.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/printk.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time_device.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

#define GPT_BASE      OPTION_GET(NUMBER, base_addr)
#define GPT_IRQ       OPTION_GET(NUMBER, irq_nr)
#define GPT_TARGET_HZ OPTION_GET(NUMBER, freq)

#define GPT_PRESCALER 0 /* 2^0 */

#define GPT_CR   (GPT_BASE + 0x00) /* Control register */
#define GPT_PR   (GPT_BASE + 0x04) /* Prescaler register */
#define GPT_SR   (GPT_BASE + 0x08) /* Status register */
#define GPT_IR   (GPT_BASE + 0x0C) /* Interrupt register */
#define GPT_OCR1 (GPT_BASE + 0x10) /* Output compare register 1 */
#define GPT_OCR2 (GPT_BASE + 0x14) /* Output compare register 2 */
#define GPT_OCR3 (GPT_BASE + 0x18) /* Output compare register 3 */
#define GPT_ICR1 (GPT_BASE + 0x1C) /* Input capture register 1 */
#define GPT_ICR2 (GPT_BASE + 0x20) /* Input capture register 2 */
#define GPT_CNT  (GPT_BASE + 0x24) /* Counter register */

#define GPT_IR_INT1   (1 << 0)     /* Enable interruption on OutputCompare1 */
#define GPT_CR_MASK   (0b10000011) /* Enable interruptable periodic timer mode */
#define GPT_SR_CLR    (0)          /* Clear status mask */

#define GPT_CR_EN     (1u <<  0)
#define GPT_CR_ENMOD  (1u <<  1)
#define GPT_CR_DBGEN  (1u <<  2)
#define GPT_CR_WAITEN (1u <<  3)
#define GPT_CR_DOZEEN (1u <<  4)
#define GPT_CR_STOPEN (1u <<  5)
#define GPT_CR_FRR    (1u <<  9)
#define GPT_CR_SWR    (1u << 15)
#define GPT_CR_FO1    (1u << 29)
#define GPT_CR_FO2    (1u << 30)
#define GPT_CR_FO3    (1u << 31)

#define GPT_CR_CLKSRC_OFFT  6
#define GPT_CR_IM1_OFFT    16
#define GPT_CR_IM2_OFFT    18
#define GPT_CR_OM1_OFFT    20
#define GPT_CR_OM2_OFFT    23
#define GPT_CR_OM3_OFFT    26

#define GPT_IR_OF1IE (1 << 0)

#define CLKSRC_PERIPH  1
#define CLKSRC_LOWFREQ 4

/* By default, periphal clock source is PPL2, 528MHz */
#define _BASE_HZ     528000000
#define _BASE_SCALER 8

static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	clock_tick_handler(data);

	REG32_CLEAR(GPT_SR, GPT_SR_CLR);
	return IRQ_HANDLED;
}

static int imx6_gpt_init(struct clock_source *cs) {
	/* We can configure GPT only when disabled */
	REG32_STORE(GPT_CR,   0);
	REG32_STORE(GPT_IR,   0);

	return irq_attach(GPT_IRQ,
	                  clock_handler,
	                  0,
	                  cs,
	                  "i.MX6 General Purpose Timer");
}

static int imx6_gpt_set_periodic(struct clock_source *cs) {
	REG32_STORE(GPT_PR, GPT_PRESCALER);
	REG32_STORE(GPT_OCR1, _BASE_HZ / _BASE_SCALER / GPT_TARGET_HZ); 
	REG32_STORE(GPT_IR, GPT_IR_INT1);
	REG32_STORE(GPT_CR, GPT_CR_MASK); 
	
	return 0;
}

static cycle_t imx6_gpt_get_cycles(struct clock_source *cs) {
	return REG32_LOAD(GPT_CNT);
}

static struct time_event_device imx6_gpt_event = {
	.set_periodic = imx6_gpt_set_periodic,
	.irq_nr = GPT_IRQ,
};

static struct time_counter_device imx6_gpt_counter = {
	.get_cycles = imx6_gpt_get_cycles,
	.cycle_hz = GPT_TARGET_HZ,
};

STATIC_IRQ_ATTACH(GPT_IRQ, clock_handler, &this_clock_source);

PERIPH_MEMORY_DEFINE(imx_gpt, GPT_BASE, 0x40);

CLOCK_SOURCE_DEF(imx6_gpt,  imx6_gpt_init, NULL,
	&imx6_gpt_event, &imx6_gpt_counter);
