/**
 * @file
 * @brief
 *
 * @date 18.06.15
 * @author Vita Loginova
 */

#include <kernel/printk.h>
#include <util/logging.h>
#include <stdarg.h>

char *log_levels[LOG_DEBUG] = {
	"error",
	"warning",
	"info",
	"debug"
};

void logging_raw(struct logging *logging, int level, const char* fmt, ...) {
	if (logging && level <= logging->level) {
		va_list args;

		va_start(args, fmt);
		vprintk(fmt, args);
		va_end(args);
	}
}
