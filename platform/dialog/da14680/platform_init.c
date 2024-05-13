/**
 * @file
 *
 * @date 05.05.2020
 * @author Alexander Kalmuk
 */
#include <arm/fpu.h>
#include <framework/mod/options.h>
#include <hal/platform.h>
#include <hal/reg.h>

#define GPREG_BASE                OPTION_GET(NUMBER, gpregs_base)

#define GPREG_SET_FREEZE_REG      (GPREG_BASE + 0x0)
#define GPREG_SET_FREEZE_SYS_WDOG (1 << 3)

/* Clock Generation Controller Registers */
#define CGR_BASE                  0x50000000
#define CGR_CLK_AMBA_REG          (CGR_BASE + 0x0)
#define CLK_AMBA_HCLK_DIV_MASK    0x7
#define CLK_AMBA_HCLK_DIV_SHIFT   0

#define FPU_CPACR                 0xE000ED88
#define FPU_FPCCR                 0xE000EF34

void platform_init(void) {
#ifdef ARM_FPU_VFP
	/* Enable FPU */
	/** FIXME Currently FPU is enabled permanently */
	REG32_ORIN(FPU_CPACR, 0xf << 20);
	/* Disable FPU context preservation/restoration on exception
	 * entry and exit, because we can guarantee every irq handler
	 * execute without using FPU */
	REG32_CLEAR(FPU_FPCCR, 0x3 << 30);
#endif

	/* Disable watchdog. It was enabled by bootloader. */
	REG16_STORE(GPREG_SET_FREEZE_REG, GPREG_SET_FREEZE_SYS_WDOG);

	/* Clear clock divider. If it is zero it means divider is 1. */
	REG16_CLEAR(CGR_CLK_AMBA_REG, CLK_AMBA_HCLK_DIV_MASK);
}
