/**
 * @file
 *
 * @date 12.09.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <string.h>
#include <stdio.h>
#include <util/math.h>
#include <fs/fs.h>
#include <fs/file.h>
#include <fs/node.h>
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

static void *_open(const char *fname, const char *mode) {
	return NULL;
}

static int _close(void *file) {
	return 0;
}

static size_t _read(void *buf, size_t size, size_t count, void *file) {
	char *ch_buf = (char *) buf;

	struct tty_buf *tty = (struct tty_buf *) ((node_t *) file)->file_info;

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
	struct tty_buf *tty = (struct tty_buf *) ((node_t *) file)->file_info;
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
	struct tty_buf *tty = (struct tty_buf *) ((node_t *) file)->file_info;

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
	FILE *file = (FILE *) p->file;
	reopen(0, file);
	reopen(1, file);
	reopen(2, file);
	p->run();
	return NULL;
}

static int _ioctl(void *file, int request, va_list args) {
	struct tty_buf *tty = (struct tty_buf *) ((node_t *) file)->file_info;
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
	tty->file_op.fread = _canon_read;
	tty->file_op.fclose = _close;
	tty->file_op.fwrite = _write;
	tty->file_op.fopen = _open;
	tty->file_op.ioctl = _ioctl;

	tty->inp_begin = tty->inp_end = tty->inp_len = 0;

	tty->canon_pos = tty->canon_left = 0;
	tty->canonical = 1;

}

extern file_system_driver_t *devfs_get_fs(void);

void tty_ng_manager(int count, void (*init)(struct tty_buf *tty), void (*run)(void)) {
	struct thread *thds[count];
	struct tty_buf ttys[count];
	node_t nodes[count];
	struct param params[count];
	char ch;
	char nm[] = "0";

	for (int i = 0; i < count; i++) {
		init(&ttys[i]);
		tty_init(&ttys[i]);
		nodes[i].fs_type = devfs_get_fs();
		nodes[i].file_info = (void *) &ttys[i].file_op;
		nodes[i].unchar = EOF;
		strcpy((char *) nodes[i].name, nm);
		nm[0]++;
		params[i].file = (FILE *) &nodes[i];
		params[i].run = run;
		thread_create(&thds[i], THREAD_FLAG_IN_NEW_TASK, thread_handler, &params[i]);
	}

	current_tty = &ttys[0];
	current_tty->make_active(current_tty);

	while (1) {
		sleep(0);
		read(0, &ch, 1);
		if (ch == '`') {
			read(0, &ch, 1);
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

