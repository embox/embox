#include <drivers/clk/niiet_rcu.h>

#include <hal/reg.h>

#include <config/board_config.h>

//#define FLASH_LATENCY            2
#define FLASH_LATENCY            CONF_FLASHM_MISC_FLASH_LATENCY

//#define FLASHM_CTRL_BASE         (0x50002000UL)
#define FLASHM_CTRL_BASE         CONF_FLASHM_REGION_BASE

#define FLASHM_CTRL_CTRL_REG     (FLASHM_CTRL_BASE + 0x48UL)

static void niiet_system_flash_latency_set(void) {
	REG32_ORIN(FLASHM_CTRL_CTRL_REG, (FLASH_LATENCY << 16));
}

void platform_init(void) {
	niiet_system_flash_latency_set();

	niiet_sysclk_init();
}
