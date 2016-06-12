/**
 * @file imx6_gpt.c
 * @brief General Purpose Timer for i.MX6 board
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-06-12
 */

#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time_device.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

#define GPT_BASE OPTION_GET(NUMBER, base_addr)
#define GPT_IRQ  OPTION_GET(NUMBER, irq_nr)

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

static struct clock_source imx6_gpt_clock_source;
static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	return IRQ_HANDLED;
}

static int imx6_gpt_init(void) {
	return irq_attach(GPT_IRQ,
	                  clock_handler,
	                  0,
	                  &imx6_gpt_clock_source,
	                  "i.MX6 General Purpose Timer");
}

static int imx6_gpt_config(struct time_dev_conf * conf) {
	return 0;
}

static cycle_t imx6_gpt_read(void) {
	return 0;
}

static struct time_event_device imx6_gpt_event = {
	.config   = imx6_gpt_config,
	.event_hz = 1000,
	.irq_nr   = GPT_IRQ,
};

static struct time_counter_device imx6_gpt_counter = {
	.read     = imx6_gpt_read,
	.cycle_hz = 1000,
};

static struct clock_source imx6_gpt_clock_source = {
	.name           = "imx6_gpt",
	.event_device   = &imx6_gpt_event,
	.counter_device = &imx6_gpt_counter,
	.read           = clock_source_read,
};

EMBOX_UNIT_INIT(imx6_gpt_init);

STATIC_IRQ_ATTACH(GPT_IRQ, clock_handler, &imx6_clock_source);
