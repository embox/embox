/**
 * @
 *
 * @date Nov 23, 2020
 * @author Anton Bondarev
 */

#ifndef SRC_DRIVERS_RTC_RTC_H_
#define SRC_DRIVERS_RTC_RTC_H_

#include <stdint.h>
#include <time.h>

#define RTC_DEVICE_NAME_SIZE 8

/* interrupt flags */
#define RTC_IRQF  0x80 /* any of the following is active */
#define RTC_PF    0x40
#define RTC_AF    0x20
#define RTC_UF    0x10

struct rtc_task;
struct rtc_ops;

struct rtc_device {
	struct rtc_task *irq_task;
	void *priv_data;
	char name[RTC_DEVICE_NAME_SIZE];
	struct rtc_ops *rtc_ops;
};

#define RTC_DEVICE_DEF(rtc_dev) \
		struct rtc_device *global_rtc_device = rtc_dev;

struct rtc_ops {
	int (*get_time)(struct rtc_device *, struct tm *);
	int (*set_time)(struct rtc_device *, struct tm *);
	int (*get_alarm)(struct rtc_device *, struct tm *);
	int (*set_alarm)(struct rtc_device *, struct tm *);
	int (*alarm_irq_enable)(struct rtc_device *, int enabled);
};

extern void rtc_update_irq(struct rtc_device *rtc, int num, uint32_t events);

struct rtc_device;

struct rtc_wkalrm {
	unsigned char enabled;  /* 0 = alarm disabled, 1 = alarm enabled */
	unsigned char pending;  /* 0 = alarm not pending, 1 = alarm pending */
	struct tm tm;           /* time the alarm is set to */
};

extern struct rtc_device *rtc_get_device(char *name);

extern int rtc_get_time(struct rtc_device *rtc, struct tm *tm);

extern int rtc_set_time(struct rtc_device *rtc, struct tm *tm);

extern int rtc_get_alarm(struct rtc_device *rtc, struct rtc_wkalrm *alarm);
extern int rtc_set_alarm(struct rtc_device *rtc, struct rtc_wkalrm *alarm);

struct rtc_task {
	void (*func)(void *private_data);
	void *private_data;
};

extern int rtc_irq_register(struct rtc_device *rtc, struct rtc_task *t);
extern void rtc_irq_unregister(struct rtc_device *rtc, struct rtc_task *t);

extern int rtc_alarm_irq_enable(struct rtc_device *rtc, int enabled);

#endif /* SRC_DRIVERS_RTC_RTC_H_ */
