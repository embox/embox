/**
 * @file 
 * @brief 
 *
 * @author  Anton Kozlov 
 * @date    30.06.2015
 */

#ifndef VIDEO_FB_VIDEOMODES_H_
#define VIDEO_FB_VIDEOMODES_H_

#include <stdint.h>

struct video_resbpp;
struct fb_var_screeninfo;

struct fb_videomode {
	const char *name;	/* optional */
	uint32_t refresh;	/* optional */
	uint32_t xres;
	uint32_t yres;
	uint32_t pixclock;
	uint32_t left_margin;
	uint32_t right_margin;
	uint32_t upper_margin;
	uint32_t lower_margin;
	uint32_t hsync_len;
	uint32_t vsync_len;
	uint32_t sync;
	uint32_t vmode;
	uint32_t flag;
};

extern void fb_videomode_to_var(struct fb_var_screeninfo *var,
		const struct fb_videomode *mode);

extern void fb_var_to_videomode(struct fb_videomode *mode,
		const struct fb_var_screeninfo *var);

extern const struct fb_videomode *video_fbmode_by_resbpp(const struct video_resbpp *resbpp);

#endif /* VIDEO_FB_VIDEOMODES_H_ */
