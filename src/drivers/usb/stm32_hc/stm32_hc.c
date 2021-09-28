/**
 * @file
 * @brief
 *
 * @author  Filipp Chubukov
 * @date    02.03.2021
 */

#include <util/log.h>

#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <kernel/time/ktime.h>

#include <mem/misc/pool.h>

#include <embox/unit.h>

#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_hcd.h>

#include <drivers/usb/usb.h>
#include "stm32_hc.h"

EMBOX_UNIT_INIT(stm32hc_init);

#define USB_IRQ OPTION_GET(NUMBER, irq)

POOL_DEF(stm32_hcds, struct stm32_hcd, USB_MAX_HCD);

static inline struct stm32_hcd *hcd_to_stm32hcd(struct usb_hcd *hcd) {
	assert(hcd);
	return (struct stm32_hcd *) hcd->hci_specific;
}

static volatile int stm32_hub_inited = 0;
static volatile int must_notify_hub = 0;

static int stm32_chan_init(uint8_t ch_num, uint8_t endp_num, uint8_t dev_addr, uint8_t speed, uint8_t ed_type, uint16_t mps){
	return HAL_HCD_HC_Init(&stm32_hcd_handler,ch_num, endp_num,dev_addr,speed,ed_type, mps);
}

static void *stm32_hcd_alloc(struct usb_hcd *hcd, void *args) {
	struct stm32_hcd *stm32_hcd = pool_alloc(&stm32_hcds);

	assert(stm32_hcd);
	memset(stm32_hcd, 0, sizeof(struct stm32_hcd));

	stm32_hcd->hcd = hcd;
	stm32_hcd->hhcd = &stm32_hcd_handler;

	stm32_hcd_handler.pData = stm32_hcd;
	return stm32_hcd;
}

/**
 * Called when host_port_status has been updated so that any status change
 * interrupt transfer that was sent to the root hub can be fulfilled.
 */
static void stm32_hcd_port_status_changed(void) {
	extern void usb_hubs_notify(void);
	usb_hubs_notify();
}


void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum, HCD_URBStateTypeDef urb_state) {

	/* To be used with OS to sync URB state with the global state machine */
	/* URB_IDLE URB_NOTREADY URB_DONE */
}

void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef *hhcd) {
	struct stm32_hcd *stm32_hcd = hhcd2stm_hcd(hhcd);
	stm32_hcd->port_status = STM32_PORT_IDLE;

	log_debug("Port Disabled.");
}

void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef *hhcd) {
	struct stm32_hcd *stm32_hcd = hhcd2stm_hcd(hhcd);
	stm32_hcd->port_status = STM32_PORT_CONNECTED;
	log_debug("");
}

void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd) {
	struct stm32_hcd *stm32_hcd = hhcd2stm_hcd(hhcd);

	log_error("");

	if(stm32_hcd->port_status == STM32_PORT_IDLE) {
		if (stm32_hub_inited) {
			stm32_hcd_port_status_changed();
		} else {
			must_notify_hub = 1;
		}
	}
	stm32_hcd->port_status = STM32_PORT_CONNECTED;
}

void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd) {
	struct stm32_hcd *stm32_hcd = hhcd2stm_hcd(hhcd);

	log_error("");

	if(stm32_hcd->port_status != STM32_PORT_IDLE) {
		stm32_hcd_port_status_changed();
	}
	stm32_hcd->port_status = STM32_PORT_IDLE;
}

static irq_return_t stm32_irq_handler(unsigned int irq_nr, void *data) {
	HAL_HCD_IRQHandler(&stm32_hcd_handler);
	return IRQ_HANDLED;
}

static int stm32_hc_start (struct usb_hcd *hcd) {
	struct usb_dev *udev;

	/* Init HCD_HandleTypeDef structure*/
	stm32_hcd_handler.Instance = USB_OTG_FS;
    stm32_hcd_handler.Init.Host_channels = 11;
    stm32_hcd_handler.Init.dma_enable = 0;
    stm32_hcd_handler.Init.low_power_enable = 0;
    stm32_hcd_handler.Init.phy_itface = HCD_PHY_EMBEDDED;
    stm32_hcd_handler.Init.Sof_enable = 0;
    stm32_hcd_handler.Init.speed = HCD_SPEED_FULL;
    stm32_hcd_handler.Init.vbus_sensing_enable = 0;
    stm32_hcd_handler.Init.lpm_enable = 0;

	/* Init USB Core */
    if (HAL_HCD_Init(&stm32_hcd_handler) != HAL_OK) {
		log_error("HAL_HCD_Init failed");
		return -1;
	}

	/* Turn OFF SOF interrupts */
	stm32_hcd_handler.Instance->GINTMSK &= 0xFFFFFFF0;

	/* Start USB in Host mode*/
	if (HAL_HCD_Start(&stm32_hcd_handler) != HAL_OK) {
		log_error("HAL_HCD_Start failed");
		return -1;
	}

	/* Start VBus */
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_Delay(2000);

	/* Create root hub */
	udev = usb_new_device(NULL, hcd, 0);

	if (!udev) {
		log_error("usb_new_device failed");
		return -1;
	}

	if (must_notify_hub) {
		stm32_hcd_port_status_changed();
	}
	stm32_hub_inited = 1;

	return 0;
}

static int stm32_hc_stop (struct usb_hcd *hcd) {
	HAL_HCD_Stop(&stm32_hcd_handler);
	return 0;
}

static int stm32_common_request(struct usb_request *req) {

	uint32_t packet_type = (req->token & USB_TOKEN_OUT) ? STM32_URB_OUT  : STM32_URB_IN;

	if (req->len > STM32_MAX_PACKET_SIZE) {
		/* If URB > 64 send few URBs */
		uint8_t *tmp = (uint8_t *)req->buf;
		uint8_t *end = tmp + req->len; /* end of buffer */
		uint32_t packet_size;
		uint32_t max_size = STM32_MAX_PACKET_SIZE;

		while (tmp < end) {
			packet_size = end - tmp; /* size of remaining data */
			if (packet_size > max_size) {
				packet_size = max_size;
			}

			/* Send OUT URB */
			if (packet_type == STM32_URB_OUT) {
				if (HAL_HCD_HC_SubmitRequest(&stm32_hcd_handler, STM32_PIPE_BULK_OUT,
						0, EP_TYPE_BULK, 1, tmp, packet_size, 0) != HAL_OK) {
							log_error("error while processing usb request.");
							return -1;
				}
				HAL_Delay(200);
			}

			/* Send IN URB */
			if (packet_type == STM32_URB_IN) {
				if (HAL_HCD_HC_SubmitRequest(&stm32_hcd_handler, STM32_PIPE_BULK_IN,
						1, EP_TYPE_BULK, 1, tmp, packet_size, 0) != HAL_OK) {
							log_error("error while processing usb request.");
							return -1;
				}
				HAL_Delay(200);
			}

			tmp += packet_size;
		}

	} else {
		/* If URB < 64 send one URB */
		if (packet_type == STM32_URB_OUT) {
			if (HAL_HCD_HC_SubmitRequest(&stm32_hcd_handler, STM32_PIPE_BULK_OUT,
					0, EP_TYPE_BULK, 1, (uint8_t*)req->buf,req->len, 0) != HAL_OK) {
						log_error("error while processing usb request.");
						return -1;
			}
			HAL_Delay(200);
		}

		if (packet_type == STM32_URB_IN) {
			if (HAL_HCD_HC_SubmitRequest(&stm32_hcd_handler, STM32_PIPE_BULK_IN,
					1, EP_TYPE_BULK, 1, (uint8_t*)req->buf,req->len, 0) != HAL_OK) {
						log_error("error while processing usb request.");
						return -1;
			}
			HAL_Delay(200);
		}
	}

	req->actual_len = req->len;
	usb_request_complete(req);
	return 0;
}

static int stm32_control_request(struct usb_request *req) {
	uint32_t packet_type = (req->token & USB_TOKEN_OUT) ? STM32_URB_OUT : STM32_URB_IN;

	/* Setup  URB */
	if (HAL_HCD_HC_SubmitRequest(&stm32_hcd_handler, STM32_PIPE_CONTROL_OUT, 0,
			EP_TYPE_CTRL, 0, (uint8_t*)&req->ctrl_header,
				sizeof(req->ctrl_header), 0) != HAL_OK) {
					log_error("error while processing request.");
					return -1;
	}
	HAL_Delay(200);

	if (req->len > 1) {
		/* Send OUT URB */
		if (packet_type == STM32_URB_OUT) {
			if (HAL_HCD_HC_SubmitRequest(&stm32_hcd_handler, STM32_PIPE_CONTROL_OUT,
					0, EP_TYPE_CTRL, 1, (uint8_t*)req->buf,req->len, 0) != HAL_OK) {
						log_error("error while processing request.");
						return -1;
			}
			HAL_Delay(200);
		}

		/* Send IN URB */
		if (packet_type == STM32_URB_IN) {
			if (HAL_HCD_HC_SubmitRequest(&stm32_hcd_handler, STM32_PIPE_CONTROL_IN,
					1, EP_TYPE_CTRL, 1, (uint8_t*)req->buf,req->len, 0) != HAL_OK) {
						log_error("error while processing request.");
						return -1;
			}
			HAL_Delay(200);
		}
	}

	req->actual_len = req->len;
	usb_request_complete(req);
	return 0;
}

static int stm32_request_do(struct usb_request *req) {
	switch (req->endp->type) {
	case USB_COMM_CONTROL:
		stm32_control_request(req);
		break;
	case USB_COMM_BULK:
	case USB_COMM_INTERRUPT:
	case USB_COMM_ISOCHRON:
		stm32_common_request(req);
		break;
	default:
		panic("stm32_request: Unsupported enpd type %d", req->endp->type);
	}

	return 0;
}

static int stm32_request (struct usb_request *req) {
	return stm32_request_do(req);
}

static void stm32_port_reset(struct stm32_hcd *stm32_hcd) {
//	HAL_HCD_ResetPort(stm32_hcd->hhcd);

	log_debug("");
}

static void stm32_port_set_power(struct stm32_hcd *stm32_hcd, int value) {
	log_debug("");
}

static uint32_t stm32_roothub_portstatus(struct stm32_hcd *stm32_hcd) {
	/* return values from other USB drivers : TODO : make adequate return*/
	if (stm32_hcd->port_status == STM32_PORT_IDLE) {
		return 256; /* No device in port */
	}
	if (stm32_hcd->port_status == STM32_PORT_CONNECTED) {
		return 0x1114371; /* Device in port, not enable */
	}
	if (stm32_hcd->port_status == STM32_PORT_READY){
		return 0x100103; /* Device in port, enable */
	}

	return 0;
}

static void stm32_get_hub_descriptor(struct usb_desc_hub *desc) {
	desc->b_desc_length = 7;
	desc->b_desc_type = USB_DT_HUB;
	desc->b_nbr_ports = 1;
}

static int stm32_root_hub_control (struct usb_request *req) {
	struct stm32_hcd *stm32_hcd;
	struct usb_control_header *ctrl;

	stm32_hcd = hcd_to_stm32hcd(req->endp->dev->hcd);
	ctrl = &req->ctrl_header;

	uint32_t type_req = (ctrl->bm_request_type << 8) | ctrl->b_request;

	switch (type_req) {
	case USB_GET_HUB_DESCRIPTOR:
		stm32_get_hub_descriptor((struct usb_desc_hub *) req->buf);
		break;
	case USB_GET_PORT_STATUS:
		*(uint32_t *)req->buf = stm32_roothub_portstatus(stm32_hcd);
		break;
	case USB_SET_PORT_FEATURE:
		switch (ctrl->w_value) {
		case USB_PORT_FEATURE_RESET:
			stm32_port_reset(stm32_hcd);
			break;
		case USB_PORT_FEATURE_POWER:
			stm32_port_set_power(stm32_hcd, 1);
			break;
		default:
			log_error("Unknown port set feature: 0x%x\n", ctrl->w_value);
			return -1;
		}
		break;

    case USB_CLEAR_PORT_FEATURE:
  		switch (ctrl->w_value) {
  		case USB_PORT_FEATURE_POWER:
  			break;
  		case USB_PORT_FEATURE_C_CONNECTION:
			/* Reset port and change port status in stm32_hcd struct */
			HAL_HCD_ResetPort(&stm32_hcd_handler);
			struct stm32_hcd *stm32_hcd = hhcd2stm_hcd(&stm32_hcd_handler);
			stm32_hcd->port_status = STM32_PORT_READY;
  			break;
  		case USB_PORT_FEATURE_C_RESET:
  			break;
  		default:
			log_error("Unknown port clear feature: 0x%x\n", ctrl->w_value);
  			return -1;
  		}
		break;
	default:
		panic("stm32_root_hub_control: Unknown req_type=0x%x, request=0x%x\n",
				ctrl->bm_request_type, ctrl->b_request);
    	break;

	}
	req->req_stat = USB_REQ_NOERR;
	usb_request_complete(req);
	return 0;
}

static void *stm32_endp_alloc(struct usb_endp *endp) {
	struct stm32_hcd *stm32_hcd;

	stm32_hcd = hcd_to_stm32hcd(endp->dev->hcd);
	if (endp->address == 0) {
		switch (stm32_hcd->free_chan_idx) {
		case 0:
			stm32_chan_init(STM32_PIPE_CONTROL_OUT, STM32_ENDP_CONTROL_OUT, endp->dev->addr, endp->dev->speed, endp->type, endp->max_packet_size);
			stm32_hcd->free_chan_idx ++;
			break;
		case 1:
			stm32_chan_init(STM32_PIPE_CONTROL_IN, STM32_ENDP_CONTROL_IN, endp->dev->addr, endp->dev->speed, endp->type, endp->max_packet_size);
			stm32_hcd->free_chan_idx ++;
			break;
		}
		return &stm32_hcd_handler;
	} else {
		if (endp->direction == USB_DIRECTION_IN) {
			stm32_chan_init(STM32_PIPE_BULK_IN, STM32_ENDP_BULK_IN, STM32_USB_DEV_ADDR, endp->dev->speed, EP_TYPE_BULK, endp->max_packet_size);
		} else {
			stm32_chan_init(STM32_PIPE_BULK_OUT, STM32_ENDP_BULK_OUT, STM32_USB_DEV_ADDR, endp->dev->speed, EP_TYPE_BULK, endp->max_packet_size);
		}
	}

	return &stm32_hcd_handler;
}

static void stm32_endp_free(struct usb_endp *endp, void *spec) {

}

static struct usb_hcd_ops stm32_hcd_ops = {
	.hcd_hci_alloc = stm32_hcd_alloc,
	.endp_hci_alloc = stm32_endp_alloc,
	.endp_hci_free = stm32_endp_free,
	.hcd_start = stm32_hc_start,
	.hcd_stop = stm32_hc_stop,
	.root_hub_control = stm32_root_hub_control,
	.request = stm32_request
};

static int stm32hc_init(void) {
	int ret;
	struct usb_hcd *hcd;

	hcd = usb_hcd_alloc(&stm32_hcd_ops, 0);
	if (!hcd) {
		return -ENOMEM;
	}

	ret = irq_attach(USB_IRQ, stm32_irq_handler, 0, NULL,"");
	if (ret < 0) {
		log_error("irq_attach failed.");
		return -1;
	}

	return usb_hcd_register(hcd);
}

STATIC_IRQ_ATTACH(USB_IRQ, stm32_irq_handler, NULL);
