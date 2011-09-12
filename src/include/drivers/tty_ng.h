/**
 * @file
 * @brief interface for tty new generation
 *
 * @date 12.09.11
 * @author Anton Kozlov
 */

#ifndef DRIVERS_TTY_NG_H_
#define DRIVERS_TTY_NG_H_

#define TTY_INP_Q_LEN 32

#include <kernel/thread/sync/mutex.h>

struct tty_buf {
	file_operations_t file_op; /**< must be first */
	int id; /**< id, used for implementation special purpose */
	/* output */
	char *out_buf;/**< output store for refresh on make_active */

	char inp[TTY_INP_Q_LEN];
	struct mutex *inp_mutex;
	int inp_len;
	int inp_begin;
	int inp_end;

	void (*putc)(struct tty_buf *tty, char ch);
	void (*make_active)(struct tty_buf *tty);
	void (*make_inactive)(struct tty_buf *tty);

} __attribute__((packed));

extern void tty_ng_manager(int count, void (*init)(struct tty_buf *tty), void (*run)(void));

#endif

