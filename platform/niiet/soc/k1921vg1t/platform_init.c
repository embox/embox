#include <drivers/clk/niiet_rcu.h>

#include <hal/reg.h>

#define FLASH_LATENCY            2

#define FLASHM_CTRL_BASE         (0x50002000UL)
#define FLASHM_CTRL_CTRL_REG     (FLASHM_CTRL_BASE + 0x48UL)

void platform_init(void) {
	REG32_ORIN(FLASHM_CTRL_CTRL_REG, (FLASH_LATENCY << 16));

	niiet_sysclk_init();
}
