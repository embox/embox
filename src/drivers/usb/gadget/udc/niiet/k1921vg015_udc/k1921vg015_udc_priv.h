/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    03.07.2026
 */

#ifndef _DRIVERS_USB_GADGET_UDC_STM32CUBE_PRIV_H_
#define _DRIVERS_USB_GADGET_UDC_STM32CUBE_PRIV_H_

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
  uint8_t  is_used;
  uint16_t bInterval;
};

struct niiet_usbd_ep_regs {
	volatile uint32_t DATA_BUF; /*!< 0x00 DATA_BUF EndPoint Data Register */
	volatile uint32_t IRQ_STAT; /*!< 0x04 IRQ_STAT Interrupt flag register no control buffer */
	volatile uint32_t IRQ_ENB; /*!< 0x08 IRQ_ENB Enable register Interrupt no control buffer */
	volatile const uint32_t AVAIL_CNT; /*!< 0x0C AVAIL_CNT EndPoint Available count register */
	volatile uint32_t RSP_SC; /*!< 0x10 RSP_SC EndPoint Response Set/Clear register */
	volatile uint32_t MPS; /*!< 0x14 MPS EndPoint maximum packet size register  */
	volatile uint32_t CNT; /*!< 0x18 CNT EndPoint Transfer count register */
	volatile uint32_t USB_EP_CFG; /*!< 0x1C USB_EP_CFG EndPoint configuration register */
	volatile uint32_t START_ADDR; /*!< 0x20 START_ADDR EndPoint RAM start addres register */
	volatile uint32_t END_ADDR; /*!< 0x24 END_ADDR EndPoint RAM end addres register */

	volatile uint32_t Reserved0[6];
};

struct niiet_usbd_regs {
	volatile uint32_t INTSTAT0; /*!< 0x000 INTSTAT0  Interrupt status register */
	volatile uint32_t Reserved0;
	volatile uint32_t INTEN0; /*!< 0x008 INTEN0 Interrupt enable register */
	volatile uint32_t Reserved1;
	volatile uint32_t INTSTAT1; /*!< 0x010 INTSTAT1 USB Interrupt status register  */
	volatile uint32_t INTEN1; /*!< 0x014 INTEN1 USB Interrupt enable register */
	volatile uint32_t OPERATIONS; /*!< 0x018 OPERATIONS USB Operations register */
	volatile const uint32_t FRAMECNT; /*!< 0x01C FRAMECNT USB Frame counter register */
	volatile uint32_t USBADDR; /*!< 0x020 USBADDR USB Address register */

	volatile uint32_t Reserved2;

	volatile uint32_t CEP_DATA_BUF; /*!< 0x028 CEP_DATA_BUF Data buffer for transmission register*/
	volatile uint32_t CEP_CTRL_STAT; /*!< 0x02C CEP_CTRL_STAT  Control register and buffer status of Control EndPoint */
	volatile uint32_t CEP_IRQ_ENB; /*!< 0x030 CEP_IRQ_ENB Interrupt Enable buffer register of Control EndPoint */
	volatile uint32_t CEP_IRQ_STAT; /*!< 0x034 CEP_IRQ_STAT Buffer interrupt flag register of Control EndPoint */
	volatile uint32_t CEP_IN_XFRCNT; /*!< 0x038 CEP_IN_XFRCNT In transfer data count register  of Control EndPoint*/
	volatile const uint32_t CEP_OUT_XFRCNT; /*!< 0x03C CEP_OUT_XFRCNT Out-transfer data count register of Control EndPoint */
	volatile const uint32_t CEP_DATA_AVL; /*!< 0x040 CEP_DATA_AVL Register number of bytes received */
	volatile const uint32_t CEP_SETUP1_0; /*!< 0x044 CEP_SETUP1_0 Register zero and first byte packet Setup  of Control EndPoint */
	volatile const uint32_t CEP_SETUP3_2; /*!< 0x048 CEP_SETUP3_2 Register 2nd and 3rd bytes of the packet Setup  of Control EndPoint */
	volatile const uint32_t CEP_SETUP5_4; /*!< 0x04C CEP_SETUP5_4  Register 4th and 5th bytes of the packet Setup  of Control EndPoint */
	volatile const uint32_t CEP_SETUP7_6; /*!< 0x050 CEP_SETUP7_6 Register on the 6th and 7th byte packet Setup of Control EndPoint */
	volatile uint32_t CEP_START_ADDR; /*!< 0x054 CEP_START_ADDR Control EndPoint RAM start Addr register */
	volatile uint32_t CEP_END_ADDR; /*!< 0x058 CEP_END_ADDR Control EndPoint RAM end Addr register */
	volatile uint32_t DMA_CTRL_STS; /*!< 0x05C DMA_CTRL_STS DMA Control Register */
	volatile uint32_t DMA_CNT; /*!< 0x060 DMA_CNT DMA Count byte Register */

	volatile uint32_t Reserved3[3];

	struct niiet_usbd_ep_regs USB_EP[4];

	volatile uint32_t Reserved4[356];

	volatile uint32_t AHB_DMA_ADDR; /*!< 0x700 AHB_DMA_ADDR AHB addr register  */

	volatile uint32_t Reserved5[47];

	volatile uint32_t PHY_PD; /*!< 0x7C0 PHY_PD USB PHY PowerDown Register */

	volatile uint32_t Reserved6[15];

	volatile uint32_t PLLUSBCFG0; /*!< 0x800 PLLUSBCFG0 PLL configudration 0 register */
	volatile uint32_t PLLUSBCFG1; /*!< 0x804 PLLUSBCFG1 PLL configudration 1 register  */
	volatile uint32_t PLLUSBCFG2; /*!< 0x808 PLLUSBCFG2 PLL configudration 2 register*/
	volatile uint32_t PLLUSBCFG3; /*!< 0x80C PLLUSBCFG3 PLL configudration 3 register  */
	volatile const uint32_t PLLUSBSTAT; /*!< 0x810 PLLUSBSTAT PLL Status Register */
};

#define PLLUSBCFG0_PLLEN         (1 << 0)

#define PLLUSBCFG3_USBCLKSEL     (1 << 24)

struct niiet_udc {
	struct usb_udc udc;
	struct usb_gadget_ep *eps[NIIET_UDC_EPS_COUNT];
	struct usb_gadget_request *requests[NIIET_UDC_EPS_COUNT];
	struct ep_status ep_info[NIIET_UDC_EPS_COUNT];
	uint32_t ep0_data_len;
	struct niiet_usbd_regs hpcd;
};


#endif /* _DRIVERS_USB_GADGET_UDC_STM32CUBE_PRIV_H_ */
