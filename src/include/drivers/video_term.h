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
#include <drivers/vtesc.h>

struct vterm_video;

struct vterm_ops {
	void (*init)(const struct vterm_video *t);
	void (*cursor)(const struct vterm_video *t, unsigned short x, unsigned short y);
	void (*putc)(const struct vterm_video *t, char ch, unsigned short x, unsigned short y);
	void (*clear_rows)(const struct vterm_video *t, short row, unsigned short count);
	void (*copy_rows)(const struct vterm_video *t, unsigned short to, unsigned short from, short nrows);
};

struct vterm_video {
	const struct vterm_ops *ops;
	unsigned short width;
	unsigned short height;
};

struct vterm {
	unsigned short cur_x;
	unsigned short cur_y;
	unsigned short back_cx, back_cy;

	const struct vterm_video *video;
	struct input_dev *indev;

	struct termios termios;
	struct vtesc_executor executor;
};

extern void vterm_init(struct vterm *t, const struct vterm_video *video, struct input_dev *indev);
extern void vterm_putc(struct vterm *t, char ch);
extern void vterm_open_indev(struct vterm *t, const char *name);

/* tty ioctls */
#define TTY_IOCTL_GETATTR  0x1
#define TTY_IOCTL_SETATTR  0x2
#define TTY_IOCTL_SETBAUD  0x3


#endif /* DRIVERS_VIDEO_VTERM_H_ */
