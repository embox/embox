/**
 * @file
 * @brief
 *
 * @date 25.03.13
 * @author Eldar Abusalimov
 */

#ifndef DRIVERS_PTY_H_
#define DRIVERS_PTY_H_

#include <stddef.h>
#include <stdint.h>

#include <drivers/tty.h>

struct pty {
	struct tty tty; /* slave side */
	struct idesc *master;
};

#define pty_from_tty(t) member_cast_out(t, struct pty, tty)
#define pty_to_tty(p)   member_cast_in(p, tty)

struct idesc;
extern struct pty *pty_init(struct pty *p, struct idesc *master, struct idesc *slave);
extern size_t pty_read(struct pty *, struct idesc *idesc, char *, size_t);
extern size_t pty_write(struct pty *, const char *, size_t);

#endif /* DRIVERS_PTY_H_ */
