/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    03.07.2026
 */

#ifndef _DRIVERS_USB_GADGET_UDC_NIIET_PRIV_H_
#define _DRIVERS_USB_GADGET_UDC_NIIET_PRIV_H_

#include <stdint.h>

#include <drivers/usb/gadget/udc.h>
#include <drivers/usb/usb_defines.h>

#define USB_MAX_EP0_SIZE 64U

#define NIIET_UDC_EPS_COUNT 4 /* IN and OUT */

struct ep_status {
	uint32_t status;
	uint32_t total_length;
	uint32_t rem_length;
	uint32_t maxpacket;
	uint8_t is_used;
	uint16_t bInterval;
};

struct niiet_usbd_ep_regs {
	volatile uint32_t CTRL;
	volatile uint32_t DMACTRL;
	volatile uint32_t DMADESC;
	volatile uint32_t STAT;
};

struct niiet_usbd_regs {
	struct niiet_usbd_ep_regs ep_in[4];
	uint32_t reserved0[48];
	struct niiet_usbd_ep_regs ep_out[4];  /* 0x100 */
	uint32_t reserved1[48];
	volatile uint32_t GCTRL;              /* 0x200 */
	volatile uint32_t GSTAT;              /* 0x204 */
};

struct niiet_udc {
	struct usb_udc udc;
	struct usb_gadget_ep *eps[NIIET_UDC_EPS_COUNT];
	struct usb_gadget_request *requests[NIIET_UDC_EPS_COUNT];
	struct ep_status ep_info[NIIET_UDC_EPS_COUNT];
	uint32_t ep0_data_len;
	struct niiet_usbd_regs regs;
};

#endif /* _DRIVERS_USB_GADGET_UDC_NIIET_PRIV_H_ */
