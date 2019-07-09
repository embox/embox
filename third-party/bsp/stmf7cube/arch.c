/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.10.2014
 */

#include <assert.h>
#include <stdint.h>

#include <hal/arch.h>
#include <hal/clock.h>
#include <hal/ipl.h>

#include <system_stm32f7xx.h>
#include <stm32f7xx_hal.h>

#if defined STM32F746xx
#include <stm32746g_discovery.h>
#elif defined STM32F769xx
#include <stm32f769i_discovery.h>
#else
#error Unsupported platform
#endif

#include <stm32f7xx_hal_cortex.h>

#include <framework/mod/options.h>
#include <module/embox/arch/system.h>

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
	printf(">>> SystemClock_Config failed\n");
  }

  /* activate the OverDrive to reach the 216 Mhz Frequency */
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
	printf(">>> SystemClock_Config failed\n");
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
	printf(">>> SystemClock_Config failed\n");

  }
}

extern void nvic_table_fill_stubs(void);

void arch_init(void) {
	ipl_t ipl = ipl_save();

	static_assert(OPTION_MODULE_GET(embox__arch__system, NUMBER, core_freq) == 216000000);

	SystemInit();
	HAL_Init();

	nvic_table_fill_stubs();

	SystemClock_Config();

	ipl_restore(ipl);
}

void arch_idle(void) {

}

void arch_shutdown(arch_shutdown_mode_t mode) {
	switch (mode) {
	case ARCH_SHUTDOWN_MODE_HALT:
	case ARCH_SHUTDOWN_MODE_REBOOT:
	case ARCH_SHUTDOWN_MODE_ABORT:
	default:
		HAL_NVIC_SystemReset();
		break;
	}

	/* NOTREACHED */
	while(1) {

	}
}

uint32_t HAL_GetTick(void) {
	return clock_sys_ticks();
}
