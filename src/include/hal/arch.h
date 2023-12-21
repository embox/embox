/**
 * @brief Defines ARCH interface (deprecated)
 *
 * @date 25.11.09
 * @author Eldar Abusalimov
 */

#ifndef HAL_ARCH_H_
#define HAL_ARCH_H_

#include <compiler.h>

#include <asm/hal/arch.h>
#include <hal/platform.h>

#define ARCH_SHUTDOWN_MODE_HALT   SHUTDOWN_MODE_HALT
#define ARCH_SHUTDOWN_MODE_REBOOT SHUTDOWN_MODE_REBOOT
#define ARCH_SHUTDOWN_MODE_ABORT  SHUTDOWN_MODE_ABORT

typedef shutdown_mode_t arch_shutdown_mode_t;

extern void arch_init(void);

extern void arch_idle(void);

extern void _NORETURN arch_shutdown(arch_shutdown_mode_t mode);

#endif /* HAL_ARCH_H_ */
