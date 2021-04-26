/**
 * @file
 * @brief
 *
 * @author  Filipp Chubukov
 * @date    02.03.2021
 */

#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/panic.h>
#include <kernel/time/ktime.h>

#include <mem/misc/pool.h>
#include <util/log.h>
#include <stdio.h>
#include <stdlib.h>

#include <embox/unit.h>


#include <stm32f4xx.h>
#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_hcd.h>

#include <drivers/usb/usb.h>

EMBOX_UNIT_INIT(stm32hc_init);

#define USB_IRQ OPTION_GET(NUMBER, irq)

static HCD_HandleTypeDef stm32_hcd_handler;

struct stm32_hcd {
	struct usb_hcd *hcd;
	HCD_HandleTypeDef *hhcd;
	int port_status; /* 0 - device not attached, 1- device attached, port not ready, 2 - device attached, port ready */
	int control_pipes; /* 0 - control pipes closed, 1 - control pipes open */
	int bulk_pipes; /* 0 - bulk pipes closed, 1 - bulk pipes open */
};

POOL_DEF(stm32_hcds, struct stm32_hcd, USB_MAX_HCD);

static void *stm32_hcd_alloc(struct usb_hcd *hcd, void *args) {
	struct stm32_hcd *stm32_hcd = pool_alloc(&stm32_hcds);

	assert(stm32_hcd);
	memset(stm32_hcd, 0, sizeof(struct stm32_hcd));

	stm32_hcd->hcd = hcd;
	stm32_hcd->hhcd = &stm32_hcd_handler;

	stm32_hcd_handler.pData = stm32_hcd;
	return stm32_hcd;
}

void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd) {
	struct stm32_hcd *stm32_hcd = (struct stm32_hcd *)stm32_hcd_handler.pData;
	stm32_hcd->port_status = 1;
}

static irq_return_t stm32_irq_handler(unsigned int irq_nr, void *data) {
	HAL_HCD_IRQHandler(&stm32_hcd_handler);
	return IRQ_HANDLED;
}

static int stm32_hc_start (struct usb_hcd *hcd) {
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
		log_error("stm32_hc_start: HAL_HCD_Init failed");
		return -1;
	}

	/* Turn OFF SOF interrupts */
	stm32_hcd_handler.Instance->GINTMSK &= 0xFFFFFFF0;

	/* Start USB in Host mode*/
	if (HAL_HCD_Start(&stm32_hcd_handler) != HAL_OK) {
		log_error("stm32_hc_start: HAL_HCD_Start failed");
		return -1;
	}

	/* Start VBus */
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_Delay(200);

	struct usb_dev *udev = usb_new_device(NULL, hcd, 0);
	if (!udev) {
		log_error("uhci_start: usb_new_device failed\n");
		return -1;
	}
	return 0;
}

static int stm32_hc_stop (struct usb_hcd *hcd) {
	HAL_HCD_Stop(&stm32_hcd_handler);
	return 0;
}

static int stm32_common_request(struct usb_request *req) {
	/* Request properties */
	uint32_t speed = req->endp->dev->speed;
	struct stm32_hcd *stm32_hcd = (struct stm32_hcd *)stm32_hcd_handler.pData;
	uint32_t packet_type;
	/* Open pipes */
	if (!stm32_hcd->bulk_pipes) {
		if (HAL_HCD_HC_Init(&stm32_hcd_handler, 3, 0x80U, req->endp->dev->addr, speed, 0u, 64) != HAL_OK) {
			log_error("stm32_common_request: error while opening pipe.");
			return -1;
		}
		if (HAL_HCD_HC_Init(&stm32_hcd_handler, 4, 0x00U, req->endp->dev->addr, speed, 0u, 64) != HAL_OK) {
			log_error("stm32_common_request: error while opening pipe.");
			return -1;
		}
		stm32_hcd->bulk_pipes = 1;
	}

	packet_type = (req->token & USB_TOKEN_OUT) ? 1 : 0;
	if (packet_type == 1) {
			if (HAL_HCD_HC_SubmitRequest(&stm32_hcd_handler, 4, 0, EP_TYPE_BULK, 1, (uint8_t*)req->buf, req->len, 0) != HAL_OK) {
				log_error("stm32_common_request: error while processing usb request.");
				return -1;
			}
		}

	if (packet_type == 0) {
			if (HAL_HCD_HC_SubmitRequest(&stm32_hcd_handler, 3, 1, EP_TYPE_BULK, 1, (uint8_t*)req->buf, req->len, 0) != HAL_OK) {
				log_error("stm32_common_request: error while processing usb request.");
				return -1;
			}
	}

	req->actual_len = req->len;
	usb_request_complete(req);
	return 0;
}

static int stm32_control_request(struct usb_request *req) {
	/* Request properties */
	uint32_t speed = req->endp->dev->speed;
	struct stm32_hcd *stm32_hcd = (struct stm32_hcd *)stm32_hcd_handler.pData;
	uint32_t packet_type;
	/* Open pipes */
	if (!stm32_hcd->control_pipes) {
		if (HAL_HCD_HC_Init(&stm32_hcd_handler, 1, 0x80U, req->endp->dev->addr, speed, 0u, 64) != HAL_OK) {
			log_error("stm32_control_request: error while opening pipe.");
			return -1;
		}
		if (HAL_HCD_HC_Init(&stm32_hcd_handler, 2, 0x00U, req->endp->dev->addr, speed, 0u, 64) != HAL_OK) {
			log_error("stm32_control_request: error while opening pipe.");
			return -1;
		}
		stm32_hcd->control_pipes = 1;
	}

	/* Setup */
	if (HAL_HCD_HC_SubmitRequest(&stm32_hcd_handler, 2, 0, EP_TYPE_CTRL, 0, (uint8_t*)&req->ctrl_header, sizeof(req->ctrl_header), 0) != HAL_OK) {
		log_error("stm32_control_request: error while processing request.");
		return -1;
	}

	packet_type = (req->token & USB_TOKEN_OUT) ? 1 : 0;
	if (req->len > 0) {
		if (packet_type == 1) {
			if (HAL_HCD_HC_SubmitRequest(&stm32_hcd_handler, 2, 0, EP_TYPE_CTRL, 1, (uint8_t*)req->buf,req->len, 0) != HAL_OK) {
				log_error("stm32_control_request: error while processing request.");
				return -1;
			}
		}
		if (packet_type == 0) {
			if (HAL_HCD_HC_SubmitRequest(&stm32_hcd_handler, 1, 1, EP_TYPE_CTRL, 1, (uint8_t*)req->buf,req->len, 0) != HAL_OK) {
				log_error("stm32_control_request: error while processing request.");
				return -1;
			}
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

static uint32_t stm32_roothub_portstatus() {
	struct stm32_hcd *stm32_hcd = (struct stm32_hcd *)stm32_hcd_handler.pData;

	/* return values from other USB drivers */
	if (stm32_hcd->port_status == 0) {
		return 256; /* No device in port */
	}
	if (stm32_hcd->port_status == 1) {
		return 0x1114371; /* Device in port, not enable */
	}
	if (stm32_hcd->port_status == 2){
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
	struct usb_control_header *ctrl = &req->ctrl_header;

	uint32_t type_req = (ctrl->bm_request_type << 8) | ctrl->b_request;

	switch (type_req) {
	case USB_GET_HUB_DESCRIPTOR:
		stm32_get_hub_descriptor((struct usb_desc_hub *) req->buf);
		break;
	case USB_GET_PORT_STATUS:
		*(uint32_t *)req->buf = stm32_roothub_portstatus();
		break;
	case USB_SET_PORT_FEATURE:
		switch (ctrl->w_value) {
		case USB_PORT_FEATURE_RESET:
			break;
		case USB_PORT_FEATURE_POWER:
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
			struct stm32_hcd *stm32_hcd = (struct stm32_hcd *)stm32_hcd_handler.pData;
			stm32_hcd->port_status = 2;
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

static struct usb_hcd_ops stm32_hcd_ops = {
	.hcd_hci_alloc = stm32_hcd_alloc,
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
		log_error("stm32hc_init: irq_attach failed.\n");
		return -1;
	}

	return usb_hcd_register(hcd);
}

STATIC_IRQ_ATTACH(USB_IRQ, stm32_irq_handler, NULL);
