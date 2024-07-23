/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */

#include <stdarg.h>

#include <kernel/printk.h>
#include <util/log.h>

static void klog_handler(struct mod_logger *logger, int flags, const char *fmt,
    va_list args) {
	if (flags & LOG_BEG) {
		printk("[%s] ", log_prio2str(LOG_PRIO(flags)));
		if (logger) {
			printk("(%s) ", logger->mod_name);
		}
	}
	vprintk(fmt, args);
	if (flags & LOG_END) {
		printk("\n");
	}
}

void klog_init(void) {
	log_set_handler(klog_handler);
}
