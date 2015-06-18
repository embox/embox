/**
 * @file
 * @brief
 *
 * @date 18.06.15
 * @author Vita Loginova
 */

#include <kernel/printk.h>
#include <util/log.h>
#include <stdarg.h>

char *log_levels[LOG_DEBUG] = {
	"error",
	"warning",
	"info",
	"debug"
};

void __log_raw(struct logger *logger, int level, const char* fmt, ...) {
	if (logger && level <= logger->level) {
		va_list args;

		va_start(args, fmt);
		vprintk(fmt, args);
		va_end(args);
	}
}
