/**
 * @file
 *
 * @date 30.03.2017
 * @author Anton Bondarev
 */
#include <util/log.h>
#include <stdint.h>

#include <drivers/common/memory.h>
#include <drivers/clk/ccm_imx6.h>
#include <hal/reg.h>

#include <embox/unit.h>

#include "dw_hdmi.h"

#include <framework/mod/options.h>

#define DW_HDMI_BASE          OPTION_GET(NUMBER, base_addr)

EMBOX_UNIT_INIT(dw_hdmi_init);

struct dw_hdmi {
	uint32_t regs;
	uint8_t (*read)(struct dw_hdmi *hdmi, int offset);
	void (*write)(struct dw_hdmi *hdmi, uint8_t val, int offset) ;
};

static struct dw_hdmi dw_hdmi;

static void dw_hdmi_writeb(struct dw_hdmi *hdmi, uint8_t val, int offset)
{
	REG8_STORE(hdmi->regs + offset, val);
}

static uint8_t dw_hdmi_readb(struct dw_hdmi *hdmi, int offset)
{
	return REG8_LOAD(hdmi->regs + offset);
}

static inline void hdmi_writeb(struct dw_hdmi *hdmi, uint8_t val, int offset)
{
	hdmi->write(hdmi, val, offset);
}

static inline uint8_t hdmi_readb(struct dw_hdmi *hdmi, int offset)
{
	return hdmi->read(hdmi, offset);
}

static int dw_hdmi_init(void) {
	struct dw_hdmi *hdmi;
	hdmi = &dw_hdmi;

	dw_hdmi.regs = DW_HDMI_BASE;
	dw_hdmi.write = dw_hdmi_writeb;
	dw_hdmi.read = dw_hdmi_readb;

	if (-1 == clk_enable("isfr")) {
		log_error("Clk 'isfr' didn't find");
	}
	if (-1 == clk_enable("iahb")) {
		log_error("Clk 'iahb' didn't find");
	}
	log_debug("Detected HDMI controller 0x%x:0x%x:0x%x:0x%x\n",
		 hdmi_readb(hdmi, HDMI_DESIGN_ID),
		 hdmi_readb(hdmi, HDMI_REVISION_ID),
		 hdmi_readb(hdmi, HDMI_PRODUCT_ID0),
		 hdmi_readb(hdmi, HDMI_PRODUCT_ID1));
	return 0;
}

PERIPH_MEMORY_DEFINE(dw_hdmi, DW_HDMI_BASE, 0x8000);
