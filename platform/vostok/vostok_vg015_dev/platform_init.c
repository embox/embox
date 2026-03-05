#include <drivers/clk/niiet_rcu.h>

void platform_init(void) {
	niiet_sysclk_init();
}
