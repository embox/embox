/**
 * @file
 * @brief
 *
 * @date 08.02.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_VIDEO_TTY_H_
#define DRIVERS_VIDEO_TTY_H_

#include <types.h>

struct tty;

struct tty_ops {
	void (*init)(struct tty *t);
	void (*cursor)(struct tty *t, uint32_t x, uint32_t y);
	void (*putc)(struct tty *t, char ch, uint32_t x, uint32_t y);
	void (*clear)(struct tty *t, uint32_t x, uint32_t y,
			uint32_t width, uint32_t height);
	void (*move)(struct tty *t, uint32_t sx, uint32_t sy, uint32_t width,
			uint32_t height, uint32_t dx, uint32_t dy);
};

struct tty {
	uint32_t cur_x;
	uint32_t cur_y;
	uint32_t width;
	uint32_t height;
	const struct tty_ops *ops;
	void *data;

	int esc_state;
	int esc_args[5];
	int esc_args_count;
};

extern void tty_init(struct tty *t, uint32_t width, uint32_t height,
		const struct tty_ops *ops, void *data);
extern void tty_scroll(struct tty *t, int32_t delta);
extern void tty_clear(struct tty *t);
extern void tty_cursor(struct tty *t);
extern void tty_putc(struct tty *t, char ch);

#endif /* DRIVERS_VIDEO_TTY_H_ */
