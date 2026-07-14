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

/* Bit field positions: */
#define USBDC_GCTRL_SU_Pos 0  /*!< Set USB address */
#define USBDC_GCTRL_UA_Pos 1  /*!< USB address */
#define USBDC_GCTRL_TM_Pos 8  /*!< Enable test mode */
#define USBDC_GCTRL_TS_Pos 9  /*!< Testmode selector */
#define USBDC_GCTRL_RW_Pos 12 /*!< Remote wakeup */
#define USBDC_GCTRL_DH_Pos 13 /*!< Disable High-speed */
#define USBDC_GCTRL_EP_Pos 14 /*!< Enable Pull-Up */
#define USBDC_GCTRL_FT_Pos 15 /*!< Functional test mode */
#define USBDC_GCTRL_FI_Pos 27 /*!< Frame number received interrupt */
#define USBDC_GCTRL_SP_Pos 28 /*!< Speed mode interrupt */
#define USBDC_GCTRL_VI_Pos 29 /*!< VBUS valid interrupt */
#define USBDC_GCTRL_UI_Pos 30 /*!< USB reset */
#define USBDC_GCTRL_SI_Pos 31 /*!< Suspend interrupt */

#define GCTRL_SU        (1 << USBDC_GCTRL_SU_Pos)
#define GCTRL_UA(addr)  (addr & 0x7F << USBDC_GCTRL_UA_Pos)
#define GCTRL_TM        (1 << USBDC_GCTRL_TM_Pos)
#define GCTRL_TS        (1 << USBDC_GCTRL_TS_Pos)
#define GCTRL_RW        (1 << USBDC_GCTRL_RW_Pos)
#define GCTRL_DH        (1 << USBDC_GCTRL_DH_Pos)
#define GCTRL_EP        (1 << USBDC_GCTRL_EP_Pos)
#define GCTRL_FT        (1 << USBDC_GCTRL_FT_Pos)
#define GCTRL_FI        (1 << USBDC_GCTRL_FI_Pos)
#define GCTRL_SP        (1 << USBDC_GCTRL_SP_Pos)
#define GCTRL_VI        (1 << USBDC_GCTRL_VI_Pos)
#define GCTRL_UI        (1 << USBDC_GCTRL_UI_Pos)
#define GCTRL_SI        (1 << USBDC_GCTRL_SI_Pos)

struct niiet_usbphy_regs {
	volatile uint32_t HOSTEN;  /*!< HOSTEN USB Host Enable register */
	volatile uint32_t PHYSTAT; /*!< PHYSTAT USB PHY Layer Status register  */
	volatile uint32_t PHYCFG0; /*!< PHYCFG0 USB Physical Layer Config 0 register  */
	volatile uint32_t PHYCFG1; /*!< PHYCFG1 USB Physical Layer Config 1 register */
};

/* Bit field positions: */
#define USBCTR_PHYCFG0_EXTPHY_Pos 0 /*!< Use external PHY Layer */
#define USBCTR_PHYCFG0_POL_Pos    1 /*!< Polarity */
#define USBCTR_PHYCFG0_ATPSEL_Pos 2 /*!< ATP select */
#define USBCTR_PHYCFG0_DTPSEL_Pos 8 /*!< DTP select */

#define USBCTR_PHYCFG0_EXTPHY (1 << USBCTR_PHYCFG0_EXTPHY_Pos)
#define USBCTR_PHYCFG0_POL    (1 << USBCTR_PHYCFG0_POL_Pos)
#define USBCTR_PHYCFG0_ATPSEL (1 << USBCTR_PHYCFG0_ATPSEL_Pos)
#define USBCTR_PHYCFG0_DTPSEL (1 << USBCTR_PHYCFG0_DTPSEL_Pos)

struct niiet_udc {
	struct usb_udc udc;
	struct usb_gadget_ep *eps[NIIET_UDC_EPS_COUNT];
	struct usb_gadget_request *requests[NIIET_UDC_EPS_COUNT];
	struct ep_status ep_info[NIIET_UDC_EPS_COUNT];
	uint32_t ep0_data_len;

	struct niiet_usbd_regs *regs;
	struct niiet_usbphy_regs *usbphy_regs;
	uint32_t status;
	uint8_t addr;
};

extern void usb_phy_utmi_bconf_init(struct niiet_udc *niiet_udc) ;

#endif /* _DRIVERS_USB_GADGET_UDC_NIIET_PRIV_H_ */
