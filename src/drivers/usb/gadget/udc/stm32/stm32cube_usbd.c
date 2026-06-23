/**
 * @file
 * @brief
 *
 * @author Erick Cafferata
 * @date 03.07.2020
 */

#include <util/log.h>

#include <assert.h>

#include <kernel/irq.h>

#include <kernel/lthread/lthread.h>

#include <framework/mod/options.h>


#include <kernel/printk.h>

#include <bsp/stm32cube_hal.h>


#define USB_IRQ OPTION_GET(NUMBER, irq)

static int stm32cube_usbd_reset_hnd(struct lthread *self);
static LTHREAD_DEF(stm32cube_usbd_reset_lt, stm32cube_usbd_reset_hnd, 200);

static int stm32cube_usbd_reset_hnd(struct lthread *self) {
	//deInit();
	//hw_usb_reset_event();
	return 0;
}

/*** PCD Driver required functions ***/

/**
 * @brief  Initializes the PCD MSP.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd) {
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
		__HAL_RCC_SYSCFG_CLK_DISABLE();
	}
}

/**
	* @brief  Connect callback.
	* @param  hpcd: PCD handle
	* @retval None
	*/
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd) {
	if (log_level_self() >= LOG_DEBUG) {
		printk("usb: vbus\n");
	}
}

/*** END OF PCD Driver required functions ***/

PCD_HandleTypeDef hpcd;
extern void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);
static irq_return_t stm32cube_usbd_usb_irq_handler(unsigned int irq_nr, void *data) {
	printk("usb: irq entry\n");
	HAL_PCD_IRQHandler(&hpcd);
	return IRQ_HANDLED;
}

int stm32cube_usbd_init(void) {
	int ret = 0;

	/*Set LL Driver parameters */
	/* FIXME: should be dependent on gadget */
	hpcd.Instance = USB_OTG_HS;
	hpcd.Init.dev_endpoints = 9;
	hpcd.Init.use_dedicated_ep1 = 0;
	hpcd.Init.dma_enable = 0;
	hpcd.Init.low_power_enable = 0;
	hpcd.Init.lpm_enable = 0;
	hpcd.Init.phy_itface = PCD_PHY_ULPI;
	hpcd.Init.Sof_enable = 0;
	hpcd.Init.speed = PCD_SPEED_HIGH;
	hpcd.Init.vbus_sensing_enable = 1;
	/* Link The driver to the stack */
//	hpcd.pData = pdev;
//	pdev->pData = &hpcd;
	/*Initialize LL Driver */
	HAL_PCD_Init(&hpcd);

	HAL_PCDEx_SetRxFiFo(&hpcd, 0x200);
	HAL_PCDEx_SetTxFiFo(&hpcd, 0, 0x80);
	HAL_PCDEx_SetTxFiFo(&hpcd, 1, 0x100);
	HAL_PCDEx_SetTxFiFo(&hpcd, 2, 0x40);

	ret = irq_attach(USB_IRQ, stm32cube_usbd_usb_irq_handler, 0, NULL, "usbd");
	if (ret != 0) {
		log_error("USB irq attach failed");
		return ret;
	}

	HAL_PCD_Start(&hpcd);

	return 0;
}

STATIC_IRQ_ATTACH(USB_IRQ, stm32cube_usbd_usb_irq_handler, NULL);
