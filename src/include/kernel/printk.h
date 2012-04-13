/**
 * @file
 * @brief Printk declaration
 *
 * @date 13.02.10
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_PRINTK_H_
#define KERNEL_PRINTK_H_

extern int printk(const char *format, ...) __attribute__ ((format
				(printf, 1, 2)));

#endif /* KERNEL_PRINTK_H_ */
