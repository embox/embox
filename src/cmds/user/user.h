/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    24.02.2014
 */

#ifndef USER_H_
#define USER_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <shadow.h>

#define BUF_LEN 64

#define PASSWD_FILE "/tmp/passwd"
#define SHADOW_FILE "/tmp/shadow"
#define ADDUSER_FILE "/tmp/adduser.conf"

static void put_string(char *data, FILE *fd, char delim) {
	fwrite(data, sizeof(char), strlen(data), fd);
	fputc(delim, fd);
}

static void put_int(int data, FILE *fd, char delim) {
	char buf[20];

	sprintf(buf, "%i", data);
	fwrite(buf, sizeof(char), strlen(buf), fd);
	fputc(delim, fd);
}

static int is_user_exists(char *name) {
	char pwdbuf[BUF_LEN];
	struct passwd pwd, *result;

	return getpwnam_r(name, &pwd, pwdbuf, BUF_LEN, &result) || result;
}

static int get_group(char *group) {
	struct group _group, *_group_res;
	char buf[80];
	int res = 0;

	if (isdigit(group[0])) {
		res = getgrgid_r(atoi(group), &_group, buf, 80, &_group_res);
	} else {
		res = getgrnam_r(group, &_group, buf, 80, &_group_res);
	}
	return !res && _group_res != NULL ? _group_res->gr_gid : -1;
}

static void write_user_passwd(struct passwd *pwd, FILE *fd) {
	put_string(pwd->pw_name, fd, ':');
	put_string(pwd->pw_passwd, fd, ':');
	put_int(pwd->pw_uid, fd, ':');
	put_int(pwd->pw_gid, fd, ':');
	put_string(pwd->pw_gecos, fd, ':');
	put_string(pwd->pw_dir, fd, ':');
	put_string(pwd->pw_shell, fd, '\n');
}

static void write_user_spwd(struct spwd *spwd, FILE *fd) {
	put_string(spwd->sp_namp, fd, ':');
	put_string(spwd->sp_pwdp, fd, ':');
	put_string("::::::", fd, '\n');
#if 0
	put_int(spwd->sp_lstchg, fd, ':');
	put_int(spwd->sp_min, fd, ':');
	put_int(spwd->sp_max, fd, ':');
	put_int(spwd->sp_warn, fd, ':');
	put_int(spwd->sp_inact, fd, ':');
	put_int(spwd->sp_expire, fd, ':');
	put_int(spwd->sp_expire, fd, '\n');
#endif
}

static int set_options_passwd(struct passwd *result, char *home, char *shell,
		char *gecos, int group) {
	if (0 != strcmp(home, "")) {
		result->pw_dir = home;
	}

	if (0 != strcmp(shell, "")) {
		result->pw_shell = shell;
	}

	if (0 != strcmp(gecos, "")) {
		result->pw_gecos = gecos;
	}

	if (group >= 0) {
		result->pw_gid = group;
	}

	return 0;
}

static int set_options_spwd(struct spwd *result, char *name, char *pswd) {
	if (0 != strcmp(name, "")) {
		result->sp_namp = name;
	}

	if (0 != strcmp(pswd, "")) {
		result->sp_pwdp = pswd;
	}

	return 0;
}

#endif /* USER_H_ */
