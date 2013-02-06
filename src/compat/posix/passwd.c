/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    06.02.2013
 */

#include <types.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <pwd.h>

#define PASSWD_FILE "/passwd"

static int open_db(const char *db_path, FILE **result) {
	FILE *f;
	if (NULL == (f = fopen(db_path, "r"))) {
		return errno;
	}

	*result = f;
	return 0;
}

static int read_field(FILE *stream, char **buf, size_t *buflen, char **field,
		int delim) {
	int ch = fgetc(stream);

	*field = *buf;

	while (ch != delim) {
		if (ch == ':' || ch == '\n') {
			return -EIO;
		}

		if (*buflen <= 0) {
			return -ERANGE;
		}

		*((*buf)++) = (char) ch;
		(*buflen)--;

		ch = fgetc(stream);
	}

	if (*buflen <= 0) {
		return -ERANGE;
	}

	*((*buf)++) = '\0';
	(*buflen)--;

	return 0;
}

static int read_int_field(FILE *stream, unsigned short *field, int delim) {
	int val;
	int ret = fscanf(stream, "%d", &val);

	if (0 > ret) {
		return ret;
	}

	*field = val;

	if (delim != (val = fgetc(stream))) {
		return -EIO;
	}

	return 0;
}

static int read_pwd(FILE *stream, char *buf, size_t buflen, struct passwd *pwd) {
	int res;
	char *p = buf;
	size_t llen = buflen;

	if (0 != (res = read_field(stream, &p, &llen, &pwd->pw_name, ':'))) {
		return res;
	}

	if (0 != (res = read_field(stream, &p, &llen, &pwd->pw_passwd, ':'))) {
		return res;
	}

	if (0 != (res = read_int_field(stream, &pwd->pw_uid, ':'))) {
		return res;
	}

	if (0 != (res = read_int_field(stream, &pwd->pw_gid, ':'))) {
		return res;
	}

	if (0 != (res = read_field(stream, &p, &llen, &pwd->pw_gecos, ':'))) {
		return res;
	}

	if (0 != (res = read_field(stream, &p, &llen, &pwd->pw_dir, ':'))) {
		return res;
	}

	if (0 != (res = read_field(stream, &p, &llen, &pwd->pw_shell, '\n'))) {
		return res;
	}

	return 0;
}

int getpwby_db_r(const char *db_path, const char *name, uid_t uid,
		struct passwd *pwd, char *buf, size_t buflen,
		struct passwd **result) {
	int res;
	FILE *file;
	if (0 != (res = open_db(db_path, &file))) {
		return res;
	}

	if (name) {
		while (0 == (res = read_pwd(file, buf, buflen, pwd))) {
			if (0 == strcmp(pwd->pw_name, name)) {
				break;
			}
		}
	} else {
		while (0 == (res = read_pwd(file, buf, buflen, pwd))) {
			if (pwd->pw_uid == uid) {
				break;
			}
		}
	}

	fclose(file);

	if (res) {
		errno = -res;
		result = NULL;
	} else {
		*result = pwd;
	}

	return res ? -1 : 0;
}

int getpwnam_r(const char *name, struct passwd *pwd,
		char *buf, size_t buflen, struct passwd **result) {
	return getpwby_db_r(PASSWD_FILE, name, 0, pwd, buf, buflen, result);
}


int getpwuid_r(uid_t uid, struct passwd *pwd,
		char *buf, size_t buflen, struct passwd **result) {
	return getpwby_db_r(PASSWD_FILE, NULL, uid, pwd, buf, buflen, result);
}

