/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#include <kernel/printk.h>
#include <util/log.h>

static void klog_handler(struct logger *logger, unsigned flags, const char *fmt,
    va_list args) {
	if (flags & LOG_PREFIX) {
		printk("%s", log_get_prefix(LOG_PRIO(flags)));
	}
	vprintk(fmt, args);
	if (flags & LOG_POSTFIX) {
		printk("\n");
	}
}

void klog_init(void) {
	log_set_handler(klog_handler);
}
