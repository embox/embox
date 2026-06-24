/**
 * @file
 * @brief
 *
 * @author Erick Cafferata
 * @date 03.07.2020
 */

#include <util/log.h>

#include <assert.h>


#include <drivers/gpio.h>

#include <framework/mod/options.h>

#include <util/macro.h>
#include <config/board_config.h>

#include <kernel/printk.h>

#include <bsp/stm32cube_hal.h>

#define USB_PORT(name) \
			MACRO_CONCAT(MACRO_CONCAT(CONF_USB_OTG_PIN_, name), _PORT)
#define USB_PIN(name) \
			MACRO_CONCAT(MACRO_CONCAT(CONF_USB_OTG_PIN_, name), _NR)
#define USB_AF(name)  \
			MACRO_CONCAT(MACRO_CONCAT(CONF_USB_OTG_PIN_, name), _AF)

/*** PCD Driver required functions ***/

/**
 * @brief  Initializes the PCD MSP.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd) {
#if 0
	GPIO_InitTypeDef GPIO_InitStruct;

	if (hpcd->Instance == USB_OTG_HS) {

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
		GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5
				| GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
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

	}
#else

		/* CLK */
		gpio_setup_mode(USB_PORT(CLK), (1 << USB_PIN(CLK)),
	    	GPIO_MODE_ALT_SET(USB_AF(CLK)) | GPIO_MODE_OUT_PUSH_PULL);

		/* D0 */
		gpio_setup_mode(USB_PORT(D0), (1 << USB_PIN(D0)),
	    	GPIO_MODE_ALT_SET(USB_AF(D0)) | GPIO_MODE_OUT_PUSH_PULL);
		/* D1 */
		gpio_setup_mode(USB_PORT(D1), (1 << USB_PIN(D1)),
	    	GPIO_MODE_ALT_SET(USB_AF(D1)) | GPIO_MODE_OUT_PUSH_PULL);
		/* D2 */
		gpio_setup_mode(USB_PORT(D2), (1 << USB_PIN(D2)),
	    	GPIO_MODE_ALT_SET(USB_AF(D2)) | GPIO_MODE_OUT_PUSH_PULL);
		/* D3 */
		gpio_setup_mode(USB_PORT(D3), (1 << USB_PIN(D3)),
	    	GPIO_MODE_ALT_SET(USB_AF(D3)) | GPIO_MODE_OUT_PUSH_PULL);
		/* D4 */
		gpio_setup_mode(USB_PORT(D4), (1 << USB_PIN(D4)),
	    	GPIO_MODE_ALT_SET(USB_AF(D4)) | GPIO_MODE_OUT_PUSH_PULL);
		/* D5 */
		gpio_setup_mode(USB_PORT(D5), (1 << USB_PIN(D5)),
	    	GPIO_MODE_ALT_SET(USB_AF(D5)) | GPIO_MODE_OUT_PUSH_PULL);
		/* D6 */
		gpio_setup_mode(USB_PORT(D6), (1 << USB_PIN(D6)),
	    	GPIO_MODE_ALT_SET(USB_AF(D6)) | GPIO_MODE_OUT_PUSH_PULL);
		/* D7 */
		gpio_setup_mode(USB_PORT(D7), (1 << USB_PIN(D7)),
	    	GPIO_MODE_ALT_SET(USB_AF(D7)) | GPIO_MODE_OUT_PUSH_PULL);

		/* STP */
		gpio_setup_mode(USB_PORT(STP), (1 << USB_PIN(STP)),
	    	GPIO_MODE_ALT_SET(USB_AF(STP)) | GPIO_MODE_OUT_PUSH_PULL);

		/* NXT */
		gpio_setup_mode(USB_PORT(NXT), (1 << USB_PIN(NXT)),
	    	GPIO_MODE_ALT_SET(USB_AF(NXT)) | GPIO_MODE_OUT_PUSH_PULL);

		/* DIR */
		gpio_setup_mode(USB_PORT(DIR), (1 << USB_PIN(DIR)),
	    	GPIO_MODE_ALT_SET(USB_AF(DIR)) | GPIO_MODE_OUT_PUSH_PULL);

		/* Enable ULPI Clocks */
		CONF_USB_OTG_CLK_DEF_PHY();

		/* Enable USB HS Clocks */
		CONF_USB_OTG_CLK_DEF_USB();

#endif

}

/**
 * @brief  DeInitializes the PCD MSP.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd) {
	if (hpcd->Instance == USB_OTG_HS) {
		/* Disable USB HS Clocks */
		__HAL_RCC_USB_OTG_HS_CLK_DISABLE();
	}
}

/*** END OF PCD Driver required functions ***/
