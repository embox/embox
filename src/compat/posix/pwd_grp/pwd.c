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

static int read_pwd(FILE *stream, char *buf, size_t buflen, struct passwd *pwd) {
	int res;
	char *p = buf;
	size_t llen = buflen;

	if (0 != (res = pwd_grp_read_field(stream, &p, &llen, &pwd->pw_name, ':'))) {
		return res;
	}

	if (0 != (res = pwd_grp_read_field(stream, &p, &llen, &pwd->pw_passwd, ':'))) {
		return res;
	}

	if (0 != (res = pwd_grp_read_int_field(stream, "%hd", &pwd->pw_uid, ':'))) {
		return res;
	}

	if (0 != (res = pwd_grp_read_int_field(stream, "%hd", &pwd->pw_gid, ':'))) {
		return res;
	}

	if (0 != (res = pwd_grp_read_field(stream, &p, &llen, &pwd->pw_gecos, ':'))) {
		return res;
	}

	if (0 != (res = pwd_grp_read_field(stream, &p, &llen, &pwd->pw_dir, ':'))) {
		return res;
	}

	if (0 != (res = pwd_grp_read_field(stream, &p, &llen, &pwd->pw_shell, '\n'))) {
		return res;
	}

	return 0;
}

int fgetpwent_r(FILE *file, struct passwd *pwd, char *buf, size_t buflen,
		struct passwd **result) {

	int res = read_pwd(file, buf, buflen, pwd);

	if (res == EOF) {
		*result = NULL;
		return ENOENT;
	}

	*result = pwd;
	return res;

}

int getpwnam_r(const char *name, struct passwd *pwd,
		char *buf, size_t buflen, struct passwd **result) {
	int res;
	FILE *file = NULL;

	if (buflen > sysconf(_SC_GETPW_R_SIZE_MAX)) {
		*result = NULL;
		return -ERANGE;
	}

	if (0 != (res = open_db(PASSWD_FILE, &file))) {
		*result = NULL;
		return res;
	}

	while (0 == (res = fgetpwent_r(file, pwd, buf, buflen, result))) {
		if (0 == strcmp(pwd->pw_name, name)) {
			break;
		}
	}

	fclose(file);

	if (0 != res) {
		result = NULL;
		return res == ENOENT ? 0 : -1;
	}

	return 0;
}

struct passwd *getpwnam(const char *name) {
	static struct passwd getpwnam_buffer;
	static char buff[0x80];
	struct passwd *res;
	int ret;

	if (0 != (ret = getpwnam_r(name, &getpwnam_buffer, buff, sizeof(buff),  &res))) {
		SET_ERRNO(-ret);
		return NULL;
	}

	return res;
}

int getpwuid_r(uid_t uid, struct passwd *pwd,
		char *buf, size_t buflen, struct passwd **result) {
	int res;
	FILE *file = NULL;

	if (0 != (res = open_db(PASSWD_FILE, &file))) {
		*result = NULL;
		return res;
	}

	while (0 == (res = fgetpwent_r(file, pwd, buf, buflen, result))) {
		if (pwd->pw_uid == uid) {
			break;
		}
	}

	fclose(file);

	if (0 != res) {
		result = NULL;
		return res == ENOENT ? 0 : -1;
	}

	return 0;
}

struct passwd *getpwuid(uid_t uid) {
	static struct passwd getpwuid_buffer;
	static char buff[0x80];
	struct passwd *res;
	int ret;

	if (0 != (ret = getpwuid_r(uid, &getpwuid_buffer, buff, sizeof(buff), &res))) {
		SET_ERRNO(-ret);
		return NULL;
	}

	return res;
}


int getmaxuid() {
	int res, curmax = 0;
	FILE *file = NULL;
	static char buff[0x80];
	struct passwd *result, pwd;

	if (0 != (res = open_db(PASSWD_FILE, &file))) {
		return res;
	}

	while (0 == (res = fgetpwent_r(file, &pwd, buff, sizeof(buff), &result))) {
		if (pwd.pw_uid > curmax) {
			curmax = pwd.pw_uid;
		}
	}

	fclose(file);

	return curmax;
}

int get_defpswd(struct passwd *passwd, char *buf, size_t buf_len) {
	FILE *passwdf;
	char *temp;
	int res = 0;

	if (NULL == (passwdf = fopen(ADDUSER_FILE, "r"))) {
		return errno;
	}

	while (pwd_grp_read_field(passwdf, &buf, &buf_len, &temp, '=') != EOF) {
		if(0 == strcmp(temp, "GROUP")) {
			if (0 != pwd_grp_read_int_field(passwdf, "%hd", &passwd->pw_gid, '\n')) {
				res = -1;
				goto out;
			}
			continue;
		}

		if(0 == strcmp(temp, "HOME")) {
			if (0 != pwd_grp_read_field(passwdf, &buf, &buf_len,
					&passwd->pw_dir, '\n')) {
				res = -1;
				goto out;
			}
			continue;
		}

		if(0 == strcmp(temp, "SHELL")) {
			if (0 != pwd_grp_read_field(passwdf, &buf, &buf_len,
					&passwd->pw_shell, '\n')) {
				res = -1;
				goto out;
			}
			continue;
		}
	}

out:
	fclose(passwdf);
	return res;
}
