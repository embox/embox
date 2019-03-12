/**
 * @file video.h
 * @brief Helper functions to work with frame buffers
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 30.10.2018
 */

#ifndef VIDEO_FB_OVERLAY_H_
#define VIDEO_FB_OVERLAY_H_

#include <drivers/video/fb.h>

extern void fb_overlay_init(struct fb_info *fbi, void *base);
extern void fb_overlay_put_char(int x, int y, char ch);
extern void fb_overlay_put_string(int x, int y, char *str);
extern void fb_overlay_put_line(int x, int y, char *str);

#endif /* VIDEO_FB_OVERLAY_H_ */
