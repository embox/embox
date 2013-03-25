/**
 * @file
 * @brief Pseudo TTY master-side and slave driver.
 *
 * @date 25.03.13
 * @author Eldar Abusalimov
 */

#include <drivers/pty.h>

static void pty_tx_char(struct tty *t, char ch) {
	// struct pty *p = pty_from_tty(t);
}

const struct tty_ops pty_ops = {
	.tx_char = pty_tx_char,
};

struct pty *pty_init(struct pty *p) {
	tty_init(pty_to_tty(p), &pty_ops);

	return p;
}

