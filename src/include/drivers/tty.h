/**
 * @file
 * @brief
 *
 * @date 08.02.13
 * @author Eldar Abusalimov
 */

#ifndef DRIVERS_TTY_H_
#define DRIVERS_TTY_H_

#include <stddef.h>
#include <stdint.h>
#include <termios.h>

#include <kernel/irq_lock.h>
#include <kernel/event.h>
// #include <kernel/thread/sync/mutex.h>
#include <kernel/work.h>
#include <util/ring.h>

#include <framework/mod/options.h>
#include <module/embox/driver/tty/tty.h>

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

#define TTY_IO_BUFF_SZ \
	OPTION_MODULE_GET(embox__driver__tty__tty, NUMBER, io_buff_sz)

#define TTY_RX_BUFF_SZ \
	OPTION_MODULE_GET(embox__driver__tty__tty, NUMBER, rx_buff_sz)

struct tty_ops;

struct tty {
	const struct tty_ops *ops;

	struct termios    termios;

	// struct mutex      lock; /* serialize operations on tty, also used in pty */

	struct work       rx_work;
	struct ring       rx_ring;
	uint16_t          rx_buff[TTY_RX_BUFF_SZ]; /* flag (MSB) and char (LSB) */

	struct event      i_event;
	struct ring       i_ring;
	char              i_buff[TTY_IO_BUFF_SZ];
	struct ring       i_canon_ring; /* cooked range inside the ring buffer */

	struct event      o_event;
	struct ring       o_ring;
	char              o_buff[TTY_IO_BUFF_SZ];
	pid_t			  pgrp; /* process group (TODO: lonely process now) */

	//TODO this must be in file
	uint32_t file_flags;
};

struct tty_ops {
	void (*setup)(struct tty *, struct termios *);
	void (*out_wake)(struct tty *);
};

extern struct tty *tty_init(struct tty *, const struct tty_ops *);

extern size_t tty_read(struct tty *, char *, size_t);
extern size_t tty_write(struct tty *, const char *, size_t);
extern int tty_ioctl(struct tty *, int, void *);

/* TTY RX queue operations, must be explicitly IRQ-protected. */

extern int tty_rx_enqueue(struct tty *, char ch, unsigned char flag);
extern int tty_rx_dequeue(struct tty *);

/* These functions can be called from IRQ context. */

extern void tty_post_rx(struct tty *);

static inline int tty_rx_putc(struct tty *t, char ch, unsigned char flag) {
	int rc = IRQ_LOCKED_DO(tty_rx_enqueue(t, ch, flag));
	if (rc != -1) {
		tty_post_rx(t);
	}
	return rc;
}

static inline int tty_out_getc(struct tty *t) {
	char ch;
	if (!ring_read_all_into(&t->o_ring, t->o_buff, TTY_IO_BUFF_SZ, &ch, 1))
		return -1;
	return (int) ch;
}

struct kfile_operations;
extern int tty_register(const char *name, void *dev, const struct kfile_operations *file_ops);

#endif /* DRIVERS_TTY_H_ */
