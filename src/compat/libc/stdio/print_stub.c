/**
 * @file
 *
 *  @date May 9, 2023
 *  @author Anton Bondarev
 */

#include <stdarg.h>

#include "printf_impl.h"

int __print(int (*printchar_handler)(struct printchar_handler_data *d, int c),
    struct printchar_handler_data *printchar_data, const char *format,
    va_list args) {
	return -1;
}
