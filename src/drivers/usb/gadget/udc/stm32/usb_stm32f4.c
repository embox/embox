/**
 * @file
 * @brief
 *
 * @author Erick Cafferata
 * @date 03.07.2020
 */

#include <assert.h>
#include <kernel/irq.h>
#include <kernel/lthread/lthread.h>
#include <util/log.h>
#include <framework/mod/options.h>

/* FIX: add dependency */
#include <kernel/printk.h>

#include "stm32f4xx_hal.h"

#include <drivers/udc/stm32/usb_stm32.h>

#define USB_IRQ OPTION_GET(NUMBER, irq)

static int usb_stm32f4_reset_hnd(struct lthread *self);
static LTHREAD_DEF(usb_stm32f4_reset_lt, usb_stm32f4_reset_hnd, 200);

static int usb_stm32f4_reset_hnd(struct lthread *self) {
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

	if (hpcd->Instance == USB_OTG_FS) {
		/* Configure USB FS GPIOs */
		__HAL_RCC_GPIOA_CLK_ENABLE();

		/* Configure DM DP Pins */
		GPIO_InitStruct.Pin = (GPIO_PIN_11 | GPIO_PIN_12);
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Configure VBUS Pin */
		GPIO_InitStruct.Pin = GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* Configure ID pin */
		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* Enable USB FS Clocks */
		__HAL_RCC_USB_OTG_FS_CLK_ENABLE();

		/* Set USBFS Interrupt priority */
//		HAL_NVIC_SetPriority(USB_IRQ, 6, 0);

		/* Enable USBFS Interrupt */
//		HAL_NVIC_EnableIRQ(USB_IRQ);
	}
}

/**
 * @brief  DeInitializes the PCD MSP.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd) {
	if (hpcd->Instance == USB_OTG_FS) {
		/* Disable USB FS Clocks */
		__HAL_RCC_USB_OTG_FS_CLK_DISABLE();
		__HAL_RCC_SYSCFG_CLK_DISABLE();
	}
}

/**
	* @brief  Connect callback.
	* @param  hpcd: PCD handle
	* @retval None
	*/
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd) {
	printk("usb: vbus\n");
}

/*** END OF PCD Driver required functions ***/

PCD_HandleTypeDef hpcd;
extern void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);
static irq_return_t usb_stm32f4_usb_irq_handler(unsigned int irq_nr, void *data) {
	printk("usb: irq entry\n");
	HAL_PCD_IRQHandler(&hpcd);
	return IRQ_HANDLED;
}

int usb_stm32f4_init(void) {
	int ret = 0;

//	NVIC_SetPriority (SysTick_IRQn, 0);

	/*Set LL Driver parameters */
	/* FIXME: should be dependent on gadget */
	hpcd.Instance = USB_OTG_FS;
	hpcd.Init.dev_endpoints = 4;
	hpcd.Init.use_dedicated_ep1 = 0;
	hpcd.Init.dma_enable = 0;
	hpcd.Init.low_power_enable = 0;
	hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;
	hpcd.Init.Sof_enable = 0;
	hpcd.Init.speed = PCD_SPEED_FULL;
	hpcd.Init.vbus_sensing_enable = 1;
	/* Link The driver to the stack */
//	hpcd.pData = pdev;
//	pdev->pData = &hpcd;
	/*Initialize LL Driver */
	HAL_PCD_Init(&hpcd);

	HAL_PCDEx_SetRxFiFo(&hpcd, 0x80);
	HAL_PCDEx_SetTxFiFo(&hpcd, 0, 0x40);
	HAL_PCDEx_SetTxFiFo(&hpcd, 1, 0x80);

	ret = irq_attach(USB_IRQ, usb_stm32f4_usb_irq_handler, 0, NULL,
		"usb stm32f4");
	if (ret != 0) {
		log_error("USB irq attach failed");
		return ret;
	}

	HAL_PCD_Start(&hpcd);

	return 0;
}

STATIC_IRQ_ATTACH(USB_IRQ, usb_stm32f4_usb_irq_handler, NULL);
