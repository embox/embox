/**
 * @file
 *
 * @date   26.11.2020
 * @author Alexander Kalmuk
 */

#include <time.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <drivers/rtc.h>
#include <framework/mod/options.h>
#include <embox/unit.h>

#include <config/custom_config_qspi.h>
#include <hw_rtc.h>

#define DA1469X_RTC_IRQ       OPTION_GET(NUMBER, irq)

EMBOX_UNIT_INIT(da1469x_rtc_init);

static irq_return_t da1469x_rtc_irq_handler(unsigned int irq_nr, void *data) {
	hw_rtc_get_event_flags();

	rtc_update_irq((struct rtc_device *) data, 1, (RTC_AF | RTC_IRQF));

	return IRQ_HANDLED;
}

static int da1469x_rtc_get_time(struct rtc_device *dev, struct tm *tm) {
	hw_rtc_time_t time;
	hw_rtc_calendar_t clndr;

	hw_rtc_get_time_clndr(&time, &clndr);

	tm->tm_hour = time.hour;
	tm->tm_min  = time.minute;
	tm->tm_sec  = time.sec;

	tm->tm_year = clndr.year - 1900;
	tm->tm_mon  = clndr.month - 1;
	tm->tm_mday = clndr.mday;
	tm->tm_wday = clndr.wday - 1;

	return 0;
}

static int da1469x_rtc_set_time(struct rtc_device *dev, struct tm *tm) {
	const hw_rtc_time_t time = {
		.hour   = tm->tm_hour,
		.minute = tm->tm_min,
		.sec    = tm->tm_sec,
		.hsec   = 0,
	};
	const hw_rtc_calendar_t clndr = {
		.year  = tm->tm_year + 1900,
		.month = tm->tm_mon + 1,
		.mday  = tm->tm_mday,
		.wday  = tm->tm_wday + 1,
	};

	hw_rtc_set_time_clndr(&time, &clndr);

	return 0;
}

static int da1469x_rtc_get_alarm(struct rtc_device *dev, struct tm *tm) {
	hw_rtc_time_t time;
	hw_rtc_alarm_calendar_t clndr;

	hw_rtc_get_alarm(&time, &clndr, NULL);

	tm->tm_hour = time.hour;
	tm->tm_min  = time.minute;
	tm->tm_sec  = time.sec;

	tm->tm_mon  = clndr.month - 1;
	tm->tm_mday = clndr.mday;

	return 0;
}

int da1469x_rtc_set_alarm(struct rtc_device *dev, struct tm *tm) {
	const hw_rtc_time_t time = {
		.hour   = tm->tm_hour,
		.minute = tm->tm_min,
		.sec    = tm->tm_sec,
		.hsec   = 0,
	};
	const hw_rtc_alarm_calendar_t clndr = {
		.mday  = tm->tm_mday,
		.month = tm->tm_mon + 1,
	};

	hw_rtc_set_alarm(&time, &clndr, 0xff);

	return 0;
}

int da1469x_rtc_alarm_irq_enable(struct rtc_device *dev, int enabled) {
	if (enabled) {
		hw_rtc_interrupt_enable(HW_RTC_INT_ALRM);
	} else {
		hw_rtc_interrupt_disable(HW_RTC_INT_ALRM);
	}

	return 0;
}

static struct rtc_ops da1469x_rtc_ops = {
	.get_time = da1469x_rtc_get_time,
	.set_time = da1469x_rtc_set_time,
	.get_alarm = da1469x_rtc_get_alarm,
	.set_alarm = da1469x_rtc_set_alarm,
	.alarm_irq_enable = da1469x_rtc_alarm_irq_enable,
};

static struct rtc_device da1469x_rtc_device = {
	.rtc_ops = &da1469x_rtc_ops
};

RTC_DEVICE_DEF(&da1469x_rtc_device);

static int da1469x_rtc_init(void) {
	return irq_attach(DA1469X_RTC_IRQ,
			da1469x_rtc_irq_handler,
			0,
			&da1469x_rtc_device,
			"da1469x_rtc");
}

STATIC_IRQ_ATTACH(DA1469X_RTC_IRQ, da1469x_rtc_irq_handler, &da1469x_rtc_device);
