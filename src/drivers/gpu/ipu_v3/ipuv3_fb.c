/**
 * @file ipuv3_fb.c
 * @brief Framebuffer for IPU (no regs/cloks/etc here)
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 05.10.2017
 */
#include <util/log.h>

#include <string.h>

#include <embox/unit.h>
#include <drivers/video/fb.h>
#include <kernel/irq.h>

#include "ipu_regs.h"
#include "ipu_priv.h"

#define IPU_MAX_WIDTH  OPTION_GET(NUMBER, xres)
#define IPU_MAX_HEIGHT OPTION_GET(NUMBER, yres)

/* TODO handle this other way! */
#define UPPER_MARGIN OPTION_GET(NUMBER, upper_margin)
#define LOWER_MARGIN OPTION_GET(NUMBER, lower_margin)
#define LEFT_MARGIN  OPTION_GET(NUMBER, left_margin)
#define RIGHT_MARGIN OPTION_GET(NUMBER, right_margin)
#define VSYNC_LEN    OPTION_GET(NUMBER, vsync_len)
#define HSYNC_LEN    OPTION_GET(NUMBER, hsync_len)

static uint16_t ipu_fb[IPU_MAX_WIDTH * IPU_MAX_HEIGHT]
			__attribute__ ((aligned (0x8)));

struct mxcfb_info {
	ipu_channel_t ipu_ch;
	int ipu_id;
	int ipu_di;
	void *ipu;

	struct fb_info *fbi;
};

static struct mxcfb_info mxc_fbi;

static int mxcfb_set_par(struct fb_info *fbi, const struct fb_var_screeninfo *var) {
	if (mxc_fbi.ipu_ch != MEM_BG_SYNC)
		return 0;

	ipu_disable_channel(mxc_fbi.ipu, mxc_fbi.ipu_ch, 0);
	ipu_uninit_channel(mxc_fbi.ipu, mxc_fbi.ipu_ch);

	fbi->screen_base = (void*) ipu_fb;
	fbi->screen_size = IPU_MAX_WIDTH * IPU_MAX_HEIGHT * 2;

	memset((char *)fbi->screen_base, 0x00, fbi->screen_size);

	ipu_init_channel(mxc_fbi.ipu, mxc_fbi.ipu_ch, 0);

	fbi->var = (struct fb_var_screeninfo) {
		.xres           = IPU_MAX_WIDTH,
		.yres           = IPU_MAX_HEIGHT,
		.bits_per_pixel = 16, /* Always use R5G6B5 */
		.upper_margin   = UPPER_MARGIN,
		.lower_margin   = LOWER_MARGIN,
		.left_margin    = LEFT_MARGIN,
		.right_margin   = RIGHT_MARGIN,
		.hsync_len      = HSYNC_LEN,
		.vsync_len      = VSYNC_LEN,
	};

	ipu_init_sync_panel(mxc_fbi.ipu, mxc_fbi.ipu_di,
				fbi,
				IPU_PIX_FMT_RGB666);

	ipu_init_channel_buffer(mxc_fbi.ipu,
					 mxc_fbi.ipu_ch, IPU_INPUT_BUFFER,
					 IPU_PIX_FMT_RGB565,
					 fbi->var.xres, fbi->var.yres,
					 fbi->var.xres * fbi->var.bits_per_pixel / 8,
					 (dma_addr_t) fbi->screen_base);

	ipu_enable_channel(mxc_fbi.ipu, mxc_fbi.ipu_ch);

	return 0;
}

static int mxcfb_set_base(struct fb_info *fbi, void *new_base) {
	ipu_init_channel_buffer(mxc_fbi.ipu,
					 mxc_fbi.ipu_ch, IPU_INPUT_BUFFER,
					 IPU_PIX_FMT_RGB565,
					 fbi->var.xres, fbi->var.yres,
					 fbi->var.xres * fbi->var.bits_per_pixel / 8,
					 (uint32_t) new_base);
	return 0;
}

static struct fb_ops mxcfb_ops = {
	.fb_set_var  = mxcfb_set_par,
	.fb_set_base = mxcfb_set_base,
};

extern void dcache_flush(const void *p, size_t size);
static irq_return_t mxcfb_irq_handler(unsigned int irq, void *data) {
	struct ipu_soc *ipu = mxc_fbi.ipu;
	int i;
	uint32_t int_stat, int_ctrl;
	const int int_reg[] = { 1, 2, 3, 4, 11, 12, 13, 14, 15, 0 };

	dcache_flush(ipu_fb, sizeof(ipu_fb) * 2);

	for (i = 0; int_reg[i] != 0; i++) {
		int_stat = ipu_cm_read(ipu, IPU_INT_STAT(int_reg[i]));
		int_ctrl = ipu_cm_read(ipu, IPU_INT_CTRL(int_reg[i]));
		int_stat &= int_ctrl;
		ipu_cm_write(ipu, int_stat, IPU_INT_STAT(int_reg[i]));
	}

	return IRQ_HANDLED;
}

static int ipu_init(void) {
	ipu_probe();

	mxc_fbi.fbi    = fb_create(&mxcfb_ops,
			              (char *) ipu_fb,
				      2 * IPU_MAX_WIDTH * IPU_MAX_HEIGHT);
	mxc_fbi.ipu    = ipu_get();
	mxc_fbi.ipu_ch = MEM_BG_SYNC;

	irq_attach(IPU1_SYNC_IRQ, mxcfb_irq_handler, 0, NULL, "IPU framebuffer");
	ipu_request_irq(ipu_get(), 23, /*NULL,*/ 0, "", NULL);

	mxcfb_set_par(mxc_fbi.fbi, &mxc_fbi.fbi->var);

	return 0;
}
EMBOX_UNIT_INIT(ipu_init);
