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

extern void fps_set_format(const char *format);
extern void fps_print(struct fb_info *fb);

#endif /* LIB_FPS_H_ */
