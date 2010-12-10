/**
 * @file
 *
 * @brief
 *
 * @date 25.11.2010
 * @author Anton Bondarev
 */

#ifndef PROM_PRINTF_H_
#define PROM_PRINTF_H_

#include <stdio.h>

extern int prom_printf(const char *format, ...) __attribute__ ((format
				(printf, 1, 2)));

#endif /* PROM_PRINTF_H_ */
