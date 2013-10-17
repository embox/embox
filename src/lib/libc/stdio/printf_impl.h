/**
 * @file
 * @brief
 *
 * @date 27.01.13
 * @author Ilia Vaprol
 */

#ifndef PRINTF_IMPL_H_
#define PRINTF_IMPL_H_

#include <stdarg.h>

struct printchar_handler_data;

extern int __print(void (*printchar_handler)(struct printchar_handler_data *d, int c),
		struct printchar_handler_data *printchar_data,
		const char *format, va_list args);

#endif /* PRINTF_IMPL_H_ */
