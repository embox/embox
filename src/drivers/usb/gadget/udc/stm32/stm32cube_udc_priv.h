#ifndef _DRIVERS_USB_GADGET_UDC_STM32CUBE_PRIV_H_
#define _DRIVERS_USB_GADGET_UDC_STM32CUBE_PRIV_H_

#include <stdint.h>

#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/usb_defines.h>

#include <bsp/stm32cube_hal.h>

#define USB_MAX_EP0_SIZE 64U

#define STM32CUBE_UDC_EPS_COUNT 8 /* IN and OUT */

//extern PCD_HandleTypeDef hpcd;


struct ep_status {
	//	uint32_t status;
	uint32_t total_length;
	uint32_t rem_length;
	uint32_t maxpacket;
	uint8_t is_used;
	//	uint16_t bInterval;
};

struct stm32cube_udc {
	struct usb_udc udc;
	struct usb_gadget_ep *eps[STM32CUBE_UDC_EPS_COUNT];
	struct usb_gadget_request *requests[STM32CUBE_UDC_EPS_COUNT];
	struct ep_status ep_info[STM32CUBE_UDC_EPS_COUNT];
	uint32_t ep0_data_len;
	PCD_HandleTypeDef hpcd;
};

extern int stm32cube_usbd_init(struct stm32cube_udc *stm32cube_udc);


static inline unsigned int stm32cube_udc_ep_in_idx(uint8_t epnum) {
	return 0x4U | epnum;
}

static inline unsigned int stm32cube_udc_ep_out_idx(uint8_t epnum) {
	return 0x0U | epnum;
}

static inline uint8_t stm32cube_udc_ep_type(struct usb_gadget_ep *ep) {
	return ep->desc->bm_attributes & USB_DESC_ENDP_TYPE_MASK;
}

extern struct stm32cube_udc stm32cube_udc;

#endif /* _DRIVERS_USB_GADGET_UDC_STM32CUBE_PRIV_H_ */
