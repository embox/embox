/**
 * @file
 * @brief
 *
 * @date   11.08.2020
 * @author Alexander Kalmuk
 */

#include <drivers/video/fb.h>
#include "rawfb.h"

void rawfb_init(struct rawfb_fb_info *rfb) {
}

void rawfb_clear_screen(struct rawfb_fb_info *rfb) {
	int i;
	uint32_t *p = rfb->fb_buf[rfb->fb_buf_idx];

	for (i = 0; i < rfb->width * rfb->height; i++) {
		p[i] = rfb->clear_color;
	}
}

void rawfb_swap_buffers(struct rawfb_fb_info *rfb) {
	if (rfb->fb_info->var.fmt != BGRA8888) {
		pix_fmt_convert(rfb->fb_buf[rfb->fb_buf_idx],
						rfb->fb_info->screen_base, rfb->width * rfb->height,
						BGRA8888, rfb->fb_info->var.fmt);
	} else {
		memcpy(rfb->fb_info->screen_base, rfb->fb_buf[rfb->fb_buf_idx],
			rfb->width * rfb->height * rfb->bpp);
	}
}
