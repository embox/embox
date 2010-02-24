/**
 * @file
 * @brief TODO
 *
 * @date 30.01.2010
 * @author Eldar Abusalimov
 */

#ifndef PANIC_H_
#define PANIC_H_

#include <kernel/printk.h>
#include <hal/arch.h>

#define panic(...) \
	do { \
		printk(__VA_ARGS__); \
		arch_shutdown(ARCH_SHUTDOWN_MODE_ABORT); \
	} while(0)

#endif /* PANIC_H_ */
