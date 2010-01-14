/**
 * @file arch.h
 *
 * @brief TODO
 *
 * @date 25.11.2009
 * @author Eldar Abusalimov
 */

#ifndef HAL_ARCH_H_
#define HAL_ARCH_H_

#include <types.h>

typedef enum {
	ARCH_SHUTDOWN_MODE_HALT,
	ARCH_SHUTDOWN_MODE_REBOOT,
	ARCH_SHUTDOWN_MODE_DUMP,
} arch_shutdown_mode_t;

#if 0
/**
 * Low level structure prepared by boot loader.
 * TODO Provides necessary
 */
typedef struct {

	struct {
		paddr_t base;
		psize_t size;
	} memory[];
	int memory_sz;

	char (*getc)(void);
	void (*putc)(char ch);

} arch_bootinfo_t;
#endif

/**
 * Performs basic machine-dependent initialization.
 *
 * @note Implementation have to setup such low-level features as e.g. cache.
 */
void arch_init(void);

void arch_idle(void);

void arch_shutdown(arch_shutdown_mode_t mode);

#if 0
arch_bootinfo_t *arch_bootinfo_get(void);
#endif

#endif /* HAL_ARCH_H_ */
