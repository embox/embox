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
#include <drivers/vtesc.h>

struct vterm_video;

struct vterm_video_ops {
	void (*init)(struct vterm_video *t);
	void (*cursor)(struct vterm_video *t, unsigned short x, unsigned short y);
	void (*putc)( struct vterm_video *t, char ch, unsigned short x, unsigned short y);
	void (*clear_rows)(struct vterm_video *t, short row, unsigned short count);
	void (*copy_rows)(struct vterm_video *t, unsigned short to, unsigned short from, short nrows);
};

struct vterm_video {
	const struct vterm_video_ops *ops;
	unsigned short width;
	unsigned short height;
};

struct vterm {
	unsigned short cur_x;
	unsigned short cur_y;

	struct vterm_video *video;
	struct input_dev *indev;

	struct termios termios;
	struct tty tty;
	struct vtesc_executor executor;
};

extern void vterm_init(struct vterm *t, struct vterm_video *video, struct input_dev *indev);
extern void vterm_putc(struct vterm *t, char ch);
extern void vterm_open_indev(struct vterm *t, const char *name);

#endif /* DRIVERS_VIDEO_VTERM_H_ */
