/**
 * @file
 *
 * @date 31.03.2017
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include <bsp/stm32cube_hal.h>

/* FROM board_config.h*/
#define CLK_NAME_GPIO "CLK_GPIO"
#define CLK_NAME_UART "CLK_UART"
#define CLK_NAME_SPI  "CLK_SPI"
#define CLK_NAME_I2C  "CLK_I2C"
#define CLK_NAME_TMR  "CLK_TMR"
#define CLK_NAME_CAN  "CLK_CAN"
#define CLK_NAME_ETH  "CLK_ETH"
#define CLK_NAME_DMA  "CLK_DMA"

int stm32cube_rcc_tmr_en(int num) {
	switch (num) {
#if defined(__HAL_RCC_TIM1_CLK_ENABLE)
	case 1:
		__HAL_RCC_TIM1_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM1_CLK_ENABLE) */
#if defined(__HAL_RCC_TIM2_CLK_ENABLE)
	case 2:
		__HAL_RCC_TIM2_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM2_CLK_ENABLE) */
#if defined(__HAL_RCC_TIM3_CLK_ENABLE)
	case 3:
		__HAL_RCC_TIM3_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM3_CLK_ENABLE) */
#if defined(__HAL_RCC_TIM4_CLK_ENABLE)
	case 4:
		__HAL_RCC_TIM4_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM4_CLK_ENABLE) */
#if defined(__HAL_RCC_TIM5_CLK_ENABLE)
	case 5:
		__HAL_RCC_TIM5_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM5_CLK_ENABLE) */
#if defined(__HAL_RCC_TIM6_CLK_ENABLE)
	case 6:
		__HAL_RCC_TIM6_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM6_CLK_ENABLE) */
#if defined(__HAL_RCC_TIM7_CLK_ENABLE)
	case 7:
		__HAL_RCC_TIM7_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM7_CLK_ENABLE) */
#if defined(__HAL_RCC_TIM8_CLK_ENABLE)
	case 8:
		__HAL_RCC_TIM8_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM8_CLK_ENABLE) */
#if defined(__HAL_RCC_TIM9_CLK_ENABLE)
	case 9:
		__HAL_RCC_TIM9_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM9_CLK_ENABLE) */
#if defined(__HAL_RCC_TIM10_CLK_ENABLE)
	case 10:
		__HAL_RCC_TIM10_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM10_CLK_ENABLE) */
#if defined(__HAL_RCC_TIM11_CLK_ENABLE)
	case 11:
		__HAL_RCC_TIM11_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM11_CLK_ENABLE) */
#if defined(__HAL_RCC_TIM12_CLK_ENABLE)
	case 12:
		__HAL_RCC_TIM12_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM12_CLK_ENABLE) */
#if defined(__HAL_RCC_TIM13_CLK_ENABLE)
	case 13:
		__HAL_RCC_TIM13_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM13_CLK_ENABLE) */
#if defined(__HAL_RCC_TIM14_CLK_ENABLE)
	case 14:
		__HAL_RCC_TIM14_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_TIM14_CLK_ENABLE) */
	default:
		return -ENOTSUP;
	}

	return -ENOTSUP;
}

int stm32cube_rcc_can_en(int num) {
	switch (num) {
#if defined(__HAL_RCC_CAN1_CLK_ENABLE)
	case 1:
		__HAL_RCC_CAN1_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_CAN1_CLK_ENABLE) */
#if defined(__HAL_RCC_CAN2_CLK_ENABLE)
	case 2:
		__HAL_RCC_CAN2_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_CAN2_CLK_ENABLE) */
	default:
		return -ENOTSUP;
	}

	return -ENOTSUP;
}

int stm32cube_rcc_gpio_en(int num) {
	switch (num) {
#if defined(__HAL_RCC_GPIOA_CLK_ENABLE)
	case 0:
		__HAL_RCC_GPIOA_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_GPIOA_CLK_ENABLE) */
#if defined(__HAL_RCC_GPIOB_CLK_ENABLE)
	case 1:
		__HAL_RCC_GPIOB_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_GPIOB_CLK_ENABLE) */
#if defined(__HAL_RCC_GPIOC_CLK_ENABLE)
	case 2:
		__HAL_RCC_GPIOC_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_GPIOC_CLK_ENABLE) */
#if defined(__HAL_RCC_GPIOD_CLK_ENABLE)
	case 3:
		__HAL_RCC_GPIOD_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_GPIOD_CLK_ENABLE) */
#if defined(__HAL_RCC_GPIOE_CLK_ENABLE)
	case 4:
		__HAL_RCC_GPIOE_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_GPIOE_CLK_ENABLE) */
#if defined(__HAL_RCC_GPIOF_CLK_ENABLE)
	case 5:
		__HAL_RCC_GPIOF_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_GPIOF_CLK_ENABLE) */
#if defined(__HAL_RCC_GPIOG_CLK_ENABLE)
	case 6:
		__HAL_RCC_GPIOG_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_GPIOG_CLK_ENABLE) */
#if defined(__HAL_RCC_GPIOH_CLK_ENABLE)
	case 7:
		__HAL_RCC_GPIOH_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_GPIOH_CLK_ENABLE) */
#if defined(__HAL_RCC_GPIOI_CLK_ENABLE)
	case 8:
		__HAL_RCC_GPIOI_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_GPIOI_CLK_ENABLE) */
#if defined(__HAL_RCC_GPIOJ_CLK_ENABLE)
	case 9:
		__HAL_RCC_GPIOJ_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_GPIOJ_CLK_ENABLE) */
#if defined(__HAL_RCC_GPIOK_CLK_ENABLE)
	case 10:
		__HAL_RCC_GPIOK_CLK_ENABLE();
		return 0;
#endif /* defined(__HAL_RCC_GPIOK_CLK_ENABLE) */
	default:
		return -ENOTSUP;
	}
	return -ENOTSUP;
}

int stm32cube_rcc_uart_en(int num) {
  switch (num) {
#if defined(__HAL_RCC_USART1_CLK_ENABLE)
	case 1:
		__HAL_RCC_USART1_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_USART1_CLK_ENABLE) */
#if defined(__HAL_RCC_USART2_CLK_ENABLE)
	case 2:
		__HAL_RCC_USART2_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_USART2_CLK_ENABLE) */
#if defined(__HAL_RCC_USART3_CLK_ENABLE)
	case 3:
		__HAL_RCC_USART3_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_USART3_CLK_ENABLE) */
#if defined(__HAL_RCC_UART4_CLK_ENABLE)
	case 4:
		__HAL_RCC_UART4_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_UART4_CLK_ENABLE) */
#if defined(__HAL_RCC_UART5_CLK_ENABLE)
	case 5:
		__HAL_RCC_UART5_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_UART5_CLK_ENABLE) */
#if defined(__HAL_RCC_USART6_CLK_ENABLE)
	case 6:
		__HAL_RCC_USART6_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_USART6_CLK_ENABLE) */
#if defined(__HAL_RCC_UART7_CLK_ENABLE)
	case 7:
		__HAL_RCC_UART7_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_UART7_CLK_ENABLE) */
#if defined(__HAL_RCC_UART8_CLK_ENABLE)
	case 8:
		__HAL_RCC_UART8_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_UART8_CLK_ENABLE) */
	default:
		return -ENOTSUP;
	}

	return -ENOTSUP;
}

int stm32cube_rcc_spi_en(int num) {
	switch (num) {
#if defined(__HAL_RCC_SPI1_CLK_ENABLE)
	case 1:
		__HAL_RCC_SPI1_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_SPI1_CLK_ENABLE) */
#if defined(__HAL_RCC_SPI2_CLK_ENABLE)
	case 2:
		__HAL_RCC_SPI2_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_SPI2_CLK_ENABLE) */
#if defined(__HAL_RCC_SPI3_CLK_ENABLE)
	case 3:
		__HAL_RCC_SPI3_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_SPI3_CLK_ENABLE) */
#if defined(__HAL_RCC_SPI4_CLK_ENABLE)
	case 4:
		__HAL_RCC_SPI4_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_SPI4_CLK_ENABLE) */
#if defined(__HAL_RCC_SPI5_CLK_ENABLE)
	case 5:
		__HAL_RCC_SPI5_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_SPI5_CLK_ENABLE) */
#if defined(__HAL_RCC_SPI6_CLK_ENABLE)
	case 6:
		__HAL_RCC_SPI6_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_SPI6_CLK_ENABLE) */
#if defined(__HAL_RCC_SPI7_CLK_ENABLE)
	case 7:
		__HAL_RCC_SPI7_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_SPI7_CLK_ENABLE) */
	default:
		return -ENOTSUP;
	}

	return -ENOTSUP;
}

int stm32cube_rcc_i2c_en(int num) {
	switch (num) {
#if defined(__HAL_RCC_I2C1_CLK_ENABLE)
	case 1:
		__HAL_RCC_I2C1_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_I2C1_CLK_ENABLE) */
#if defined(__HAL_RCC_I2C2_CLK_ENABLE)
	case 2:
		__HAL_RCC_I2C2_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_I2C2_CLK_ENABLE) */
#if defined(__HAL_RCC_I2C3_CLK_ENABLE)
	case 3:
		__HAL_RCC_I2C3_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_I2C3_CLK_ENABLE) */
#if defined(__HAL_RCC_I2C4_CLK_ENABLE)
	case 4:
		__HAL_RCC_I2C4_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_I2C4_CLK_ENABLE) */
#if defined(__HAL_RCC_I2C5_CLK_ENABLE)
	case 5:
		__HAL_RCC_I2C5_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_I2C5_CLK_ENABLE) */
	default:
		return -ENOTSUP;
	}

	return -ENOTSUP;
}

int stm32cube_rcc_dma_en(int num) {
	switch (num) {
#if defined(__HAL_RCC_DMA1_CLK_ENABLE)
	case 1:
		__HAL_RCC_DMA1_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_DMA1_CLK_ENABLE) */
#if defined(__HAL_RCC_DMA2_CLK_ENABLE)
	case 2:
		__HAL_RCC_DMA2_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_DMA2_CLK_ENABLE) */
	default:
		return -ENOTSUP;
	}

	return -ENOTSUP;
}

int stm32cube_rcc_eth_en(int num) {
	switch (num) {
#if defined(__HAL_RCC_ETH_CLK_ENABLE)
	case 0:
		__HAL_RCC_ETH_CLK_ENABLE();
		return 0;
#endif /* defined (__HAL_RCC_ETH_CLK_ENABLE) */
	default:
		return -ENOTSUP;
	}

	return -ENOTSUP;
}

int clk_enable(char *clk_name) {
	int num;

	if (0 == strncmp(clk_name, CLK_NAME_GPIO, sizeof(CLK_NAME_GPIO) - 1)) {
		num = clk_name[sizeof(CLK_NAME_GPIO) - 1] - 'A';
		stm32cube_rcc_gpio_en(num);
		return 0;
	}
	if (0 == strncmp(clk_name, CLK_NAME_UART, sizeof(CLK_NAME_UART) - 1)) {
		num = clk_name[sizeof(CLK_NAME_UART) - 1] - '0';
		stm32cube_rcc_uart_en(num);
		return 0;
	}
  if (0 == strncmp(clk_name, CLK_NAME_SPI, sizeof(CLK_NAME_SPI) - 1)) {
		num = clk_name[sizeof(CLK_NAME_SPI) - 1] - '0';
		stm32cube_rcc_spi_en(num);
		return 0;
	}
  if (0 == strncmp(clk_name, CLK_NAME_I2C, sizeof(CLK_NAME_I2C) - 1)) {
		num = clk_name[sizeof(CLK_NAME_I2C) - 1] - '0';
		stm32cube_rcc_i2c_en(num);
		return 0;
	}
	if (0 == strncmp(clk_name, CLK_NAME_TMR, sizeof(CLK_NAME_TMR) - 1)) {
		num = clk_name[sizeof(CLK_NAME_TMR) - 1] - '0';
		stm32cube_rcc_tmr_en(num);
		return 0;
	}
	if (0 == strncmp(clk_name, CLK_NAME_CAN, sizeof(CLK_NAME_CAN) - 1)) {
		num = clk_name[sizeof(CLK_NAME_CAN) - 1] - '0';
		stm32cube_rcc_can_en(num);
		return 0;
	}
	if (0 == strncmp(clk_name, CLK_NAME_ETH, sizeof(CLK_NAME_ETH) - 1)) {
		stm32cube_rcc_eth_en(0);
		return 0;
	}
  if (0 == strncmp(clk_name, CLK_NAME_DMA, sizeof(CLK_NAME_DMA) - 1)) {
		num = clk_name[sizeof(CLK_NAME_DMA) - 1] - '0';
		stm32cube_rcc_dma_en(num);
		return 0;
	}

	return -ENOSUPP;
}
