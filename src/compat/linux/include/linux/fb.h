/**
 * @file
 *
 * @date Jun 30, 2017
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_LINUX_INCLUDE_LINUX_FB_H_
#define SRC_COMPAT_LINUX_INCLUDE_LINUX_FB_H_

#include <drivers/video/fb.h>

/* ioctls
   0x46 is 'F' */
#define FBIOGET_VSCREENINFO	0x4600
#define FBIOPUT_VSCREENINFO	0x4601
#define FBIOGET_FSCREENINFO	0x4602

#endif /* SRC_COMPAT_LINUX_INCLUDE_LINUX_FB_H_ */
