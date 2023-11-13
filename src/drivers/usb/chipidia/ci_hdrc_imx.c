/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 26.06.2019
 */

#include <errno.h>

#include <util/log.h>

#include <drivers/clk/ccm_imx6.h>
#include <drivers/common/memory.h>
#include <hal/reg.h>
#include <kernel/irq.h>

#include <drivers/usb/usb.h>
#include <drivers/usb/ehci.h>

#include <embox/unit.h>

#include "usb_ehci_mxc_regs.h"

EMBOX_UNIT_INIT(imx_usb_init);

#define IMX_USB_CORE_BASE     OPTION_GET(NUMBER, base_addr)
#define IMX_USB_IRQ           OPTION_GET(NUMBER, irq_num)

struct ehci_mxc_usbphy;
struct ehci_mxc_usbphy *imx_usb_phy_create(int i);
extern void imx_usb_phy_enable(int port);
extern void imx_usb_powerup(int port);
extern int usbmisc_imx6q_init(int index);

struct ehci_mxc_hc {
	uintptr_t base;
	int irq_num;
	struct ehci_mxc_usbphy *usbphy;
	struct ehci_hdc *ehci;
};

static struct ehci_mxc_hc ehci_mxc_hc;

static inline uint32_t ehci_mxc_read(struct ehci_mxc_hc *hc, int reg) {
	return REG32_LOAD(hc->base + reg);
}

static inline void ehci_mxc_write(struct ehci_mxc_hc *hc, int reg, uint32_t val) {
	REG32_STORE(hc->base + reg, val);
}

#define IMX6_USB0_IRQ       72
#define IMX6_USB1_IRQ       73
#define IMX6_USB2_IRQ       74
#define IMX6_USB_OTG_IRQ    75

static void ehci_mxc_regdump(struct ehci_mxc_hc *hc) {
	log_info("USB_UOG_ID         %08x\n", ehci_mxc_read(hc, USB_UOG_ID));
	log_info("USB_UOG_HWGENERAL  %08x\n", ehci_mxc_read(hc, USB_UOG_HWGENERAL));
	log_info("USB_UOG_HWHOST     %08x\n", ehci_mxc_read(hc, USB_UOG_HWHOST));
	log_info("USB_UOG_HWDEVICE   %08x\n", ehci_mxc_read(hc, USB_UOG_HWDEVICE));
	log_info("USB_UOG_HWTXBUF    %08x\n", ehci_mxc_read(hc, USB_UOG_HWTXBUF));
	log_info("USB_UOG_HWRXBUF    %08x\n", ehci_mxc_read(hc, USB_UOG_HWRXBUF));

}

static inline irq_return_t imx6_irq(unsigned int irq_nr, void *data) {
	log_debug("IRQ%d enter", irq_nr);

	return IRQ_HANDLED;
}

static inline int ehci_mxc_init(struct ehci_mxc_hc *hc) {
	uint32_t uog_id;

	uog_id = ehci_mxc_read(hc, USB_UOG_ID);
	log_info("USB 2.0 high-Speed code rev 0x%02x NID 0x%02x ID 0x%02x\n",
		(uog_id >> USB_UOG_ID_REV_OFFT) & USB_UOG_ID_REV_MASK,
		(uog_id >> USB_UOG_ID_NID_OFFT) & USB_UOG_ID_NID_MASK,
		(uog_id >> USB_UOG_ID_ID_OFFT) & USB_UOG_ID_ID_MASK);

	ehci_mxc_regdump(hc);

	return 0;
}

static int imx_usb_init(void) {
	clk_enable("usboh3");

	ehci_mxc_hc.base = IMX_USB_CORE_BASE;
	ehci_mxc_hc.irq_num = IMX6_USB0_IRQ;
	ehci_mxc_hc.ehci = NULL;
	ehci_mxc_hc.usbphy = imx_usb_phy_create(1);

	usbmisc_imx6q_init(1);
	imx_usb_phy_enable(1);
	imx_usb_powerup(1);

	ehci_mxc_init(&ehci_mxc_hc);

	ehci_hcd_register((void *) (ehci_mxc_hc.base + 0x100), ehci_mxc_hc.irq_num);

	/* Configure GPIO pins */

	return 0;
}

PERIPH_MEMORY_DEFINE(imx_usb, IMX_USB_CORE_BASE, 0x7B0);
