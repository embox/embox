/**
 * @file
 * @brief
 *
 * @date 14.02.10
 * @author Eldar Abusalimov
 */

#include <asm/cache.h>

extern void kernel_start(void);

void sparc_kernel_start(void) {
	cache_enable();
	kernel_start();
}
