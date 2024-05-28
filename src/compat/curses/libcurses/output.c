/**
 * @file
 * @brief
 *
 * @date 1.09.22
 * @author Aleksey Zhmulin
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "curses_priv.h"

static char output_buf[OUTPUT_BUF_SIZE];
static size_t output_buf_inuse;

int _curs_putc(int ch) {
	if (output_buf_inuse + 1 >= OUTPUT_BUF_SIZE) {
		if (ERR == _curs_flush()) {
			return ERR;
		}
	}

	output_buf[output_buf_inuse++] = ch;

	return OK;
}

int _curs_puts(void *str, size_t len) {
	if (output_buf_inuse + len >= OUTPUT_BUF_SIZE) {
		if ((len >= OUTPUT_BUF_SIZE) || (ERR == _curs_flush())) {
			return ERR;
		}
	}

	memcpy(&output_buf[output_buf_inuse], str, len);

	output_buf_inuse += len;
	_curs_curcol += len;

	return OK;
}

int _curs_flush(void) {
	char *buf;
	size_t want;
	ssize_t have;

	if (SP == NULL) {
		return ERR;
	}

	if (output_buf_inuse == 0) {
		return OK;
	}

	buf = output_buf;
	want = output_buf_inuse;

	while (want) {
		have = write(fileno(SP->ofp), buf, want);
		if (have > 0) {
			want -= (size_t)have;
			buf += have;
		}
		else if ((errno != EINTR) && (errno != EAGAIN)) {
			return ERR;
		}
	}

	output_buf_inuse = 0;

	return OK;
}
