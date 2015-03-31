/**
 * @file
 * @brief Defines panic macros
 *
 * @date 30.01.10
 * @author Eldar Abusalimov
 */

#ifndef KERNEL_PANIC_H_
#define KERNEL_PANIC_H_

#include <hal/arch.h>
#include <hal/ipl.h>
#include <kernel/printk.h>
#include <lib/debug/whereami.h>

#define panic(...) \
	do { \
		ipl_disable(); \
		printk(__VA_ARGS__); \
		whereami(); \
		arch_shutdown(ARCH_SHUTDOWN_MODE_ABORT); \
	} while (0)

#endif /* KERNEL_PANIC_H_ */
