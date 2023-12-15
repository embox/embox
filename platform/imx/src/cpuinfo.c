/**
 * @file cpuinfo.c
 * @brief This module helps to detect the revision and stuff i.MX6 CPU
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 22.05.2017
 */

#include <inttypes.h>
#include <stdio.h>

#include <arch/arm/imx/cpuinfo.h>
#include <drivers/common/memory.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <util/log.h>

EMBOX_UNIT_INIT(imx_cpuinfo_init);

/* Chip identifaction data is stored in USB registers, so we map and use it */

#define USB_ANALOG_BASE OPTION_GET(NUMBER, usb_analog_base)

#define REV_REG (USB_ANALOG_BASE + 0x260)

static uint32_t _imx_revision(void) {
	return REG32_LOAD(REV_REG);
}

const char *_imx_type(uint32_t id) {
	switch (id) {
	case MXC_CPU_MX7S:
		return "7S";
	case MXC_CPU_MX7D:
		return "7D";
	case MXC_CPU_MX6QP:
		return "6QP";
	case MXC_CPU_MX6DP:
		return "6DP";
	case MXC_CPU_MX6Q:
		return "6Q";
	case MXC_CPU_MX6D:
		return "6D";
	case MXC_CPU_MX6DL:
		return "6DL";
	case MXC_CPU_MX6SOLO:
		return "6SOLO";
	case MXC_CPU_MX6SL:
		return "6SL";
	case MXC_CPU_MX6SLL:
		return "6SLL";
	case MXC_CPU_MX6SX:
		return "6SX";
	case MXC_CPU_MX6UL:
		return "6UL";
	case MXC_CPU_MX6ULL:
		return "6ULL";
	case MXC_CPU_MX51:
		return "51";
	case MXC_CPU_MX53:
		return "53";
	default:
		return "??";
	}
}

void print_imx_cpuinfo() {
	uint32_t rev;

	rev = _imx_revision();

	log_info("imx: CPU identification:");
	log_info("imx: i.MX%s", _imx_type((rev >> 16) & 0xFF));
	log_info("imx: Revision: %" PRIu32 ".%" PRIu32, (rev >> 8) & 0xFF,
	    rev & 0xFF);
}

static int imx_cpuinfo_init(void) {
	print_imx_cpuinfo();
	return 0;
}

PERIPH_MEMORY_DEFINE(imx_usb, USB_ANALOG_BASE, 0x300);
