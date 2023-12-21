/**
 * @brief Defines platform interface
 *
 * @date 20.12.23
 * @author Aleksey Zhmulin
 */

#ifndef HAL_PLATFORM_H_
#define HAL_PLATFORM_H_

#include <compiler.h>

typedef enum shutdown_mode {
	SHUTDOWN_MODE_HALT,
	SHUTDOWN_MODE_REBOOT,
	SHUTDOWN_MODE_ABORT,
} shutdown_mode_t;

extern void platform_init(void);

extern void platform_idle(void);

extern void _NORETURN platform_shutdown(shutdown_mode_t mode);

#endif /* HAL_PLATFORM_H_ */
