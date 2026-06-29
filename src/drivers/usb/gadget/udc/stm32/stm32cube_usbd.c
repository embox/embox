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

#include "stm32cube_udc_priv.h"

#include <bsp/stm32cube_hal.h>

static int stm32cube_usbd_reset_hnd(struct lthread *self);
static LTHREAD_DEF(stm32cube_usbd_reset_lt, stm32cube_usbd_reset_hnd, 200);

static int stm32cube_usbd_reset_hnd(struct lthread *self) {
	//deInit();
	//hw_usb_reset_event();
	return 0;
}


//PCD_HandleTypeDef hpcd;

int stm32cube_usbd_init(struct stm32cube_udc *stm32cube_udc) {
	PCD_HandleTypeDef *hpcd = &stm32cube_udc->hpcd;

	/*Set LL Driver parameters */
	/* FIXME: should be dependent on gadget */
	hpcd->Instance = (void *)(uintptr_t)CONF_USB_OTG_REGION_BASE;
	hpcd->Init.dev_endpoints = CONF_USB_OTG_MISC_EP_MAX;
	hpcd->Init.use_dedicated_ep1 = 0;
	hpcd->Init.dma_enable = 0;
	hpcd->Init.low_power_enable = 0;
	hpcd->Init.lpm_enable = 0;
	hpcd->Init.phy_itface = CONF_USB_OTG_MISC_PHY_ITFACE;
	hpcd->Init.Sof_enable = 0;
	hpcd->Init.speed = CONF_USB_OTG_MISC_SPEED;
	hpcd->Init.vbus_sensing_enable = 1;
	/* Link The driver to the stack */
	hpcd->pData = stm32cube_udc;
//	pdev->pData = &hpcd;
	/*Initialize LL Driver */
	HAL_PCD_Init(hpcd);

#if CONF_USB_OTG_MISC_SPEED == PCD_SPEED_HIGH
	HAL_PCDEx_SetRxFiFo(hpcd, 0x200);
	HAL_PCDEx_SetTxFiFo(hpcd, 0, 0x80);
	HAL_PCDEx_SetTxFiFo(hpcd, 1, 0x100);
	HAL_PCDEx_SetTxFiFo(hpcd, 2, 0x40);
#else /* PCD_SPEED_FULL */
	HAL_PCDEx_SetRxFiFo(hpcd, 0x80);
	HAL_PCDEx_SetTxFiFo(hpcd, 0, 0x40);
	HAL_PCDEx_SetTxFiFo(hpcd, 1, 0x80);
#endif /* CONF_USB_OTG_MISC_SPEED */

	HAL_PCD_Start(hpcd);

	return 0;
}

