/**
 * @file
 * @brief
 *
 * @author  Erick Cafferata
 * @date    03.07.2020
 */

#include <assert.h>
#include <string.h>

#include <drivers/udc/stm32/usb_stm32.h>
#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/usb_defines.h>
#include <embox/unit.h>
#include <kernel/irq.h>
#include <lib/libds/array.h>
#include <util/log.h>
#include <util/math.h>

#include "stm32f4xx_hal.h"

EMBOX_UNIT_INIT(stm32f4_udc_init);

#define USB_MAX_EP0_SIZE 64U

#define EP0_BUFFER_SIZE       1024
#define STM32F4_UDC_EPS_COUNT 8 /* IN and OUT */

#define STM32F4_UDC_IN_EP_MASK  ((1 << 5) | (1 << 6) | (1 << 7))
#define STM32F4_UDC_OUT_EP_MASK ((1 << 1) | (1 << 2) | (1 << 3))

struct ep_status {
	//	uint32_t status;
	uint32_t total_length;
	uint32_t rem_length;
	uint32_t maxpacket;
	uint8_t is_used;
	//	uint16_t bInterval;
};

struct stm32f4_udc {
	struct usb_udc udc;
	struct usb_gadget_ep *eps[STM32F4_UDC_EPS_COUNT];
	struct usb_gadget_request *requests[STM32F4_UDC_EPS_COUNT];
	struct ep_status ep_info[STM32F4_UDC_EPS_COUNT];
	uint32_t ep0_data_len;
};

extern PCD_HandleTypeDef hpcd;

static uint8_t ep0_buffer[EP0_BUFFER_SIZE];

static int stm32f4_udc_start(struct usb_udc *udc) {
	usb_stm32f4_init();

	return 0;
}

static int stm32f4_udc_ep_queue(struct usb_gadget_ep *ep,
    struct usb_gadget_request *req) {
	struct stm32f4_udc *u = (struct stm32f4_udc *)ep->udc;

	assert(ep && req);

	log_debug("> ep=%d, len=%d", ep->nr, req->len);

	u->requests[ep->nr] = req;

	if (ep->nr == 0 || ep->dir == USB_DIR_IN) {
		/* It would be better to use queue here, put req in queue,
		 * then get next req from queue after current finished. */
		while (u->ep_info[0x4 | ep->nr].is_used) {}
		/* shouldnt this be able to handle requests over EP max length? */
		//pdev->ep0_state = USBD_EP0_DATA_IN;
		u->ep_info[0x4 | ep->nr].is_used = 1;
		u->ep_info[0x4 | ep->nr].total_length = req->len;
		u->ep_info[0x4 | ep->nr].rem_length = req->len;

		HAL_PCD_EP_Transmit(&hpcd, ep->nr, req->buf, req->len);
	}

	return 0;
}

static void stm32f4_udc_ep_enable(struct usb_gadget_ep *ep) {
	assert(ep);
}

static struct stm32f4_udc stm32f4_udc = {
    .udc =
        {
            .name = "stm32f4 udc",
            .udc_start = stm32f4_udc_start,
            .ep_queue = stm32f4_udc_ep_queue,
            .ep_enable = stm32f4_udc_ep_enable,

            .in_ep_mask = STM32F4_UDC_IN_EP_MASK,
            .out_ep_mask = STM32F4_UDC_OUT_EP_MASK,
        },
};

static int stm32f4_udc_init(void) {
	usb_gadget_register_udc(&stm32f4_udc.udc);

	return 0;
}

/* hardware-specific handlers */

static void stm32f4_ll_set_address(struct usb_control_header *req) {
	uint8_t dev_addr;

	if ((req->w_index == 0U) && (req->w_length == 0U) && (req->w_value < 128U)) {
		dev_addr = (uint8_t)(req->w_value) & 0x7FU;
		HAL_PCD_SetAddress(&hpcd, dev_addr);
		log_debug("addr=0x%x", dev_addr);

		//TODO: create function(CtlSendStatus)
		HAL_PCD_EP_Transmit(&hpcd, 0x00U, NULL, 0U);
	}
	else {
		HAL_PCD_EP_SetStall(&hpcd, 0x80U); //equivalent to CtlError()
		HAL_PCD_EP_SetStall(&hpcd, 0U);
	}
}

#if 0
static void stm32f4_ll_set_configuration(struct usb_control_header *req) {
    int config = req->w_value & 0xff;

    log_debug("conf=0x%x", config);
#if 1
    HAL_PCD_EP_SetStall(&hpcd, 0x80U); //equivalent to CtlError()
    HAL_PCD_EP_SetStall(&hpcd, 0U);
#else
    /*TODO: add check for config not found */
    usb_gadget_set_config(stm32f4_udc.udc.composite, config);

    HAL_PCD_EP_Transmit(&hpcd, 0x00U, NULL, 0U);
#endif
}
#endif

static void stm32f4_ll_get_status(struct usb_control_header *req) {
	uint16_t status = 0;

	switch (req->bm_request_type & USB_REQ_RECIP_MASK) {
	case USB_REQ_RECIP_DEVICE:
		/*TODO: add check for w_length != 2bytes */

		HAL_PCD_EP_Transmit(&hpcd, 0x00U, (uint8_t *)&status, 2U);

		break;
	/* TODO: add case for EPs recipient */
	/* case USB_REQ_RECIP_ENDP: */
	default:
		log_error("Unsupported RECIP 0x%x",
		    req->bm_request_type & USB_REQ_RECIP_MASK);
		HAL_PCD_EP_SetStall(&hpcd, req->bm_request_type & 0x80U);
		break;
	}
}

/**
* @brief  stm32f4_ll_handle_standard_request
*         Handle USB device requests
* @param  req: USB request
* @retval status
*/
static void stm32f4_ll_handle_standard_request(struct usb_control_header *req) {
	int ret;

	switch (req->b_request) {
	case USB_REQ_SET_ADDRESS:
		stm32f4_ll_set_address(req);
		break;
	case USB_REQ_SET_CONFIG:
		ret = usb_gadget_setup(stm32f4_udc.udc.composite,
		    (const struct usb_control_header *)req, NULL);
		if (ret != 0) {
			log_error("SET_CONFIGURATION failed, req=0x%x", req->b_request);
			HAL_PCD_EP_SetStall(&hpcd, req->bm_request_type & 0x80U);
		}
		break;
	case USB_REQ_GET_CONFIG:
		//stm32f4_ll_get_configuration(req);
		log_debug("GET_CONFIGURATION");
		break;
	case USB_REQ_GET_STATUS:
		stm32f4_ll_get_status(req);
		break;
	case USB_REQ_SET_FEATURE:
		//stm32f4_ll_set_feature(req);
		log_debug("SET_FEATURE");
		break;
	case USB_REQ_CLEAR_FEATURE:
		//stm32f4_ll_clear_feature(req);
		log_debug("CLEAR_FEATURE");
		break;
	default:
		log_debug("usb:if\n");
		ret = usb_gadget_setup(stm32f4_udc.udc.composite,
		    (const struct usb_control_header *)req, NULL);
		if (ret != 0) {
			log_debug("if_error\n");
			log_error("Not implemented req 0x%x", req->b_request);
			HAL_PCD_EP_SetStall(&hpcd, req->bm_request_type & 0x80U);
		}
		break;
	}
}

/**
 * @brief  Setup stage callback.
 * @param  hpcd: PCD handle
 * @retval None
 */
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd) {
	//TODO: nice to have a gadget state struct to contain all the info
	int ret;

	/* TODO: test changing memcpy with simple re-cast.
	 * struct usb_control_header *ctrl = (struct usb_control_header *)hpcd->Setup; */
	struct usb_control_header ctrl;
	memcpy(&ctrl, hpcd->Setup, sizeof(struct usb_control_header));

	log_debug("usb: setupstage\n");
	log_debug("bmRequestType:0x%x\nbRequest:0x%x\nwValue:0x%x\nwIndex:0x%"
	          "x\nwLength:0x%x\n",
	    ctrl.bm_request_type, ctrl.b_request, ctrl.w_value, ctrl.w_index,
	    ctrl.w_length);

	stm32f4_udc.ep0_data_len = ctrl.w_length;

	switch (ctrl.bm_request_type & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		stm32f4_ll_handle_standard_request(&ctrl);
		break;
	default:
		ret = usb_gadget_setup(stm32f4_udc.udc.composite,
		    (const struct usb_control_header *)&ctrl, ep0_buffer);
		if (ret != 0) {
			log_error("Setup failed, request=0x%x", ctrl.b_request);
			HAL_PCD_EP_SetStall(hpcd, ctrl.bm_request_type & 0x80U);
		}
		break;
	}
}

/**
  * @brief  Data In stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	log_debug("usb: dataINstage of 0x%x\n", epnum);

	struct ep_status *pep;

	if (epnum == 0U) {
		pep = &stm32f4_udc.ep_info[0x4 | epnum];

		if (pep->rem_length > pep->maxpacket) {
			pep->rem_length -= pep->maxpacket;

			HAL_PCD_EP_Transmit(hpcd, 0U, hpcd->IN_ep[epnum].xfer_buff,
			    pep->rem_length);
			/* Prepare endpoint for premature end of transfer */
			/* Do not prepare to receive here. Current UDC assumption:
			 * INs and OUTs have to non-intermixed. */
			//HAL_PCD_EP_Receive(hpcd, 0U, NULL, 0U);
		}
		else {
			/* last packet is MPS multiple, so send ZLP packet */
			if ((pep->maxpacket == pep->rem_length)
			    && (pep->total_length >= pep->maxpacket)
			    && (pep->total_length < stm32f4_udc.ep0_data_len)) {
				HAL_PCD_EP_Transmit(hpcd, 0U, NULL, 0U);
				stm32f4_udc.ep0_data_len = 0U;
				/* Prepare endpoint for premature end of transfer */
				/* Do not prepar, req->lene to receive here. Current UDC assumption:
				 * INs and OUTs have to non-intermixed. */
				//HAL_PCD_EP_Receive(hpcd, 0U, NULL, 0U);
			}
			else {
				log_debug("usb: din:?\n");

				stm32f4_udc.ep_info[0x4 | epnum].is_used = 0;
				HAL_PCD_EP_SetStall(hpcd, 0x80U);
				/* Start the transfer */
				HAL_PCD_EP_Receive(hpcd, 0U, NULL, 0U);
			}
		}
#if 1
	}
	else {
		log_debug("usb: din: EP%d\n", epnum);
	}
#else /*  uncomment for EP!=0 later */
	}
	else if ((pdev->pClass->DataIn != NULL)
	         && (pdev->dev_state == USBD_STATE_CONFIGURED)) {
		(USBD_StatusTypeDef) pdev->pClass->DataIn(pdev, epnum);
	}
	else {
		/* should never be in this condition */
		/* maybe add a log instead of return */
	}
#endif
}

/**
 * @brief  Data Out stage callback.
 * @param  hpcd: PCD handle
 * @param  epnum: Endpoint Number
 * @retval None
 */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	log_debug("usb: dataOUTstage of 0x%x\n", epnum);
#if 0
	struct ep_status *pep;

	if (epnum == 0U) {
		pep = &stm32f4_udc.ep_info[0x00 | epnum];
		log_debug("usb: dout: epnum=0x%x;rem=0x%x;maxpkt=0x%x\n", epnum, pep->rem_length, pep->maxpacket);

//		if (pdev->ep0_state == USBD_EP0_DATA_OUT) {
		if (pep->rem_length > pep->maxpacket) {
			uint32_t len = pep->rem_length < pep->maxpacket ? pep->rem_length : pep->maxpacket;

			pep->rem_length -= pep->maxpacket;

			HAL_PCD_EP_Receive(hpcd, 0U, hpcd->OUT_ep[epnum].xfer_buff, len);
		} else {
			log_debug("usb: dout:?\n");
			//if ((pdev->pClass->EP0_RxReady != NULL) &&
			//		(pdev->dev_state == USBD_STATE_CONFIGURED)) {
			//	pdev->pClass->EP0_RxReady(pdev);
			//}
			//(void)USBD_CtlSendStatus(pdev);
			HAL_PCD_EP_Transmit(hpcd, 0x00U, NULL, 0U);
		}
//		}
	} else {
		log_debug("usb: dout: EP%d\n", epnum);
	}
#endif
}

/**
  * @brief  Reset callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd) {
	int i;

	/* Reset Device */
	log_debug("usb: reset\n");

	/*TODO: DeInit some stuff */

	/* Open EP0 OUT */
	HAL_PCD_EP_Open(hpcd, 0x00U, USB_MAX_EP0_SIZE, EP_TYPE_CTRL); /* EP0_MAX_SIZE */

	stm32f4_udc.ep_info[0x00U | 0x00U].maxpacket = USB_MAX_EP0_SIZE;

	/* Open EP0 IN */
	HAL_PCD_EP_Open(hpcd, 0x80U, USB_MAX_EP0_SIZE, EP_TYPE_CTRL); /* EP0_MAX_SIZE */

	stm32f4_udc.ep_info[0x04U | 0x00U].maxpacket = USB_MAX_EP0_SIZE;

	for (i = 0; i < ARRAY_SIZE(stm32f4_udc.ep_info); i++) {
		stm32f4_udc.ep_info[i].is_used = 0;
	}
}
