/**
 * @file
 * @brief
 *
 * @date 08.02.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_TTY_H_
#define DRIVERS_TTY_H_

#include <stddef.h>
#include <stdint.h>
#include <termios.h>

#include <framework/mod/options.h>
#include <module/embox/tty/tty.h>

#define TTY_QUEUE_SZ \
	OPTION_MODULE_GET(embox__driver__tty__tty, NUMBER, queue_sz)

/* tty ioctls */
#define TTY_IOCTL_GETATTR  0x1
#define TTY_IOCTL_SETATTR  0x2
#define TTY_IOCTL_SETBAUD  0x3

struct tty;

struct tty_queue {
	char buff[TTY_QUEUE_SZ];
	char *head, *tail;
};

struct tty {
	const struct tty_ops *ops;
	struct termios termios;
	struct tty_queue rx, tx;
};

struct tty_ops {
	void (*setup)(struct tty *t, struct termios *termios);
	void (*start_tx)(struct tty *t, struct tty_queue *txq);
};

static inline size_t tty_queue_count(struct tty_queue *tq) {
	return 0; // TODO
}

extern void tty_init(struct tty *t);
extern void tty_scroll(struct tty *t, int32_t delta);
extern void tty_clear(struct tty *t);
extern void tty_cursor(struct tty *t);
extern void tty_putc(struct tty *t, char ch);


#endif /* DRIVERS_TTY_H_ */
