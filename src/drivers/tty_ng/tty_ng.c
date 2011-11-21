/**
 * @file
 *
 * @date 12.09.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <util/math.h>
#include <fs/fs.h>
#include <fs/file.h>
#include <fs/ioctl.h>
#include <fs/file_desc.h>
#include <kernel/thread/api.h>
#include <lib/linenoise.h>
#include <drivers/tty_ng.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(tty_ng_manager_init);

struct param {
	FILE *file;
	void (*run)(void);
};

static struct tty_buf *current_tty;

static size_t _read(void *buf, size_t size, size_t count, void *file) {
	char *ch_buf = (char *) buf;

	struct tty_buf *tty = (struct tty_buf *) ((struct file_desc *) file)->ops;

	int i = count * size;

//	mutex_lock(tty->inp_mutex);

	while (i--) {
		while (tty->inp_len == 0) {
//			mutex_unlock(tty->inp_mutex);
			sleep(0);
//			mutex_lock(tty->inp_mutex);
		}

		tty->inp_len -= 1;
		*(ch_buf++) = tty->inp[tty->inp_begin];
		tty->inp_begin = (tty->inp_begin + 1) % TTY_INP_Q_LEN;

//		mutex_unlock(tty->inp_mutex);
	}
	return count * size;
}

static size_t _canon_read(void *buf, size_t size, size_t count, void *file) {
	struct tty_buf *tty = (struct tty_buf *) ((struct file_desc *) file)->ops;
	if (tty->canonical) {
		int to_write;
		if (tty->canon_left == 0) {
			tty->canon_left = linenoise("", tty->canon_inp, TTY_CANON_INP_LEN, NULL, NULL);
			tty->canon_pos = 0;
		}
		to_write = min(count * size, tty->canon_left);
		memcpy(buf, tty->canon_inp + tty->canon_pos, to_write);
		tty->canon_left -= to_write;
		tty->canon_pos += to_write;
		return to_write;
	}
	return _read(buf, size, count, file);
}

static size_t _write(const void *buff, size_t size, size_t count, void *file) {
	size_t cnt = 0;
	char *b = (char*) buff;
	struct tty_buf *tty = (struct tty_buf *) ((struct file_desc *) file)->ops;

	while (cnt != count * size) {
		tty->putc(tty, b[cnt++]);
	}
	return count * size;
}

static void tty_putc_buf(struct tty_buf *tty, char ch) {

//	mutex_lock(tty->inp_mutex);

	while (tty->inp_len >= TTY_INP_Q_LEN) {
//		mutex_unlock(tty->inp_mutex);
		sleep(0);
//		mutex_lock(tty->inp_mutex);
	}

	tty->inp[tty->inp_end] = ch;
	tty->inp_end = (tty->inp_end + 1) % TTY_INP_Q_LEN;
	tty->inp_len += 1;

//	mutex_unlock(tty->inp_mutex);
}

static void *thread_handler(void* args) {
	struct param *p = (struct param *) args;
#if 0
	FILE *file = (FILE *) p->file;
	freopen(stdin, file);
	freopen(stdout, file);
	freopen(stderr, file);
#endif
	p->run();
	return NULL;
}

static int _ioctl(void *file, int request, va_list args) {
	struct tty_buf *tty = (struct tty_buf *) ((struct file_desc *) file)->ops;
	switch (request) {
		case TTY_IOCTL_SET_RAW:
			tty->canonical = 0;
			break;
		case TTY_IOCTL_SET_CANONICAL:
			tty->canonical = 1;
			break;
		case TTY_IOCTL_REQUEST_MODE:
			return tty->canonical;
		default:
			break;
	}
	return 0;
}

static void tty_init(struct tty_buf *tty) {
	tty->file_op.fopen = NULL;
	tty->file_op.fclose = NULL;
	tty->file_op.fread = _canon_read;
	tty->file_op.fwrite = _write;
	tty->file_op.ioctl = _ioctl;

	tty->inp_begin = tty->inp_end = tty->inp_len = 0;

	tty->canon_pos = tty->canon_left = 0;
	tty->canonical = 1;

}

extern fs_drv_t *devfs_get_fs(void);

void tty_ng_manager(int count, void (*init)(struct tty_buf *tty), void (*run)(void)) {
	struct thread *thds[count];
	struct tty_buf ttys[count];
	struct file_desc descs[count];
	struct param params[count];
	char ch;
	char nm[] = "0";

	for (int i = 0; i < count; i++) {
		init(&ttys[i]);
		tty_init(&ttys[i]);
		descs[i].ops = &ttys[i].file_op;
		descs[i].has_ungetc = 0;
		nm[0]++;
		params[i].file = (FILE *) &descs[i];
		params[i].run = run;
		thread_create(&thds[i], THREAD_FLAG_IN_NEW_TASK, thread_handler, &params[i]);
	}

	current_tty = &ttys[0];
	current_tty->make_active(current_tty);

	fioctl(stdin, O_NONBLOCK_SET, NULL);
	while (1) {
		while (-EAGAIN == read(0, &ch, 1)) {
			sleep(0);
		}
		if (ch == '`') {
			while (-EAGAIN == read(0, &ch, 1)) {
				sleep(0);
			}
			if ('0' <= ch && ch <= '9') {
				int n = ch - '0';
				if (n < count) {
					current_tty->make_inactive(current_tty);
					current_tty = &ttys[n];
					current_tty->make_active(current_tty);
				}
				continue;
			}
		}

		tty_putc_buf(current_tty, ch);
	}
}

static int tty_ng_manager_init(void) {
	return 0;
}

