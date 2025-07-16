/**
 * @file
 *
 * @date Nov 28, 2019
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <stddef.h>
#include <unistd.h>

#include <drivers/clk/ccm_imx6.h>
#include <drivers/common/memory.h>
#include <hal/reg.h>

#include <framework/mod/options.h>

#define BASE_ADDR          OPTION_GET(NUMBER,base_addr)
#define USB_PHY_QUANTITY   OPTION_GET(NUMBER,phy_quantity)

/* PHY-related stuff */
/* There are two ports in i.MX6 processors */
#define USBPHY_BASE(port)          (BASE_ADDR + 0x1000 * port)

#define IMX6_USB_PHY_UTMI0  OPTION_GET(NUMBER,phy0_irq_num) /* 76 */
#define IMX6_USB_PHY_UTMI1  OPTION_GET(NUMBER,phy1_irq_num) /* 77 */

#define USBPHY_PWD(port)           (USBPHY_BASE(port) + 0x00)
#define USBPHY_PWD_SET(port)       (USBPHY_BASE(port) + 0x04)
#define USBPHY_PWD_CLR(port)       (USBPHY_BASE(port) + 0x08)
#define USBPHY_PWD_TOG(port)       (USBPHY_BASE(port) + 0x0C)
#define USBPHY_TX(port)            (USBPHY_BASE(port) + 0x10)
#define USBPHY_TX_SET(port)        (USBPHY_BASE(port) + 0x14)
#define USBPHY_TX_CLR(port)        (USBPHY_BASE(port) + 0x18)
#define USBPHY_TX_TOG(port)        (USBPHY_BASE(port) + 0x1C)
#define USBPHY_RX(port)            (USBPHY_BASE(port) + 0x20)
#define USBPHY_RX_SET(port)        (USBPHY_BASE(port) + 0x24)
#define USBPHY_RX_CLR(port)        (USBPHY_BASE(port) + 0x28)
#define USBPHY_RX_TOG(port)        (USBPHY_BASE(port) + 0x2C)
#define USBPHY_CTRL(port)          (USBPHY_BASE(port) + 0x30)
#define USBPHY_CTRL_SET(port)      (USBPHY_BASE(port) + 0x34)
#define USBPHY_CTRL_CLR(port)      (USBPHY_BASE(port) + 0x38)
#define USBPHY_CTRL_TOG(port)      (USBPHY_BASE(port) + 0x3C)
# define USBPHY_CTRL_SFTRST                (1u << 31)
# define USBPHY_CTRL_CLKGATE               (1u << 30)
# define USBPHY_CTRL_ENAUTOSET_USBCLKS     (1u << 26)
# define USBPHY_CTRL_ENAUTOCLR_USBCLKGATE  (1u << 25)
# define USBPHY_CTRL_ENAUTOCLR_PHY_PWD     (1u << 20)
# define USBPHY_CTRL_ENAUTOCLR_CLKGATE     (1u << 19)
# define USBPHY_CTRL_ENAUTO_PWRON_PLL      (1u << 18)
# define USBPHY_CTRL_ENUTMILEVEL3          (1u << 15)
# define USBPHY_CTRL_ENUTMILEVEL2          (1u << 14)
#define USBPHY_STATUS(port)        (USBPHY_BASE(port) + 0x40)
#define USBPHY_DEBUG(port)         (USBPHY_BASE(port) + 0x50)
#define USBPHY_DEBUG_SET(port)     (USBPHY_BASE(port) + 0x54)
#define USBPHY_DEBUG_CLR(port)     (USBPHY_BASE(port) + 0x58)
#define USBPHY_DEBUG_TOG(port)     (USBPHY_BASE(port) + 0x5C)
#define USBPHY_DEBUG0_STATUS(port) (USBPHY_BASE(port) + 0x60)
#define USBPHY_DEBUG1(port)        (USBPHY_BASE(port) + 0x70)
#define USBPHY_DEBUG1_SET(port)    (USBPHY_BASE(port) + 0x74)
#define USBPHY_DEBUG1_CLR(port)    (USBPHY_BASE(port) + 0x78)
#define USBPHY_DEBUG1_TOG(port)    (USBPHY_BASE(port) + 0x7C)
#define USBPHY_VERSION(port)       (USBPHY_BASE(port) + 0x80)

#define USB_ANALOG_BASE(port) (0x20C81A0 + 0x80 * port)

#define USB_ANALOG_VBUS_DETECT(port)      (USB_ANALOG_BASE(port) + 0x00)
#define USB_ANALOG_VBUS_DETECT_SET(port)  (USB_ANALOG_BASE(port) + 0x04)
#define USB_ANALOG_VBUS_DETECT_CLR(port)  (USB_ANALOG_BASE(port) + 0x08)
#define USB_ANALOG_VBUS_DETECT_TOG(port)  (USB_ANALOG_BASE(port) + 0x0C)
#define USB_ANALOG_CHRG_DETECT(port)      (USB_ANALOG_BASE(port) + 0x10)
#define USB_ANALOG_CHRG_DETECT_SET(port)  (USB_ANALOG_BASE(port) + 0x14)
#define USB_ANALOG_CHRG_DETECT_CLR(port)  (USB_ANALOG_BASE(port) + 0x18)
#define USB_ANALOG_CHRG_DETECT_TOG(port)  (USB_ANALOG_BASE(port) + 0x1C)
# define USB_ANALOG_EN_B                  (1 << 20)
# define USB_ANALOG_CHK_CHRG_B            (1 << 19)
# define USB_ANALOG_CHK_CONTACT           (1 << 18)
#define USB_ANALOG_VBUS_DETECT_STAT(port) (USB_ANALOG_BASE(port) + 0x20)
#define USB_ANALOG_CHRG_DETECT_STAT(port) (USB_ANALOG_BASE(port) + 0x30)
#define USB_ANALOG_MISC_DETECT(port)      (USB_ANALOG_BASE(port) + 0x40)
#define USB_ANALOG_MISC_DETECT_SET(port)  (USB_ANALOG_BASE(port) + 0x44)
#define USB_ANALOG_MISC_DETECT_CLR(port)  (USB_ANALOG_BASE(port) + 0x48)
#define USB_ANALOG_MISC_DETECT_TOG(port)  (USB_ANALOG_BASE(port) + 0x4C)
#define USB_ANALOG_DIGPROG                (USB_ANALOG_BASE(0)    + 0xA0)

struct ehci_mxc_usbphy {
	int phy_num;
	uintptr_t base;
};

static struct ehci_mxc_usbphy ehci_mxc_usbphys[USB_PHY_QUANTITY];

static inline void ehci_mxc_regdump(struct ehci_mxc_usbphy *usbphy) {
	log_info("========= PHY REGISTERS =========\n");
	log_info("USBPHY%d_PWD     %08x\n", usbphy->phy_num, REG32_LOAD(USBPHY_PWD(usbphy->phy_num)));
	log_info("USBPHY%d_TX      %08x\n", usbphy->phy_num, REG32_LOAD(USBPHY_TX(usbphy->phy_num)));
	log_info("USBPHY%d_RX      %08x\n", usbphy->phy_num, REG32_LOAD(USBPHY_RX(usbphy->phy_num)));
	log_info("USBPHY%d_CTRL    %08x\n", usbphy->phy_num, REG32_LOAD(USBPHY_CTRL(usbphy->phy_num)));
	log_info("USBPHY%d_STATUS  %08x\n", usbphy->phy_num, REG32_LOAD(USBPHY_STATUS(usbphy->phy_num)));
}

struct ehci_mxc_usbphy *imx_usb_phy_create(int i) {
	if (i >= USB_PHY_QUANTITY) {
		return NULL;
	}
	ehci_mxc_usbphys[i].base = USBPHY_BASE(i);
	ehci_mxc_usbphys[i].phy_num = i;

	return &ehci_mxc_usbphys[i];
}
extern int stmp_reset_block(void *reset_addr) ;
void imx_usb_phy_enable(int port) {
	uint32_t val;

	ccm_analog_usb_init(port);

	/* FIXME Should it be here? */
	usleep(1 * 1000);

	stmp_reset_block((void *)(uintptr_t)USBPHY_CTRL(port));
	REG32_STORE(USBPHY_PWD(port), 0);
	log_debug("reg(%x) val(%x)", 0, 0);

	/* enable FS/LS device */
	val = USBPHY_CTRL_ENAUTOSET_USBCLKS |
			USBPHY_CTRL_ENAUTOCLR_USBCLKGATE |
			USBPHY_CTRL_ENAUTOCLR_PHY_PWD |
			USBPHY_CTRL_ENAUTOCLR_CLKGATE |
			USBPHY_CTRL_ENAUTO_PWRON_PLL |
			USBPHY_CTRL_ENUTMILEVEL2 |
			USBPHY_CTRL_ENUTMILEVEL3;
	REG32_STORE(USBPHY_CTRL_SET(port), val);
	log_debug("reg(%x) val(%x)", 0x34, val);
}

void imx_usb_powerup(int port) {
	//ccm_analog_usb_init(port);
}

PERIPH_MEMORY_DEFINE(imx_usb_phy, USBPHY_BASE(0), 0x2000);
