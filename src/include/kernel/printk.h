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

#ifdef CONFIG_PRINTK
# define printk(...)  printf(__VA_ARGS__)
#else
# define printk(...)  do { } while(0)
#endif /* CONFIG_PRINTK */

#if 0
extern int printk(const char *format, ...) __attribute__ ((format
				(printf, 1, 2)));
#endif

#endif /* PRINTK_H_ */
