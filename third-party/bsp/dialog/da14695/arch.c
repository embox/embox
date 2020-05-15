/**
 * @file
 * @brief
 *
 * @date 10.05.2020
 * @author Alexander Kalmuk
 */

#include <string.h>
#include <assert.h>
#include <kernel/irq.h>
#include <util/log.h>
#include <hal/arch.h>
#include <hal/reg.h>
#include <arm/fpu.h>

#include <config/custom_config_qspi.h>

#include <sys_clock_mgr.h>

#define GPREG_SET_FREEZE_REG (GPREG_BASE + 0x0)
# define GPREG_SET_FREEZE_SYS_WDOG (1 << 3)

#define FPU_CPACR  0xE000ED88
#define FPU_FPCCR  0xE000EF34

#define PLL_LOCK_IRQ      49
static_assert(PLL_LOCK_IRQ == PLL_LOCK_IRQn + 16);

#define XTAL32M_RDY_IRQ   42
static_assert(XTAL32M_RDY_IRQ == XTAL32M_RDY_IRQn + 16);

extern void XTAL32M_Ready_Handler(void);
static irq_return_t xtal32m_irq_handler(unsigned int irq_nr,
		void *data) {
	XTAL32M_Ready_Handler();
	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(XTAL32M_RDY_IRQ, xtal32m_irq_handler, NULL);

extern void PLL_Lock_Handler(void);
static irq_return_t pll_lock_irq_handler(unsigned int irq_nr,
		void *data) {
	PLL_Lock_Handler();
	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(PLL_LOCK_IRQ, pll_lock_irq_handler, NULL);

extern void SystemInitPre(void);
extern void da1469x_SystemInit(void);

extern char _bss_vma;
extern char _bss_len;

void arch_init(void) {
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

	SystemInitPre();
	da1469x_SystemInit();

	/* SystemInitPre and da1469x_SystemInit use BSS variables, so reinit BSS.*/
	memset(&_bss_vma, 0, (int) &_bss_len);
}

void arch_idle(void) {
}

void _NORETURN arch_shutdown(arch_shutdown_mode_t mode) {
	while (1) {
	}
}
