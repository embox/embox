/**
 * @file fps.h
 * @brief Simple library for counting frames per second
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 11.03.2019
 */

#ifndef LIB_FPS_H_
#define LIB_FPS_H_

#include <drivers/video/fb.h>

/* Refer to fps.c for docs */
extern void fps_set_format(const char *format);
extern void fps_print(struct fb_info *fb);
extern void fps_set_base_frame(struct fb_info *fb, void *base);
extern void fps_set_back_frame(struct fb_info *fb, void *base);
extern void *fps_enable_swap(struct fb_info *fb);
extern int fps_swap(struct fb_info *fb);
extern void *fps_current_frame(struct fb_info *fb);

#endif /* LIB_FPS_H_ */
