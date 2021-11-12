/**
 * @file
 *
 * @date Oct 9, 2021
 * @author Anton Bondarev
 */

#include <drivers/usb/hc/stm32_hc_conf.h>

#include <stm32h7xx.h>
#include <stm32h7xx_hal.h>
#include <stm32h7xx_hal_hcd.h>

void HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd) {
	GPIO_InitTypeDef GPIO_InitStruct;

	if (hhcd->Instance == USB2_OTG_FS) {
		/* Configure USB FS GPIOs */
		__HAL_RCC_GPIOA_CLK_ENABLE();

		/* Configure DM DP Pins */
		GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF10_OTG2_FS;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* Configure ID pin */
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Alternate = GPIO_AF10_OTG2_FS;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* Configure VBUS Pin */
		GPIO_InitStruct.Pin = GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* Configure POWER_SWITCH pin */
		GPIO_InitStruct.Pin = GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	    /* Set USBFS Interrupt priority */
	    HAL_NVIC_SetPriority(OTG_FS_IRQn, 6, 0);

	    /* Enable USBFS Interrupt */
	    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

		/* Enable USB FS Clocks */
		__HAL_RCC_USB2_OTG_FS_CLK_ENABLE();

	} else if (hhcd->Instance == USB1_OTG_HS) {
		/* Configure USB FS GPIOs */
		__GPIOA_CLK_ENABLE();
		__GPIOB_CLK_ENABLE();
		__GPIOC_CLK_ENABLE();
		__GPIOH_CLK_ENABLE();
		__GPIOI_CLK_ENABLE();

		/* CLK */
		GPIO_InitStruct.Pin = GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_HS;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* D0 */
		GPIO_InitStruct.Pin = GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_HS;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* D1 D2 D3 D4 D5 D6 D7 */
		GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5 | GPIO_PIN_10
				| GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_HS;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* STP */
		GPIO_InitStruct.Pin = GPIO_PIN_0;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_HS;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		/* NXT */
		GPIO_InitStruct.Pin = GPIO_PIN_4;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_HS;
		HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

		/* DIR */
		GPIO_InitStruct.Pin = GPIO_PIN_11;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_HS;
		HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);
		__HAL_RCC_USB1_OTG_HS_ULPI_CLK_ENABLE();

		/* Enable USB HS Clocks */
		__HAL_RCC_USB1_OTG_HS_CLK_ENABLE();

	}
}
