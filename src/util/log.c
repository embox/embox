/**
 * @file
 * @brief
 *
 * @date 18.06.15
 * @author Vita Loginova
 */

#include <assert.h>
#include <stdarg.h>

#include <kernel/printk.h>
#include <util/log.h>

char *log_levels[LOG_DEBUG] = {"error", "warning", "info", "debug"};

void logging_raw(struct logging *logging, int level, const char *fmt, ...) {
	assert(logging);

	if (level <= logging->level) {
		va_list args;

		va_start(args, fmt);
		vprintk(fmt, args);
		va_end(args);
	}
}
