/**
 * @file
 * @brief
 *
 * @date   11.08.2020
 * @author Alexander Kalmuk
 */

#ifndef GRFX_RAWFB_RAWFB_H_
#define GRFX_RAWFB_RAWFB_H_

struct fb_info;

struct rawfb_fb_info {
	struct fb_info *fb_info;
	void *fb_buf[2];
	int fb_buf_idx;
	int width;
	int height;
	int bpp;
	int clear_color;
};

extern void rawfb_init(struct rawfb_fb_info *fbs);
extern void rawfb_swap_buffers(struct rawfb_fb_info *fbs);
extern void rawfb_clear_screen(struct rawfb_fb_info *fbs);

#endif /* GRFX_RAWFB_RAWFB_H_ */
