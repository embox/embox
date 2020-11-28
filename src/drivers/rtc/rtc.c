/**
 * @
 *
 * @date Nov 23, 2020
 * @author Anton Bondarev
 */

#include <errno.h>
#include <time.h>
#include <stddef.h>

#include <kernel/lthread/lthread.h>

#include <drivers/rtc.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(rtc_unit_init);

static struct lthread rtc_lthread;

extern struct rtc_device *global_rtc_device;

struct rtc_device *rtc_get_device(char *name) {
	return global_rtc_device;
}

int rtc_get_time(struct rtc_device *rtc, struct tm *tm) {
	int res;

	if (!rtc || !rtc->rtc_ops || !rtc->rtc_ops->get_time) {
		return -EINVAL;
	}

	res = rtc->rtc_ops->get_time(rtc, tm);

	return res;
}

int rtc_set_time(struct rtc_device *rtc, struct tm *tm) {
	int res;

	if (!rtc || !rtc->rtc_ops || !rtc->rtc_ops->set_time) {
		return -EINVAL;
	}

	res = rtc->rtc_ops->set_time(rtc, tm);

	return res;
}

int rtc_get_alarm(struct rtc_device *rtc, struct rtc_wkalrm *alarm) {
	int res;

	if (!rtc || !rtc->rtc_ops || !rtc->rtc_ops->get_alarm) {
		return -EINVAL;
	}

	res = rtc->rtc_ops->get_alarm(rtc, &(alarm->tm));

	return res;
}

int rtc_set_alarm(struct rtc_device *rtc, struct rtc_wkalrm *alarm) {
	int res;

	if (!rtc || !rtc->rtc_ops || !rtc->rtc_ops->set_alarm) {
		return -EINVAL;
	}

	res = rtc_alarm_irq_enable(rtc, 0);
	if (res) {
		goto out;
	}
	res = rtc->rtc_ops->set_alarm(rtc, &(alarm->tm));
	if (res) {
		goto out;
	}
	res = rtc_alarm_irq_enable(rtc, 1);

out:
	return res;
}

int rtc_irq_register(struct rtc_device *rtc, struct rtc_task *t) {
	int res;

	if (!rtc || !t) {
		return -EINVAL;
	}

	if (rtc->irq_task) {
		return -EBUSY;
	}

	res = rtc_alarm_irq_enable(rtc, 0);
	if (res) {
		goto out;
	}
	rtc->irq_task = t;
	res = rtc_alarm_irq_enable(rtc, 1);
out:
	return res;
}

void rtc_irq_unregister(struct rtc_device *rtc, struct rtc_task *t) {
	int res;

	if (!rtc || (rtc->irq_task != t)) {
		return;
	}

	if (rtc->irq_task) {
		return;
	}

	res = rtc_alarm_irq_enable(rtc, 0);
	if (res) {
		goto out;
	}
	rtc->irq_task = NULL;

out:
	return;
}

int rtc_alarm_irq_enable(struct rtc_device *rtc, int enabled) {
	int res;

	if (!rtc || !rtc->rtc_ops || !rtc->rtc_ops->alarm_irq_enable) {
		return -EINVAL;
	}

	res = rtc->rtc_ops->alarm_irq_enable(rtc,enabled);

	return res;
}

void rtc_update_irq(struct rtc_device *rtc, int num, uint32_t events) {
	rtc_alarm_irq_enable(rtc, 0);
	if (rtc->irq_task && rtc->irq_task->func) {
		lthread_launch(&rtc_lthread);
	}
}

static int rtc_lthread_handler(struct lthread *self) {

	if (!global_rtc_device) {
		return 0;
	}

	if (global_rtc_device->irq_task && global_rtc_device->irq_task->func) {
		global_rtc_device->irq_task->func(global_rtc_device->irq_task->private_data);
	}

	return 0;
}


static int rtc_unit_init(void) {
	lthread_init(&rtc_lthread, rtc_lthread_handler);
	return 0;
}
