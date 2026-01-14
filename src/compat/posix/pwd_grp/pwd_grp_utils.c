/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.02.2013
 */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#include <pwd.h>
#include <grp.h>
#include <shadow.h>
#include <util/binalign.h>

#include <pwd_db.h>


int pwd_grp_read_field(FILE *stream, char **buf, size_t *buflen, char **field,
		int delim) {
	int ch = fgetc(stream);

	*field = *buf;

	while (ch != delim) {
		if (ch == EOF) {
			if (ferror(stream))
				return EIO;
			return EOF;
		}

		if (ch == ':' || ch == '\n') {
			return EIO;
		}

		if (*buflen <= 0) {
			return ERANGE;
		}

		*((*buf)++) = (char) ch;
		(*buflen)--;

		ch = fgetc(stream);
	}

	if (*buflen <= 0) {
		return ERANGE;
	}

	*((*buf)++) = '\0';
	(*buflen)--;

	return 0;
}

int pwd_grp_read_int_field(FILE *stream, const char *format, void *field, int delim) {
	int val;
	int ret;

	sscanf("0", format, field);

	ret = fscanf(stream, format, field);
	if (0 > ret && ferror(stream)) {
		return EIO;
	}

	if (delim != (val = fgetc(stream))) {
		/* fscanf can eat blank charactes, so if it used as delim, here we beleive it
 		 * was on place, but stolen. Otherwise, report inproper stream format
		 */
		if (isspace(delim)) {
			ungetc(val, stream);
		} else {
			return EIO;
		}
	}

	return 0;
}
