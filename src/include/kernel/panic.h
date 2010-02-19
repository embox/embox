/**
 * @file panic.h
 * @brief TODO
 *
 * @date 30.01.2010
 * @author Eldar Abusalimov
 */

#ifndef PANIC_H_
#define PANIC_H_

extern void panic(const char *format, ...) __attribute__ ((noreturn,
				format (printf, 1, 2)));

#endif /* PANIC_H_ */
