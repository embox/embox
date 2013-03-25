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

#include <kernel/thread/sync/cond.h>

struct pty {
	struct tty tty;

	struct cond i_cond; /* master write() */
	struct cond o_cond; /* master read() */
};

#endif /* DRIVERS_PTY_H_ */
