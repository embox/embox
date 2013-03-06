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
	void (*cursor)(struct vterm *t, uint32_t x, uint32_t y);
	void (*putc)(struct vterm *t, char ch, uint32_t x, uint32_t y);
	void (*clear)(struct vterm *t, uint32_t x, uint32_t y,
			uint32_t width, uint32_t height);
	void (*move)(struct vterm *t, uint32_t sx, uint32_t sy, uint32_t width,
			uint32_t height, uint32_t dx, uint32_t dy);
	void (*scroll)(struct vterm *t, int32_t delta);
};

struct vterm {
	uint32_t cur_x;
	uint32_t cur_y;
	uint32_t back_cx, back_cy;
	uint32_t width;
	uint32_t height;

	const struct vterm_ops *ops;
	void *data;

	struct termios termios;
	struct vtesc_executor executor;
};

extern void vterm_init(struct vterm *t, uint32_t width, uint32_t height,
		const struct vterm_ops *ops, void *data);
extern void vterm_putc(struct vterm *t, char ch);

/* tty ioctls */
#define TTY_IOCTL_GETATTR  0x1
#define TTY_IOCTL_SETATTR  0x2
#define TTY_IOCTL_SETBAUD  0x3


#endif /* DRIVERS_VIDEO_VTERM_H_ */
