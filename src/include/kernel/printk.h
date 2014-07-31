/**
 * @file
 * @brief Printk declaration
 *
 * @date 13.02.10
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_PRINTK_H_
#define KERNEL_PRINTK_H_

#include <stdarg.h>

extern int printk(const char *format, ...) __attribute__ ((format
				(printf, 1, 2)));
extern int vprintk(const char *format, va_list args) __attribute__ ((format
				(printf, 1, 0)));

#endif /* KERNEL_PRINTK_H_ */
