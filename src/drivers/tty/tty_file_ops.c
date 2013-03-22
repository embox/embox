/**
 * @file
 *
 * @date Mar 6, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <string.h>
#include <kernel/irq.h>

#include <drivers/tty.h>
#include <kernel/thread/sched.h>

size_t tty_write(struct tty *tty, char *buff, size_t size) {
	return 0;
}
