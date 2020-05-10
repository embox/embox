/**
 * @file
 *
 * @date 05.05.2020
 * @author Alexander Kalmuk
 */
#include <hal/arch.h>
#include <hal/reg.h>
#include <framework/mod/options.h>

#define GPREG_BASE           OPTION_GET(NUMBER,gpregs_base)

#define GPREG_SET_FREEZE_REG (GPREG_BASE + 0x0)
# define GPREG_SET_FREEZE_SYS_WDOG (1 << 3)

/* Clock Generation Controller Registers */
#define CGR_BASE        0x50000000
#define CGR_CLK_AMBA_REG (CGR_BASE + 0x0)
# define CLK_AMBA_HCLK_DIV_MASK   0x7
# define CLK_AMBA_HCLK_DIV_SHIFT  0

void arch_init(void) {
	/* Disable watchdog. It was enabled by bootloader. */
	REG16_STORE(GPREG_SET_FREEZE_REG, GPREG_SET_FREEZE_SYS_WDOG);

	/* Clear clock divider. If it is zero it means divider is 1. */
	REG16_CLEAR(CGR_CLK_AMBA_REG, CLK_AMBA_HCLK_DIV_MASK);
}

void arch_idle(void) {
}

void _NORETURN arch_shutdown(arch_shutdown_mode_t mode) {
	while (1) {
	}
}
