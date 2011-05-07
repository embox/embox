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
#include <types.h>

typedef enum {
	ARCH_SHUTDOWN_MODE_HALT,
	ARCH_SHUTDOWN_MODE_REBOOT,
	ARCH_SHUTDOWN_MODE_ABORT,
} arch_shutdown_mode_t;

#if 0
/**
 * Low level structure prepared by boot loader.
 */
typedef const struct {

	struct {
		paddr_t base;
		psize_t size;
	} memory[];
	int memory_sz;

} arch_bootinfo_t;
#endif

/**
 * Performs basic machine-dependent initialization.
 *
 * @note Implementation have to setup such low-level features as e.g. cache.
 */
void arch_init(void);

void arch_idle(void);

void arch_shutdown(arch_shutdown_mode_t mode) __attribute__ ((noreturn));

#if 0
arch_bootinfo_t *arch_bootinfo(void);
#endif

#endif /* HAL_ARCH_H_ */
