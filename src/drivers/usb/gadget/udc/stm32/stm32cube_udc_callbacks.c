/**
 * @file
 * @brief
 *
 * @author  Erick Cafferata
 * @date    03.07.2020
 */

#include <util/log.h>

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/usb_defines.h>

#include <kernel/irq.h>
#include <lib/libds/array.h>

#include <embox/unit.h>

#include <util/math.h>

#include <bsp/stm32cube_hal.h>

#include "stm32cube_udc_priv.h"


#define EP0_BUFFER_SIZE       1024

#define STM32CUBE_UDC_IN_EP_MASK  ((1 << 1) | (1 << 2) | (1 << 3))
#define STM32CUBE_UDC_OUT_EP_MASK ((1 << 1) | (1 << 2) | (1 << 3))

#define USB_URB_TRACE 0
#define USB_DUMP_BYTES 64

#if USB_URB_TRACE
static void dump_bytes(const char *tag, const void *data, int len) {
	const uint8_t *p = data;
	int n = len > USB_DUMP_BYTES ? USB_DUMP_BYTES : len;
	int i;

	if (!data || len <= 0) {
		log_debug("%s: <no data> len=%d", tag, len);
		return;
	}

	log_debug("%s: len=%d dump_first=%d", tag, len, n);

	for (i = 0; i < n; i += 16) {
		char line[128];
		int j, pos = 0;

		pos += snprintf(line + pos, sizeof(line) - pos, "%04x: ", i);

		for (j = 0; j < 16 && (i + j) < n; j++) {
			pos += snprintf(line + pos, sizeof(line) - pos,
			                "%02x ", p[i + j]);
		}

		log_debug("%s", line);
	}
}
#else
static inline void dump_bytes(const char *tag, const void *data, int len) { }
#endif

static uint8_t ep0_buffer[EP0_BUFFER_SIZE];


/* hardware-specific handlers */

static void stm32cube_ll_set_address(struct usb_control_header *req) {
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


static void stm32cube_ll_get_status(struct usb_control_header *req) {
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
* @brief  stm32cube_ll_handle_standard_request
*         Handle USB device requests
* @param  req: USB request
* @retval status
*/
static void stm32cube_ll_handle_standard_request(struct usb_control_header *req) {
	int ret;

	switch (req->b_request) {
	case USB_REQ_SET_ADDRESS:
		stm32cube_ll_set_address(req);
		break;
	case USB_REQ_SET_CONFIG:
		ret = usb_gadget_setup(stm32cube_udc.udc.composite,
        (const struct usb_control_header *)req, NULL);

    if (ret != 0) {
        log_error("SET_CONFIGURATION failed, req=0x%x", req->b_request);
        HAL_PCD_EP_SetStall(&hpcd, req->bm_request_type & 0x80U);
    } else {
        /* status stage for control-OUT request */
        HAL_PCD_EP_Transmit(&hpcd, 0x00U, NULL, 0U);
        // HAL_PCD_EP_Receive(&hpcd, 0x00U, NULL, 0U);
    }
    break;
	case USB_REQ_GET_CONFIG:
		//stm32cube_ll_get_configuration(req);
		log_debug("GET_CONFIGURATION");
		break;
	case USB_REQ_GET_STATUS:
		stm32cube_ll_get_status(req);
		break;
	case USB_REQ_SET_FEATURE:
		//stm32cube_ll_set_feature(req);
		log_debug("SET_FEATURE");
		break;
	case USB_REQ_CLEAR_FEATURE:
		//stm32cube_ll_clear_feature(req);
		log_debug("CLEAR_FEATURE");
		break;
	default:
		log_debug("usb:if\n");
		ret = usb_gadget_setup(stm32cube_udc.udc.composite,
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

	stm32cube_udc.ep0_data_len = ctrl.w_length;

	switch (ctrl.bm_request_type & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		stm32cube_ll_handle_standard_request(&ctrl);
		break;
	default:
		ret = usb_gadget_setup(stm32cube_udc.udc.composite,
		    (const struct usb_control_header *)&ctrl, ep0_buffer);
		if (ret != 0) {
			log_error("Setup failed, request=0x%x", ctrl.b_request);
			HAL_PCD_EP_SetStall(hpcd, ctrl.bm_request_type & 0x80U);
		}
		break;
	}

	log_debug("SETUP: bm=0x%02x bReq=0x%02x wVal=0x%04x wIdx=0x%04x wLen=%u",
          ctrl.bm_request_type, ctrl.b_request,
          ctrl.w_value, ctrl.w_index, ctrl.w_length);
	dump_bytes("SETUP raw", hpcd->Setup, 8);

}

/**
  * @brief  Data In stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint Number
  * @retval None
  */
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	if (epnum != 0U) {
		unsigned int idx = stm32cube_udc_ep_in_idx(epnum);
		struct usb_gadget_request *req;

		stm32cube_udc.ep_info[idx].is_used = 0;

		req = stm32cube_udc.requests[idx];
		stm32cube_udc.requests[idx] = NULL;

		if (req != NULL && req->complete != NULL) {
			req->actual_len = req->len;
			req->complete(stm32cube_udc.eps[idx], req);
		}
		return;
	}
	log_debug("usb: dataINstage of 0x%x\n", epnum);

	struct ep_status *pep;

	if (epnum == 0U) {
		pep = &stm32cube_udc.ep_info[0x4 | epnum];

		if (pep->rem_length > pep->maxpacket) {
			pep->rem_length -= pep->maxpacket;

			HAL_PCD_EP_Transmit(hpcd, 0U,
					hpcd->IN_ep[epnum].xfer_buff,
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
			    && (pep->total_length < stm32cube_udc.ep0_data_len)) {
				HAL_PCD_EP_Transmit(hpcd, 0U, NULL, 0U);
				stm32cube_udc.ep0_data_len = 0U;
				/* Prepare endpoint for premature end of transfer */
				/* Do not prepar, req->lene to receive here. Current UDC assumption:
				 * INs and OUTs have to non-intermixed. */
				//HAL_PCD_EP_Receive(hpcd, 0U, NULL, 0U);
			}
			else {
				stm32cube_udc.ep_info[0x4 | epnum].is_used = 0;
				HAL_PCD_EP_Receive(hpcd, 0U, NULL, 0U);
			}
		}
		log_debug("IN: ep=%u xfer_count=%u",
          epnum, (unsigned)hpcd->IN_ep[epnum].xfer_count);

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
	struct usb_gadget_request *req;
	unsigned int idx = stm32cube_udc_ep_out_idx(epnum);
	unsigned int actual_len;

	log_debug("DOUT: ep=%u xfer_count=%u", epnum, (unsigned)hpcd->OUT_ep[epnum].xfer_count);
	if (epnum < STM32CUBE_UDC_EPS_COUNT) {
		stm32cube_udc.ep_info[idx].is_used = 0;
	}
	log_debug("usb: dataOUTstage of 0x%x\n", epnum);
	log_debug("OUT: ep=%u xfer_count=%u",
	    epnum, (unsigned)hpcd->OUT_ep[epnum].xfer_count);
	dump_bytes("OUT payload", hpcd->OUT_ep[epnum].xfer_buff,
	    hpcd->OUT_ep[epnum].xfer_count);

	if ((epnum != 0U) && (idx < STM32CUBE_UDC_EPS_COUNT)) {
		req = stm32cube_udc.requests[idx];
		if (req != NULL) {
			actual_len = hpcd->OUT_ep[epnum].xfer_count;
			req->actual_len = min(actual_len, req->len);
			stm32cube_udc.requests[idx] = NULL;

			if (req->complete != NULL) {
				req->complete(stm32cube_udc.eps[idx], req);
			}
		}
	}

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

	stm32cube_udc.ep_info[0x00U | 0x00U].maxpacket = USB_MAX_EP0_SIZE;

	/* Open EP0 IN */
	HAL_PCD_EP_Open(hpcd, 0x80U, USB_MAX_EP0_SIZE, EP_TYPE_CTRL); /* EP0_MAX_SIZE */

	stm32cube_udc.ep_info[0x04U | 0x00U].maxpacket = USB_MAX_EP0_SIZE;

	for (i = 0; i < ARRAY_SIZE(stm32cube_udc.ep_info); i++) {
		stm32cube_udc.ep_info[i].is_used = 0;
	}
}
