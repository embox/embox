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

static int read_int_field(FILE *stream, const char *format, void *field, int delim) {
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

	if (0 != (res = read_int_field(stream, "%hd", &pwd->pw_uid, ':'))) {
		return res;
	}

	if (0 != (res = read_int_field(stream, "%hd", &pwd->pw_gid, ':'))) {
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
	FILE *file;

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
	FILE *file;

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

int fgetgrent_r(FILE *fp, struct group *gbuf, char *tbuf,
		size_t buflen, struct group **gbufp) {
	int res;
	char *buf = tbuf;
	size_t buf_len = buflen;
	char *ch, **pmem;

	*gbufp = NULL;

	if (0 != (res = read_field(fp, &buf, &buf_len, &gbuf->gr_name, ':'))) {
		if (EOF == res)
			return ENOENT;
		return res;
	}

	if (0 != (res = read_field(fp, &buf, &buf_len, &gbuf->gr_passwd, ':'))) {
		if (EOF == res)
			return ENOENT;
		return res;
	}

	if (0 != (res = read_int_field(fp, "%hd", &gbuf->gr_gid, ':'))) {
		if (EOF == res)
			return ENOENT;
		return res;
	}

	if (0 != (res = read_field(fp, &buf, &buf_len, &ch, '\n'))) {
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
	FILE *file;

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
	FILE *file;

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

int getmaxuid() {
	int res, curmax = 0;
	FILE *file;
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

#define SHADOW_NAME_BUF_LEN 64
#define SHADOW_PSWD_BUF_LEN 128

static struct spwd spwd;
static char spwd_buf[SHADOW_NAME_BUF_LEN + SHADOW_PSWD_BUF_LEN];

struct spwd *fgetspent(FILE *file) {
	int res;
	char *buf = spwd_buf;
	size_t buf_len = SHADOW_NAME_BUF_LEN + SHADOW_PSWD_BUF_LEN;

	if (0 != (res = read_field(file, &buf, &buf_len, &spwd.sp_namp, ':'))) {
		return NULL;
	}

	if (0 != (res = read_field(file, &buf, &buf_len, &spwd.sp_pwdp, ':'))) {
		return NULL;
	}

	if (0 != (res = read_int_field(file, "%ld", &spwd.sp_lstchg, ':'))) {
		return NULL;
	}

	if (0 != (res = read_int_field(file, "%ld", &spwd.sp_min, ':'))) {
		return NULL;
	}

	if (0 != (res = read_int_field(file, "%ld", &spwd.sp_max, ':'))) {
		return NULL;
	}

	if (0 != (res = read_int_field(file, "%ld", &spwd.sp_warn, ':'))) {
		return NULL;
	}

	if (0 != (res = read_int_field(file, "%ld", &spwd.sp_inact, ':'))) {
		return NULL;
	}

	if (0 != (res = read_int_field(file, "%ld", &spwd.sp_expire, ':'))) {
		return NULL;
	}

	if (0 != (res = read_int_field(file, "%ld", &spwd.sp_flag, '\n'))) {
		return NULL;
	}

	return &spwd;
}

struct spwd *spwd_find(const char *spwd_path, const char *name) {
	struct spwd *spwd;
	FILE *shdwf;

	if (NULL == (shdwf = fopen(spwd_path, "r"))) {
		return NULL;
	}

	while (NULL != (spwd = fgetspent(shdwf))) {
		if (0 == strcmp(spwd->sp_namp, name)) {
			break;
		}
	}

	fclose(shdwf);

	return spwd;
}

struct spwd *getspnam_f(const char *name) {
	return spwd_find(SHADOW_FILE, name);
}

struct spwd *getspnam(char *name) {
	/* FIXME */
	return getspnam_f(name);
}

int get_defpswd(struct passwd *passwd, char *buf, size_t buf_len) {
	FILE *passwdf;
	char *temp;
	int res = 0;

	if (NULL == (passwdf = fopen(ADDUSER_FILE, "r"))) {
		return errno;
	}

	while (read_field(passwdf, &buf, &buf_len, &temp, '=') != EOF) {
		if(0 == strcmp(temp, "GROUP")) {
			if (0 != read_int_field(passwdf, "%hd", &passwd->pw_gid, '\n')) {
				res = -1;
				goto out;
			}
			continue;
		}

		if(0 == strcmp(temp, "HOME")) {
			if (0 != read_field(passwdf, &buf, &buf_len,
					&passwd->pw_dir, '\n')) {
				res = -1;
				goto out;
			}
			continue;
		}

		if(0 == strcmp(temp, "SHELL")) {
			if (0 != read_field(passwdf, &buf, &buf_len,
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

static int write_int_field(FILE *fp, long int val, char delim) {
	if (val) {
		fprintf(fp, "%ld", val);
	}
	fputc(delim, fp);
	return 0;
}

int putspent(struct spwd *p, FILE *fp) {

	fprintf(fp, "%s:%s:", p->sp_namp, p->sp_pwdp);
	write_int_field(fp, p->sp_lstchg, ':');
	write_int_field(fp, p->sp_min, ':');
	write_int_field(fp, p->sp_max, ':');
	write_int_field(fp, p->sp_warn, ':');
	write_int_field(fp, p->sp_inact, ':');
	write_int_field(fp, p->sp_expire, ':');
	write_int_field(fp, p->sp_flag, '\n');

	return 0;
}
