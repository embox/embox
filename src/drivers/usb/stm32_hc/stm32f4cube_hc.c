/**
 * @file
 * @brief
 *
 * @author  Filipp Chubukov
 * @date    05.03.2021
 */

#include <util/log.h>

#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_hcd.h>

void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd) {
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOs clocks */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

#if defined(STM32F429xx)
/* Ethernet pins configuration ************************************************/
	/*
	 USB_ID ----------------------> PA10
	 USB_VBUS ---------------------> PG6
	 USB_DM -----------------------> PA11
	 USB_DP -----------------------> PA12
	 */
#else
#error "don't support platform"
#endif

#if defined(STM32F429xx)
	GPIO_InitStructure.Pin = GPIO_PIN_11 | GPIO_PIN_12;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Alternate = GPIO_AF10_OTG_FS;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_10;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = GPIO_PIN_6;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
#endif
	/* Enable USB clock  */
	__HAL_RCC_USB_OTG_FS_CLK_ENABLE();
}
