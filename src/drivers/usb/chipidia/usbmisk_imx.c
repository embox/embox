/**
 * @file
 *
 * @date Nov 29, 2019
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <stdint.h>

#include <hal/reg.h>

#include <drivers/common/memory.h>

#define IMX_USB_NONCORE_BASE 0x2184800

#define BIT(n) (1 << (n))

#define MX6_BM_NON_BURST_SETTING BIT(1)
#define MX6_BM_OVER_CUR_DIS      BIT(7)
#define MX6_BM_OVER_CUR_POLARITY BIT(8)
#define MX6_BM_PWR_POLARITY      BIT(9)
#define MX6_BM_WAKEUP_ENABLE     BIT(10)
#define MX6_BM_UTMI_ON_CLOCK     BIT(13)
#define MX6_BM_ID_WAKEUP         BIT(16)
#define MX6_BM_VBUS_WAKEUP       BIT(17)
#define MX6SX_BM_DPDM_WAKEUP_EN  BIT(29)
#define MX6_BM_WAKEUP_INTR       BIT(31)

static int usbmisc_imx6q_set_wakeup(int index, int enabled) {
	uint32_t val;
	uint32_t wakeup_setting = (MX6_BM_WAKEUP_ENABLE |
		MX6_BM_VBUS_WAKEUP | MX6_BM_ID_WAKEUP);
	int ret = 0;

	val = REG_LOAD(IMX_USB_NONCORE_BASE + index * 4);
	if (enabled) {
		val |= wakeup_setting;
	} else {
		if (val & MX6_BM_WAKEUP_INTR) {
			log_debug("wakeup int at ci_hdrc.%d", index);
		}
		val &= ~wakeup_setting;
	}
	REG_STORE(IMX_USB_NONCORE_BASE + index * 4, val);
	log_debug("addr(%x), val(%x)", index * 4, val);

	return ret;
}

int usbmisc_imx6q_init(int index) {
	uint32_t reg;
/*
	reg = REG_LOAD(IMX_USB_NONCORE_BASE + index * 4);
	reg |= MX6_BM_OVER_CUR_DIS;
	REG_STORE(IMX_USB_NONCORE_BASE + index * 4, reg);
*/
	/* SoC non-burst setting */
	reg = REG_LOAD(IMX_USB_NONCORE_BASE + index * 4);
	reg |= MX6_BM_NON_BURST_SETTING;
	REG_STORE(IMX_USB_NONCORE_BASE + index * 4, reg);

	usbmisc_imx6q_set_wakeup(index, 0);

	return 0;
}

PERIPH_MEMORY_DEFINE(imx_usb, IMX_USB_NONCORE_BASE, 0x20);
