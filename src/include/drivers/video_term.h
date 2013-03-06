/**
 * @file
 * @brief
 *
 * @date 08.02.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_VIDEO_VTERM_H_
#define DRIVERS_VIDEO_VTERM_H_

#include <stdint.h>
#include <termios.h>
#include <drivers/vtesc.h>

struct vterm;

struct vterm_ops {
	void (*init)(struct vterm *t);
	void (*cursor)(struct vterm *t, unsigned short x, unsigned short y);
	void (*putc)(struct vterm *t, char ch, unsigned short x, unsigned short y);
	void (*clear_strip)(struct vterm *t, short row, unsigned short count);
	void (*copy_rows)(struct vterm *t, unsigned short to, unsigned short from, short nrows);
	void (*clear)(struct vterm *t, unsigned short x, unsigned short y,
			unsigned short width, unsigned short height);
	void (*scroll)(struct vterm *t, short delta);
};

struct vterm {
	unsigned short cur_x;
	unsigned short cur_y;
	unsigned short back_cx, back_cy;
	unsigned short width;
	unsigned short height;

	const struct vterm_ops *ops;
	void *data;

	struct termios termios;
	struct vtesc_executor executor;
};

extern void vterm_init(struct vterm *t, unsigned short width, unsigned short height,
		const struct vterm_ops *ops, void *data);
extern void vterm_putc(struct vterm *t, char ch);

/* tty ioctls */
#define TTY_IOCTL_GETATTR  0x1
#define TTY_IOCTL_SETATTR  0x2
#define TTY_IOCTL_SETBAUD  0x3


#endif /* DRIVERS_VIDEO_VTERM_H_ */
