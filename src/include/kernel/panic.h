/**
 * @file
 * @brief Defines panic macros
 *
 * @date 30.01.10
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_PANIC_H_
#define KERNEL_PANIC_H_

#include <kernel/printk.h>
#include <hal/arch.h>

#define panic(...) \
	do { \
		printk(__VA_ARGS__); \
		arch_shutdown(ARCH_SHUTDOWN_MODE_ABORT); \
	} while (0)

#endif /* KERNEL_PANIC_H_ */
