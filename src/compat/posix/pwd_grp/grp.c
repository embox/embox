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

static inline int open_db(const char *db_path, FILE **result) {
	FILE *f;
	if (NULL == (f = fopen(db_path, "r"))) {
		return errno;
	}

	*result = f;
	return 0;
}

extern int pwd_grp_read_field(FILE *stream, char **buf, size_t *buflen, char **field,
		int delim);

extern int pwd_grp_read_int_field(FILE *stream, const char *format, void *field, int delim);

/************  grp.h ***************/
int fgetgrent_r(FILE *fp, struct group *gbuf, char *tbuf,
		size_t buflen, struct group **gbufp) {
	int res;
	char *buf = tbuf;
	size_t buf_len = buflen;
	char *ch, **pmem;

	*gbufp = NULL;

	if (0 != (res = pwd_grp_read_field(fp, &buf, &buf_len, &gbuf->gr_name, ':'))) {
		if (EOF == res)
			return ENOENT;
		return res;
	}

	if (0 != (res = pwd_grp_read_field(fp, &buf, &buf_len, &gbuf->gr_passwd, ':'))) {
		if (EOF == res)
			return ENOENT;
		return res;
	}

	if (0 != (res = pwd_grp_read_int_field(fp, "%hd", &gbuf->gr_gid, ':'))) {
		if (EOF == res)
			return ENOENT;
		return res;
	}

	if (0 != (res = pwd_grp_read_field(fp, &buf, &buf_len, &ch, '\n'))) {
		if (EOF == res)
			return ENOENT;
		return res;
	}

	gbuf->gr_mem = pmem = (char **)binalign_bound((uintptr_t)buf, sizeof(void *));
	buf_len -= (uintptr_t)pmem - (uintptr_t)buf;

	*pmem = ch;

	while (NULL != (ch = strchr(ch, ','))) {

		if (buf_len < sizeof(char *)) {
			return ERANGE;
		}

		buf_len -= sizeof(char *);

		*ch++ = '\0';

		*(++pmem) = ch;

	}

	if (buf_len < sizeof(char *)) {
		return ERANGE;
	}
	*(++pmem) = NULL;

	*gbufp = gbuf;

	return 0;
}

struct group *fgetgrent(FILE *stream) {
	static struct group grp_buf;
	static char buf[64];
	struct group *res;
	int ret;

	if (0 != (ret = fgetgrent_r(stream, &grp_buf, buf, sizeof(buf), &res))) {
		SET_ERRNO(ret);
		return NULL;
	}

	return res;
}

int getgrnam_r(const char *name, struct group *grp,
	char *buf, size_t buflen, struct group **result) {
	int res;
	FILE *file = NULL;

	*result = NULL;

	if (0 != (res = open_db(GROUP_FILE, &file))) {
		return res;
	}

	while (0 == (res = fgetgrent_r(file, grp, buf, buflen, result))) {
		if (0 == strcmp((*result)->gr_name, name)) {
			break;
		}
	}

	fclose(file);
	return res == ENOENT ? 0 : res;
}

struct group * getgrgid(gid_t gid) {
	static struct group grp;
	static char buf[64];
	struct group *result;
	int ret;

	ret = getgrgid_r(gid, &grp, buf, sizeof buf, &result);
	if (ret != 0) {
		SET_ERRNO(ret);
		return NULL;
	}

	return result;
}

struct group *getgrnam(const char *name) {
	static struct group grp_buf;
	static char buf[64];
	struct group *res;
	int ret;

	ret = getgrnam_r(name, &grp_buf, buf, sizeof(buf), &res);
	if (0 != ret) {
		SET_ERRNO(ret);
		return NULL;
	}

	return res;
}

int getgrgid_r(gid_t gid, struct group *grp,
	char *buf, size_t buflen, struct group **result) {
	int res;
	FILE *file = NULL;

	*result = NULL;

	if (0 != (res = open_db(GROUP_FILE, &file))) {
		return res;
	}

	while (0 == (res = fgetgrent_r(file, grp, buf, buflen, result))) {
		if ((*result)->gr_gid == gid) {
			break;
		}
	}

	fclose(file);
	return res == ENOENT ? 0 : res;
}

static FILE *_grp_stream = NULL;

int getgrent_r(struct group *gbuf, char *buf,
		size_t buflen, struct group **gbufp) {
	int ret;

	if (NULL == _grp_stream) {
		if (0 != (ret = open_db(GROUP_FILE, &_grp_stream))) {
			*gbufp = NULL;
			return ret;
		}
	}
	if (0 != (ret = fgetgrent_r(_grp_stream, gbuf, buf, buflen, gbufp))) {
		if (ENOENT != ret) {
			fclose(_grp_stream);
			_grp_stream = NULL;
		}
		*gbufp = NULL;
		return ret;
	}

	return 0;
}

struct group *getgrent(void) {
	static struct group grp_buf;
	static char buf[64];
	struct group *res;
	int ret;

	if (0 != (ret = getgrent_r(&grp_buf, buf, sizeof(buf), &res))) {
		SET_ERRNO(ret);
		return NULL;
	}

	return res;
}

void setgrent(void) {
	if (NULL != _grp_stream) {
		if (0 != fseek(_grp_stream, 0L, SEEK_SET)) {
			fclose(_grp_stream);
			_grp_stream = NULL;
			SET_ERRNO(EIO);
		}
	}
}

void endgrent(void) {
	if (NULL != _grp_stream) {
		fclose(_grp_stream);
		_grp_stream = NULL;
	}
}
