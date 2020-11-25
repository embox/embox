/**
 * @file
 * @brief ARM PrimeCell Real Time Clock driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2019-07-18
 */

#include <time.h>

#include <drivers/common/memory.h>

#include <hal/reg.h>
#include <kernel/irq.h>

#include <drivers/rtc.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(pl031_init);

#define PL031_BASE      OPTION_GET(NUMBER, base_addr)
#define PL031_IRQ       OPTION_GET(NUMBER, irq_nr)
#define PL031_TARGET_HZ 1

#define PL031_DR   (PL031_BASE + 0x00) /* Data register */
#define PL031_MR   (PL031_BASE + 0x04) /* Match register */ 
#define PL031_LR   (PL031_BASE + 0x08) /* Load register */
#define PL031_CR   (PL031_BASE + 0x0C) /* Control register */
#define PL031_IMSC (PL031_BASE + 0x10) /* Interrupt Mask Set or Clear register */
#define PL031_RIS  (PL031_BASE + 0x14) /* Raw Interrupt Status register */
#define PL031_MIS  (PL031_BASE + 0x18) /* Masked Interrupt Status register */
#define PL031_ICR  (PL031_BASE + 0x1C) /* Interrupt Clear register */

#define PL031_CR_START     (1 << 0)
#define PL031_IMSC_EN      (1 << 0)
#define PL031_ICR_CLEAR    (1 << 0)

static irq_return_t pl031_irq_handler(unsigned int irq_nr, void *data) {

	REG32_STORE(PL031_ICR, PL031_ICR_CLEAR); /* Clear interrupt */

	rtc_update_irq(data, 1, (RTC_AF | RTC_IRQF));
	return IRQ_HANDLED;
}

static int pl031_get_time(struct rtc_device *dev, struct tm *tm) {
	time_t time;
	time = REG32_LOAD(PL031_DR);
	gmtime_r(&time, tm);
	return 0;
}

static int pl031_set_time(struct rtc_device *dev, struct tm *tm) {
	REG32_STORE(PL031_LR, mktime(tm));
	return 0;
}

static int pl031_get_alarm(struct rtc_device *dev, struct tm *tm) {
	time_t time;
	time = REG32_LOAD(PL031_MR);
	gmtime_r(&time, tm);
	return 0;
}

static int pl031_set_alarm(struct rtc_device *dev, struct tm *tm) {
	REG32_STORE(PL031_MR, mktime(tm));
	return 0;
}

int pl031_alarm_irq_enable(struct rtc_device *dev, int enabled) {
	if (enabled) {
		REG32_STORE(PL031_IMSC, PL031_IMSC_EN); /* Enable IRQ */
	} else {
		REG32_STORE(PL031_IMSC, 0); /* disable IRQ */
	}
	return 0;
}

static struct rtc_ops pl031_ops = {
	.get_time = pl031_get_time,
	.set_time = pl031_set_time,
	.get_alarm = pl031_get_alarm,
	.set_alarm = pl031_set_alarm,
	.alarm_irq_enable = pl031_alarm_irq_enable
};

static struct rtc_device pl031_rtc_device = {
	.rtc_ops = &pl031_ops
};

RTC_DEVICE_DEF(&pl031_rtc_device);

static int pl031_init(void) {
	REG32_STORE(PL031_CR, PL031_CR_START);  /* Enable counter */

	return irq_attach(PL031_IRQ,
			pl031_irq_handler,
			0,
			&pl031_rtc_device,
			"PL031");
}

STATIC_IRQ_ATTACH(PL031_IRQ, pl031_irq_handler, &pl031_rtc_device;);

PERIPH_MEMORY_DEFINE(pl031, PL031_BASE, 0x20);
