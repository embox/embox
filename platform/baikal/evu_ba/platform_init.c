/**
 * @file
 *
 * @date Sep 26, 2025
 * @author Anton Bondarev
 */

#include <hal/platform.h>

extern void board_clocks_init(void);

void platform_init(void) {
	board_clocks_init();
}
