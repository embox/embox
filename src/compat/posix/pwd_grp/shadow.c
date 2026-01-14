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

extern int pwd_grp_read_field(FILE *stream, char **buf, size_t *buflen, char **field,
		int delim);

extern int pwd_grp_read_int_field(FILE *stream, const char *format, void *field, int delim);

#define SHADOW_NAME_BUF_LEN 64
#define SHADOW_PSWD_BUF_LEN 128

static struct spwd spwd;
static char spwd_buf[SHADOW_NAME_BUF_LEN + SHADOW_PSWD_BUF_LEN];

struct spwd *fgetspent(FILE *file) {
	int res;
	char *buf = spwd_buf;
	size_t buf_len = SHADOW_NAME_BUF_LEN + SHADOW_PSWD_BUF_LEN;

	if (0 != (res = pwd_grp_read_field(file, &buf, &buf_len, &spwd.sp_namp, ':'))) {
		return NULL;
	}

	if (0 != (res = pwd_grp_read_field(file, &buf, &buf_len, &spwd.sp_pwdp, ':'))) {
		return NULL;
	}

	if (0 != (res = pwd_grp_read_int_field(file, "%ld", &spwd.sp_lstchg, ':'))) {
		return NULL;
	}

	if (0 != (res = pwd_grp_read_int_field(file, "%ld", &spwd.sp_min, ':'))) {
		return NULL;
	}

	if (0 != (res = pwd_grp_read_int_field(file, "%ld", &spwd.sp_max, ':'))) {
		return NULL;
	}

	if (0 != (res = pwd_grp_read_int_field(file, "%ld", &spwd.sp_warn, ':'))) {
		return NULL;
	}

	if (0 != (res = pwd_grp_read_int_field(file, "%ld", &spwd.sp_inact, ':'))) {
		return NULL;
	}

	if (0 != (res = pwd_grp_read_int_field(file, "%ld", &spwd.sp_expire, ':'))) {
		return NULL;
	}

	if (0 != (res = pwd_grp_read_int_field(file, "%ld", &spwd.sp_flag, '\n'))) {
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
