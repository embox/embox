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

#include <kernel/thread/event.h>

#include <framework/mod/options.h>
#include <module/embox/driver/tty/tty.h>

#define TTY_QUEUE_SZ \
	OPTION_MODULE_GET(embox__driver__tty__tty, NUMBER, queue_sz)

/* tty ioctls */
#define TTY_IOCTL_GETATTR  0x1
#define TTY_IOCTL_SETATTR  0x2
#define TTY_IOCTL_SETBAUD  0x3

/* Defaults */

#define TTY_TERMIOS_CC_INIT \
	{ \
		[VEOF]   = __TTY_CTRL('d'),  \
		[VEOL]   = ((cc_t) ~0), /* undef */ \
		[VERASE] = 0177,             \
		[VINTR]  = __TTY_CTRL('c'),  \
		[VKILL]  = __TTY_CTRL('u'),  \
		[VMIN]   = 1,                \
		[VQUIT]  = __TTY_CTRL('\\'), \
		[VTIME]  = 0,                \
		[VSUSP]  = __TTY_CTRL('z'),  \
		[VSTART] = __TTY_CTRL('q'),  \
		[VSTOP]  = __TTY_CTRL('s'),  \
	}

#define __TTY_CTRL(ch)  (cc_t) ((ch) & 0x1f)

#define TTY_TERMIOS_IFLAG_INIT  (tcflag_t) (BRKINT | ICRNL | IXON)
#define TTY_TERMIOS_OFLAG_INIT  (tcflag_t) (OPOST | ONLCR | OXTABS)
#define TTY_TERMIOS_CFLAG_INIT  (tcflag_t) (CREAD | CS8 | HUPCL)
#define TTY_TERMIOS_LFLAG_INIT  (tcflag_t) (ICANON | ISIG | \
			ECHO | ECHOE | ECHOK | ECHONL)

struct tty_ops;

struct tty_queue {
	char   buff[TTY_QUEUE_SZ];
	size_t offset;
	size_t count;
};

struct tty {
	struct tty_ops   *ops;
	struct termios    termios;
	struct tty_queue  rx_queue, tx_queue;
	struct event      rx_event, tx_event;
};

struct tty_ops {
	void (*setup)(struct tty *, struct termios *);
	void (*tx_start)(struct tty *);
};

extern struct tty *tty_init(struct tty *, struct tty_ops *);

extern size_t tty_read(struct tty *, char *, size_t);
extern size_t tty_write(struct tty *, char *, size_t);
extern int tty_ioctl(struct tty *, int, void *);

/* These functions can be called from IRQ context. */

extern void tty_tx_done(struct tty *);
extern void tty_rx_char(struct tty *, char);

/* TTY RX/TX queues operations. */

extern struct tty_queue *tty_queue_init(struct tty_queue *);
extern int tty_enqueue(struct tty_queue *, char);
extern int tty_dequeue(struct tty_queue *);

struct kfile_operations;
extern int tty_register(const char *name, void *dev, const struct kfile_operations *file_ops);

#endif /* DRIVERS_TTY_H_ */
