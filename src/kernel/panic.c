/**
 * @file
 * @brief TODO
 *
 * @date 13.02.2010
 * @author Eldar Abusalimov
 */

#include <stdarg.h>

#include <kernel/panic.h>
#include <kernel/printk.h>

void __attribute__ ((noreturn, format (printf, 1, 2)))
panic(const char *format, ...) {
	va_list args;

	printk("Kernel panic: ");

	va_start(args, format);
	printk(format, args);
	va_end(args);

	while (1) {
	}

}
