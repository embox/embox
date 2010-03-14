/**
 * @file
 * @brief TODO
 *
 * @date 13.02.2010
 * @author Eldar Abusalimov
 */

#ifndef PRINTK_H_
#define PRINTK_H_

#include <stdio.h>

extern int printk(const char *format, ...) __attribute__ ((format
				(printf, 1, 2)));

#endif /* PRINTK_H_ */
