/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 10.11.23
 */
#include <stdarg.h>
#include <stdint.h>
#include <time.h>

#include <framework/mod/options.h>
#include <kernel/printk.h>
#include <util/log.h>

#define PRINT_MOD_NAME OPTION_GET(BOOLEAN, print_mod_name)

static void klog_handler(struct logger *logger, uint16_t flags, const char *fmt,
    va_list args) {
	if (flags & LOG_BEG) {
		printk("[%s] ", log_prio2str(LOG_PRIO(flags)));
#if PRINT_MOD_NAME
		if (logger) {
			printk("(%s) ", logger->mod->build_info->mod_name);
		}
#endif
	}
	vprintk(fmt, args);
	if (flags & LOG_END) {
		printk("\n");
	}
}

void klog_init(void) {
	log_set_handler(klog_handler);
}
