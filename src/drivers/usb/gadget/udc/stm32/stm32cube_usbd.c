/**
 * @file
 * @brief
 *
 * @author Erick Cafferata
 * @date 03.07.2020
 */

#include <util/log.h>

#include <stdint.h>
#include <assert.h>

#include <kernel/irq.h>

#include <kernel/lthread/lthread.h>

#include <framework/mod/options.h>

#include <util/macro.h>
#include <config/board_config.h>

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


extern void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);
static irq_return_t stm32cube_usbd_usb_irq_handler(unsigned int irq_nr, void *data) {
	PCD_HandleTypeDef *hpcd = data;
	if (log_level_self() >= LOG_DEBUG) {
		printk("usb: irq entry\n");
	}
	HAL_PCD_IRQHandler(hpcd);
	return IRQ_HANDLED;
}

PCD_HandleTypeDef hpcd;
int stm32cube_usbd_init(void) {
	int ret = 0;

	/*Set LL Driver parameters */
	/* FIXME: should be dependent on gadget */
	hpcd.Instance = (void *)(uintptr_t)CONF_USB_OTG_REGION_BASE;
	hpcd.Init.dev_endpoints = 9;
	hpcd.Init.use_dedicated_ep1 = 0;
	hpcd.Init.dma_enable = 0;
	hpcd.Init.low_power_enable = 0;
	hpcd.Init.lpm_enable = 0;
	hpcd.Init.phy_itface = CONF_USB_OTG_MISC_PHY_ITFACE;
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

	ret = irq_attach(USB_IRQ, stm32cube_usbd_usb_irq_handler, 0, &hpcd, "usbd");
	if (ret != 0) {
		log_error("USB irq attach failed");
		return ret;
	}

	HAL_PCD_Start(&hpcd);

	return 0;
}

STATIC_IRQ_ATTACH(USB_IRQ, stm32cube_usbd_usb_irq_handler, &hpcd);
