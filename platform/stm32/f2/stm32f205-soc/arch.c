/* Definition of the Ethernet driver buffers size and count */   
//#define ETH_RX_BUF_SIZE               	ETH_MAX_PACKET_SIZE /* buffer size for receive           	*/
//#define ETH_TX_BUF_SIZE               	ETH_MAX_PACKET_SIZE /* buffer size for transmit          	*/
//#if defined(__EMBOX__)
//#include <framework/mod/options.h>
//#include <module/third_party/bsp/stmf2cube/cube.h>

#define ETH_RXBUFNB \
    OPTION_MODULE_GET(third_party__bsp__stmf2cube__cube, NUMBER, eth_rx_packet_count)
#define ETH_TXBUFNB \
    OPTION_MODULE_GET(third_party__bsp__stmf2cube__cube, NUMBER, eth_tx_packet_count)
#else

#define ETH_RXBUFNB                   	5U   	/* 5 Rx buffers of size ETH_RX_BUF_SIZE  */
#define ETH_TXBUFNB                   	5U   	/* 5 Tx buffers of size ETH_TX_BUF_SIZE  */
#endif /*__EMBOX__*/


#include <stdint.h>

#include <hal/arch.h>
#include <hal/ipl.h>


#include <system_stm32f2xx.h>
#include <stm32f2xx_hal.h>

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 120000000
  *            HCLK(Hz)                       = 120000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 240
  *            PLL_P                          = 2
  *            PLL_Q                          = 5
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 3
  * @param  None
  * @retval None
  */
void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 240;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 5;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);
}

void arch_init(void) {
	ipl_t ipl = ipl_save();

	SystemInit();
	HAL_Init();

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
