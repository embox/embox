/**
 * @file
 * @brief Defines ARCH interface
 *
 * @date 25.11.09
 * @author Eldar Abusalimov
 */

#ifndef HAL_ARCH_H_
#define HAL_ARCH_H_

#include <asm/hal/arch.h>
#include <compiler.h>

typedef enum {
	ARCH_SHUTDOWN_MODE_HALT,
	ARCH_SHUTDOWN_MODE_REBOOT,
	ARCH_SHUTDOWN_MODE_ABORT,
} arch_shutdown_mode_t;

/**
 * Performs basic machine-dependent initialization.
 *
 * @note Implementation have to setup such low-level features as e.g. cache.
 */
extern void arch_init(void);

extern void arch_idle(void);

extern void arch_shutdown(arch_shutdown_mode_t mode) _NORETURN;

#endif /* HAL_ARCH_H_ */
