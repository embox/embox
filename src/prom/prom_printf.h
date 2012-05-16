/**
 * @file
 *
 * @brief
 *
 * @date 06.09.2011
 * @author Anton Bondarev
 */

#ifndef PROM_PRINTF_IMPL_H_
#define PROM_PRINTF_IMPL_H_

#include <stdio.h>

extern int prom_printf(const char *format, ...) __attribute__ ((format
				(printf, 1, 2)));

extern int prom_vprintf(const char *format, va_list args);

#endif /* PROM_PRINTF_IMPL_H_ */
