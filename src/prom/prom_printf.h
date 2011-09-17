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

#endif /* PROM_PRINTF_IMPL_H_ */
