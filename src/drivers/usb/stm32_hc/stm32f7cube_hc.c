/**
 * @file
 *
 * @date Dec 13, 2021
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <drivers/usb/hc/stm32_hc_conf.h>

#include <stm32f7xx.h>
#include <stm32f7xx_hal.h>
#include <stm32f7xx_hal_hcd.h>

void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd) {
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Configure USB HS GPIOs */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();

	/* CLK */
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* D0 */
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* D1 D2 D3 D4 D5 D6 D7 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_10
			| GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* STP */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* NXT */
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	/* DIR */
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Alternate = GPIO_AF10_OTG_HS;
	HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
	__HAL_RCC_USB_OTG_HS_ULPI_CLK_ENABLE();

	/* Enable USB HS Clocks */
	__HAL_RCC_USB_OTG_HS_CLK_ENABLE();

	/* Set USBHS Interrupt priority */
	HAL_NVIC_SetPriority(OTG_HS_IRQn, 6, 0);

	/* Enable USBHS Interrupt */
	HAL_NVIC_EnableIRQ(OTG_HS_IRQn);

}
