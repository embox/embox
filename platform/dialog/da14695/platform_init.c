/**
 * @file
 * @brief
 *
 * @date 10.05.2020
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <hw_cache.h>
#include <hw_otpc.h>
#include <hw_rtc.h>
#include <hw_sys.h>
#include <hw_watchdog.h>
#include <string.h>
#include <sys_clock_mgr.h>
#include <sys_power_mgr.h>

#include <arm/fpu.h>
#include <framework/mod/options.h>
#include <hal/platform.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <util/log.h>

#include <config/custom_config_qspi.h>

#define GPREG_SET_FREEZE_REG      (GPREG_BASE + 0x0)
#define GPREG_SET_FREEZE_SYS_WDOG (1 << 3)

#define FPU_CPACR                 0xE000ED88
#define FPU_FPCCR                 0xE000EF34

#define PLL_LOCK_IRQ              OPTION_GET(NUMBER, pll_lock_irq)
static_assert(PLL_LOCK_IRQ == PLL_LOCK_IRQn, "");

#define XTAL32M_RDY_IRQ OPTION_GET(NUMBER, xtal32m_rdy_irq)
static_assert(XTAL32M_RDY_IRQ == XTAL32M_RDY_IRQn, "");

#define FLASH_SIZE OPTION_GET(NUMBER, flash_size)

#if FLASH_SIZE == 256
#define CACHE_FLASH_SIZE HW_CACHE_FLASH_REGION_SZ_256KB
#elif FLASH_SIZE == 512
#define CACHE_FLASH_SIZE HW_CACHE_FLASH_REGION_SZ_512KB
#elif FLASH_SIZE == 1024
#define CACHE_FLASH_SIZE HW_CACHE_FLASH_REGION_SZ_1MB
#elif FLASH_SIZE == 2048
#define CACHE_FLASH_SIZE HW_CACHE_FLASH_REGION_SZ_2MB
#elif FLASH_SIZE == 4096
#define CACHE_FLASH_SIZE HW_CACHE_FLASH_REGION_SZ_4MB
#elif FLASH_SIZE == 8192
#define CACHE_FLASH_SIZE HW_CACHE_FLASH_REGION_SZ_8MB
#else
#error Unsupported flash size
#endif

extern void XTAL32M_Ready_Handler(void);
static irq_return_t xtal32m_irq_handler(unsigned int irq_nr, void *data) {
	XTAL32M_Ready_Handler();
	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(XTAL32M_RDY_IRQ, xtal32m_irq_handler, NULL);

extern void PLL_Lock_Handler(void);
static irq_return_t pll_lock_irq_handler(unsigned int irq_nr, void *data) {
	PLL_Lock_Handler();
	return IRQ_HANDLED;
}
STATIC_IRQ_ATTACH(PLL_LOCK_IRQ, pll_lock_irq_handler, NULL);

extern void SystemInitPre(void);
extern void da1469x_SystemInit(void);

extern char _bss_vma, _bss_end;
extern char __zero_table_start__, __zero_table_end__;

static void rtc_init(void) {
	// Enable the RTC peripheral clock
	hw_rtc_clock_enable();

	// Start the RTC
	hw_rtc_time_start();
}

__attribute__((section(".text.bootcode"))) void hardware_init_hook(void) {
	/* Default value is 512Kb. I will be changed only after software reset. */
	if (hw_cache_flash_get_region_size() == HW_CACHE_FLASH_REGION_SZ_512KB) {
		hw_cache_flash_set_region_size(CACHE_FLASH_SIZE);
		NVIC_SystemReset();
	}
}

void platform_init(void) {
	int i;

	/* Disable watchdog. It was enabled by bootloader. */
	REG16_STORE(GPREG_SET_FREEZE_REG, GPREG_SET_FREEZE_SYS_WDOG);

	for (i = 0; i < 1 * 1000 * 1000 * 1; i++) {}

#ifdef ARM_FPU_VFP
	/* Enable FPU */
	/** FIXME Currently FPU is enabled permanently */
	REG32_ORIN(FPU_CPACR, 0xf << 20);
	/* Disable FPU context preservation/restoration on exception
	 * entry and exit, because we can guarantee every irq handler
	 * execute without using FPU */
	REG32_CLEAR(FPU_FPCCR, 0x3 << 30);
#endif

	SystemInitPre();
	da1469x_SystemInit();

	/* Init all BSS once more except retained data. */
	memset(&_bss_vma, 0, &__zero_table_start__ - &_bss_vma);
	memset(&__zero_table_end__, 0, &_bss_end - &__zero_table_end__);

	/* From pm_system_init */
#if dg_configUSE_HW_OTPC
	if (dg_configCODE_LOCATION != NON_VOLATILE_IS_OTP) {
		hw_otpc_disable(); // Make sure OTPC is off
	}
#endif

	extern void ad_pmu_init(void);
	ad_pmu_init();

	/* from system_init() */
	/* Use appropriate XTAL for each device */
	cm_sys_clk_init(sysclk_XTAL32M);
	cm_apb_set_clock_divider(apb_div1);
	cm_ahb_set_clock_divider(ahb_div1);
	// cm_lp_clk_init();

	/* from pm_system_init */
	/* Enables the COM power domain (for example, it's used to enable GPIO) */
	hw_sys_pd_com_enable();

	hw_sys_setup_retmem();
	hw_sys_set_cache_retained();

	hw_watchdog_freeze(); // Stop watchdog
	hw_watchdog_set_pos_val(dg_configWDOG_IDLE_RESET_VALUE);

	//qspi_operations_init();

	rtc_init();
}
