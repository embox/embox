/**
 * @file
 * @brief
 *
 * @date 08.02.13
 * @author Ilia Vaprol, Vita Loginova
 */

#ifndef DRIVERS_VIDEO_VTERM_H_
#define DRIVERS_VIDEO_VTERM_H_

#include <stdint.h>
#include <termios.h>
#include <drivers/tty.h>
#include <drivers/vterm_video.h>

struct vterm {

	struct vterm_video *video;
	struct input_dev *indev;

	struct tty tty;
};

extern void vterm_init(struct vterm *t, struct vterm_video *video, struct input_dev *indev);
extern void vterm_open_indev(struct vterm *t, const char *name);

static inline void vterm_putc(struct vterm *t, char ch) {
	vterm_video_putc(t->video, ch);
}

#endif /* DRIVERS_VIDEO_VTERM_H_ */
