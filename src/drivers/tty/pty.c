/**
 * @file
 * @brief Pseudo TTY master-side and slave driver.
 *
 * @date 25.03.13
 * @author Eldar Abusalimov
 */

#include <drivers/pty.h>

static void pty_out_wake(struct tty *t) {
	// struct pty *p = pty_from_tty(t);
}

const struct tty_ops pty_ops = {
	.out_wake = pty_out_wake,
};

struct pty *pty_init(struct pty *p) {
	tty_init(pty_to_tty(p), &pty_ops);

	return p;
}

