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
#include <compiler.h>

extern int printk(const char *format, ...) _PRINTF_FORMAT(1, 2);
extern int vprintk(const char *format, va_list args) _PRINTF_FORMAT(1, 0);

#endif /* KERNEL_PRINTK_H_ */
